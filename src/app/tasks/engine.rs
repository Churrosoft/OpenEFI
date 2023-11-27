// use fugit::Duration;
use rtic::Mutex;
use rtic::mutex_prelude::{TupleExt03, TupleExt04};
use stm32f4xx_hal::gpio::ExtiPin;
use rtic_monotonics::systick::*;
use crate::{
    app,
};
use crate::app::engine::cpwm::{angle_to_time, get_crank_angle};

use crate::app::engine::efi_cfg::VRSensor;

// from: https://github.com/noisymime/speeduino/blob/master/speeduino/decoders.ino#L453
pub(crate) fn ckp_trigger(mut ctx: app::ckp_trigger::Context) {
    let mut efi_cfg = ctx.shared.efi_cfg;
    let mut efi_status = ctx.shared.efi_status;

    let mut ckp_status = ctx.shared.ckp;

    let mut ckp = VRSensor::new();
    let mut rpm = 0;

    // lock previo y libero los recursos que no se vuelven a usar para otra cosa aca adentro
    efi_cfg.lock(|cfg| { ckp = cfg.engine.ckp });
    efi_status.lock(|status| { rpm = status.rpm });

    ckp_status.lock(|ckp_status| {
        ctx.shared.timer4.lock(|t4| { ckp_status.current_time = t4.now().ticks(); });
        ckp_status.current_gap = ckp_status.current_time - ckp_status.tooth_last_time;

        if ckp_status.current_gap >= ckp.trigger_filter_time {
            ckp_status.tooth_current_count += 1;


            if ckp_status.tooth_last_time > 0 && ckp_status.tooth_last_minus_one_tooth_time > 0 {
                ckp_status.is_missing_tooth = false;

                /*
                  Performance Optimisation:
                  Only need to try and detect the missing tooth if:
                  1. WE don't have sync yet
                  2. We have sync and are in the final 1/4 of the wheel (Missing tooth will/should never occur in the first 3/4)
                  3. RPM is under 2000. This is to ensure that we don't interfere with strange timing when cranking or idling. Optimisation not really required at these speeds anyway
                */
                if ckp_status.has_sync == false || rpm < 2000 || ckp_status.tooth_current_count >= (3 * ckp.trigger_actual_teeth >> 2) {
                    //Begin the missing tooth detection
                    //If the time between the current tooth and the last is greater than 1.5x the time between the last tooth and the tooth before that, we make the assertion that we must be at the first tooth after the gap
                    if ckp.missing_tooth == 1 {
                        //Multiply by 1.5 (Checks for a gap 1.5x greater than the last one) (Uses bitshift to multiply by 3 then divide by 2. Much faster than multiplying by 1.5)
                        ckp_status.target_gap = (3 * (ckp_status.tooth_last_time - ckp_status.tooth_last_minus_one_tooth_time)) >> 1;
                    } else {
                        //Multiply by 2 (Checks for a gap 2x greater than the last one)
                        ckp_status.target_gap = (ckp_status.tooth_last_time - ckp_status.tooth_last_minus_one_tooth_time) * ckp.missing_tooth;
                    }

                    // initial startup, missing one time
                    if ckp_status.tooth_last_time == 0 || ckp_status.tooth_last_minus_one_tooth_time == 0 {
                        ckp_status.target_gap = 0;
                    }

                    if (ckp_status.current_gap > ckp_status.target_gap) || (ckp_status.tooth_current_count > ckp.trigger_actual_teeth) {
                        //Missing tooth detected
                        ckp_status.is_missing_tooth = true;

                        if ckp_status.tooth_current_count < ckp.trigger_actual_teeth {
                            // This occurs when we're at tooth #1, but haven't seen all the other teeth. This indicates a signal issue so we flag lost sync so this will attempt to resync on the next revolution.
                            ckp_status.has_sync = false;
                            ckp_status.sync_loss_counter += 1;
                        }
                        //This is to handle a special case on startup where sync can be obtained and the system immediately thinks the revs have jumped:
                        else {
                            if ckp_status.has_sync {
                                ctx.shared.led.lock(|l| { l.led_check.toggle() });
                                ckp_status.start_revolution += 1;
                            } else {
                                ckp_status.start_revolution = 0;
                                ctx.shared.led.lock(|l| { l.led_mil.toggle() });
                            }

                            ckp_status.tooth_current_count = 1;

                            // tiempo entre vuelta completa
                            ckp_status.tooth_one_minus_one_time = ckp_status.tooth_one_time;
                            ckp_status.tooth_one_time = ckp_status.current_time;

                            // TODO: hay mas checks aca cuando es con inyección secuencial
                            ckp_status.has_sync = true;
                            //This is used to prevent a condition where serious intermittent signals (Eg someone furiously plugging the sensor wire in and out) can leave the filter in an unrecoverable state
                            efi_cfg.lock(|ec| { ec.engine.ckp.trigger_filter_time = 0; });
                            ckp_status.tooth_last_minus_one_tooth_time = ckp_status.tooth_last_time;
                            ckp_status.tooth_last_time = ckp_status.current_time;
                            // ctx.shared.ckp_tooth_last_time.lock(|ckp_t| { *ckp_t = ckp_status.current_time });
                        }
                    }
                }

                if !ckp_status.is_missing_tooth {
                    efi_cfg.lock(|ec| { ec.engine.ckp.trigger_filter_time = ckp_status.current_gap >> 2; });
                    ckp_status.tooth_last_minus_one_tooth_time = 0;
                }
            } else {
                // initial startup
                ckp_status.tooth_last_minus_one_tooth_time = ckp_status.tooth_last_time;
                ckp_status.tooth_last_time = ckp_status.current_time;
                //  ctx.shared.ckp_tooth_last_time.lock(|ckp_t| { *ckp_t = ckp_status.current_time });
            }
        }
    });
    // Obtain access to the peripheral and Clear Interrupt Pending Flag
    ctx.local.ckp.clear_interrupt_pending_bit();
}

