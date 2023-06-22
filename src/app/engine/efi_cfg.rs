use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, Debug)]
pub struct Engine {
    pub cilinder_count: u8,
    pub displacement: u32,
    pub ckp_tooth_count: u32,
    pub ckp_missing_tooth: u32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct InjectorConfig {
    pub flow_cc_min: f32,
    pub injector_count: u8,
    pub fuel_pressure: f32,
    pub fuel_density: f32,
    pub on_time: f32,
    pub off_time: f32,
}

#[allow(non_snake_case)]
#[derive(Serialize, Deserialize, Debug)]
pub struct InjectionConfig {
    pub target_lambda: f32,
    pub target_stoich: f32,
    pub enable_alphaN: bool,
    pub enable_speedDensity: bool,
    pub injector: InjectorConfig,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct EngineConfig {
    pub ready: bool,
    pub injection: InjectionConfig,
    pub engine: Engine,
}

impl EngineConfig {
    pub fn new()->EngineConfig{
        get_default_efi_cfg()
    }
}

pub fn get_default_efi_cfg() -> EngineConfig {
    let cfg = EngineConfig {
        ready: false,
        engine: Engine {
            cilinder_count: 4,
            displacement: 1596,
            ckp_tooth_count: 60,
            ckp_missing_tooth: 2,
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
            },
        },
    };

    return cfg;
}
