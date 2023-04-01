//! examples/locals.rs
#![feature(proc_macro_hygiene)]
#![no_main]
#![no_std]

use panic_halt as _;

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM5,TIM7,TIM4])]
mod app {
    pub mod engine;
    pub mod gpio;
    pub mod gpio_legacy;
    pub mod injection;
    pub mod logging;
    pub mod memory;
    pub mod util;
    pub mod webserial;

    use crate::app::engine::efi_cfg::{get_default_efi_cfg, EngineConfig};
    use crate::app::engine::engine_status::{get_default_engine_status, EngineStatus};
    use crate::app::gpio_legacy::init_gpio;
    use crate::app::injection::calculate_time_isr;
    use crate::app::injection::injection_setup;
    use crate::app::memory::tables::Tables;
    use crate::app::webserial::{handle_tables, send_message, SerialMessage, SerialStatus};
    use arrayvec::ArrayVec;
    use cortex_m_semihosting::hprintln;
    use embedded_hal::spi::{Mode, Phase, Polarity};
    use stm32f4xx_hal::{
        crc32,
        crc32::Crc32,
        gpio::{Edge, Input},
        otg_fs,
        otg_fs::UsbBusType,
        otg_fs::USB,
        pac::{TIM2, TIM3},
        prelude::*,
        spi::*,
        timer::{self, Event},
    };
    use usb_device::bus::UsbBusAllocator;
    use usb_device::device::UsbDevice;
    use usbd_serial::SerialPort;
    use usbd_webusb::{url_scheme, WebUsb};
    use w25q::series25::FlashInfo;

    #[shared]
    struct Shared {
        usb_cdc: SerialPort<'static, UsbBusType, [u8; 128], [u8; 4000]>,
        usb_web: WebUsb<UsbBusType>,

        // core:
        timer: timer::CounterMs<TIM2>,
        timer3: timer::CounterUs<TIM3>,
        leds: gpio_legacy::LedGpioMapping,
        string: serde_json_core::heapless::String<1000>,
        str_lock: bool,
        crc: Crc32,

        // EFI Related:
        efi_cfg: EngineConfig,
        efi_status: EngineStatus,
        flash: memory::tables::FlashT,
        flash_info: FlashInfo,
        tables: Tables,
    }
    #[local]
    struct Local {
        usb_dev: UsbDevice<'static, UsbBusType>,
        cdc_input_buffer: ArrayVec<u8, 128>,

        // EFI Related:
        ckp: stm32f4xx_hal::gpio::PC6<Input>,
    }

    #[init(local = [USB_BUS: Option<UsbBusAllocator<UsbBusType>> = None])]
    fn init(mut ctx: init::Context) -> (Shared, Local, init::Monotonics) {
        let mut dp = ctx.device;

        ctx.core.DWT.enable_cycle_counter(); // TODO: Disable this in release builds
        logging::host::debug!("Hello :)");

        let gpioa = dp.GPIOA.split();
        let gpiob = dp.GPIOB.split();
        let gpioc = dp.GPIOC.split();
        let gpiod = dp.GPIOD.split();
        let gpioe = dp.GPIOE.split();

        let mut gpio_config = init_gpio(gpioa, gpiob, gpioc, gpiod, gpioe);

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
            .use_hse(25.MHz())
            .sysclk(120.MHz())
            .require_pll48clk()
            .freeze();

        let mut timer: timer::CounterMs<TIM2> = dp.TIM2.counter_ms(&clocks);
        let mut timer3: timer::CounterUs<TIM3> = dp.TIM3.counter_us(&clocks);

        timer.start(2000.millis()).unwrap();

        // Set up to generate interrupt when timer expires
        timer.listen(Event::Update);
        timer3.listen(Event::Update);

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
            3.MHz(),
            &clocks,
        );

        // CRC32:
        let mut crc = crc32::Crc32::new(dp.CRC);

        let mut flash = w25q::series25::Flash::init(spi2, gpio_config.memory_cs).unwrap();

        let id = flash.read_jedec_id().unwrap();

        let flash_info = flash.get_device_info().unwrap();

        hprintln!("FLASH: {:?}", id);
        hprintln!("FLASH: Size {:?}", flash_info.capacity_kb);
        hprintln!("FLASH: Block Count {:?}", flash_info.block_count);
        hprintln!("FLASH: Page Count {:?}", flash_info.page_count);

        /*         hprintln!(
            "Find 2 in vec1: {:?}",
            ldata[0].into_iter().position(|x| x <= 307200)
        ); */

        // EFI Setup:
        let mut table = Tables {
            tps_rpm_ve: None,
            injector_delay: None,
        };

        injection_setup(&mut table, &mut flash, &flash_info, &mut crc);

        // REMOVE: solo lo estoy hardcodeando aca para probar el AlphaN
        _efi_status.rpm = 1500;

        calculate_time_isr(&mut _efi_status, &_efi_cfg);

        hprintln!("AF {:?}", _efi_status.injection.air_flow);

        let mut serialized: serde_json_core::heapless::String<1000> =
            serde_json_core::to_string(&_efi_cfg).unwrap();

