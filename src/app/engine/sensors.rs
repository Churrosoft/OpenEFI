use crate::app::gpio::ADCMapping;
use micromath::F32Ext;

use stm32f4xx_hal::{
    adc::{config::SampleTime, Adc},
    pac::ADC2,
};

const EMA_LP_ALPHA: f32 = 0.45f32;

pub enum SensorTypes {
    MAP,
    TPS,
    CooltanTemp,
    AirTemp,
    BatteryVoltage,
    ExternalLambda,
}

#[derive(Debug,Clone,Copy)]
pub struct SensorValues {
    pub map: f32,
    pub tps: f32,
    pub cooltan_temp: f32,
    pub air_temp: f32,
    pub batt: f32,
    pub ext_o2: f32,

    // private:
    raw_map: f32,
    raw_tps: f32,
    raw_temp: f32,
    raw_air_temp: f32,
    raw_batt: f32,
    raw_ext_o2: f32,
}

impl SensorValues {
    pub fn new() -> SensorValues {
        SensorValues {
            map: 0.0f32,
            tps: 20.0f32,
            cooltan_temp: 45.69f32,
            air_temp: 0.0f32,
            batt: 13.42f32,
            ext_o2: 0.0f32,
            // valores en raw son en bits del ADC; luego se pasan a mV
            raw_map: 0.0f32,
            raw_tps: 0.0f32,
            raw_temp: 0.0f32,
            raw_air_temp: 0.0f32,
            raw_batt: 0.0f32,
            raw_ext_o2: 0.0f32,
        }
    }

    // TODO: falta calcular cada valor del sensor ademas del low pass
    pub fn update(&mut self, raw_value: u16, sensor_type: SensorTypes) {
        match sensor_type {
            SensorTypes::AirTemp => {
                self.raw_air_temp = EMA_LP_ALPHA * (raw_value as f32)
                    + (1.0 - EMA_LP_ALPHA) * (self.raw_air_temp as f32);

                // Ford ECC-IV type sensor
                // TODO: make configurable
                // FIXME: esta para el recontra culo la cuenta
                let result = 143.73f32 * 0.99936f32.powf(self.raw_air_temp);

                self.air_temp = result;

            }
            SensorTypes::CooltanTemp => {
                self.raw_temp = EMA_LP_ALPHA * (raw_value as f32)
                    + (1.0 - EMA_LP_ALPHA) * (self.raw_temp as f32);
            }
            SensorTypes::MAP => {
                self.raw_map = EMA_LP_ALPHA * (raw_value as f32)
                    + (1.0 - EMA_LP_ALPHA) * (self.raw_map as f32);
            }
            SensorTypes::TPS => {
                self.raw_tps = EMA_LP_ALPHA * (raw_value as f32)
                    + (1.0 - EMA_LP_ALPHA) * (self.raw_tps as f32);

                // SDUBTUD: tan fácil iba a ser?
                let result = (24.6914 * self.raw_tps - 11.11f32).clamp(0f32,100f32);
                self.tps = result;
            }
            SensorTypes::BatteryVoltage => {
                self.raw_batt = EMA_LP_ALPHA * (raw_value as f32)
                    + (1.0 - EMA_LP_ALPHA) * (self.raw_batt as f32);

                self.batt = 14.2;
            }

            SensorTypes::ExternalLambda => {
                self.ext_o2 = raw_value as f32;
            }
        }
    }
}

pub fn get_sensor_raw(
    sensor_type: SensorTypes,
    adc_pins: &mut ADCMapping,
    adc: &mut Adc<ADC2>,
) -> u16 {
    let a = sensor_type as u8;
    let b = a;
    let c = a;
    adc_pins.mux_a.set_state(((a & (1 << 0)) != 0).into());
    adc_pins.mux_b.set_state(((b & (1 << 1)) != 0).into());
    adc_pins.mux_c.set_state(((c & (1 << 2)) != 0).into());
    let sample = adc.convert(&adc_pins.analog_in, SampleTime::Cycles_480);

    return adc.sample_to_millivolts(sample);
}
