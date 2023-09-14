#![allow(dead_code)]

use crate::app::engine::{
    efi_cfg::EngineConfig, engine_status::EngineStatus, get_engine_cycle_duration,
};
use crate::app::logging::host;
use crate::app::memory::tables::{DataT, Tables};

use super::injectors::fuel_mass_to_time;

const STD_AIR_PRES: f32 = 101.325f32;
const STD_AIR_TEMP: f32 = 293.15f32;
const AIR_R: f32 = 8.31445f32 / 28.965f32;

pub fn calculate_injection_fuel(es: &mut EngineStatus, ecfg: &EngineConfig, tables: &mut Tables) -> f32 {
    if es.rpm == 0 || !ecfg.ready {
        return 0.0;
    }

    let ve = get_ve(es, tables.tps_rpm_ve);
    let air_mass = get_air_mass(ve, ecfg.engine.cylinder_count, ecfg.engine.displacement);

    let lambda = ecfg.injection.target_lambda;
    let stoich = ecfg.injection.target_stoich;

    let afr = stoich * lambda;

    let base_fuel = air_mass / afr;

    {
        es.injection.air_flow = (air_mass * ecfg.engine.cylinder_count as f32
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

// esta funcion es solo para alphaN, cuando implemente speed-density o maf se puede migrar modificando las constantes
pub fn get_air_mass(ve: f32, engine_cilinders: u8, engine_displacement: u32) -> f32 {
    let full_cycle: f32 =
        (ve / 10000.0) * (engine_displacement as f32 * STD_AIR_PRES / (AIR_R * STD_AIR_TEMP));

    full_cycle / engine_cilinders as f32
}

// en este caso se revisa RPM y % del TPS
pub fn get_ve(es: &EngineStatus, ve_table: Option<DataT>) -> f32 {
    if es.rpm == 0 || ve_table.is_none() {
        return 0.0;
    }

    let table = ve_table.unwrap();
    let rpm_index = table[0].into_iter().position(|x| x >= (es.rpm * 100)).unwrap_or(1).clamp(1, 17);
    let load_index = table.into_iter().rposition(|y| y[0] >= es.sensors.tps as i32).unwrap_or(1).clamp(1, 17);

    return table[load_index][rpm_index] as f32 / 100.0;
}
