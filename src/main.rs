#![feature(type_alias_impl_trait)]
#![feature(exclusive_range_pattern)]
#![feature(proc_macro_hygiene)]
#![feature(int_roundings)]
#![feature(is_some_and)]
#![feature(stdsimd)]

#![no_main]
#![no_std]

#![allow(stable_features)]
#![allow(unused_mut)]


use arrayvec::ArrayVec;
use rtic;
use rtic::Mutex;
use rtic_monotonics::systick::*;
use w25q::series25::FlashInfo;

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

use usb_device::{bus::UsbBusAllocator, device::UsbDevice};
use usbd_serial::SerialPort;
use usbd_webusb::{url_scheme, WebUsb};

use crate::app::{
    engine::{
        efi_cfg::{EngineConfig, get_default_efi_cfg},
        engine_status::{EngineStatus, get_default_engine_status},
        sensors::{get_sensor_raw, SensorTypes, SensorValues},
        cpwm::VRStatus,
        pmic::{PMIC, PmicT},
    },
    gpio_legacy::{
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
    webserial::{handle_engine, handle_pmic, handle_realtime_data,handle_tables, send_message, SerialMessage, SerialStatus,finish_message},
    // tasks::{engine::ckp_trigger, engine::motor_checks, ignition::ignition_schedule}
};

// use panic_halt as _;
use panic_semihosting as _;
#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM4, TIM7, TIM8_CC])]
mod app {
    use super::*;
    use rtic_sync::{channel::*, make_channel};
    pub mod debug;
    pub mod engine;
    pub mod gpio;
    pub mod gpio_legacy;
    pub mod injection;
    pub mod logging;
    pub mod memory;
    pub mod util;
    pub mod webserial;
    pub mod tasks;
    const CDC_CAPACITY: usize = 20;

    #[shared]
    struct Shared {
        // Timers:
        // delay: Delay,
        timer: timer::CounterUs<TIM2>,
        timer3: timer::CounterUs<TIM3>,
        timer4: timer::CounterUs<TIM5>,
        timer13: timer::DelayUs<TIM13>,

        // Core I/O
        led: gpio_legacy::LedGpioMapping,
        inj_pins: InjectionGpioMapping,
        ign_pins: IgnitionGpioMapping,
        aux_pins: AuxIoMapping,
        relay_pins: RelayMapping,
        stepper_pins: StepperMapping,

        // USB:
        // TODO: se puede reducir implementando los channels de RTIC, de paso fixeo el bug de las tablas
        usb_cdc: SerialPort<'static, UsbBusType, [u8; 128], [u8; 4000]>,
        usb_web: WebUsb<UsbBusType>,

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
        ignition_running:bool,
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
    }



    #[init(local = [USB_BUS: Option < UsbBusAllocator < UsbBusType >> = None])]
    fn init(cx: init::Context) -> (Shared, Local) {
        // Setup clocks
        //let mut flash = cx.device.FLASH.constrain();
        let mut device = cx.device;
        let mut rcc = device.RCC.constrain();

        // Initialize the systick interrupt & obtain the token to prove that we did
        let systick_mono_token = rtic_monotonics::create_systick_token!();
        Systick::start(cx.core.SYST, 120_000_000, systick_mono_token); // default STM32F407 clock-rate is 36MHz

        let _clocks = rcc.cfgr.use_hse(25.MHz()).sysclk(120.MHz()).require_pll48clk().freeze();

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
        host::debug!("init gpio");
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
        host::debug!("init timers");
        timer.start((150).millis()).unwrap();

        // Set up to generate interrupt when timer expires
        timer.listen(Event::Update);
        timer3.listen(Event::Update);
        timer13.listen(Event::Update);
        // timer4.start((70).minutes()).unwrap();
        timer4.start(1_000_000_u32.micros()).unwrap();

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
        static mut __USB_TX: [u8; 4000] = [0; 4000];
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


        // EFI Related:
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

        // gpio_config.led.led_check.toggle();
        // gpio_config.led.led_mil.toggle();

        let mut sensors = SensorValues::new();

        let mut spi_lock = false;
        let mut pmic = PMIC::init(spi_pmic, gpio_config.pmic.pmic_cs).unwrap();

        let tick_init = timer4.now().ticks();
        timer13.delay_us(1000u16);
        let tick_end = timer4.now().ticks();



        host::debug!("Time {:?} mS, Init: {:?} , End {:?}", tick_end-tick_init,tick_init,tick_end);

        let mut ignition_running = false;

        ckp.enable_interrupt(&mut device.EXTI);

        let mut ckp_status = VRStatus::new();

      //  let (cdc_sender, cdc_receiver) = make_channel!(u32, CDC_CAPACITY);
        static mut cdc_channel:Channel<u32,8> = Channel::new();

        // let (cdc_sender,cdc_receiver) = unsafe {cdc_channel.split()};
        // let cdc_s = cdc_sender.clone();
        // webserial_sender::spawn(cdc_receiver).unwrap();

        // Schedule the blinking task
        blink::spawn().ok();
        blink2::spawn().ok();

        return (
            // Initialization of shared resources
            Shared {
                // Timers:
                // delay,
                timer,
                timer3,
                timer4,
                timer13,

                // USB
                usb_cdc,
                usb_web,

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
                ignition_running:false
            }, Local {
                usb_dev,
                cdc_input_buffer: cdc_buff,
                // cdc_output: cdc_s,
               // cdc_sender,

                adc,
                ckp,
                analog_pins: gpio_config.adc,

                state: false,
                state2: false,
            });
    }

    #[task(local = [state], shared = [led])]
    async fn blink(mut cx: blink::Context) {
        loop {
            if *cx.local.state {
                cx.shared.led.lock(|l| l.led_0.set_high());
                *cx.local.state = false;
            } else {
                cx.shared.led.lock(|l| l.led_0.set_low());
                *cx.local.state = true;
            }
            Systick::delay(100.millis()).await;
        }
    }

    #[task(local = [state2], shared = [led])]
    async fn blink2(mut cx: blink2::Context) {
        loop {
            if *cx.local.state2 {
                cx.shared.led.lock(|l| l.led_1.set_high());
                *cx.local.state2 = false;
            } else {
                cx.shared.led.lock(|l| l.led_1.set_low());
                *cx.local.state2 = true;
            }
            Systick::delay(50.millis()).await;
        }
    }

    // #[task(shared = [usb_cdc], priority = 2)]
    // async fn send_message(
    //     mut ctx: send_message::Context,
    //     status: SerialStatus,
    //     code: u8,
    //     mut message: SerialMessage,
    // ) {
    //     message.status = status as u8;
    //     message.code = code;
    //
    //     ctx.shared.usb_cdc.lock(|cdc| {
    //         cdc.write(&finish_message(message)).unwrap();
    //     });
    //     Systick::delay(50.millis()).await;
    // }

    #[task(priority = 2, shared = [flash_info, efi_cfg, tables, crc, flash, spi_lock])]
    async fn table_cdc_callback(ctx: table_cdc_callback::Context, serial_cmd: SerialMessage) {
        let flash = ctx.shared.flash;
        let flash_info = ctx.shared.flash_info;
        let tables = ctx.shared.tables;
        let crc = ctx.shared.crc;
        let spi_lock = ctx.shared.spi_lock;

        (flash, flash_info, tables, crc, spi_lock).lock(|flash, flash_info, tables, crc, spi_lock| {
            *spi_lock = true;
            handle_tables::handler(serial_cmd, flash, flash_info, tables, crc);
            *spi_lock = false;
        });
    }

    #[task(priority = 2, shared = [spi_lock, pmic])]
    async fn pmic_cdc_callback(ctx: pmic_cdc_callback::Context, serial_cmd: SerialMessage) {
        let spi_lock = ctx.shared.spi_lock;
        let pmic = ctx.shared.pmic;

        (spi_lock, pmic).lock(|spi_lock, pmic| {
            *spi_lock = true;
            handle_pmic::handler(serial_cmd, pmic);
            *spi_lock = false;
        })
    }

    #[task(priority = 2, shared = [spi_lock, flash, flash_info, efi_cfg, crc])]
    async fn engine_cdc_callback(ctx: engine_cdc_callback::Context, serial_cmd: SerialMessage) {
        let spi_lock = ctx.shared.spi_lock;
        let flash = ctx.shared.flash;
        let flash_info = ctx.shared.flash_info;
        let efi_cfg = ctx.shared.efi_cfg;
        let crc = ctx.shared.crc;

        (spi_lock, flash, flash_info, efi_cfg, crc).lock(
            |spi_lock, flash, flash_info, efi_cfg, crc| {
                *spi_lock = true;
                handle_engine::handler(flash, flash_info, crc, efi_cfg, serial_cmd);
                *spi_lock = false;
            }
        )
    }

    #[task(priority = 2, shared = [efi_status, sensors, crc])]
    async fn realtime_data_cdc_callback(
        ctx: realtime_data_cdc_callback::Context,
        serial_cmd: SerialMessage,
    ) {
        let efi_status = ctx.shared.efi_status;
        let sensors = ctx.shared.sensors;
        let crc = ctx.shared.crc;

        (crc, efi_status, sensors).lock(|crc, efi_status, sensors| {
            handle_realtime_data::handler(crc, efi_status, sensors, serial_cmd);
        })
    }

    #[task(priority = 2,
    shared = [inj_pins, ign_pins, aux_pins, relay_pins, stepper_pins, timer13, flash])]
    async fn debug_demo(ctx: debug_demo::Context, demo_mode: u8) {
        let inj_pins = ctx.shared.inj_pins;
        let ign_pins = ctx.shared.ign_pins;
        let stepper_pins = ctx.shared.stepper_pins;
        let relay_pins = ctx.shared.relay_pins;
        let timer13 = ctx.shared.timer13;
        let flash = ctx.shared.flash;

        (inj_pins, ign_pins, stepper_pins, relay_pins, timer13, flash).lock(
            |inj_pins, ign_pins, stepper_pins, relay_pins, timer13, flash| {
                match demo_mode {
                    0x0 => {
                        host::debug!("Init spark_demo");
                        debug::spark_demo(ign_pins, timer13);
                        host::debug!("spark_demo done");
                    }

                    0x1 => debug::injector_demo(inj_pins, timer13),
                    0x2 => debug::external_idle_demo(stepper_pins, timer13),
                    0x3 => debug::relay_demo(relay_pins, timer13),
                    0x4 => {
                        host::debug!("Init clear flash");
                        let _ = flash.erase_all();
                        host::debug!("clear flash done");
                    }
                    0x5..=u8::MAX => debug::external_idle_demo(stepper_pins, timer13),
                }
            }
        )
    }

    #[task(priority = 8, local = [analog_pins, adc], shared = [sensors, efi_status])]
    async fn sensors_callback(mut ctx: sensors_callback::Context) {
        let mut sensors = ctx.shared.sensors;
        let mut engine_status = ctx.shared.efi_status;
        let adc_pins = ctx.local.analog_pins;
        let adc = ctx.local.adc;

        sensors.lock(|sensors| {
            sensors.update(
                get_sensor_raw(SensorTypes::AirTemp, adc_pins, adc),
                SensorTypes::AirTemp,
            );

            sensors.update(get_sensor_raw(SensorTypes::TPS, adc_pins, adc), SensorTypes::TPS);

            sensors.update(get_sensor_raw(SensorTypes::MAP, adc_pins, adc), SensorTypes::MAP);

            sensors.update(
                get_sensor_raw(SensorTypes::CooltanTemp, adc_pins, adc),
                SensorTypes::CooltanTemp,
            );

            sensors.update(
                get_sensor_raw(SensorTypes::BatteryVoltage, adc_pins, adc),
                SensorTypes::BatteryVoltage,
            );
        });

        (sensors, engine_status).lock(|sensors, engine_status| {
            engine_status.sensors = *sensors;
        });
    }

    #[task(shared = [led,ign_pins,timer3],priority = 1)]
    async fn ignition_trigger(mut ctx: ignition_trigger::Context) {
        let mut ecn = ctx.shared.ign_pins;
        let mut timer3 = ctx.shared.timer3;

        ctx.shared.led.lock(|l| { l.led_2.set_low() });
        (ecn, timer3).lock(|ecn, timer3| {
            ecn.ecn_1.set_high();
            ecn.ecn_2.set_high();
            ecn.ecn_3.set_high();
            ecn.ecn_4.set_high();
            timer3.start((1200).micros()).unwrap();
        });
    }


    // #[idle]
    // async fn idle(_: idle::Context) -> ! {
    //     loop {
    //         sensors_callback::spawn().unwrap();
    //         // motor_checks::spawn().unwrap();
    //         // cortex_m::asm::wfi();
    //         Systick::delay(50.millis()).await;
    //     }
    // }


    //TODO: reciclar para encendido
    #[task(binds = TIM2, priority = 1, local = [], shared = [timer, timer3, led])]
     fn timer2_exp(mut ctx: timer2_exp::Context) {
        ctx.shared.timer.lock(|tim| tim.clear_interrupt(Event::Update));
        ctx.shared.led.lock(|l| l.led_0.toggle());
    }

    #[task(binds = TIM3, local = [], shared = [timer3, led, tables, ign_pins,ignition_running])]
     fn timer3_exp(mut ctx: timer3_exp::Context) {
        ctx.shared.timer3.lock(|tim| {
            tim.clear_interrupt(Event::Update);
            tim.cancel().unwrap();
        });
        ctx.shared.ign_pins.lock(|ecn| {
            ecn.ecn_1.set_low();
            ecn.ecn_2.set_low();
            ecn.ecn_3.set_low();
            ecn.ecn_4.set_low();
        });
        ctx.shared.led.lock(|l| l.led_2.set_high());
        ctx.shared.ignition_running.lock(|ignition_running| { *ignition_running = false });
    }

    #[task(binds = OTG_FS, local = [usb_dev, cdc_input_buffer/*,cdc_output*/], shared = [usb_cdc, usb_web])]
    fn usb_handler(mut ctx: usb_handler::Context) {
        let device = ctx.local.usb_dev;
       // let mut cdc_output = ctx.local.cdc_output;

        ctx.shared.usb_cdc.lock(|cdc| {
            // USB dev poll only in the interrupt handler
            (ctx.shared.usb_web, ).lock(|web| {
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
                                     //   cdc_output
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
        #[task(shared = [usb_cdc], priority = 2)]
        async fn send_message(
            ctx: send_message::Context,
            status: SerialStatus,
            code: u8,
            message: SerialMessage,
        );

        // from: https://github.com/noisymime/speeduino/blob/master/speeduino/decoders.ino#L453
        // #[task(binds = EXTI9_5,
        //     local = [ckp],
        //     shared = [led, efi_status, flash_info, efi_cfg, timer, timer3, timer4, ckp, ign_pins]
        // )]
        // fn ckp_trigger(ctx: ckp_trigger::Context);
        //
        // #[task(
        //     shared = [led, efi_status, efi_cfg, timer3, timer4, ckp, ign_pins],
        //     priority = 1
        // )]
        // async fn ignition_schedule(ctx: ignition_schedule::Context);

        // #[task(shared = [efi_cfg, ckp, timer4, efi_status,ignition_running], priority = 1)]
        // async fn motor_checks(ctx: motor_checks::Context);

    }

    #[task(shared = [usb_cdc], priority = 2)]
    async fn webserial_sender(mut ctx:webserial_sender::Context, mut receiver: Receiver<'static, SerialMessage, CDC_CAPACITY>){
        while let Ok(message) = receiver.recv().await {
            ctx.shared.usb_cdc.lock(|cdc| {
                cdc.write(&finish_message(message)).unwrap();
            });
        }
    }

//     #[idle]
//     fn idle(_cx: idle::Context) -> ! {
//         loop {}
//     }
}