// FOR uEFI v3.x boards

use stm32f4xx_hal::{
    gpio::{
        self, gpioa, gpiob, gpioc, gpiod, gpioe, Alternate, Analog, Input, Output, Pin, PushPull,
    },
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
    pub in_1: gpio::PC4<Input>,
    pub in_2: gpio::PC5<Input>,

    pub out_1: gpio::PB5<Output<PushPull>>,
    pub out_2: gpio::PB6<Output<PushPull>>,
    pub out_3: gpio::PB7<Output<PushPull>>,
    pub out_4: gpio::PB8<Output<PushPull>>,

    pub cs_1: gpio::PE14<Output<PushPull>>,
    pub cs_2: gpio::PE15<Output<PushPull>>,
}

pub struct RelayMapping {
    pub iny: gpio::PE2<Output<PushPull>>,
    pub gnc: gpio::PE3<Output<PushPull>>,
    pub ac: gpio::PE4<Output<PushPull>>,
    pub lmb: gpio::PE0<Output<PushPull>>,
}

pub struct ADCMapping {
    pub mux_a: gpio::PD2<Output<PushPull>>,
    pub mux_b: gpio::PD3<Output<PushPull>>,
    pub mux_c: gpio::PD4<Output<PushPull>>,
    pub analog_in: gpio::PA0<Analog>,
}

pub struct StepperMapping{
    pub step: gpio::PE7<Output<PushPull>>,
    pub dir: gpio::PE8<Output<PushPull>>,
    pub fault: gpio::PE9<Output<PushPull>>,
    pub enable: gpio::PE10<Output<PushPull>>,
}

pub struct GpioMapping {
    // LED's / User feedback
    pub led: LedGpioMapping,

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
    pub stepper: StepperMapping,

    // SPI Flash
    pub memory_cs: gpio::PE13<Output<PushPull>>,

    // AUX I/O
    pub aux: AuxIoMapping,

    pub usb_dp: gpio::PA11<Alternate<10, PushPull>>,
    pub usb_dm: gpio::PA12<Alternate<10, PushPull>>,

    pub spi_sck: gpio::PB10<Alternate<5>>,
    pub spi_miso: gpio::PB14<Alternate<5>>,
    pub spi_mosi: gpio::PB15<Alternate<5, PushPull>>,

    pub adc: ADCMapping,
}

pub fn init_gpio(
    gpio_a: gpioa::Parts,
    gpio_b: gpiob::Parts,
    gpio_c: gpioc::Parts,
    gpio_d: gpiod::Parts,
    gpio_e: gpioe::Parts,
) -> GpioMapping {
    let mut gpio = GpioMapping {
        // LED's / User feedback
        led: LedGpioMapping {
            led_0: gpio_c.pc13.into_push_pull_output(),
            led_1: gpio_c.pc14.into_push_pull_output(),
            led_2: gpio_c.pc15.into_push_pull_output(),
            led_can_tx: gpio_d.pd6.into_push_pull_output(),
            led_can_rx: gpio_d.pd7.into_push_pull_output(),
            led_check: gpio_c.pc9.into_push_pull_output(),
            led_mil: gpio_c.pc8.into_push_pull_output(),
        },

        // Injection
        injection: InjectionGpioMapping {
            iny_1: gpio_d.pd8.into_push_pull_output(),
            iny_2: gpio_d.pd9.into_push_pull_output(),
            iny_3: gpio_d.pd10.into_push_pull_output(),
            iny_4: gpio_d.pd11.into_push_pull_output(),
        },

        // Ignition
        ignition: IgnitionGpioMapping {
            ecn_1: gpio_d.pd12.into_push_pull_output(),
            ecn_2: gpio_d.pd13.into_push_pull_output(),
            ecn_3: gpio_d.pd14.into_push_pull_output(),
            ecn_4: gpio_d.pd15.into_push_pull_output(),
        },

        // PMIC
        pmic: PMICGpioMapping {
            pmic_enable: gpio_b.pb12.into_push_pull_output(),
            pmic_spark: gpio_b.pb13.into_input(),
            pmic_cs: gpio_a.pa15.into_push_pull_output(),
            pmic_nomi: gpio_c.pc10.into_input(),
            pmic_maxi: gpio_c.pc11.into_input(),
        },

        // CKP/CMP
        ckp: gpio_c.pc6.into_input(),
        cmp: gpio_c.pc7.into_input(),

        // RELAY's
        relay: RelayMapping {
            iny: gpio_e.pe2.into_push_pull_output(),
            gnc: gpio_e.pe3.into_push_pull_output(),
            ac: gpio_e.pe4.into_push_pull_output(),
            lmb: gpio_e.pe0.into_push_pull_output(),
        },

        // Step-Step motor TODO
        stepper: StepperMapping {
            step: gpio_e.pe7.into_push_pull_output(),
            dir: gpio_e.pe8.into_push_pull_output(),
            fault: gpio_e.pe9.into_push_pull_output(),
            enable: gpio_e.pe10.into_push_pull_output(),
        },

        // SPI Flash
        memory_cs: gpio_e.pe13.into_push_pull_output(),

        // AUX I/O
        aux: AuxIoMapping {
            in_1: gpio_c.pc4.into_input(),
            in_2: gpio_c.pc5.into_input(),

            out_1: gpio_b.pb5.into_push_pull_output(),
            out_2: gpio_b.pb6.into_push_pull_output(),
            out_3: gpio_b.pb7.into_push_pull_output(),
            out_4: gpio_b.pb8.into_push_pull_output(),

            cs_1: gpio_e.pe14.into_push_pull_output(),
            cs_2: gpio_e.pe15.into_push_pull_output(),
        },

        // USB
        usb_dp: gpio_a.pa11.into_alternate(),
        usb_dm: gpio_a.pa12.into_alternate(),

        // SPI
        spi_sck: gpio_b.pb10.into_alternate(),
        spi_miso: gpio_b.pb14.into_alternate(),
        spi_mosi: gpio_b.pb15.into_alternate().internal_pull_up(true),

        adc: ADCMapping {
            mux_a: gpio_d.pd2.into_push_pull_output(),
            mux_b: gpio_d.pd3.into_push_pull_output(),
            mux_c: gpio_d.pd4.into_push_pull_output(),
            analog_in: gpio_a.pa0.into_analog(),
        },
    };

    // set default state on I/O
    gpio.led.led_0.set_high();
    gpio.led.led_1.set_high();
    gpio.led.led_2.set_high();
    gpio.led.led_can_rx.set_high();
    gpio.led.led_can_tx.set_high();
    gpio.led.led_check.set_low();
    gpio.led.led_mil.set_low();

    gpio.aux.cs_1.set_high();
    gpio.aux.cs_2.set_high();
    gpio.stepper.enable.set_high();
    // 4231 reversa
    // 1234 derecho

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
