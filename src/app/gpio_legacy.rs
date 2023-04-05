// FOR uEFI v3.x boards

use stm32f4xx_hal::{
    gpio::{self, gpioa, gpiob, gpioc, gpiod, gpioe, Alternate, Input, Output, Pin, PushPull},
    pac::SPI2,
    spi::Spi,
};

pub struct InjectionGpioMapping {
    pub iny_1: gpio::PD8<Output<PushPull>>,
    pub iny_2: gpio::PD9<Output<PushPull>>,
    pub iny_3: gpio::PD10<Output<PushPull>>,
    pub iny_4: gpio::PD11<Output<PushPull>>,
}

pub struct IgnitionGpioMapping {
    pub ecn_1: gpio::PD12<Output<PushPull>>,
    pub ecn_2: gpio::PD13<Output<PushPull>>,
    pub ecn_3: gpio::PD14<Output<PushPull>>,
    pub ecn_4: gpio::PD15<Output<PushPull>>,
}

pub struct PMICGpioMapping {
    pub pmic_enable: gpio::PB12<Output<PushPull>>,
    pub pmic_spark: gpio::PB13<Input>,
    pub pmic_cs: gpio::PA15<Output<PushPull>>,
    pub pmic_nomi: gpio::PC10<Input>,
    pub pmic_maxi: gpio::PC11<Input>,
}

pub struct LedGpioMapping {
    pub led_0: gpio::PC13<Output<PushPull>>,
    pub led_1: gpio::PC14<Output<PushPull>>,
    pub led_2: gpio::PC15<Output<PushPull>>,
    pub led_can_tx: gpio::PD6<Output<PushPull>>,
    pub led_can_rx: gpio::PD7<Output<PushPull>>,
    pub led_check: gpio::PC9<Output<PushPull>>,
    pub led_mil: gpio::PC8<Output<PushPull>>,
}

pub struct AuxIoMapping {
    pub in_1: gpio::PA0<Input>,
    pub in_2: gpio::PA1<Input>,
    pub in_3: gpio::PA3<Input>,

    pub out_1: gpio::PC4<Output<PushPull>>,
    pub out_2: gpio::PC5<Output<PushPull>>,
    pub out_3: gpio::PB0<Output<PushPull>>,

    pub cs_1: gpio::PE14<Output<PushPull>>,
    pub cs_2: gpio::PE15<Output<PushPull>>,
}

pub struct RelayMapping {
    pub iny: gpio::PE2<Output<PushPull>>,
    pub gnc: gpio::PE3<Output<PushPull>>,
    pub ac: gpio::PE4<Output<PushPull>>,
    pub lmb: gpio::PE0<Output<PushPull>>,
}

pub struct GpioMapping {
    // LED's / User feedback
    pub leds: LedGpioMapping,

    // Injection
    pub injection: InjectionGpioMapping,

    // Ignition
    pub ignition: IgnitionGpioMapping,

    // PMIC
    pub pmic: PMICGpioMapping,

    // CKP/CMP
    pub ckp: gpio::PC6<Input>,
    pub cmp: gpio::PC7<Input>,

    // RELAY's
    pub relay: RelayMapping,

    // Step-Step motor TODO
    pub mtr_step: gpio::PE7<Output<PushPull>>,
    pub mtr_dir: gpio::PE8<Output<PushPull>>,
    pub mtr_fault: gpio::PE9<Output<PushPull>>,
    pub mtr_enable: gpio::PE10<Output<PushPull>>,

    // SPI Flash
    pub memory_cs: gpio::PE13<Output<PushPull>>,

    // AUX I/O
    pub aux: AuxIoMapping,

    pub usb_dp: gpio::PA11<Alternate<10, PushPull>>,
    pub usb_dm: gpio::PA12<Alternate<10, PushPull>>,

