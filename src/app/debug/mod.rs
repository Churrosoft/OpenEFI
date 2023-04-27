use super::gpio_legacy::{AuxIoMapping, IgnitionGpioMapping, InjectionGpioMapping, RelayMapping};

use cortex_m::prelude::{
    _embedded_hal_blocking_delay_DelayMs, _embedded_hal_blocking_delay_DelayUs,
};
use stm32f4xx_hal::{pac::TIM13, timer};

pub fn spark_demo(ecn: &mut IgnitionGpioMapping, tim6: &mut timer::DelayUs<TIM13>) {
    for i in 1..15 {
        ecn.ecn_1.set_high();
        tim6.delay_us(1200u32);
        ecn.ecn_1.set_low();
        tim6.delay_us(1200u32);

        ecn.ecn_2.set_high();
        tim6.delay_us(1200u32);
        ecn.ecn_2.set_low();
        tim6.delay_us(1200u32);

        ecn.ecn_3.set_high();
        tim6.delay_us(1200u32);
        ecn.ecn_3.set_low();
        tim6.delay_us(1200u32);

        ecn.ecn_4.set_high();
        tim6.delay_us(1200u32);
        ecn.ecn_4.set_low();
        tim6.delay_us(1200u32);
    }
    ecn.ecn_1.set_low();
    ecn.ecn_2.set_low();
    ecn.ecn_3.set_low();
    ecn.ecn_4.set_low();
}

pub fn injector_demo(inj: &mut InjectionGpioMapping, tim6: &mut timer::DelayUs<TIM13>) {
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

pub fn external_idle_demo(aux: &mut AuxIoMapping, tim6: &mut timer::DelayUs<TIM13>) {
    for _ in 0..3 {
        (0..45).for_each(|_| {
            aux.out_1.set_low();
            aux.out_2.set_low();
            aux.out_3.set_low();
            aux.out_4.set_high();
            tim6.delay_ms(2_u32);

            aux.out_1.set_low();
            aux.out_2.set_high();
            aux.out_3.set_low();
            aux.out_4.set_low();

            tim6.delay_ms(2_u32);
            aux.out_1.set_low();
            aux.out_2.set_low();
            aux.out_3.set_high();
            aux.out_4.set_low();

            tim6.delay_ms(2_u32);
            aux.out_1.set_high();
            aux.out_2.set_low();
            aux.out_3.set_low();
            aux.out_4.set_low();
            tim6.delay_ms(2_u32);
        });

        (0..45).for_each(|_| {
            aux.out_1.set_high();
            aux.out_2.set_low();
            aux.out_3.set_low();
            aux.out_4.set_low();

            tim6.delay_ms(2_u32);
            aux.out_1.set_low();
            aux.out_2.set_low();
            aux.out_3.set_high();
            aux.out_4.set_low();

            tim6.delay_ms(2_u32);
            aux.out_1.set_low();
            aux.out_2.set_high();
            aux.out_3.set_low();
            aux.out_4.set_low();

            tim6.delay_ms(2_u32);
            aux.out_1.set_low();
            aux.out_2.set_low();
            aux.out_3.set_low();
            aux.out_4.set_high();
            tim6.delay_ms(2_u32);
        });
    }
}

pub fn relay_demo(rly: &mut RelayMapping, tim6: &mut timer::DelayUs<TIM13>) {
    for i in 1..10 {
        rly.ac.set_high();
        rly.gnc.set_high();
        rly.iny.set_high();
        rly.lmb.set_high();
        tim6.delay_ms(250u32);

        rly.ac.set_low();
        rly.gnc.set_low();
        rly.iny.set_low();
        rly.lmb.set_low();
        tim6.delay_ms(250u32);
    }

    rly.ac.set_low();
    rly.gnc.set_low();
    rly.iny.set_low();
    rly.lmb.set_low();
}
