use crate::app::engine::efi_cfg::VRSensor;

#[derive(Copy, Clone)]
pub struct VRStatus {
    pub current_time: u32,
    pub current_gap: u32,
    pub target_gap: u32,
    pub is_missing_tooth: bool,
    pub tooth_last_minus_one_tooth_time: u32,
    pub tooth_last_time: u32,
    // esta se comparte con el loop para detectar stall
    pub tooth_current_count: u32,
    pub tooth_one_time: u32,
    pub tooth_one_minus_one_time: u32,
    pub has_sync: bool,
    // por ahora es el 'half-sinc' de speeduino
    pub sync_loss_counter: u128,
    pub start_revolution: u128,
    pub last_rpm: u32,
    //todo: ver de mover a structs mas peques
    pub revolution_time: u32,
    pub degreesPeruSx32768: f32,
}

impl VRStatus {
    pub fn new() -> VRStatus {
        return VRStatus {
            current_time: 0,
            current_gap: 0,
            target_gap: 0,
            is_missing_tooth: false,
            tooth_last_minus_one_tooth_time: 0,
            tooth_last_time: 0,
            tooth_current_count: 0,
            tooth_one_time: 0,
            tooth_one_minus_one_time: 0,
            has_sync: false,
            sync_loss_counter: 0,
            start_revolution: 0,
            last_rpm: 0,
            revolution_time: 0,
            degreesPeruSx32768: 0.0,
        };
    }

    pub fn reset(&mut self) {
        self.current_time = 0;
        self.current_gap = 0;
        self.target_gap = 0;
        self.is_missing_tooth = false;
        self.tooth_last_minus_one_tooth_time = 0;
        self.tooth_last_time = 0;
        self.tooth_current_count = 0;
        self.tooth_one_time = 0;
        self.tooth_one_minus_one_time = 0;
        self.has_sync = false;
        self.sync_loss_counter = 0;
        self.start_revolution = 0;
        self.last_rpm = 0;
        self.revolution_time = 0;
        self.degreesPeruSx32768 = 0.0;
    }
}

pub fn get_cranking_rpm(trigger: &mut VRStatus, trigger_config: &VRSensor) -> u32 {
    let mut temp_rpm = 0;

    // rev desde el arranque para habilitar el encendido (para estabilizar todo el chisme)
    if /* trigger.start_revolution >= 50 && */ trigger.has_sync {
        if (trigger.tooth_last_time > 0) && (trigger.tooth_last_minus_one_tooth_time > 0) && (trigger.tooth_last_time > trigger.tooth_last_minus_one_tooth_time) {
            let mut revolution_time = (trigger.tooth_last_time - trigger.tooth_last_minus_one_tooth_time) * trigger_config.tooth_count;
            //TODO: Make configurable (720/360/etc)
            if 720 == 720 {
                revolution_time = revolution_time / 2;
            }
            // us in minute
            temp_rpm = 60_000_000 / revolution_time;
            // filtrado por ruido (18k max rpm)
            if temp_rpm >= 18_000 {
                temp_rpm = trigger.last_rpm;
            }
            trigger.revolution_time = revolution_time;
        }
    } else { return 0; }
    trigger.last_rpm = temp_rpm;

    return temp_rpm;
}

pub fn angle_to_time(trigger: &VRStatus, angle: &u32) -> i32 {
    // por ahora la estimacion de tiempo mas simple
    return ((*angle * trigger.revolution_time) / 360) as i32;
}

pub fn time_to_angle(trigger: &VRStatus, time: &u32) -> i32 {
    // por ahora la estimacion de tiempo mas simple
    return (*time as f32 * trigger.degreesPeruSx32768) as i32 / 32768;
}

pub fn get_crank_angle(trigger: &VRStatus, trigger_config: &VRSensor, cpu_tick: u32) -> i32 {
    const CRANK_ANGLE_MAX: i32 = 360;

    //Number of teeth that have passed since tooth 1, multiplied by the angle each tooth represents, plus the angle that tooth 1 is ATDC. This gives accuracy only to the nearest tooth.
    let mut crank_angle = ((trigger.tooth_current_count - 1) as f32 * trigger_config.trigger_tooth_angle) as i32 + 20;

    let elapsed_time = cpu_tick - trigger.tooth_last_time;
    crank_angle += time_to_angle(&trigger, &elapsed_time);

    if crank_angle >= 720 {
        crank_angle -= 720;
    } else if crank_angle > CRANK_ANGLE_MAX {
        crank_angle -= CRANK_ANGLE_MAX;
    }

    if crank_angle < 0 {
        crank_angle += CRANK_ANGLE_MAX;
    }

    return crank_angle;
}