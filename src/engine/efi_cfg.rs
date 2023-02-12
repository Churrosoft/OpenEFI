pub struct InjectorConfig {
    pub flow_cc_min: f32,
    pub injector_count: u8,
    pub fuel_pressure: f32,
    pub fuel_density: f32,
    pub on_time: f32,
    pub off_time: f32,
}

pub struct InjectionConfig {
    pub target_lambda: f32,
    pub target_stoich: f32,
    pub enable_alphaN: bool,
    pub enable_speedDensity: bool,
    pub injector: InjectorConfig,
}

pub struct EngineConfig {
    pub ready: bool,
    pub injection: InjectionConfig,
}

pub fn get_default_efi_cfg() -> EngineConfig {
    let cfg = EngineConfig {
        ready: false,
        injection: InjectionConfig {
            target_lambda: 0.0,
            target_stoich: 0.0,
            enable_alphaN: false,
            enable_speedDensity: false,
            injector: InjectorConfig {
                flow_cc_min: 0.0,
                injector_count: 0,
                fuel_pressure: 0.0,
                fuel_density: 0.0,
                on_time: 0.0,
                off_time: 0.0,
            },
        },
    };

    return cfg;
}