    pub spi_sck: gpio::PB10<Alternate<5>>,
    pub spi_miso: gpio::PB14<Alternate<5>>,
    pub spi_mosi: gpio::PB15<Alternate<5, PushPull>>,
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
        leds: LedGpioMapping {
            led_0: gpioc.pc13.into_push_pull_output(),
            led_1: gpioc.pc14.into_push_pull_output(),
            led_2: gpioc.pc15.into_push_pull_output(),
            led_can_tx: gpiod.pd6.into_push_pull_output(),
            led_can_rx: gpiod.pd7.into_push_pull_output(),
            led_check: gpioc.pc9.into_push_pull_output(),
            led_mil: gpioc.pc8.into_push_pull_output(),
        },

        // Injection
        injection: InjectionGpioMapping {
            iny_1: gpiod.pd8.into_push_pull_output(),
            iny_2: gpiod.pd9.into_push_pull_output(),
            iny_3: gpiod.pd10.into_push_pull_output(),
            iny_4: gpiod.pd11.into_push_pull_output(),
        },

        // Ignition
        ignition: IgnitionGpioMapping {
            ecn_1: gpiod.pd12.into_push_pull_output(),
            ecn_2: gpiod.pd13.into_push_pull_output(),
            ecn_3: gpiod.pd14.into_push_pull_output(),
            ecn_4: gpiod.pd15.into_push_pull_output(),
        },

        // PMIC
        pmic: PMICGpioMapping {
            pmic_enable: gpiob.pb12.into_push_pull_output(),
            pmic_spark: gpiob.pb13.into_input(),
            pmic_cs: gpioa.pa15.into_push_pull_output(),
            pmic_nomi: gpioc.pc10.into_input(),
            pmic_maxi: gpioc.pc11.into_input(),
        },

        // CKP/CMP
        ckp: gpioc.pc6.into_input(),
        cmp: gpioc.pc7.into_input(),

        // RELAY's
        relay: RelayMapping {
            iny: gpioe.pe2.into_push_pull_output(),
            gnc: gpioe.pe3.into_push_pull_output(),
            ac: gpioe.pe4.into_push_pull_output(),
            lmb: gpioe.pe0.into_push_pull_output(),
        },

        // Step-Step motor TODO
        mtr_step: gpioe.pe7.into_push_pull_output(),
        mtr_dir: gpioe.pe8.into_push_pull_output(),
        mtr_fault: gpioe.pe9.into_push_pull_output(),
        mtr_enable: gpioe.pe10.into_push_pull_output(),

        // SPI Flash
        memory_cs: gpioe.pe13.into_push_pull_output(),

        // AUX I/O
        aux: AuxIoMapping {
            in_1: gpioa.pa0.into_input(),
            in_2: gpioa.pa1.into_input(),
            in_3: gpioa.pa3.into_input(),

            out_1: gpioc.pc4.into_push_pull_output(),
            out_2: gpioc.pc5.into_push_pull_output(),
            out_3: gpiob.pb0.into_push_pull_output(),

            cs_1: gpioe.pe14.into_push_pull_output(),
            cs_2: gpioe.pe15.into_push_pull_output(),
        },

        // USB
        usb_dp: gpioa.pa11.into_alternate(),
        usb_dm: gpioa.pa12.into_alternate(),

        // SPI
        spi_sck: gpiob.pb10.into_alternate(),
        spi_miso: gpiob.pb14.into_alternate(),
        spi_mosi: gpiob.pb15.into_alternate().internal_pull_up(true),
    };

    // set default state on I/O
    gpio.leds.led_0.set_high();
    gpio.leds.led_1.set_high();
    gpio.leds.led_2.set_high();
    gpio.leds.led_can_rx.set_high();
    gpio.leds.led_can_tx.set_high();
    gpio.leds.led_check.set_low();
    gpio.leds.led_mil.set_low();

    gpio.aux.cs_1.set_high();
    gpio.aux.cs_2.set_high();
    gpio.mtr_enable.set_high();
    gpio.pmic.pmic_cs.set_high();
    gpio.memory_cs.set_high();

    return gpio;
}

pub type ISPI = Spi<
    SPI2,
    (
        Pin<'B', 10, Alternate<5>>,
        Pin<'B', 14, Alternate<5>>,
        Pin<'B', 15, Alternate<5>>,
    ),
    false,
>;