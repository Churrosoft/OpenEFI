//! examples/locals.rs
#![feature(proc_macro_hygiene)]
#![no_main]
#![no_std]
#![feature(stdsimd)]

use panic_halt as _;

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM5, TIM7, TIM8_CC])]
mod app {
    use core::arch::arm::__breakpoint;

    use arrayvec::ArrayVec;
    use embedded_hal::spi::{Mode, Phase, Polarity};
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
    use w25q::series25::FlashInfo;

    use logging::host;

    use crate::app::{
        engine::{
            efi_cfg::{EngineConfig, get_default_efi_cfg},
            engine_status::{EngineStatus, get_default_engine_status},
            sensors::{get_sensor_raw, SensorTypes, SensorValues},
        },
        gpio_legacy::{
            ADCMapping, AuxIoMapping, IgnitionGpioMapping, init_gpio, InjectionGpioMapping,
            RelayMapping, StepperMapping,
        },
        injection::{calculate_time_isr, injection_setup},
        memory::tables::{SpiT, Tables},
        webserial::{handle_tables, send_message, SerialMessage, SerialStatus},
    };
    use crate::app::engine::pmic;
    use crate::app::engine::pmic::PMIC;

    pub mod debug;
    pub mod engine;
    pub mod gpio;
    pub mod gpio_legacy;
    pub mod injection;
    pub mod logging;
    pub mod memory;
    pub mod util;
    pub mod webserial;

    #[shared]
    struct Shared {
        // Timers:
        timer5: timer::CounterUs<TIM5>,
        timer13: timer::DelayUs<TIM13>,
        timer: timer::CounterMs<TIM2>,
        timer3: timer::CounterUs<TIM3>,

        // Core I/O
        led: gpio_legacy::LedGpioMapping,
        inj_pins: InjectionGpioMapping,
        ign_pins: IgnitionGpioMapping,
        aux_pins: AuxIoMapping,
        relay_pins: RelayMapping,
        stepper_pins: StepperMapping,

        // USB:
        usb_cdc: SerialPort<'static, UsbBusType, [u8; 128], [u8; 4000]>,
        usb_web: WebUsb<UsbBusType>,

        // core:
        string: serde_json_core::heapless::String<1000>,
        str_lock: bool,
        // el implement de "shared_bus_resources" anda para el culo;
        // asi que hago el lock a mano
        spi_lock: bool,
        crc: Crc32,

        // EFI Related:
        efi_cfg: EngineConfig,
        efi_status: EngineStatus,
        flash: memory::tables::FlashT,
        flash_info: FlashInfo,
        tables: Tables,
        sensors: SensorValues,
    }

    #[local]
    struct Local {
        usb_dev: UsbDevice<'static, UsbBusType>,
        cdc_input_buffer: ArrayVec<u8, 128>,

        // EFI Related:
        ckp: stm32f4xx_hal::gpio::PC6<Input>,
        adc: Adc<ADC1>,
        analog_pins: ADCMapping,
    }


