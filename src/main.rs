//! examples/locals.rs

#![deny(warnings)]
#![no_main]
#![no_std]

use panic_halt as _;

pub mod core;

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true)]
mod app {
    // use cortex_m_semihosting::{debug, hprintln};

    use crate::core::gpio::init_gpio;
    use crate::core::usb;
    use stm32f4xx_hal::otg_fs::USB;
    use stm32f4xx_hal::{
        gpio::{self, Edge, Output, PushPull},
        otg_fs,
        otg_fs::UsbBusType,
        pac::TIM2,
        pac::TIM3,
        prelude::*,
        timer::{self, Event},
    };
    use usb_device::bus::UsbBusAllocator;
    use usb_device::device::UsbDevice;
    use usbd_serial::SerialPort;
    use usbd_webusb::{url_scheme, WebUsb};

    #[shared]
    struct Shared {
        timer: timer::CounterMs<TIM2>,
        timer3: timer::CounterUs<TIM3>,
        usb_cdc: SerialPort<'static, UsbBusType>,
        usb_web: WebUsb<UsbBusType>,
    }
    #[local]
    struct Local {
        delayval: u32,
        button: gpio::PD8<Output<PushPull>>,
        led: gpio::PC13<Output<PushPull>>,
        led2: gpio::PC14<Output<PushPull>>,
        usb_dev: UsbDevice<'static, UsbBusType>,
    }

    #[init(local = [USB_BUS: Option<UsbBusAllocator<UsbBusType>> = None])]
    fn init(ctx: init::Context) -> (Shared, Local, init::Monotonics) {
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

        // 2) Configure Pin and Obtain Handle
        // let _led = gpioc.pc15.into_push_pull_output();

        // Configure the button pin as input and obtain handle
        // On the Nucleo FR401 there is a button connected to pin PC13
        // 1) Promote the GPIOC PAC struct
        // 2) Configure Pin and Obtain Handle
        // no me borre todavia esto del boton que lo voy a reciclar para los interrupts del ckp/cmp
        let mut button = gpio_config.iny_1;

        // Configure Button Pin for Interrupts
        // 1) Promote SYSCFG structure to HAL to be able to configure interrupts
        let mut syscfg = dp.SYSCFG.constrain();
        // 2) Make button an interrupt source
        button.make_interrupt_source(&mut syscfg);
        // 3) Make button an interrupt source
        button.trigger_on_edge(&mut dp.EXTI, Edge::Rising);
        // 4) Enable gpio interrupt for button
        button.enable_interrupt(&mut dp.EXTI);

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
        // esto del timer2 es caaaassiiii lo que necesito para el tema del encendido / inyeccion, tengo que ver como apagarlo cuando termina el evento nomas
        // esta otra lib soporta el oneshot: https://docs.rs/embedded-time/latest/embedded_time/timer/param/struct.OneShot.html
        // Kick off the timer with 2 seconds timeout first
        // It probably would be better to use the global variable here but I did not to avoid the clutter of having to create a crtical section
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

        let usb_dev = usb::new_device(usb_bus.as_ref().unwrap());

        (
            // Initialization of shared resources
            Shared {
                timer,
                timer3,
                usb_cdc,
                usb_web,
            },
            // Initialization of task local resources
            Local {
                button,
                led: gpio_config.led_0,
                led2: gpio_config.led_1,
                delayval: 2000_u32,
                usb_dev,
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

    #[task(binds = TIM2, priority = 1, local=[led], shared=[timer, timer3])]
    fn timer_expired(mut ctx: timer_expired::Context) {
        // When Timer Interrupt Happens Two Things Need to be Done
        // 1) Toggle the LED
        // 2) Clear Timer Pending Interrupt
        ctx.shared
            .timer
            .lock(|tim| tim.clear_interrupt(Event::Update));
        
        ctx.local.led.toggle();

        ctx.shared.timer3.lock(|tim| {
            tim.start(50000.micros()).unwrap();
        });

    }

    #[task(binds = TIM3, local=[led2], shared=[timer3])]
    fn timer3_exp(mut ctx: timer3_exp::Context) {
        // When Timer Interrupt Happens Two Things Need to be Done
        // 1) Toggle the LED
        // 2) Clear Timer Pending Interrupt
        ctx.shared.timer3.lock(|tim| {
            tim.clear_interrupt(Event::Update);
            tim.cancel().unwrap();
        });

        ctx.local.led2.toggle();
    }

    // EXTI9_5_IRQn para los pines ckp/cmp
    #[task(binds = EXTI15_10, local = [delayval, button], shared=[timer])]
    fn button_pressed(mut ctx: button_pressed::Context) {
        // When Button press interrupt happens three things need to be done
        // 1) Adjust Global Delay Variable
        // 2) Update Timer with new Global Delay value
        // 3) Clear Button Pending Interrupt

        // Obtain a copy of the delay value from the global context
        let mut delay = *ctx.local.delayval;

        // Adjust the amount of delay
        delay = delay - 500_u32;
        if delay < 500_u32 {
            delay = 2000_u32;
        }

        // Update delay value in global context
        *ctx.local.delayval = delay;

        // Update the timeout value in the timer peripheral
        ctx.shared
            .timer
            .lock(|tim| tim.start(delay.millis()).unwrap());

        // Obtain access to Button Peripheral and Clear Interrupt Pending Flag
        ctx.local.button.clear_interrupt_pending_bit();
    }

    #[task(binds = OTG_FS, local = [usb_dev], shared = [usb_cdc, usb_web])]
    fn usb_handler(mut ctx: usb_handler::Context) {
        let device = ctx.local.usb_dev;
        ctx.shared.usb_cdc.lock(|cdc| {
            // USB dev poll only in the interrupt handler
            ctx.shared.usb_web.lock(|web| {
                if device.poll(&mut [web, cdc]) {
                    // Read serial data here!
                }
            });
        });
    }
}
