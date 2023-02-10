use stm32f4xx_hal::gpio::{self, gpioc, gpiod, Input, Output, PushPull};
pub struct GpioMapping {
    // LED's / User feedback
    pub led_0: gpio::PC13<Output<PushPull>>,
    pub led_1: gpio::PC14<Output<PushPull>>,
    pub led_2: gpio::PC15<Output<PushPull>>,

    // Injection
    pub iny_1: gpio::PD8<Output<PushPull>>,
    pub iny_2: gpio::PD9<Output<PushPull>>,
    pub iny_3: gpio::PD10<Output<PushPull>>,
    pub iny_4: gpio::PD11<Output<PushPull>>,
    // Ignition
    pub ecn_1: gpio::PD12<Output<PushPull>>,
    pub ecn_2: gpio::PD13<Output<PushPull>>,
    pub ecn_3: gpio::PD14<Output<PushPull>>,
    pub ecn_4: gpio::PD15<Output<PushPull>>,
    // PMIC
    // CKP/CMP

    // RELAY's
    pub relay_iny: gpio::PE2<Output<PushPull>>,
    pub relay_gnc: gpio::PE3<Output<PushPull>>,
    pub relay_ac: gpio::PE4<Output<PushPull>>,

    // Step-Step motor TODO
    pub mtr_step: gpio::PE4<Output<PushPull>>,
    pub mtr_dir: gpio::PE4<Output<PushPull>>,
    pub mtr_fault: gpio::PE4<Output<PushPull>>,
    pub mtr_enable: gpio::PE4<Output<PushPull>>,

    // AUX I/O
    pub aux_in_1: gpio::PA0<Input>,
    pub aux_in_2: gpio::PA1<Input>,
    pub aux_in_3: gpio::PA3<Input>,

    pub aux_out_1: gpio::PC4<Output<PushPull>>,
    pub aux_out_2: gpio::PC5<Output<PushPull>>,
    pub aux_out_3: gpio::PB0<Output<PushPull>>,
}

pub fn init_gpio(gpioc: gpioc::Parts, gpiod: gpiod::Parts) -> GpioMapping {
    let mut gpio = GpioMapping {
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

    // set default state on I/O
    gpio.led_1.set_high();
    gpio.led_2.set_high();

    return gpio;
}
