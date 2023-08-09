use stm32f4xx_hal::crc32::Crc32;
use w25q::series25::FlashInfo;

use crate::app::{
    engine::{efi_cfg::EngineConfig, engine_status::EngineStatus, get_engine_cycle_duration},
    memory::tables::{FlashT, TableData, Tables},
};

pub mod alpha_n;
pub mod injectors;

pub fn injection_setup(table: &mut Tables, flash: &mut FlashT, fi: &FlashInfo, crc: &mut Crc32) {
    let mut tps_rpm_ve = TableData {
        data: None,
        crc: 0,
        address: 0x3,
        max_x: 17,
        max_y: 17,
    };

    let mut tps_rpm_afr = TableData {
        data: None,
        crc: 0,
        address: 0x4,
        max_x: 17,
        max_y: 17,
    };

    let mut injector_delay = TableData {
        data: None,
        crc: 0,
        address: 0x5,
        max_x: 8,
        max_y: 8,
    };

    //TODO: move to ignition module
    let mut load_tps_deg = TableData {
        data: None,
        crc: 0,
        address: 0x10,
        max_x: 17,
        max_y: 17,
    };

    {
        table.tps_rpm_ve = tps_rpm_ve.read_from_memory(flash, &fi, crc);
        table.tps_rpm_afr = tps_rpm_afr.read_from_memory(flash, &fi, crc);
        table.injector_delay = injector_delay.read_from_memory(flash, &fi, crc);
        table.load_tps_deg = load_tps_deg.read_from_memory(flash, &fi, crc);
    }
}

pub fn calculate_time_isr(es: &mut EngineStatus, ecfg: &EngineConfig) {
    let mut bank_a_time: f32 = 0.0;
    {
        // desactivar inyeccion mientras se calcula el tiempo nuevo
        es.injection.injection_bank_1_time = 0.0;
    }

    // TODO: revisar RPM::status para que en SPIN_UP/CRANK tengan tiempos fijos desde la memoria flash

    bank_a_time += alpha_n::calculate_injection_fuel(es, &ecfg);
    bank_a_time += alpha_n::calculate_correction_time();
    bank_a_time += injectors::get_base_time(&ecfg.injection.injector);
    bank_a_time += injectors::get_battery_correction(&bank_a_time,&ecfg.injection.injector);
    bank_a_time += injectors::get_wall_wetting_correction();
    bank_a_time += injectors::get_pressure_correction();

    {
        es.injection.injection_bank_1_time = bank_a_time;
    }

    // en criollo:
    // "si hay mas tiempo que la duracion de un ciclo, es porque tenes el multiple lleno de nasta"
    debug_assert!(
        bank_a_time * 2.0 < get_engine_cycle_duration(es.rpm) && es.rpm >= 0 || es.rpm >= 0,
        "asddsa"
    );
}
