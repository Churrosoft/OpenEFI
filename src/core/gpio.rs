use stm32f4xx_hal::gpio::{self, gpioa, gpiob, gpioc, gpiod, gpioe, Input, Output, PushPull};
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
    pub pmic_enable: gpio::PB12<Output<PushPull>>,
    pub pmic_spark: gpio::PB13<Input>,
    pub pmic_cs: gpio::PA15<Output<PushPull>>,
    pub pmic_nomi: gpio::PC10<Input>,
    pub pmic_maxi: gpio::PC11<Input>,

    // CKP/CMP
    pub ckp: gpio::PC6<Input>,
    pub cmp: gpio::PC7<Input>,
    
    // RELAY's
    pub relay_iny: gpio::PE2<Output<PushPull>>,
    pub relay_gnc: gpio::PE3<Output<PushPull>>,
    pub relay_ac: gpio::PE4<Output<PushPull>>,
    pub relay_lmb: gpio::PE0<Output<PushPull>>,

    // Step-Step motor TODO
    pub mtr_step: gpio::PE7<Output<PushPull>>,
    pub mtr_dir: gpio::PE8<Output<PushPull>>,
    pub mtr_fault: gpio::PE9<Output<PushPull>>,
    pub mtr_enable: gpio::PE10<Output<PushPull>>,

    // AUX I/O
    pub aux_in_1: gpio::PA0<Input>,
    pub aux_in_2: gpio::PA1<Input>,
    pub aux_in_3: gpio::PA3<Input>,

    pub aux_out_1: gpio::PC4<Output<PushPull>>,
    pub aux_out_2: gpio::PC5<Output<PushPull>>,
    pub aux_out_3: gpio::PB0<Output<PushPull>>,

    pub aux_cs_1: gpio::PE14<Output<PushPull>>,
    pub aux_cs_2: gpio::PE15<Output<PushPull>>,
}

pub fn init_gpio(
    gpioa: gpioa::Parts,
    gpiob: gpiob::Parts,
    gpioc: gpioc::Parts,
    gpiod: gpiod::Parts,
    gpioe: gpioe::Parts,
) -> GpioMapping {
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

        // PMIC
        pmic_enable: gpiob.pb12.into_push_pull_output(),
        pmic_spark: gpiob.pb13.into_input(),
        pmic_cs: gpioa.pa15.into_push_pull_output(),
        pmic_nomi: gpioc.pc10.into_input(),
        pmic_maxi: gpioc.pc11.into_input(),

        // CKP/CMP
        // RELAY's
        relay_iny: gpioe.pe2.into_push_pull_output(),
        relay_gnc: gpioe.pe3.into_push_pull_output(),
        relay_ac: gpioe.pe4.into_push_pull_output(),

        // Step-Step motor TODO
        mtr_step: gpioe.pe7.into_push_pull_output(),
        mtr_dir: gpioe.pe8.into_push_pull_output(),
        mtr_fault: gpioe.pe9.into_push_pull_output(),
        mtr_enable: gpioe.pe10.into_push_pull_output(),

        // AUX I/O
        aux_in_1: gpioa.pa0.into_input(),
        aux_in_2: gpioa.pa1.into_input(),
        aux_in_3: gpioa.pa3.into_input(),

        aux_out_1: gpioc.pc4.into_push_pull_output(),
        aux_out_2: gpioc.pc5.into_push_pull_output(),
        aux_out_3: gpiob.pb0.into_push_pull_output(),
    };

    // set default state on I/O
    gpio.led_0.set_high();
    gpio.led_1.set_high();
    gpio.led_2.set_high();

    return gpio;
}