    #[init(local = [USB_BUS: Option < UsbBusAllocator < UsbBusType >> = None])]
    fn init(mut ctx: init::Context) -> (Shared, Local, init::Monotonics) {
        let mut dp = ctx.device;

        ctx.core.DWT.enable_cycle_counter();
        // TODO: Disable this in release builds
        host::debug!("Hello :)");

        let gpio_a = dp.GPIOA.split();
        let gpio_b = dp.GPIOB.split();
        let gpio_c = dp.GPIOC.split();
        let gpio_d = dp.GPIOD.split();
        let gpio_e = dp.GPIOE.split();

        let mut gpio_config = init_gpio(gpio_a, gpio_b, gpio_c, gpio_d, gpio_e);

        // ADC
        let mut adc = Adc::adc1(dp.ADC1, true, AdcConfig::default());

        // configure CKP/CMP Pin for Interrupts
        let mut ckp = gpio_config.ckp;
        let mut syscfg = dp.SYSCFG.constrain();

        ckp.make_interrupt_source(&mut syscfg);
        ckp.trigger_on_edge(&mut dp.EXTI, Edge::Rising);
        ckp.enable_interrupt(&mut dp.EXTI);

        // Configure and obtain handle for delay abstraction
        let rcc = dp.RCC.constrain();
        let clocks = rcc
            .cfgr
            .use_hse((25).MHz())
            .sysclk((120).MHz())
            .require_pll48clk()
            .freeze();

        let mut timer: timer::CounterMs<TIM2> = dp.TIM2.counter_ms(&clocks);
        let mut timer3: timer::CounterUs<TIM3> = dp.TIM3.counter_us(&clocks);

        // NOTE: timer para delays en hilos
        let mut timer5: timer::CounterUs<TIM5> = dp.TIM5.counter_us(&clocks);

        // NOTE: para task de sensores
        let mut timer13: timer::DelayUs<TIM13> = dp.TIM13.delay_us(&clocks);

        //  TIM5, TIM7, TIM4

        timer.start((150).millis()).unwrap();

        // TODO: revisar cual es el mejor periodo
        timer5.start((150).millis()).unwrap();

        // Set up to generate interrupt when timer expires
        timer.listen(Event::Update);
        timer3.listen(Event::Update);
        timer13.listen(Event::Update);

        // Init USB
        let usb = USB {
            usb_global: dp.OTG_FS_GLOBAL,
            usb_device: dp.OTG_FS_DEVICE,
            usb_pwrclk: dp.OTG_FS_PWRCLK,
            pin_dm: gpio_config.usb_dp,
            pin_dp: gpio_config.usb_dm,
            hclk: clocks.hclk(),
        };

        static mut EP_MEMORY: [u32; 1024] = [0; 1024];
        static mut __USB_TX: [u8; 4000] = [0; 4000];
        static mut __USB_RX: [u8; 128] = [0; 128];

        let usb_bus = ctx.local.USB_BUS;
        unsafe {
            *usb_bus = Some(otg_fs::UsbBus::new(usb, &mut EP_MEMORY));
        }

        let usb_cdc =
            unsafe { SerialPort::new_with_store(usb_bus.as_ref().unwrap(), __USB_RX, __USB_TX) };
        let usb_web = WebUsb::new(
            usb_bus.as_ref().unwrap(),
            url_scheme::HTTPS,
            "tuner.openefi.tech",
        );

        let usb_dev = webserial::new_device(usb_bus.as_ref().unwrap());

        let cdc_buff = ArrayVec::<u8, 128>::new();

        // EFI Related:
        let mut _efi_cfg = get_default_efi_cfg();
        let mut _efi_status = get_default_engine_status();

        // SPI:

        let mode = Mode {
            polarity: Polarity::IdleLow,
            phase: Phase::CaptureOnFirstTransition,
        };

        let spi2 = Spi::new(
            dp.SPI2,
            (
                gpio_config.spi_sck,
                gpio_config.spi_miso,
                gpio_config.spi_mosi,
            ),
            mode,
            (3).MHz(),
            &clocks,
        );

        let spi_bus = shared_bus_rtic::new!(spi2, SpiT );
        let mut spi_pmic = spi_bus.acquire();

        // CRC32:
        let mut crc = crc32::Crc32::new(dp.CRC);

        let mut flash = w25q::series25::Flash::init(spi_bus.acquire(), gpio_config.memory_cs).unwrap();

        let id = flash.read_jedec_id().unwrap();

        let flash_info = flash.get_device_info().unwrap();

        host::debug!("FLASH: {:?}", id);
        // logging::host::debug!("FLASH: Size {:?}", flash_info.capacity_kb);
        // logging::host::debug!("FLASH: Block Count {:?}", flash_info.block_count);
        // logging::host::debug!("FLASH: Page Count {:?}", flash_info.page_count);

        // EFI Setup:
        let mut table = Tables {
            tps_rpm_ve: None,
            injector_delay: None,
        };

        injection_setup(&mut table, &mut flash, &flash_info, &mut crc);

        logging::host::debug!("table rpm 2/2: {:?}", table.tps_rpm_ve.unwrap()[2][2]);

        // REMOVE: solo lo estoy hardcodeando aca para probar el AlphaN
        _efi_status.rpm = 1500;

        calculate_time_isr(&mut _efi_status, &_efi_cfg);

        logging::host::debug!("AF {:?}", _efi_status.injection.air_flow);

        // NOTE: con crear el string estaria, no hace falta parsear el objecto de config
        let mut serialized: serde_json_core::heapless::String<1000> =
            serde_json_core::to_string(&_efi_cfg).unwrap();

        let mut str_lock = false; // NOTE: sesuponeque rtic hace todo el laburo de los locks asi que esto quedaria al pedo

        gpio_config.led.led_check.toggle();
        gpio_config.led.led_mil.toggle();
        debug::spark_demo(&mut gpio_config.ignition, &mut timer13);
        debug::injector_demo(&mut gpio_config.injection, &mut timer13);
        //  debug::injector_demo(&mut gpio_config.injection, &mut timer13);
        // loop {
        //     debug::spark_demo(&mut gpio_config.ignition, &mut timer13);
        // }
        // debug::injector_demo(&mut gpio_config.injection, &mut timer13);

        let sensors = SensorValues::new();

        let spi_lock = false;


        let mut spi_result = [0x0, 0x0];

        let spi_status = [0b0000_1111, 0b0000_0000]; // SPI status
        let update_mode = [0b0001_1111, 0b0000_0000];// cambio de modo ignicion => generico

        let read_all = [0b0000_1010, 0b0000_0000];// all status register
        let read_out_fault = [0b0000_1010, 0b0001_0000]; // OUT0/OUT1 fault

        let read_ignition_status = [0b0000_1010, 0b0100_0000]; // OUT0/OUT1 fault


        let mut mock_word = [0xf, 0x0];
        // let mut read_ignition3 = [0b00110000,0b00000000];
        // let mut read_ignition4 = [0b00110001,0b00000000];
        // let mut read_ignition5 = [0b00110001,0b00000000];

        // read all status register
        // gpio_config.pmic.pmic_cs.set_low();
        //
        // spi_pmic.write(&read_all).unwrap();
        //
        // let status = spi_pmic.transfer(&mut mock_word).unwrap();
        // gpio_config.pmic.pmic_cs.set_high();
        //
        // host::debug!("SPI Check: 0b{:08b} / 0b{:08b}", status[0], status[1]);
        unsafe { __breakpoint::<0>(); }

        // discard prev
        gpio_config.pmic.pmic_cs.set_low();
        spi_pmic.transfer(&mut mock_word).unwrap();
        gpio_config.pmic.pmic_cs.set_high();
        timer13.delay_ms(200u32);

        unsafe { __breakpoint::<0>(); }

        debug::spark_demo(&mut gpio_config.ignition, &mut timer13);


        // read out0/1 register
        gpio_config.pmic.pmic_cs.set_low();
        timer13.delay_us(10u32);
        spi_pmic.write(&read_out_fault).unwrap();
        gpio_config.pmic.pmic_cs.set_high();
        timer13.delay_us(10u32);
        gpio_config.pmic.pmic_cs.set_low();
        let status2 = spi_pmic.transfer(&mut mock_word).unwrap();
        gpio_config.pmic.pmic_cs.set_high();
        timer13.delay_us(100u32);
        host::debug!("SPI out0: 0b{:08b} /  0b{:08b}", status2[0], status2[1]);

        // re-read (status)
        gpio_config.pmic.pmic_cs.set_low();
        timer13.delay_us(10u32);
        spi_pmic.write(&read_all).unwrap();
        gpio_config.pmic.pmic_cs.set_high();
        timer13.delay_us(10u32);
        gpio_config.pmic.pmic_cs.set_low();
        let status2 = spi_pmic.transfer(&mut mock_word).unwrap();
        gpio_config.pmic.pmic_cs.set_high();
        timer13.delay_us(100u32);
        host::debug!("SPI status: 0b{:08b} /  0b{:08b}", status2[0], status2[1]);

        // read ignition status
        gpio_config.pmic.pmic_cs.set_low();
        timer13.delay_us(10u32);
        spi_pmic.write(&read_ignition_status).unwrap();
        gpio_config.pmic.pmic_cs.set_high();
        timer13.delay_us(10u32);
        gpio_config.pmic.pmic_cs.set_low();
        let status2 = spi_pmic.transfer(&mut mock_word).unwrap();
        gpio_config.pmic.pmic_cs.set_high();
        timer13.delay_us(100u32);

        host::debug!("SPI IGN: 0b{:08b} /  0b{:08b}", status2[0], status2[1]);

        let mut my_pmic = PMIC::init(spi_pmic, gpio_config.pmic.pmic_cs).unwrap();

        my_pmic.get_fast_status();

        // gpio_config.pmic.pmic_cs.set_low();
        // let spi2_result = spi_pmic.transfer(&mut read_ignition2).unwrap();
        // gpio_config.pmic.pmic_cs.set_high();
        // host::debug!("SPI Write iny high: 0b{:08b} / 0b{:08b}", spi2_result[0],spi2_result[1]);
        //
        //
        // timer13.delay_ms(10u32);
        //
        // gpio_config.pmic.pmic_cs.set_low();
        // let spi3_result = spi_pmic.transfer(&mut read_ignition3).unwrap();
        // gpio_config.pmic.pmic_cs.set_high();
        // host::debug!("SPI Write iny low: 0b{:08b} / 0b{:08b}", spi3_result[0],spi3_result[1]);
        //
        // timer13.delay_ms(200u32);

        // gpio_config.pmic.pmic_cs.set_low();
        // spi_pmic.transfer(&mut read_ignition4).unwrap();
        // let spi4_result = spi_pmic.transfer(&mut read_ignition5).unwrap();
        // gpio_config.pmic.pmic_cs.set_high();
        // host::debug!("SPI INY 0/1 status : 0b{:08b} / 0b{:08b}", spi4_result[0],spi4_result[1]);

        return (
            // Initialization of shared resources
            Shared {
                // Timers:
                timer,
                timer3,
                timer5,
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
                string: serialized,
                str_lock,
                crc,
                flash,
                flash_info,
                spi_lock,

                // EFI Related
                efi_cfg: _efi_cfg,
                efi_status: _efi_status,
                tables: table,
            },
            // Initialization of task local resources
            Local {
                ckp,
                usb_dev,
                cdc_input_buffer: cdc_buff,
                adc,
                analog_pins: gpio_config.adc,
            },
            // Move the monotonic timer to the RTIC run-time, this enables
            // scheduling
            init::Monotonics(),
        );
    }

