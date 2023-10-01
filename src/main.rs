#![feature(type_alias_impl_trait)]
#![feature(exclusive_range_pattern)]
#![feature(proc_macro_hygiene)]
#![feature(int_roundings)]
#![feature(is_some_and)]
#![feature(stdsimd)]
#![feature(async_closure)]

#![no_main]
#![no_std]

#![allow(stable_features)]
#![allow(unused_mut)]


use panic_halt as _;
use rtic;
use rtic_monotonics::systick::*;
use rtic_sync::{channel::*, make_channel};
use w25q::series25::FlashInfo;
use arrayvec::ArrayVec;
use usb_device::{bus::UsbBusAllocator, device::UsbDevice};
use usbd_serial::SerialPort;
use usbd_webusb::{url_scheme, WebUsb};

use stm32f4xx_hal::{
    adc::{Adc, config::AdcConfig},
    crc32,
    crc32::Crc32,
    gpio::{Edge, Input},
    otg_fs,
    otg_fs::{USB, UsbBusType},
    pac::{ADC1, TIM13, TIM2, TIM3, TIM5},
    prelude::*,
    spi::*,
    timer::{self, Event},
};

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM4, TIM7, TIM8_CC])]
mod app {
    use super::*;

    mod my_module;
    // pub mod debug;
    pub mod engine;
    pub mod gpio;
    pub mod injection;
    pub mod logging;
    pub mod memory;
    pub mod util;
    pub mod webserial;
    // pub mod tasks;


    use my_module::blink2;

    use crate::app::{
        engine::{
            efi_cfg::{EngineConfig, get_default_efi_cfg},
            engine_status::{EngineStatus, get_default_engine_status},
            sensors::{get_sensor_raw, SensorTypes, SensorValues},
            cpwm::VRStatus,
            pmic::{PMIC, PmicT},
        },
        gpio::{
            LedGpioMapping,
            AuxIoMapping,
            ADCMapping,
            IgnitionGpioMapping,
            init_gpio,
            InjectionGpioMapping,
            RelayMapping,
            StepperMapping,
        },
        injection::{calculate_time_isr, injection_setup},
        memory::tables::{SpiT, Tables},
        logging::host,
        webserial::{
            handle_engine::engine_cdc_callback,
            handle_pmic::pmic_cdc_callback,
            handle_realtime_data::realtime_data_cdc_callback,
            handle_tables::table_cdc_callback,
            send_message,
            SerialMessage,
            SerialStatus,
            finish_message,
        },
        // tasks::{engine::ckp_trigger, engine::motor_checks, ignition::ignition_schedule}
    };

    #[shared]
    struct Shared {
        // Timers:
        timer: timer::CounterUs<TIM2>,
        timer3: timer::CounterUs<TIM3>,
        timer4: timer::CounterUs<TIM5>,
        timer13: timer::DelayUs<TIM13>,

        // Core I/O
        led: LedGpioMapping,
        inj_pins: InjectionGpioMapping,
        ign_pins: IgnitionGpioMapping,
        aux_pins: AuxIoMapping,
        relay_pins: RelayMapping,
        stepper_pins: StepperMapping,

        // USB:
        // TODO: se puede reducir implementando los channels de RTIC, de paso fixeo el bug de las tablas
        usb_cdc: SerialPort<'static, UsbBusType, [u8; 128], [u8; 256]>,
        usb_web: WebUsb<UsbBusType>,
        cdc_sender: Sender<'static, SerialMessage, CDC_BUFF_CAPACITY>,

        // core:
        spi_lock: bool,
        crc: Crc32,

        // EFI Related:
        efi_cfg: EngineConfig,
        efi_status: EngineStatus,
        flash: memory::tables::FlashT,
        flash_info: FlashInfo,
        tables: Tables,
        sensors: SensorValues,
        pmic: PmicT,

        // CKP/SYNC
        ckp: VRStatus,
        ignition_running: bool,
    }

    #[local]
    struct Local {
        usb_dev: UsbDevice<'static, UsbBusType>,
        cdc_input_buffer: ArrayVec<u8, 128>,

        // EFI Related:
        ckp: stm32f4xx_hal::gpio::PC6<Input>,
        adc: Adc<ADC1>,
        analog_pins: ADCMapping,

