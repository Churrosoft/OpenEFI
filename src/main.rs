//! examples/locals.rs

#![deny(unsafe_code)]
#![deny(warnings)]
#![no_main]
#![no_std]

use panic_halt as _;

pub mod core;

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true)]
mod app {
    // use cortex_m_semihosting::{debug, hprintln};

    use stm32f4xx_hal::{
        gpio::{self, Edge, Output, PushPull},
        pac::TIM2,
        prelude::*,
        timer::{self, Event},
    };

    #[shared]
    struct Shared {
        timer: timer::CounterMs<TIM2>,
    }
    #[local]
    struct Local {
        delayval: u32,
        button: gpio::PD8<Output<PushPull>>,
        led: gpio::PC13<Output<PushPull>>,
    }

    #[init]
    fn init(ctx: init::Context) -> (Shared, Local, init::Monotonics) {
        use crate::core::gpio::init_gpio;
        let mut dp = ctx.device;

        // Configure the LED pin as a push pull ouput and obtain handle
        // On the Nucleo FR401 theres an on-board LED connected to pin PA5
        // 1) Promote the GPIOA PAC struct
        let gpioa = dp.GPIOA.split();
        let gpiob = dp.GPIOB.split();
        let gpioc = dp.GPIOC.split();
        let gpiod = dp.GPIOD.split();
        let gpioe = dp.GPIOE.split();

        let gpioConfig = init_gpio(gpioa, gpiob, gpioc, gpiod, gpioe);

        // 2) Configure Pin and Obtain Handle
        // let _led = gpioc.pc15.into_push_pull_output();

        // Configure the button pin as input and obtain handle
        // On the Nucleo FR401 there is a button connected to pin PC13
        // 1) Promote the GPIOC PAC struct
        // 2) Configure Pin and Obtain Handle
        // no me borre todavia esto del boton que lo voy a reciclar para los interrupts del ckp/cmp
        let mut button = gpioConfig.iny_1;

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
        let mut timer = dp.TIM2.counter_ms(&clocks);

        // esto del timer2 es caaaassiiii lo que necesito para el tema del encendido / inyeccion, tengo que ver como apagarlo cuando termina el evento nomas
        // esta otra lib soporta el oneshot: https://docs.rs/embedded-time/latest/embedded_time/timer/param/struct.OneShot.html
        // Kick off the timer with 2 seconds timeout first
        // It probably would be better to use the global variable here but I did not to avoid the clutter of having to create a crtical section
        timer.start(2000.millis()).unwrap();

        // Set up to generate interrupt when timer expires
        timer.listen(Event::Update);

        (
            // Initialization of shared resources
            Shared { timer },
            // Initialization of task local resources
            Local {
                button,
                led: gpioConfig.led_0,
                delayval: 2000_u32,
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

    #[task(binds = TIM2, local=[led], shared=[timer])]
    fn timer_expired(mut ctx: timer_expired::Context) {
        // When Timer Interrupt Happens Two Things Need to be Done
        // 1) Toggle the LED
        // 2) Clear Timer Pending Interrupt

        ctx.local.led.toggle();
        ctx.shared
            .timer
            .lock(|tim| tim.clear_interrupt(Event::Update));
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
}