    #[idle]
    fn idle(_: idle::Context) -> ! {
        loop {
            cortex_m::asm::wfi();
        }
    }

    //TODO: reciclar para encendido
    #[task(binds = TIM2, priority = 1, local = [], shared = [timer, timer3, led])]
    fn timer2_exp(mut ctx: timer2_exp::Context) {
        ctx.shared
            .timer
            .lock(|tim| tim.clear_interrupt(Event::Update));

        ctx.shared.led.lock(|l| l.led_0.toggle());
    }

    #[task(binds = TIM3, local = [], shared = [timer3, led, tables])]
    fn timer3_exp(mut ctx: timer3_exp::Context) {
        ctx.shared.timer3.lock(|tim| {
            tim.clear_interrupt(Event::Update);
            tim.cancel().unwrap();
        });

        ctx.shared.led.lock(|l| l.led_2.set_high());
    }

    // EXTI9_5_IRQn para los pines ckp/cmp
    #[task(
    binds = EXTI9_5,
    local = [ckp],
    shared = [led, efi_status, flash_info, efi_cfg, timer, timer3]
    )]
    fn ckp_trigger(mut ctx: ckp_trigger::Context) {
        ctx.shared.efi_status.lock(|es| {
            es.cycle_tick += 1;
        });

        let efi_cfg = ctx.shared.efi_cfg;
        let efi_status = ctx.shared.efi_status;

        // calculo de RPM && led
        (efi_cfg, efi_status, ctx.shared.led, ctx.shared.timer3).lock(
            |efi_cfg, efi_status, led, timer3| {
                if efi_status.cycle_tick
                    >= efi_cfg.engine.ckp_tooth_count - efi_cfg.engine.ckp_missing_tooth
                {
                    led.led_2.set_low();
                    // FIXME: por ahora solo prendo el led una vez por vuelta, luego lo hago funcionar con el primer cilindro
                    timer3.start((50000).micros()).unwrap();
                    efi_status.cycle_tick = 0;
                }
            },
        );

        cpwm_callback::spawn().unwrap();

        // Obtain access to the peripheral and Clear Interrupt Pending Flag
        ctx.local.ckp.clear_interrupt_pending_bit();
    }

    #[task(binds = OTG_FS, local = [usb_dev, cdc_input_buffer], shared = [usb_cdc, usb_web])]
    fn usb_handler(mut ctx: usb_handler::Context) {
        let device = ctx.local.usb_dev;

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
        // Low-priority task to send back replies via the serial port.
        #[task(shared = [usb_cdc], priority = 2, capacity = 30)]
        fn send_message(
            ctx: send_message::Context,
            status: SerialStatus,
            code: u8,
            mut message: SerialMessage,
        );
    }

    #[task(priority = 2, shared = [flash_info, efi_cfg, tables, crc, flash, spi_lock])]
    fn table_cdc_callback(ctx: table_cdc_callback::Context, serial_cmd: SerialMessage) {
        let flash = ctx.shared.flash;
        let flash_info = ctx.shared.flash_info;
        let tables = ctx.shared.tables;
        let crc = ctx.shared.crc;
        let spi_lock = ctx.shared.spi_lock;

        (flash, flash_info, tables, crc, spi_lock).lock(|flash, flash_info, tables, crc, spi_lock| {
            *spi_lock = true;
            tables.tps_rpm_ve.as_mut().unwrap()[0][0] = 40;
            handle_tables::handler(serial_cmd, flash, flash_info, tables, crc);
            *spi_lock = false;
        });
    }

    #[task(priority = 2, shared = [inj_pins, ign_pins, aux_pins, relay_pins, stepper_pins, timer13])]
    fn debug_demo(ctx: debug_demo::Context, demo_mode: u8) {
        let inj_pins = ctx.shared.inj_pins;
        let ign_pins = ctx.shared.ign_pins;
        let stepper_pins = ctx.shared.stepper_pins;
        let relay_pins = ctx.shared.relay_pins;
        let timer13 = ctx.shared.timer13;

        (inj_pins, ign_pins, stepper_pins, relay_pins, timer13).lock(
            |inj_pins, ign_pins, stepper_pins, relay_pins, timer13| match demo_mode {
                0x0 => debug::spark_demo(ign_pins, timer13),
                0x1 => debug::injector_demo(inj_pins, timer13),
                0x2 => debug::external_idle_demo(stepper_pins, timer13),
                0x3 => debug::relay_demo(relay_pins, timer13),
                0x4..=u8::MAX => debug::external_idle_demo(stepper_pins, timer13),
            },
        )
    }

    #[task(binds = TIM6_DAC, priority = 5, local = [analog_pins, adc], shared = [sensors])]
    fn sensors_callback(ctx: sensors_callback::Context) {
        let mut sensors = ctx.shared.sensors;
        let adc_pins = ctx.local.analog_pins;
        let adc = ctx.local.adc;

        sensors.lock(|sensors| {
            sensors.update(
                get_sensor_raw(SensorTypes::AirTemp, adc_pins, adc),
                SensorTypes::AirTemp,
            );

            sensors.update(
                get_sensor_raw(SensorTypes::TPS, adc_pins, adc),
                SensorTypes::TPS,
            );

            sensors.update(
                get_sensor_raw(SensorTypes::MAP, adc_pins, adc),
                SensorTypes::MAP,
            );

            sensors.update(
                get_sensor_raw(SensorTypes::CooltanTemp, adc_pins, adc),
                SensorTypes::CooltanTemp,
            );
        })
    }

    // prioridad? si; task para manejar el pwm de los inyectores; exportar luego a cpwm.rs
    #[task(priority = 10, shared = [efi_status, flash_info, efi_cfg, timer, timer3])]
    fn cpwm_callback(mut _ctx: cpwm_callback::Context) {
        // TODO: cpwm if;
    }
}