        // cdc_sender: Sender<'static, u32, 8>,

        // TODO: Remove
        state: bool,
        state2: bool,

        // WebSerial:
        table_sender: Sender<'static, SerialMessage, CDC_BUFF_CAPACITY>,
        real_time_sender: Sender<'static, SerialMessage, CDC_BUFF_CAPACITY>,
        pmic_sender: Sender<'static, SerialMessage, CDC_BUFF_CAPACITY>,
        engine_sender: Sender<'static, SerialMessage, CDC_BUFF_CAPACITY>,
    }

    const CAPACITY: usize = 5;
    const CDC_BUFF_CAPACITY: usize = 30;

    #[init(local = [USB_BUS: Option < UsbBusAllocator < UsbBusType >> = None])]
    fn init(mut cx: init::Context) -> (Shared, Local) {
        // Setup clocks
        //let mut flash = cx.device.FLASH.constrain();
        let mut device = cx.device;
        let mut rcc = device.RCC.constrain();
        cx.core.DWT.enable_cycle_counter();

        // Initialize the systick interrupt & obtain the token to prove that we did
        let systick_mono_token = rtic_monotonics::create_systick_token!();
        Systick::start(cx.core.SYST, 120_000_000, systick_mono_token); // default STM32F407 clock-rate is 36MHz

        let _clocks = rcc.cfgr.use_hse(25.MHz()).sysclk(120.MHz()).require_pll48clk().freeze();

        // TODO: Disable this in release builds
        debug!("Hello v1 :)");

        let gpio_a = device.GPIOA.split();
        let gpio_b = device.GPIOB.split();
        let gpio_c = device.GPIOC.split();
        let gpio_d = device.GPIOD.split();
        let gpio_e = device.GPIOE.split();

        let mut gpio_config = init_gpio(gpio_a, gpio_b, gpio_c, gpio_d, gpio_e);

        // ADC
        let mut adc = Adc::adc1(device.ADC1, true, AdcConfig::default());

        adc.enable();
        adc.calibrate();

        // configure CKP/CMP Pin for Interrupts
        let mut ckp = gpio_config.ckp;
        let mut syscfg = device.SYSCFG.constrain();
        debug!("init gpio");
        ckp.make_interrupt_source(&mut syscfg);
        ckp.trigger_on_edge(&mut device.EXTI, Edge::Falling);

        // configure the timers

        // timer Tiempo inyeccion
        let mut timer: timer::CounterUs<TIM2> = device.TIM2.counter_us(&_clocks);
        // timer tiempo de ignicion
        let mut timer3: timer::CounterUs<TIM3> = device.TIM3.counter_us(&_clocks);
        // timer CPWM
        let mut timer4: timer::CounterUs<TIM5> = device.TIM5.counter_us(&_clocks);

        // timer uso generico
        let mut timer13: timer::DelayUs<TIM13> = device.TIM13.delay_us(&_clocks);

        //  TIM5, TIM7, TIM4
        debug!("init timers");
        timer.start((150).millis()).unwrap();

        // Set up to generate interrupt when timer expires
        timer.listen(Event::Update);
        timer3.listen(Event::Update);
        timer13.listen(Event::Update);
        // timer4.start((70).minutes()).unwrap();
        timer4.start(1_000_000_u32.micros()).unwrap();

        let mut efi_cfg = get_default_efi_cfg();
        let mut _efi_status = get_default_engine_status();

        // SPI:
        let mode = Mode {
            polarity: Polarity::IdleLow,
            phase: Phase::CaptureOnFirstTransition,
        };

        let spi2 = Spi::new(
            device.SPI2,
            (gpio_config.spi_sck, gpio_config.spi_miso, gpio_config.spi_mosi),
            mode,
            (30).MHz(),
            &_clocks,
        );

        let spi_bus = shared_bus_rtic::new!(spi2, SpiT);
        let spi_pmic = spi_bus.acquire();

        // CRC32:
        let mut crc = crc32::Crc32::new(device.CRC);
        host::debug!("init flash");
        let mut flash = w25q::series25::Flash::init(spi_bus.acquire(), gpio_config.memory_cs).unwrap();

        // let id = flash.read_jedec_id().unwrap();

        let flash_info = flash.get_device_info().unwrap();

        // EFI Setup:
        let mut table = Tables {
            tps_rpm_ve: None,
            tps_rpm_afr: None,
            injector_delay: None,
            load_tps_deg: None,
            // get not implemented
            vbat_correction: None,
            wue: None,
            ase_taper: None,
            ase_intensity: None,
        };

        efi_cfg.read(&mut flash, &flash_info, &mut crc);

        injection_setup(&mut table, &mut flash, &flash_info, &mut crc);

        // REMOVE: solo lo estoy hardcodeando aca para probar el AlphaN
        _efi_status.rpm = 1500;

        calculate_time_isr(&mut _efi_status, &efi_cfg, &mut table);

        host::debug!("AirFlow {:?} g/s", _efi_status.injection.air_flow);
        host::debug!("AF/r {:?}", _efi_status.injection.targetAFR);
        host::debug!("Base Fuel {:?} cm3", _efi_status.injection.base_fuel);
        host::debug!("Base Air {:?} cm3", _efi_status.injection.base_air);
        host::debug!("Time {:?} mS", _efi_status.injection.injection_bank_1_time);

        let mut sensors = SensorValues::new();

        let mut spi_lock = false;
        let mut pmic = PMIC::init(spi_pmic, gpio_config.pmic.pmic_cs).unwrap();

        ckp.enable_interrupt(&mut device.EXTI);

        let mut ckp_status = VRStatus::new();

        // Init USB
        let usb = USB {
            usb_global: device.OTG_FS_GLOBAL,
            usb_device: device.OTG_FS_DEVICE,
            usb_pwrclk: device.OTG_FS_PWRCLK,
            pin_dm: gpio_config.usb_dp,
            pin_dp: gpio_config.usb_dm,
            hclk: _clocks.hclk(),
        };

        static mut EP_MEMORY: [u32; 1024] = [0; 1024];
        static mut __USB_TX: [u8; 256] = [0; 256];
        static mut __USB_RX: [u8; 128] = [0; 128];
        host::debug!("init usb");
        let usb_bus = cx.local.USB_BUS;
        unsafe {
            *usb_bus = Some(otg_fs::UsbBus::new(usb, &mut EP_MEMORY));
        }

        let usb_cdc = unsafe {
            SerialPort::new_with_store(usb_bus.as_ref().unwrap(), __USB_RX, __USB_TX)
        };
        let usb_web = WebUsb::new(
            usb_bus.as_ref().unwrap(),
            url_scheme::HTTPS,
            "tuner.openefi.tech",
        );

        let usb_dev = webserial::new_device(usb_bus.as_ref().unwrap());

        let cdc_buff = ArrayVec::<u8, 128>::new();


        // DEMO
        // Schedule the blinking task
        blink::spawn().ok();
        blink2::spawn().ok();

        let (cdc_sender, cdc_receiver) = make_channel!(SerialMessage, CDC_BUFF_CAPACITY);

        cdc_receiver::spawn(cdc_receiver).unwrap();

        (Shared {
            // Timers:
            // delay,
            timer,
            timer3,
            timer4,
            timer13,

            // USB
            usb_cdc,
            usb_web,
            cdc_sender: cdc_sender.clone(),

            // GPIO:
            led: gpio_config.led,
            inj_pins: gpio_config.injection,
            ign_pins: gpio_config.ignition,
            aux_pins: gpio_config.aux,
            relay_pins: gpio_config.relay,
            stepper_pins: gpio_config.stepper,
            sensors,

            // CORE:
            crc,
            flash,
            flash_info,
            spi_lock,

            // EFI Related
            efi_cfg,
            efi_status: _efi_status,
            tables: table,
            pmic,

            //CKP/SYNC
            ckp: ckp_status,
            ignition_running: false,
        }, Local {
            // USB
            usb_dev,
            cdc_input_buffer: cdc_buff,

            // Serial
            table_sender: cdc_sender.clone(),
            real_time_sender: cdc_sender.clone(),
            pmic_sender: cdc_sender.clone(),
            engine_sender: cdc_sender.clone(),

            adc,
            ckp,
            analog_pins: gpio_config.adc,

            state: false,
            state2: false,
        })
    }

