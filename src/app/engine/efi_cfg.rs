use crate::app::memory::tables::PlotData;

#[derive(serde::Serialize, serde::Deserialize, Debug, Copy, Clone)]
pub struct VRSensor {
    pub trigger_tooth_angle: f32,
    pub tooth_count: u32,
    pub trigger_filter_time: u32,
    pub missing_tooth: u32,
    pub trigger_actual_teeth: u32,
    pub sync_tooth_count: u32,
    pub max_stall_time: u32,
}

#[derive(serde::Serialize, serde::Deserialize, Debug, Copy, Clone)]
pub struct Engine {
    pub cylinder_count: u8,
    pub displacement: u32,
    pub max_rpm: u32,
    pub ckp: VRSensor,
}

#[derive(serde::Serialize, serde::Deserialize, Debug, Copy, Clone)]
pub struct InjectorConfig {
    pub flow_cc_min: f32,
    pub injector_count: u8,
    pub fuel_pressure: f32,
    pub fuel_density: f32,
    pub on_time: f32,
    pub off_time: f32,

    pub battery_correction: Option<PlotData>,
}

#[allow(non_snake_case)]
#[derive(serde::Serialize, serde::Deserialize, Debug, Copy, Clone)]
pub struct InjectionConfig {
    pub target_lambda: f32,
    pub target_stoich: f32,
    pub enable_alphaN: bool,
    pub enable_speedDensity: bool,
    pub injector: InjectorConfig,
}

#[derive(serde::Serialize, serde::Deserialize, Debug, Copy, Clone)]
pub struct EngineConfig {
    pub ready: bool,
    pub injection: InjectionConfig,
    pub engine: Engine,
}

impl EngineConfig {
    pub fn new() -> EngineConfig {
        get_default_efi_cfg()
    }
}


impl VRSensor {
    pub fn new() -> VRSensor {
        VRSensor {
            trigger_tooth_angle: 0.0,
            tooth_count: 0,
            trigger_filter_time: 0,
            missing_tooth: 0,
            trigger_actual_teeth: 0,
            sync_tooth_count: 0,
            max_stall_time: 0,
        }
    }
}

pub fn get_default_efi_cfg() -> EngineConfig {
    let mut cfg = EngineConfig {
        ready: false,
        engine: Engine {
            cylinder_count: 4,
            displacement: 1596,
            max_rpm: 7000,
            ckp: VRSensor {
                trigger_tooth_angle: 0.0,
                tooth_count: 60,
                trigger_filter_time: 0,
                missing_tooth: 1,
                trigger_actual_teeth: 0,
                sync_tooth_count: 0,
                max_stall_time: 0,
            },
        },
        injection: InjectionConfig {
            target_lambda: 1.1,
            target_stoich: 14.7,
            enable_alphaN: true,
            enable_speedDensity: false,
            injector: InjectorConfig {
                flow_cc_min: 110.0,
                injector_count: 4,
                fuel_pressure: 1.0,
                fuel_density: 0.726,
                // promedio a "ojimetro" de:
                // https://documents.holley.com/techlibrary_terminatorxv2injectordata.pdf
                on_time: 750.0,
                off_time: 250.0,
                // tabla correccion por bateria:
                battery_correction: None,
            },
        },
    };

    //The number of physical teeth on the wheel. Doing this here saves us a calculation each time in the interrupt
    cfg.engine.ckp.trigger_actual_teeth = cfg.engine.ckp.tooth_count - cfg.engine.ckp.missing_tooth;

    //The number of degrees that passes from tooth to tooth
    cfg.engine.ckp.trigger_tooth_angle = 360f32 / cfg.engine.ckp.tooth_count as f32;

    //Trigger filter time is the shortest possible time (in uS) that there can be between crank teeth (ie at max RPM). Any pulses that occur faster than this time will be discarded as noise
    cfg.engine.ckp.trigger_filter_time = 1000000 / (cfg.engine.max_rpm / 60 * cfg.engine.ckp.tooth_count);

    //Minimum 50rpm. (3333uS is the time per degree at 50rpm)
    cfg.engine.ckp.max_stall_time = (3333f32 * cfg.engine.ckp.trigger_tooth_angle * (cfg.engine.ckp.missing_tooth + 1) as f32) as u32;

    //50% of the total teeth.
    cfg.engine.ckp.sync_tooth_count = cfg.engine.ckp.tooth_count / 2;

    return cfg;
}
