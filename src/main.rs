#![feature(type_alias_impl_trait)]
#![feature(exclusive_range_pattern)]
#![feature(proc_macro_hygiene)]
#![feature(int_roundings)]
#![feature(is_some_and)]
#![feature(stdsimd)]

#![no_main]
#![no_std]

#![allow(stable_features)]
#![allow(unused_mut)]


use panic_halt as _;
use rtic;
//::app;
use rtic_monotonics::systick::*;
use stm32f4xx_hal::gpio::{Output, PushPull, PC13, PC14};

use stm32f4xx_hal::{
    adc::{Adc, config::AdcConfig},
    crc32,
    crc32::Crc32,
    gpio::{Edge, Input},
    otg_fs,
    otg_fs::{USB, UsbBusType},
    pac::{ADC1, TIM13, TIM2, TIM3, TIM5},
    prelude::*,
    spi::*,
    timer::{self, Event},
};

//wip
mod app;

use crate::app::{
    gpio_legacy::{init_gpio, LedGpioMapping},
    logging::host,
};

#[rtic::app(device = stm32f4xx_hal::pac, peripherals = true, dispatchers = [TIM4, TIM7, TIM8_CC])]
mod open_efi {
    use super::*;

    #[shared]
    struct Shared {
        led: LedGpioMapping,
    }

    #[local]
    struct Local {
        state: bool,
        state2: bool,
    }

    #[init]
    fn init(cx: init::Context) -> (Shared, Local) {
        // Setup clocks
        //let mut flash = cx.device.FLASH.constrain();
        let mut device = cx.device;
        let mut rcc = device.RCC.constrain();

        // Initialize the systick interrupt & obtain the token to prove that we did
        let systick_mono_token = rtic_monotonics::create_systick_token!();
        Systick::start(cx.core.SYST, 120_000_000, systick_mono_token); // default STM32F407 clock-rate is 36MHz

        let _clocks = rcc
            .cfgr
            .use_hse(25.MHz())
            .sysclk(120.MHz())
            .require_pll48clk();

        let gpio_a = device.GPIOA.split();
        let gpio_b = device.GPIOB.split();
        let gpio_c = device.GPIOC.split();
        let gpio_d = device.GPIOD.split();
        let gpio_e = device.GPIOE.split();

        let mut gpio_config = init_gpio(gpio_a, gpio_b, gpio_c, gpio_d, gpio_e);

        // ADC
        let mut adc = Adc::adc1(device.ADC1, true, AdcConfig::default());

        adc.enable();
        adc.calibrate();

        // configure CKP/CMP Pin for Interrupts
        let mut ckp = gpio_config.ckp;
        let mut syscfg = device.SYSCFG.constrain();
        host::debug!("init gpio");
        ckp.make_interrupt_source(&mut syscfg);
        ckp.trigger_on_edge(&mut device.EXTI, Edge::Falling);

        // Schedule the blinking task
        blink::spawn().ok();
        blink2::spawn().ok();

        (Shared { led: gpio_config.led }, Local { state: false, state2: false })
    }

    #[task(local = [state], shared = [led])]
    async fn blink(mut cx: blink::Context) {
        loop {
            if *cx.local.state {
                cx.shared.led.lock(|l| l.led_0.set_high());
                *cx.local.state = false;
            } else {
                cx.shared.led.lock(|l| l.led_0.set_low());
                *cx.local.state = true;
            }
            Systick::delay(100.millis()).await;
        }
    }

    #[task(local = [state2], shared = [led])]
    async fn blink2(mut cx: blink2::Context) {
        loop {
            if *cx.local.state2 {
                cx.shared.led.lock(|l| l.led_1.set_high());
                *cx.local.state2 = false;
            } else {
                cx.shared.led.lock(|l| l.led_1.set_low());
                *cx.local.state2 = true;
            }
            Systick::delay(50.millis()).await;
        }
    }
}