use rtic::Mutex;
use crate::app;
use rtic_monotonics::systick::*;
use rtic_monotonics::systick::fugit::{Duration, TimerDurationU32};
use crate::app::engine::cpwm::{angle_to_time, get_crank_angle};

pub async fn ignition_checks(mut ctx: app::ignition_checks::Context<'_>){
    loop {
        let mut cycle_time = ctx.shared.timer4.lock(|t4| t4.now().ticks());
        let ckp = ctx.shared.ckp.lock(|ckp| ckp.clone());
        let ckp_config = ctx.shared.efi_cfg.lock(|cfg| cfg.engine.ckp);

        // ignition timing:
        let mut crank_angle = get_crank_angle(&ckp, &ckp_config, cycle_time);
        const CRANK_ANGLE_MAX_IGN: i32 = 720; //ponele? no entendi bien como se setea dependiendo el tipo de encendido/cilindros
        while crank_angle > CRANK_ANGLE_MAX_IGN { crank_angle -= CRANK_ANGLE_MAX_IGN; } // SDUBTUD: no entendi para que es esto pero quien soy para cuestionar a speeduino

        let dwell_angle = 20; //TODO: get from table
        let dwell_time = angle_to_time(&ckp,&dwell_angle);

        // esto luego se repetiria para el resto de canales
        // TODO: compensar drift del clock por lo que tarda en ejectuarse la task
        if !*ctx.local.ign_channel_1 && dwell_time > 0 {
            *ctx.local.ign_channel_1 = true;
            app::ignition_trigger::spawn(dwell_time).unwrap();
        }
        Systick::delay(100.micros()).await;
    }
}

//SDUBTUD: si esto anda es de puro milagro
pub async fn ignition_trigger(mut ctx: app::ignition_trigger::Context<'_>,dwell_time:i32){
    let dwell_ticks = (120_000_000 + 10_000 / 2) / 10_000 - 1u32;

    let dwell_duration = TimerDurationU32::<10_000>::from_ticks(dwell_ticks);
    Systick::delay(dwell_duration).await;

    // FIXME: por ahora solo prendo el led, luego implementar las bobinas/resto canales
    ctx.shared.led.lock(|l| l.led_check.set_low());
    ctx.shared.timer3.lock(|t3| t3.start((50).millis()).unwrap()); // este tiempo estaria bien para probar
}