        let mut str_lock = false;

        gpio_config.leds.led_check.toggle();
        gpio_config.leds.led_mil.toggle();

        //  hprintln!("FFFF {:?}", serialized);
        (
            // Initialization of shared resources
            Shared {
                timer,
                timer3,
                usb_cdc,
                usb_web,
                // GPIO:
                leds: gpio_config.leds,
                crc,
                string: serialized,
                str_lock,
                // EFI Related
                efi_cfg: _efi_cfg,
                efi_status: _efi_status,
                flash,
                flash_info,
                tables: table,
            },
            // Initialization of task local resources
            Local {
                ckp,
                usb_dev,
                cdc_input_buffer: cdc_buff,
            },
            // Move the monotonic timer to the RTIC run-time, this enables
            // scheduling
            init::Monotonics(),
        )
    }

    #[idle]
    fn idle(_: idle::Context) -> ! {
        loop {
            cortex_m::asm::wfi();
        }
    }

    //TODO: reciclar para encendido
    #[task(binds = TIM2, priority = 1, local=[], shared=[timer,timer3,leds])]
    fn timer_expired(mut ctx: timer_expired::Context) {
        ctx.shared
            .timer
            .lock(|tim| tim.clear_interrupt(Event::Update));

        ctx.shared.leds.lock(|l| l.led_0.toggle());
    }

    #[task(binds = TIM3, local=[], shared=[timer3,leds, tables])]
    fn timer3_exp(mut ctx: timer3_exp::Context) {
        ctx.shared.timer3.lock(|tim| {
            tim.clear_interrupt(Event::Update);
            tim.cancel().unwrap();
        });

        ctx.shared.leds.lock(|l| l.led_2.set_high());
    }

    // EXTI9_5_IRQn para los pines ckp/cmp
    #[task(binds = EXTI9_5, local = [ckp], shared=[leds, efi_status,flash_info,efi_cfg,timer,timer3])]
    fn ckp_trigger(mut ctx: ckp_trigger::Context) {
        ctx.shared.efi_status.lock(|es| es.cycle_tick += 1);

        let efi_cfg = ctx.shared.efi_cfg;
        let efi_status = ctx.shared.efi_status;

        // calculo de RPM && led
        (efi_cfg, efi_status, ctx.shared.leds, ctx.shared.timer3).lock(
            |efi_cfg, efi_status, leds, timer3| {
                if efi_status.cycle_tick
                    >= efi_cfg.engine.ckp_tooth_count - efi_cfg.engine.ckp_missing_tooth
                {
                    leds.led_2.set_low();
                    // FIXME: por ahora solo prendo el led una vez por vuelta, luego lo hago funcionar con el primer cilindro
                    timer3.start(50000.micros()).unwrap();
                    efi_status.cycle_tick = 0;
                }
            },
        );

        cpwm_callback::spawn().unwrap();

        // Obtain access to the peripheral and Clear Interrupt Pending Flag
        ctx.local.ckp.clear_interrupt_pending_bit();
    }

    #[task(binds = OTG_FS, local = [usb_dev, cdc_input_buffer], shared = [usb_cdc, usb_web,flash,flash_info,tables])]
    fn usb_handler(mut ctx: usb_handler::Context) {
        let device = ctx.local.usb_dev;

        let flash = ctx.shared.flash;
        let flash_info = ctx.shared.flash_info;
        let tables = ctx.shared.tables;

        ctx.shared.usb_cdc.lock(|cdc| {
            // USB dev poll only in the interrupt handler
            (ctx.shared.usb_web, flash, flash_info, tables).lock(
                |web, flash, flash_info, tables| {
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
                                            flash,
                                            flash_info,
                                            tables,
                                        );

                                        ctx.local.cdc_input_buffer.clear();
                                    }
                                }
                            }
                            Err(_) => {}
                        };
                    }
                },
            );
        });
    }

    // Externally defined tasks
    extern "Rust" {
        // Low-priority task to send back replies via the serial port.
        #[task(shared = [usb_cdc], priority = 2,capacity = 30)]
        fn send_message(
            ctx: send_message::Context,
            status: SerialStatus,
            code: u8,
            mut message: SerialMessage,
        );
    }

    #[task(priority = 2,shared=[flash,flash_info,efi_cfg,tables])]
    fn table_cdc_callback(ctx: table_cdc_callback::Context, serial_cmd: SerialMessage) {
        let flash = ctx.shared.flash;
        let flash_info = ctx.shared.flash_info;
        let tables = ctx.shared.tables;

        (flash, flash_info, tables).lock(|flash, flash_info, tables| {
            handle_tables::handler(serial_cmd, flash, flash_info, tables);
        });
    }

    // prioridad? si; task para manejar el pwm de los inyectores; exportar luego a cpwm.rs
    #[task(priority = 10,shared=[efi_status,flash_info,efi_cfg,timer,timer3])]
    fn cpwm_callback(mut _ctx: cpwm_callback::Context) {
        // TODO: cpwm if;
    }
}
