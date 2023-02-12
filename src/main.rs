//! examples/locals.rs
#![feature(proc_macro_hygiene)]
#![no_main]
#![no_std]

use panic_halt as _;

mod engine;
mod injection;

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM5])]
mod app {
    pub mod gpio;
    pub mod util;
    pub mod webserial;

    use crate::engine::efi_cfg::{get_default_efi_cfg, EngineConfig};
    use crate::engine::efi_status::{get_default_engine_status, EngineStatus};
    use arrayvec::ArrayVec;
    use cortex_m_semihosting::hprintln;

    use stm32f4xx_hal::otg_fs::USB;
    use stm32f4xx_hal::{
        gpio::{Edge, Output, PushPull,Input},
        otg_fs,
        otg_fs::UsbBusType,
        pac::{TIM2, TIM3},
        prelude::*,
        timer::{self, Event},
    };
    use usb_device::bus::UsbBusAllocator;
    use usb_device::device::UsbDevice;
    use usbd_serial::SerialPort;
    use usbd_webusb::{url_scheme, WebUsb};

    use crate::app::gpio::init_gpio;

    #[shared]
    struct Shared {
        timer: timer::CounterMs<TIM2>,
        timer3: timer::CounterUs<TIM3>,
        usb_cdc: SerialPort<'static, UsbBusType>,
        usb_web: WebUsb<UsbBusType>,
        led2: stm32f4xx_hal::gpio::PC14<Output<PushPull>>,
        led3: stm32f4xx_hal::gpio::PC15<Output<PushPull>>,

        // EFI Related:
        efi_cfg: EngineConfig,
        efi_status: EngineStatus,
    }
    #[local]
    struct Local {
        delayval: u32,
        led: stm32f4xx_hal::gpio::PC13<Output<PushPull>>,
        usb_dev: UsbDevice<'static, UsbBusType>,
        cdc_input_buffer: ArrayVec<u8, 128>,

        // EFI Related:
        ckp: stm32f4xx_hal::gpio::PC6<Input>,
    }

    #[init(local = [USB_BUS: Option<UsbBusAllocator<UsbBusType>> = None])]
    fn init(ctx: init::Context) -> (Shared, Local, init::Monotonics) {
        hprintln!("Hello :)");
        let mut dp = ctx.device;

        // Configure the LED pin as a push pull ouput and obtain handle
        // On the Nucleo FR401 theres an on-board LED connected to pin PA5
        // 1) Promote the GPIOA PAC struct
        let gpioa = dp.GPIOA.split();
        let gpiob = dp.GPIOB.split();
        let gpioc = dp.GPIOC.split();
        let gpiod = dp.GPIOD.split();
        let gpioe = dp.GPIOE.split();

        let gpio_config = init_gpio(gpioa, gpiob, gpioc, gpiod, gpioe);

        // CKP/CMP
        let mut ckp = gpio_config.ckp;

        // Configure Button Pin for Interrupts
        // 1) Promote SYSCFG structure to HAL to be able to configure interrupts
        let mut syscfg = dp.SYSCFG.constrain();

        ckp.make_interrupt_source(&mut syscfg);
        ckp.trigger_on_edge(&mut dp.EXTI, Edge::Rising);
        ckp.enable_interrupt(&mut dp.EXTI);

        // Configure and obtain handle for delay abstraction
        // 1) Promote RCC structure to HAL to be able to configure clocks
        let rcc = dp.RCC.constrain();
        // 2) Configure the system clocks
        // 8 MHz must be used for HSE on the Nucleo-F401RE board according to manual
        let clocks = rcc
            .cfgr
            .use_hse(25.MHz())
            .sysclk(120.MHz())
            .require_pll48clk()
            .freeze();

        /*  let clocks = rcc.cfgr.use_hse(8.MHz()).freeze();
         */
        // 3) Create delay handle
        //let mut delay = dp.TIM1.delay_ms(&clocks);
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

        (
            // Initialization of shared resources
            Shared {
                timer,
                timer3,
                usb_cdc,
                usb_web,
                led2: gpio_config.led_1,
                led3: gpio_config.led_2,
                // EFI Related
                efi_cfg: _efi_cfg,
                efi_status: _efi_status,
            },
            // Initialization of task local resources
            Local {
                ckp,
                led: gpio_config.led_0,
                delayval: 2000_u32,
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
        ctx.shared.led2.lock(|l| l.toggle());

        ctx.shared.timer3.lock(|tim| {
            tim.start(50000.micros()).unwrap();
        });
    }

    #[task(binds = TIM3, local=[], shared=[timer3,led2])]
    fn timer3_exp(mut ctx: timer3_exp::Context) {
        // When Timer Interrupt Happens Two Things Need to be Done
        // 1) Toggle the LED
        // 2) Clear Timer Pending Interrupt
        ctx.shared.timer3.lock(|tim| {
            tim.clear_interrupt(Event::Update);
            tim.cancel().unwrap();
        });

        ctx.shared.led2.lock(|l| l.toggle());
    }

    // EXTI9_5_IRQn para los pines ckp/cmp
    #[task(binds = EXTI9_5, local = [ckp], shared=[led3])]
    fn ckp_trigger(mut ctx: ckp_trigger::Context) {
        ctx.shared.led3.lock(|f| f.toggle());
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
                            hprintln!("CDC Read {} bytes", count);

                            // Push bytes into the buffer
                            for i in 0..count {
                                ctx.local.cdc_input_buffer.push(buf[i]);
                                if ctx.local.cdc_input_buffer.is_full() {
                                    hprintln!("Buffer full, processing cmd.");

                                    let cdc_reply = webserial::process_command(
                                        ctx.local.cdc_input_buffer.take().into_inner().unwrap(),
                                    );
                                    ctx.local.cdc_input_buffer.clear();

                                    match cdc_reply {
                                        Some(message) => {
                                            cdc.write(&webserial::finish_message(message)).unwrap();
                                        }
                                        _ => {}
                                    }
                                }
                            }
                        }
                        Err(_) => {}
                    };
                }
            });
        });
    }
}
