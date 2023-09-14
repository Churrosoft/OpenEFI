#![feature(exclusive_range_pattern)]
#![feature(proc_macro_hygiene)]
#![feature(int_roundings)]
#![feature(is_some_and)]
#![feature(stdsimd)]

#![no_main]
#![no_std]

#![allow(stable_features)]
#![allow(unused_mut)]

use panic_halt as _;

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM4, TIM7, TIM8_CC])]
mod app {
    use arrayvec::ArrayVec;
    use embedded_hal::spi::{Mode, Phase, Polarity};
    use rtic::Mutex;
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
            ADCMapping,
            AuxIoMapping,
            IgnitionGpioMapping,
            init_gpio,
            InjectionGpioMapping,
            RelayMapping,
            StepperMapping,
        },
        injection::{calculate_time_isr, injection_setup},
        memory::tables::{SpiT, Tables},
        webserial::{handle_tables, send_message, SerialMessage, SerialStatus},
    };
    use crate::app::engine::efi_cfg::VRSensor;
    use crate::app::engine::cpwm::{angle_to_time, get_crank_angle, get_cranking_rpm, VRStatus};
    use crate::app::engine::pmic::{PMIC, PmicT};
    use crate::app::webserial::{handle_engine, handle_pmic, handle_realtime_data};
    use systick_monotonic::{Systick};
    use fugit::{Duration, ExtU32};

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
        ckp_tooth_last_time: u32,
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

        start_revolution: u128,
        ckp_ticks: [u32; 128],
        ckp_index: usize,
        ckp_last_tick: u32,
        ckp_avg_time: u32,

        // TODO: todo esto tendria que ir en EngineStatus
        // NEWWW
        ckp_current_time: u32,
        ckp_current_gap: u32,
        ckp_target_gap: u32,
        ckp_is_missing_tooth: bool,
        ckp_tooth_last_minus_one_tooth_time: u32,
        ckp_tooth_last_time: u32,
        // esta se comparte con el loop para detectar stall
        ckp_tooth_current_count: u32,
        ckp_tooth_one_time: u32,
        ckp_tooth_one_minus_one_time: u32,
        ckp_has_sync: bool,
        ckp_sync_loss_counter: u128,
    }

    #[monotonic(binds = SysTick, default = true)]
    type MyMono = Systick<100_000>; // 100_000 Hz / 0.01 ms granularity

    #[init(local = [USB_BUS: Option < UsbBusAllocator < UsbBusType >> = None])]
    fn init(mut ctx: init::Context) -> (Shared, Local, init::Monotonics) {
        let mut dp = ctx.device;
        let systick = ctx.core.SYST;

        ctx.core.DWT.enable_cycle_counter();

        // TODO: Disable this in release builds
        host::debug!("Hello v1 :)");

        let gpio_a = dp.GPIOA.split();
        let gpio_b = dp.GPIOB.split();
        let gpio_c = dp.GPIOC.split();
        let gpio_d = dp.GPIOD.split();
        let gpio_e = dp.GPIOE.split();

        let mut gpio_config = init_gpio(gpio_a, gpio_b, gpio_c, gpio_d, gpio_e);

        // ADC
        let mut adc = Adc::adc1(dp.ADC1, true, AdcConfig::default());

        adc.enable();
        adc.calibrate();

        // configure CKP/CMP Pin for Interrupts
        let mut ckp = gpio_config.ckp;
        let mut syscfg = dp.SYSCFG.constrain();
        host::debug!("init gpio");
        ckp.make_interrupt_source(&mut syscfg);
        ckp.trigger_on_edge(&mut dp.EXTI, Edge::Falling);

        // Configure and obtain handle for delay abstraction
        let rcc = dp.RCC.constrain();
        let clocks = rcc.cfgr.use_hse((25).MHz()).sysclk((120).MHz()).require_pll48clk().freeze();

        // Initialize the monotonic
        let mono = Systick::new(systick, 120_000_000);

        // NOTE: para delays debugs/etc
        // let mut delay = cortex_m::delay::Delay::new(systick, 120000000);

        // timer Tiempo inyeccion
        let mut timer: timer::CounterUs<TIM2> = dp.TIM2.counter_us(&clocks);
        // timer tiempo de ignicion
        let mut timer3: timer::CounterUs<TIM3> = dp.TIM3.counter_us(&clocks);
        // timer CPWM
        let mut timer4: timer::CounterUs<TIM5> = dp.TIM5.counter_us(&clocks);

        // timer uso generico
        let mut timer13: timer::DelayUs<TIM13> = dp.TIM13.delay_us(&clocks);

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
        host::debug!("init usb");
        let usb_bus = ctx.local.USB_BUS;
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
            dp.SPI2,
            (gpio_config.spi_sck, gpio_config.spi_miso, gpio_config.spi_mosi),
            mode,
            (30).MHz(),
            &clocks,
        );

        let spi_bus = shared_bus_rtic::new!(spi2, SpiT);
        let spi_pmic = spi_bus.acquire();

        // CRC32:
        let mut crc = crc32::Crc32::new(dp.CRC);
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


        // RPM & sinc calc:

        let mut start_revolution = 0;

        let mut ckp_ticks: [u32; 128] = [0; 128];
        let mut ckp_index = 0;


        let tick_init = timer4.now().ticks();
        timer13.delay_us(1000u16);
        let tick_end = timer4.now().ticks();
        host::debug!("Time {:?} mS, Init: {:?} , End {:?}", tick_end-tick_init,tick_init,tick_end);


        let mut ckp_last_tick = 0;
        let mut ckp_avg_time = 0;

        // new ckp sinc vars:

        let mut ckp_current_time = 0;
        let mut ckp_current_gap = 0;
        let mut ckp_target_gap = 0;
        let mut ckp_is_missing_tooth = false;
        let mut ckp_tooth_last_time = 0;
        let mut ckp_tooth_last_time_2 = 0;
        let mut ckp_tooth_one_time = 0;
        let mut ckp_tooth_last_minus_one_tooth_time = 0;
        let mut ckp_tooth_current_count = 0;
        let mut ckp_tooth_one_minus_one_time = 0; // creo que se usa para las rpm
        let mut ckp_has_sync = false;
        let mut ckp_sync_loss_counter = 0;

        let mut ignition_running = false;

        ckp.enable_interrupt(&mut dp.EXTI);

        let mut ckp_status = VRStatus::new();

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
                ckp_tooth_last_time: ckp_tooth_last_time_2,
                ckp: ckp_status,

                ignition_running,
            },
            // Initialization of task local resources
            Local {
                ckp,
                usb_dev,
                cdc_input_buffer: cdc_buff,
                adc,
                analog_pins: gpio_config.adc,
                start_revolution,
                ckp_ticks,
                ckp_index,
                ckp_last_tick,
                ckp_avg_time,

                //new rpm
                ckp_current_time,
                ckp_current_gap,
                ckp_target_gap,
                ckp_is_missing_tooth,
                ckp_tooth_last_time,
                ckp_tooth_one_time,
                ckp_tooth_last_minus_one_tooth_time,
                ckp_tooth_current_count,
                ckp_has_sync,
                ckp_sync_loss_counter,
                ckp_tooth_one_minus_one_time,
            },
            // Move the monotonic timer to the RTIC run-time, this enables
            // scheduling
            init::Monotonics(mono),
        );
    }

    #[idle]
    fn idle(_: idle::Context) -> ! {
        loop {
            sensors_callback::spawn().unwrap();
            motor_checks::spawn().unwrap();
            // cortex_m::asm::wfi();
        }
    }


    //TODO: reciclar para encendido
    #[task(binds = TIM2, priority = 1, local = [], shared = [timer, timer3, led])]
    fn timer2_exp(mut ctx: timer2_exp::Context) {
        ctx.shared.timer.lock(|tim| tim.clear_interrupt(Event::Update));
        ctx.shared.led.lock(|l| l.led_0.toggle());
    }

    //
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

    // from: https://github.com/noisymime/speeduino/blob/master/speeduino/decoders.ino#L453
    #[task(binds = EXTI9_5,
    local = [ckp],
    shared = [led, efi_status, flash_info, efi_cfg, timer, timer3, timer4, ckp, ign_pins])]
    fn ckp_trigger(mut ctx: ckp_trigger::Context) {
        let mut efi_cfg = ctx.shared.efi_cfg;
        let mut efi_status = ctx.shared.efi_status;

        let mut ckp_status = ctx.shared.ckp;

        let mut ckp = VRSensor::new();
        let mut rpm = 0;

        let mut ecn = ctx.shared.ign_pins;
        let mut timer3 = ctx.shared.timer3;

        // lock previo y libero los recursos que no se vuelven a usar para otra cosa aca adentro
        efi_cfg.lock(|cfg| { ckp = cfg.engine.ckp });
        efi_status.lock(|status| { rpm = status.rpm });

        ckp_status.lock(|ckp_status| {
            ctx.shared.timer4.lock(|t4| { ckp_status.current_time = t4.now().ticks(); });
            ckp_status.current_gap = ckp_status.current_time - ckp_status.tooth_last_time;

            if ckp_status.current_gap >= ckp.trigger_filter_time {
                ckp_status.tooth_current_count += 1;


                if ckp_status.tooth_last_time > 0 && ckp_status.tooth_last_minus_one_tooth_time > 0 {
                    ckp_status.is_missing_tooth = false;

                    /*
                      Performance Optimisation:
                      Only need to try and detect the missing tooth if:
                      1. WE don't have sync yet
                      2. We have sync and are in the final 1/4 of the wheel (Missing tooth will/should never occur in the first 3/4)
                      3. RPM is under 2000. This is to ensure that we don't interfere with strange timing when cranking or idling. Optimisation not really required at these speeds anyway
                    */
                    if ckp_status.has_sync == false || rpm < 2000 || ckp_status.tooth_current_count >= (3 * ckp.trigger_actual_teeth >> 2) {
                        //Begin the missing tooth detection
                        //If the time between the current tooth and the last is greater than 1.5x the time between the last tooth and the tooth before that, we make the assertion that we must be at the first tooth after the gap
                        if ckp.missing_tooth == 1 {
                            //Multiply by 1.5 (Checks for a gap 1.5x greater than the last one) (Uses bitshift to multiply by 3 then divide by 2. Much faster than multiplying by 1.5)
                            ckp_status.target_gap = (3 * (ckp_status.tooth_last_time - ckp_status.tooth_last_minus_one_tooth_time)) >> 1;
                        } else {
                            //Multiply by 2 (Checks for a gap 2x greater than the last one)
                            ckp_status.target_gap = ((ckp_status.tooth_last_time - ckp_status.tooth_last_minus_one_tooth_time)) * ckp.missing_tooth;
                        }

                        // initial startup, missing one time
                        if ckp_status.tooth_last_time == 0 || ckp_status.tooth_last_minus_one_tooth_time == 0 {
                            ckp_status.target_gap = 0;
                        }

                        if (ckp_status.current_gap > ckp_status.target_gap) || (ckp_status.tooth_current_count > ckp.trigger_actual_teeth) {
                            //Missing tooth detected
                            ckp_status.is_missing_tooth = true;

                            // FIXME: remove
                            if rpm > 30 {}

                            // ctx.shared.led.lock(|l| { l.led_2.set_low() });
                            // (ecn, timer3).lock(|ecn, timer3| {
                            //     ecn.ecn_1.set_high();
                            //     ecn.ecn_2.set_high();
                            //     ecn.ecn_3.set_high();
                            //     ecn.ecn_4.set_high();
                            //     timer3.start((1200).micros()).unwrap();
                            // });
                            //ctx.shared.timer3.lock(|timer3| { });

                            if ckp_status.tooth_current_count < ckp.trigger_actual_teeth {
                                // This occurs when we're at tooth #1, but haven't seen all the other teeth. This indicates a signal issue so we flag lost sync so this will attempt to resync on the next revolution.
                                ckp_status.has_sync = false;
                                ckp_status.sync_loss_counter += 1;
                            }
                            //This is to handle a special case on startup where sync can be obtained and the system immediately thinks the revs have jumped:
                            else {
                                if ckp_status.has_sync {
                                    ctx.shared.led.lock(|l| { l.led_check.toggle() });
                                    ckp_status.start_revolution += 1;
                                } else {
                                    ckp_status.start_revolution = 0;
                                    ctx.shared.led.lock(|l| { l.led_mil.toggle() });
                                }

                                ckp_status.tooth_current_count = 1;

                                // tiempo entre vuelta completa
                                ckp_status.tooth_one_minus_one_time = ckp_status.tooth_one_time;
                                ckp_status.tooth_one_time = ckp_status.current_time;

                                // TODO: hay mas checks aca cuando es con inyección secuencial
                                ckp_status.has_sync = true;
                                //This is used to prevent a condition where serious intermittent signals (Eg someone furiously plugging the sensor wire in and out) can leave the filter in an unrecoverable state
                                efi_cfg.lock(|ec| { ec.engine.ckp.trigger_filter_time = 0; });
                                ckp_status.tooth_last_minus_one_tooth_time = ckp_status.tooth_last_time;
                                ckp_status.tooth_last_time = ckp_status.current_time;
                               // ctx.shared.ckp_tooth_last_time.lock(|ckp_t| { *ckp_t = ckp_status.current_time });
                            }
                        }
                    }

                    if !ckp_status.is_missing_tooth {
                        efi_cfg.lock(|ec| { ec.engine.ckp.trigger_filter_time = ckp_status.current_gap >> 2; });
                        ckp_status.tooth_last_minus_one_tooth_time = 0;
                    }
                } else {
                    // initial startup
                    ckp_status.tooth_last_minus_one_tooth_time = ckp_status.tooth_last_time;
                    ckp_status.tooth_last_time = ckp_status.current_time;
                  //  ctx.shared.ckp_tooth_last_time.lock(|ckp_t| { *ckp_t = ckp_status.current_time });
                }
            }
        });
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
                                        ctx.local.cdc_input_buffer.take().into_inner().unwrap()
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
            handle_tables::handler(serial_cmd, flash, flash_info, tables, crc);
            *spi_lock = false;
        });
    }

    #[task(priority = 2, shared = [spi_lock, pmic])]
    fn pmic_cdc_callback(ctx: pmic_cdc_callback::Context, serial_cmd: SerialMessage) {
        let spi_lock = ctx.shared.spi_lock;
        let pmic = ctx.shared.pmic;

        (spi_lock, pmic).lock(|spi_lock, pmic| {
            *spi_lock = true;
            handle_pmic::handler(serial_cmd, pmic);
            *spi_lock = false;
        })
    }

    #[task(priority = 2, shared = [spi_lock, flash, flash_info, efi_cfg, crc])]
    fn engine_cdc_callback(ctx: engine_cdc_callback::Context, serial_cmd: SerialMessage) {
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
    fn realtime_data_cdc_callback(
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
    fn debug_demo(ctx: debug_demo::Context, demo_mode: u8) {
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
    fn sensors_callback(mut ctx: sensors_callback::Context) {
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

    // TODO: add similar stall control of speeduino
    // https://github.com/noisymime/speeduino/blob/master/speeduino/speeduino.ino#L146
    #[task(shared = [efi_cfg, ckp, timer4, efi_status,ignition_running])]
    fn motor_checks(mut ctx: motor_checks::Context) {
        let mut efi_cfg = ctx.shared.efi_cfg;
        let mut ckp = ctx.shared.ckp;
        let mut efi_status = ctx.shared.efi_status;
        let mut cycle_time = 0;

        let mut ignition_running = ctx.shared.ignition_running;

        ctx.shared.timer4.lock(|t4| { cycle_time = t4.now().ticks(); });

        (efi_cfg, ckp, efi_status,ignition_running).lock(|cfg, ckp, efi_status,ignition_running| {
            //ste hijodeputa fue por lo que se tosto la bobina
            if  ckp.tooth_last_time > cycle_time || cycle_time - ckp.tooth_last_time < 366_667 /* 50RPM */  {
                // RPM & no stall
                // en speeduino revisan "BIT_DECODER_TOOTH_ANG_CORRECT" aca, por ahora no lo agregue al trigger
                // tambien utilizan rpmDOT para ver la variacion cada 100mS, falta implementar
                // TODO: mover a fun aparte

                let mut timePerDegreex16;

                // esto calcula el tiempo por grado desde el tiempo entre los ultimos 2 dientes
                if true /* BIT_DECODER_TOOTH_ANG_CORRECT*/ && (ckp.tooth_last_time > ckp.tooth_last_minus_one_tooth_time) && 20/*(abs(currentStatus.rpmDOT)*/ > 30 {
                    timePerDegreex16 = ((ckp.tooth_last_time - ckp.tooth_last_minus_one_tooth_time) * 16) / cfg.engine.ckp.trigger_tooth_angle as u32;
                    // timePerDegree = timePerDegreex16 / 16;
                } else {
                    //Take into account any likely acceleration that has occurred since the last full revolution completed:
                    //long rpm_adjust = (timeThisRevolution * (long)currentStatus.rpmDOT) / 1000000;
                    let rpm_adjust = 0;
                    timePerDegreex16 = (2_666_656 / efi_status.rpm + rpm_adjust) as u32; //The use of a x16 value gives accuracy down to 0.1 of a degree and can provide noticeably better timing results on low resolution triggers
                    // timePerDegree = timePerDegreex16 / 16;
                }

                // ckp.degreesPeruSx2048 = 2048 / timePerDegree;
                ckp.degreesPeruSx32768 = (524288 / timePerDegreex16) as f32;

                // ignition timing:
                let mut crank_angle = get_crank_angle(ckp, &cfg.engine.ckp, cycle_time);
                let mut CRANK_ANGLE_MAX_IGN = 720; //ponele? no entendi bien como se setea dependiendo el tipo de encendido/cilindros
                while crank_angle > CRANK_ANGLE_MAX_IGN { crank_angle -= CRANK_ANGLE_MAX_IGN; } // SDUBTUD: no entendi para que es esto pero quien soy para cuestionar a speeduino

                let dwell_angle = 20; //TODO: get from table
                let dwell_time = angle_to_time(ckp,&dwell_angle);

                if !*ignition_running /* && get_cranking_rpm(ckp, &cfg.engine.ckp) != 0*/{
                    *ignition_running = true;
                    //seteamos nuevo triggerrr
                    let dwell_ticks = (120_000_000 / 100_000) * dwell_time as u64;
                    let dwell_duration = Duration::<u64, 1, 100_000>::from_ticks(dwell_ticks);
                    // SDUBTUD: si esto anda es de puro milagro
                    ignition_trigger::spawn_after(dwell_duration).unwrap();
                }

                // esto se usa para iny
                get_crank_angle(ckp, &cfg.engine.ckp, cycle_time);
            } else {
                ckp.reset();
            }
            cfg.engine.ckp.max_stall_time;
        });
    }

    #[task(shared = [led,ign_pins,timer3])]
    fn ignition_trigger(mut ctx: ignition_trigger::Context) {
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

    // prioridad? si; task para manejar el pwm de los inyectores; exportar luego a cpwm.rs
    // #[task(priority = 10, shared = [efi_status, flash_info, efi_cfg, timer, timer3])]
    // fn cpwm_callback(mut _ctx: cpwm_callback::Context, start_revolution: u128, rpm_ticks: [u32; 128]) {
    //     // skip first 30 rotations
    //     if start_revolution > 3 {
    //         //  host::debug!("RPM data {:?}",rpm_ticks[0]);
    //     }
    //     // TODO: cpwm if;
    // }
}
