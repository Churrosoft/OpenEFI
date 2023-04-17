use super::gpio_legacy::{IgnitionGpioMapping, InjectionGpioMapping};

use cortex_m::prelude::_embedded_hal_blocking_delay_DelayUs;
use stm32f4xx_hal::pac::TIM6;
use stm32f4xx_hal::timer;

pub fn spark_demo(ecn: &mut IgnitionGpioMapping, tim6: &mut timer::DelayUs<TIM6>) {
    for i in 1..15 {
        ecn.ecn_1.set_high();
        tim6.delay_us(120u32);
        ecn.ecn_1.set_low();
        tim6.delay_us(120u32);

        ecn.ecn_2.set_high();
        tim6.delay_us(120u32);
        ecn.ecn_2.set_low();
        tim6.delay_us(120u32);

        ecn.ecn_3.set_high();
        tim6.delay_us(120u32);
        ecn.ecn_3.set_low();
        tim6.delay_us(120u32);

        ecn.ecn_4.set_high();
        tim6.delay_us(120u32);
        ecn.ecn_4.set_low();
        tim6.delay_us(120u32);
    }
    ecn.ecn_1.set_low();
    ecn.ecn_2.set_low();
    ecn.ecn_3.set_low();
    ecn.ecn_4.set_low();
}

pub fn injector_demo(inj: &mut InjectionGpioMapping, tim6: &mut timer::DelayUs<TIM6>) {
    for i in 1..15 {
        inj.iny_1.set_high();
        tim6.delay_us(320u32);
        inj.iny_1.set_low();
        tim6.delay_us(320u32);

        inj.iny_2.set_high();
        tim6.delay_us(320u32);
        inj.iny_2.set_low();
        tim6.delay_us(320u32);

        inj.iny_3.set_high();
        tim6.delay_us(320u32);
        inj.iny_3.set_low();
        tim6.delay_us(320u32);

        inj.iny_4.set_high();
        tim6.delay_us(320u32);
        inj.iny_4.set_low();
        tim6.delay_us(320u32);
    }

    inj.iny_1.set_low();
    inj.iny_2.set_low();
    inj.iny_3.set_low();
    inj.iny_4.set_low();
}
