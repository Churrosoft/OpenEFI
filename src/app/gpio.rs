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
}

pub struct IgnitionGpioMapping {
    pub ecn_1: gpio::PD10<Output<PushPull>>,
    pub ecn_2: gpio::PD11<Output<PushPull>>,
}

pub struct PMICGpioMapping {
    pub pmic1_enable: gpio::PB12<Output<PushPull>>,
    pub pmic1_cs: gpio::PB11<Output<PushPull>>,
    pub pmic2_enable: gpio::PB13<Output<PushPull>>,
    pub pmic2_cs: gpio::PB9<Output<PushPull>>,
}

pub struct LedGpioMapping {
    pub led_0: gpio::PC1<Output<PushPull>>,
    pub led_1: gpio::PC2<Output<PushPull>>,
    pub led_2: gpio::PC3<Output<PushPull>>,
    pub led_can_tx: gpio::PD6<Output<PushPull>>,
    pub led_can_rx: gpio::PD7<Output<PushPull>>,
    pub led_check: gpio::PB1<Output<PushPull>>,
    pub led_mil: gpio::PA8<Output<PushPull>>,
}

pub struct AuxIoMapping {

    // on external connector
    pub in_1: gpio::PD12<Input>,
    pub in_2: gpio::PD13<Input>,
    pub in_3: gpio::PD14<Input>,
    pub in_4: gpio::PD15<Input>,

    // on expansion header

    pub in_5: gpio::PB5<Input>,
    pub in_6: gpio::PB7<Input>,
    pub in_7: gpio::PB8<Input>,
    pub in_8: gpio::PE1<Input>,


    pub out_1: gpio::PC4<Output<PushPull>>,
    pub out_2: gpio::PC5<Output<PushPull>>,
    pub out_3: gpio::PB0<Output<PushPull>>,
    pub out_4: gpio::PA10<Output<PushPull>>,

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
    pub baro_cs: gpio::PA6<Output<PushPull>>,

    pub mux_a: gpio::PD3<Output<PushPull>>,
    pub mux_b: gpio::PD4<Output<PushPull>>,
    pub mux_c: gpio::PD5<Output<PushPull>>,
    pub analog_in: gpio::PA7<Analog>,
}

pub struct ADC_DMA_Mapping {
    pub tps: gpio::PA0<Analog>,
    pub iat: gpio::PA1<Analog>,
    pub map: gpio::PA2<Analog>,
    pub clt: gpio::PA3<Analog>,
    pub o2: gpio::PA4<Analog>,
    pub vbatt: gpio::PA5<Analog>,
}


pub struct StepperMapping{
    pub step: gpio::PE7<Output<PushPull>>,
    pub dir: gpio::PE8<Output<PushPull>>,
    pub fault: gpio::PE9<Output<PushPull>>,
    pub enable: gpio::PE10<Output<PushPull>>,
}

pub struct SDCard {
    pub dat_0: Pin<'C', 8>,
    pub dat_1: Pin<'C', 9>,
    pub dat_2: Pin<'C', 10>,
    pub dat_3: Pin<'C',11>,
    pub dat_clk: gpio::PC12,
    pub cmd: Pin<'D', 2>,

    pub cd: gpio::PB6<Input>,
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

    pub sd_card: SDCard,

    pub adc: ADCMapping,
    pub adc_dma: ADC_DMA_Mapping,
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
            led_0: gpio_c.pc1.into_push_pull_output(),
            led_1: gpio_c.pc2.into_push_pull_output(),
            led_2: gpio_c.pc3.into_push_pull_output(),
            led_can_tx: gpio_d.pd6.into_push_pull_output(),
            led_can_rx: gpio_d.pd7.into_push_pull_output(),
            led_check: gpio_b.pb1.into_push_pull_output(),
            led_mil: gpio_a.pa8.into_push_pull_output(),
        },

        // Injection
        injection: InjectionGpioMapping {
            iny_1: gpio_d.pd8.into_push_pull_output(),
            iny_2: gpio_d.pd9.into_push_pull_output(),
        },

        // Ignition
        ignition: IgnitionGpioMapping {
            ecn_1: gpio_d.pd10.into_push_pull_output(),
            ecn_2: gpio_d.pd11.into_push_pull_output(),
        },

        // PMIC
        pmic: PMICGpioMapping {
            pmic1_enable: gpio_b.pb12.into_push_pull_output(),
            pmic1_cs: gpio_b.pb11.into_push_pull_output(),
            pmic2_enable: gpio_b.pb13.into_push_pull_output(),
            pmic2_cs: gpio_b.pb9.into_push_pull_output(),
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
            in_1: gpio_d.pd12.into_input(),
            in_2: gpio_d.pd13.into_input(),
            in_3: gpio_d.pd14.into_input(),
            in_4: gpio_d.pd15.into_input(),
            in_5: gpio_b.pb5.into_input(),
            in_6: gpio_b.pb7.into_input(),
            in_7: gpio_b.pb8.into_input(),
            in_8: gpio_e.pe1.into_input(),

            out_1: gpio_c.pc4.into_push_pull_output(),
            out_2: gpio_c.pc5.into_push_pull_output(),
            out_3: gpio_b.pb0.into_push_pull_output(),
            out_4: gpio_a.pa10.into_push_pull_output(),

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

        adc_dma: ADC_DMA_Mapping {
            tps: gpio_a.pa0.into_analog(),
            iat: gpio_a.pa1.into_analog(),
            map:gpio_a.pa2.into_analog(),
            clt: gpio_a.pa3.into_analog(),
            o2: gpio_a.pa4.into_analog(),
            vbatt: gpio_a.pa5.into_analog(),
        },

        adc: ADCMapping {

            baro_cs: gpio_a.pa6.into_push_pull_output(),

            mux_a: gpio_d.pd3.into_push_pull_output(),
            mux_b: gpio_d.pd4.into_push_pull_output(),
            mux_c: gpio_d.pd5.into_push_pull_output(),

            analog_in: gpio_a.pa7.into_analog(),
        },

        sd_card: SDCard {dat_0 : gpio_c.pc8.internal_pull_up(true),
            dat_1 : gpio_c.pc9.internal_pull_up(true),
            dat_2 : gpio_c.pc10.internal_pull_up(true),
            dat_3 : gpio_c.pc11.internal_pull_up(true),
            dat_clk : gpio_c.pc12,
            cmd : gpio_d.pd2.internal_pull_up(true),
            cd: gpio_b.pb6.into_input(),
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

    gpio.pmic.pmic1_cs.set_high();
    gpio.pmic.pmic2_cs.set_high();
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
