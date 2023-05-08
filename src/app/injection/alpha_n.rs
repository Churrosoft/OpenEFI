#![allow(dead_code)]

use crate::app::engine::{
    efi_cfg::EngineConfig, engine_status::EngineStatus, get_engine_cycle_duration,
};

use super::injectors::fuel_mass_to_time;

const STD_AIR_PRES: f32 = 101.325f32;
const STD_AIR_TEMP: f32 = 293.15f32;
const AIR_R: f32 = 8.31446261815324f32 / 28.9647f32;

pub fn calculate_injection_fuel(es: &mut EngineStatus, ecfg: &EngineConfig) -> f32 {
    if es.rpm == 0 {
        return 0.0;
    }

    let ve = get_ve();
    let air_mass = get_air_mass(ve, ecfg.engine.cilinder_count, ecfg.engine.displacement);

    let lambda = ecfg.injection.target_lambda;
    let stoich = ecfg.injection.target_stoich;

    let afr = stoich * lambda;

    let base_fuel = air_mass / afr;

    {
        es.injection.air_flow = (air_mass * ecfg.engine.cilinder_count as f32
            / get_engine_cycle_duration(es.rpm))
            * 3600000.0
            / 1000.0;

        es.injection.base_air = air_mass;
        es.injection.base_fuel = base_fuel;
        es.injection.injection_bank_1_time = fuel_mass_to_time(&es.injection, base_fuel) / 1000.0;
        es.cycle_duration = get_engine_cycle_duration(es.rpm);
        es.injection.targetAFR = afr;
    }
    return base_fuel;
}

pub fn calculate_correction_time() -> f32 {
    0.0
}

pub fn get_air_mass(ve: f32, engine_cilinders: u8, engine_displacement: u32) -> f32 {
    let full_cycle: f32 =
        (ve / 10000.0) * (engine_displacement as f32 * STD_AIR_PRES / (AIR_R * STD_AIR_TEMP));

    full_cycle / engine_cilinders as f32
}

pub fn get_ve() -> f32 {
    4.4
}
