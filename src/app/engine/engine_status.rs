use crate::app::engine::sensors::SensorValues;

#[allow(non_camel_case_types)]
#[repr(u8)]
#[derive(Debug)]
pub enum __rpm_status {
    STOPPED = 0, // < 50
    SPIN_UP,     // > 50
    CRANK,       // > 400 rpm
    RUNNING,     // > 750
}

// #[allow(non_camel_case_types)]
#[repr(u8)]
#[derive(Debug)]
pub enum InjectionStatus {
    FuelCutoff = 0,
    FuelIdle,
    FullLoad,
    FuelAcc,
}

#[allow(non_snake_case)]
#[derive(Debug)]
pub struct InjectionInfo {
    pub targetAFR: f32,
    pub injection_bank_1_time: f32,
    pub injection_bank_2_time: f32,
    pub air_flow: f32,
    pub base_air: f32,
    pub base_fuel: f32,
    pub fuel_flow_rate: f32,
    pub fuel_load: f32,
    pub injection_status: InjectionStatus,
}

#[derive(Debug)]
pub struct EngineStatus {
    pub injection: InjectionInfo,
    pub cycle_tick: u32,
    pub cycle_duration: f32,
    pub cycle_status: __rpm_status,
    pub rpm: i32,
    pub sensors: SensorValues,
}

pub fn get_default_engine_status() -> EngineStatus {
    let status = EngineStatus {
        sensors: SensorValues::new(),
        injection: InjectionInfo {
            targetAFR: 0.0,
            injection_bank_1_time: 0.0,
            injection_bank_2_time: 0.0,
            air_flow: 0.0,
            base_air: 0.0,
            base_fuel: 0.0,
            fuel_flow_rate: 0.0,
            fuel_load: 0.0,
            injection_status: InjectionStatus::FuelCutoff,
        },
        cycle_tick: 0,
        cycle_duration: 0.0,
        cycle_status: __rpm_status::STOPPED,
        rpm: 0,
    };
    return status;
}