    #[task(local = [state], shared = [led])]
    async fn blink(mut cx: blink::Context) {
        loop {
            if *cx.local.state {
                cx.shared.led.lock(|l| { l.led_0.set_high() });
                *cx.local.state = false;
            } else {
                cx.shared.led.lock(|l| { l.led_0.set_low() });
                *cx.local.state = true;
            }
            Systick::delay(100.millis()).await;
        }
    }


    #[task(binds = OTG_FS, local = [usb_dev, cdc_input_buffer], shared = [usb_cdc, usb_web, cdc_sender])]
    fn usb_handler(mut ctx: usb_handler::Context) {
        let device = ctx.local.usb_dev;

        ctx.shared.usb_cdc.lock(|cdc| {
            // USB dev poll only in the interrupt handler
            (ctx.shared.usb_web, ctx.shared.cdc_sender).lock(|web, cdc_sender| {
                if device.poll(&mut [web, cdc]) {
                    let mut buf = [0u8; 64];

                    match cdc.read(&mut buf[..]) {
                        Ok(count) => {
                            // Push bytes into the buffer
                            for i in 0..count {
                                ctx.local.cdc_input_buffer.push(buf[i]);
                                if ctx.local.cdc_input_buffer.is_full() {
                                    webserial::process_command(
                                        ctx.local.cdc_input_buffer.take().into_inner().unwrap(),
                                        cdc_sender,
                                    );

                                    ctx.local.cdc_input_buffer.clear();
                                }
                            }
                        }
                        Err(_) => {}
                    }
                }
            });
        });
    }

