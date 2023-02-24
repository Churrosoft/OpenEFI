//! examples/locals.rs
#![feature(proc_macro_hygiene)]
#![no_main]
#![no_std]

use panic_halt as _;

mod engine;
mod injection;
mod memory;

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM5])]
mod app {
    pub mod gpio;
    pub mod logging;
    pub mod util;
    pub mod webserial;

    use crate::engine::efi_cfg::{get_default_efi_cfg, EngineConfig};
    use crate::engine::engine_status::{get_default_engine_status, EngineStatus};
    use crate::injection::injection_setup;
    use arrayvec::ArrayVec;
    use cortex_m_semihosting::hprintln;

    use crate::app::gpio::init_gpio;
    use crate::app::webserial::{send_message, SerialMessage, SerialStatus};
    use crate::memory::tables::TableData;
    use crate::memory::tables::Tables;
    use embedded_hal::spi::{Mode, Phase, Polarity};
    use stm32f4xx_hal::{
        crc32,
        crc32::Crc32,
        gpio::{Edge, Input, Output, PushPull},
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
        timer: timer::CounterMs<TIM2>,
        timer3: timer::CounterUs<TIM3>,
        usb_cdc: SerialPort<'static, UsbBusType>,
        usb_web: WebUsb<UsbBusType>,
        led2: stm32f4xx_hal::gpio::PC14<Output<PushPull>>,
        led3: stm32f4xx_hal::gpio::PC15<Output<PushPull>>,
        crc: Crc32,
        // EFI Related:
        efi_cfg: EngineConfig,
        efi_status: EngineStatus,
        flash_info: FlashInfo,
        tables: Tables,
    }
    #[local]
    struct Local {
        led: stm32f4xx_hal::gpio::PC13<Output<PushPull>>,
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

        // Configure the LED pin as a push pull ouput and obtain handle
        // On the Nucleo FR401 theres an on-board LED connected to pin PA5
        // 1) Promote the GPIOA PAC struct
        let gpioa = dp.GPIOA.split();
        let gpiob = dp.GPIOB.split();
        let gpioc = dp.GPIOC.split();
        let gpiod = dp.GPIOD.split();
        let gpioe = dp.GPIOE.split();

        let gpio_config = init_gpio(gpioa, gpiob, gpioc, gpiod, gpioe);

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

        let usb_bus = ctx.local.USB_BUS;
        unsafe {
            *usb_bus = Some(otg_fs::UsbBus::new(usb, &mut EP_MEMORY));
        }

        let usb_cdc = SerialPort::new(usb_bus.as_ref().unwrap());
        let usb_web = WebUsb::new(
            usb_bus.as_ref().unwrap(),
            url_scheme::HTTPS,
            "tuner.openefi.tech",
        );

        let usb_dev = webserial::new_device(usb_bus.as_ref().unwrap());

        let cdc_buff = ArrayVec::<u8, 128>::new();

        // EFI Related:
        let _efi_cfg = get_default_efi_cfg();
        let _efi_status = get_default_engine_status();

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

        let ldata:[[i32; 17]; 17] = [
            [0, 600, 750, 1100, 1400, 1700, 2000, 2300, 2600, 2900, 3200, 3400, 3700, 4200, 4400, 4700, 5000],
            [100, 85, 88, 90, 92, 94, 96, 98, 100, 99, 99, 99, 99, 98, 97, 96, 94],
            [95, 82, 85, 87, 89, 91, 93, 95, 96, 96, 96, 96, 96, 95, 94, 93, 91],
            [90, 79, 82, 84, 86, 88, 89, 91, 92, 92, 92, 92, 92, 91, 90, 89, 88],
            [85, 75, 78, 80, 82, 84, 86, 88, 89, 89, 89, 89, 89, 87, 87, 86, 84],
            [75, 68, 72, 74, 76, 77, 79, 81, 82, 82, 82, 82, 81, 80, 80, 79, 78],
            [70, 64, 68, 70, 72, 74, 76, 77, 78, 78, 78, 78, 78, 77, 76, 75, 74],
            [65, 59, 64, 67, 69, 71, 72, 74, 75, 75, 75, 75, 74, 74, 73, 72, 71],
            [60, 55, 61, 63, 66, 67, 69, 70, 71, 71, 71, 71, 71, 70, 70, 69, 67],
            [55, 50, 57, 60, 62, 64, 65, 67, 68, 68, 68, 68, 67, 67, 66, 65, 64],
            [50, 46, 52, 56, 59, 60, 62, 63, 64, 64, 64, 64, 64, 63, 63, 62, 61],
            [45, 42, 48, 52, 55, 57, 58, 60, 60, 60, 60, 60, 60, 60, 59, 58, 57],
            [40, 39, 44, 47, 51, 53, 55, 56, 57, 57, 57, 57, 57, 56, 56, 55, 54],
            [35, 37, 40, 43, 47, 49, 51, 52, 53, 53, 53, 53, 53, 53, 52, 51, 51],
            [30, 36, 37, 40, 43, 45, 47, 49, 50, 50, 50, 50, 50, 49, 49, 48, 47],
            [25, 35, 36, 37, 39, 41, 43, 45, 46, 46, 46, 46, 46, 45, 45, 45, 44],
            [15, 35, 35, 35, 35, 35, 36, 37, 38, 38, 38, 38, 38, 38, 38, 38, 37],
        ];

