pub mod cpwm;
pub mod efi_cfg;
pub mod engine_status;
pub mod sensors;
pub mod pmic;
mod error;

/**
 * @brief tiempo que tarda una vuelta del cigueñal
 */
pub fn get_degree_time(rpm: i32) -> f32 {
    1000000.0f32 * 60.0 / 360.0 / rpm as f32
}

/**
 * @brief, degree time / 2, ni que fueramos a usar motores 2T
 */
pub fn get_engine_cycle_duration(rpm: i32) -> f32 {
    get_degree_time(rpm) / 2.0
}