    // Externally defined tasks
    extern "Rust" {
        // Low-priority task to send back replies via the serial port. , capacity = 30
        #[task(shared = [usb_cdc])]
        async fn send_message(
            ctx: send_message::Context,
            status: SerialStatus,
            code: u8,
            message: SerialMessage,
        );

        #[task(local=[table_sender], shared = [flash_info, efi_cfg, tables, crc, flash, spi_lock])]
        async fn table_cdc_callback(ctx: table_cdc_callback::Context, serial_cmd: SerialMessage);
        #[task(local=[real_time_sender], shared = [efi_status, sensors, crc])]
        async fn realtime_data_cdc_callback(ctx: realtime_data_cdc_callback::Context, serial_cmd: SerialMessage);

        #[task(local=[pmic_sender], shared = [efi_status, pmic, crc])]
        async fn pmic_cdc_callback(ctx: pmic_cdc_callback::Context, serial_cmd: SerialMessage);

        #[task(local=[engine_sender], shared = [flash,flash_info,efi_cfg, crc])]
        async fn engine_cdc_callback(ctx: engine_cdc_callback::Context, serial_cmd: SerialMessage);

        // from: https://github.com/noisymime/speeduino/blob/master/speeduino/decoders.ino#L453
        // #[task(binds = EXTI9_5,
        // local = [ckp],
        // shared = [led, efi_status, flash_info, efi_cfg, timer, timer3, timer4, ckp, ign_pins]
        // )]
        // fn ckp_trigger(ctx: ckp_trigger::Context);
        //
        // #[task(
        // shared = [led, efi_status, efi_cfg, timer3, timer4, ckp, ign_pins],
        // priority = 1
        // )]
        // async fn ignition_schedule(ctx: ignition_schedule::Context);
        //
        // #[task(shared = [efi_cfg, ckp, timer4, efi_status,ignition_running], priority = 1)]
        // async fn motor_checks(ctx: motor_checks::Context);
    }

    // Externally defined tasks
    extern "Rust" {
        #[task(local = [state2], shared = [led])]
        async fn blink2(cx: blink2::Context);
    }

    #[task(shared = [usb_cdc])]
    async fn cdc_receiver(mut ctx: cdc_receiver::Context, mut receiver: Receiver<'static, SerialMessage, CDC_BUFF_CAPACITY>) {
        while let Ok(message) = receiver.recv().await {
            ctx.shared.usb_cdc.lock(|cdc| {
                cdc.write(&finish_message(message)).unwrap();
            });
        }
    }
}