        hprintln!(
            "Find 2 in vec1: {:?}",
            ldata[1].into_iter().position(|x| x <= 307200)
        );

        let mut td_test2 = TableData {
            data: None,
            crc: 0,
            address: 0x3,
            max_x: 17,
            max_y: 17,
        };

        td_test2.data = td_test2.read_from_memory(&mut flash, &flash_info, &mut crc);

        let mut table = Tables { tps_rpm_ve: None };

        // EFI Setup:
        injection_setup(&mut table, &mut flash, &flash_info, &mut crc);

        (
            // Initialization of shared resources
            Shared {
                timer,
                timer3,
                usb_cdc,
                usb_web,
                led2: gpio_config.led_1,
                led3: gpio_config.led_2,
                crc,
                // EFI Related
                efi_cfg: _efi_cfg,
                efi_status: _efi_status,
                flash_info,
                tables: table,
            },
            // Initialization of task local resources
            Local {
                ckp,
                led: gpio_config.led_0,
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

    #[task(binds = TIM2, priority = 1, local=[led], shared=[timer,timer3,led2])]
    fn timer_expired(mut ctx: timer_expired::Context) {
        // When Timer Interrupt Happens Two Things Need to be Done
        // 1) Toggle the LED
        // 2) Clear Timer Pending Interrupt
        ctx.shared
            .timer
            .lock(|tim| tim.clear_interrupt(Event::Update));

        ctx.local.led.toggle();
        ctx.shared.led2.lock(|l| l.set_low());

        ctx.shared.timer3.lock(|tim| {
            tim.start(50000.micros()).unwrap();
        });
    }

    #[task(binds = TIM3, local=[], shared=[timer3,led2, tables])]
    fn timer3_exp(mut ctx: timer3_exp::Context) {
        // When Timer Interrupt Happens Two Things Need to be Done
        // 1) Toggle the LED
        // 2) Clear Timer Pending Interrupt
        ctx.shared.timer3.lock(|tim| {
            tim.clear_interrupt(Event::Update);
            tim.cancel().unwrap();
        });

/*         ctx.shared.tables.lock(|t| t.tps_rpm_ve = None); */

        ctx.shared.led2.lock(|l| l.set_high());
    }

    // EXTI9_5_IRQn para los pines ckp/cmp
    #[task(binds = EXTI9_5, local = [ckp], shared=[led3,efi_status,flash_info])]
    fn ckp_trigger(mut ctx: ckp_trigger::Context) {
        // ctx.shared.led3.lock(|f| f.toggle());
        ctx.shared.efi_status.lock(|es| es.cycle_tick += 1);
        // Obtain access to Button Peripheral and Clear Interrupt Pending Flag
        ctx.local.ckp.clear_interrupt_pending_bit();
    }

    #[task(binds = OTG_FS, local = [usb_dev, cdc_input_buffer], shared = [usb_cdc, usb_web])]
    fn usb_handler(mut ctx: usb_handler::Context) {
        let device = ctx.local.usb_dev;
        ctx.shared.usb_cdc.lock(|cdc| {
            // USB dev poll only in the interrupt handler
            ctx.shared.usb_web.lock(|web| {
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
                    };
                }
            });
        });
    }

    // Externally defined tasks
    extern "Rust" {
        // Low-priority task to send back replies via the serial port.
        #[task(shared = [usb_cdc], priority = 2)]
        fn send_message(
            ctx: send_message::Context,
            status: SerialStatus,
            code: u8,
            mut message: SerialMessage,
        );
    }
}