// TODO: add similar stall control of speeduino
// https://github.com/noisymime/speeduino/blob/master/speeduino/speeduino.ino#L146
pub(crate) async fn ckp_checks(mut ctx:  app::ckp_checks::Context<'_>) {

        //  FUTURE: esto a futuro no tendria que usar locks de acuerdo a esto,
        // ya que solo escribo en "ckp", mientras que los otros son solo de lectura por ahora
        // https://rtic.rs/2/book/en/by-example/resources.html#lock-free-access-of-shared-resources
        let efi_cfg = ctx.shared.efi_cfg;
        let mut ckp = ctx.shared.ckp;
        let mut efi_status = ctx.shared.efi_status;
        let mut cycle_time = 0;

        let mut ignition_running = ctx.shared.ignition_running;


        ctx.shared.timer4.lock(|t4| { cycle_time = t4.now().ticks(); });

        (efi_cfg, ckp, efi_status,ignition_running).lock(|cfg, ckp, efi_status,ignition_running| {
            //ste hijodeputa fue por lo que se tosto la bobina
            if ckp.tooth_last_time > cycle_time || cycle_time - ckp.tooth_last_time < 366_667 /* 50RPM */ {
                // RPM & no stall
                // en speeduino revisan "BIT_DECODER_TOOTH_ANG_CORRECT" aca, por ahora no lo agregue al trigger
                // tambien utilizan rpmDOT para ver la variacion cada 100mS, falta implementar
                // TODO: mover a fun aparte

                let mut time_per_degreex16;

                // esto calcula el tiempo por grado desde el tiempo entre los ultimos 2 dientes
                if true /* BIT_DECODER_TOOTH_ANG_CORRECT*/ && (ckp.tooth_last_time > ckp.tooth_last_minus_one_tooth_time) && 20/*(abs(currentStatus.rpmDOT)*/ > 30 {
                    time_per_degreex16 = ((ckp.tooth_last_time - ckp.tooth_last_minus_one_tooth_time) * 16) / cfg.engine.ckp.trigger_tooth_angle as u32;
                    // timePerDegree = time_per_degreex16 / 16;
                } else {
                    //Take into account any likely acceleration that has occurred since the last full revolution completed:
                    //long rpm_adjust = (timeThisRevolution * (long)currentStatus.rpmDOT) / 1000000;
                    let rpm_adjust = 0;
                    time_per_degreex16 = (2_666_656 / efi_status.rpm + rpm_adjust) as u32; //The use of a x16 value gives accuracy down to 0.1 of a degree and can provide noticeably better timing results on low resolution triggers
                    // timePerDegree = time_per_degreex16 / 16;
                }

                // ckp.degreesPeruSx2048 = 2048 / timePerDegree;
                ckp.degreesPeruSx32768 = (524288 / time_per_degreex16) as f32;
            } else {
                ckp.reset();
            }
            cfg.engine.ckp.max_stall_time;
        });

        Systick::delay(100.micros()).await;
        app::ckp_checks::spawn().unwrap();
}