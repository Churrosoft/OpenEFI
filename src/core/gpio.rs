use stm32f4xx_hal::{
    gpio::{self, Output, Pin, PushPull, gpiod, gpioc},
};
pub struct GpioMapping {
    // LED's / User feedback
    pub led_0: gpio::PC13<Output<PushPull>>,
    pub led_1: gpio::PC14<Output<PushPull>>,
    pub led_2: gpio::PC15<Output<PushPull>>,
    // Injection
    pub iny_1: Pin<'D', 8, Output<PushPull>>,
    pub iny_2: Pin<'D', 9, Output<PushPull>>,
    pub iny_3: Pin<'D', 10, Output<PushPull>>,
    pub iny_4: Pin<'D', 11, Output<PushPull>>,
    // Ignition
    pub ecn_1: Pin<'D', 12, Output<PushPull>>,
    pub ecn_2: Pin<'D', 13, Output<PushPull>>,
    pub ecn_3: Pin<'D', 14, Output<PushPull>>,
    pub  ecn_4: Pin<'D', 15, Output<PushPull>>,
    // PMIC
    // CKP/CMP

    // RELAY's

    // AUX I/O
}

pub fn init_gpio(gpioc: gpioc::Parts, gpiod: gpiod::Parts) -> GpioMapping {
    //let gpioa = dp.GPIOA.split();
    //let gpiob = dp.GPIOB.split();
/*     let gpioc = dp.GPIOC.split();
    let gpiod = dp.GPIOD.split(); */
    // let gpiof = dp.GPIOF.split();

    let gpio = GpioMapping {
        // LED's / User feedback
        led_0: gpioc.pc13.into_push_pull_output(),
        led_1: gpioc.pc14.into_push_pull_output(),
        led_2: gpioc.pc15.into_push_pull_output(),

        // Injection
        iny_1: gpiod.pd8.into_push_pull_output(),
        iny_2: gpiod.pd9.into_push_pull_output(),
        iny_3: gpiod.pd10.into_push_pull_output(),
        iny_4: gpiod.pd11.into_push_pull_output(),

        // Ignition
        ecn_1: gpiod.pd12.into_push_pull_output(),
        ecn_2: gpiod.pd13.into_push_pull_output(),
        ecn_3: gpiod.pd14.into_push_pull_output(),
        ecn_4: gpiod.pd15.into_push_pull_output(),
    };

    return gpio;
}
