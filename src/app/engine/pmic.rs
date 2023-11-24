use embedded_hal::blocking::spi::Transfer;
use embedded_hal::digital::v2::OutputPin;
use shared_bus_rtic::SharedBus;
use stm32f4xx_hal::gpio;
use stm32f4xx_hal::gpio::{Output, PushPull};

use crate::app::engine::error::Error;
use crate::app::gpio::ISPI;
use serde::{ Serialize};
pub type PmicT = PMIC<SharedBus<ISPI>, gpio::PB11<Output<PushPull>>>;

#[derive(Serialize)]
#[repr(u8)]
pub enum Commands {
    ReadRegisters = 0b0000_1010,
    AllStatus,
    SPICheck,
    ModeSetIgn,
    ModeSetGp,
    DriverEnable,
    ClockCalibration,
}

#[derive(Serialize)]
#[repr(u8)]
pub enum Registers {
    AllStatus = 0x0,
    Out01Fault = 0b0001_0000,
    Out23Fault = 0b0010_0000,
    GPGDModeFault = 0b0011_0000,
    IGNModeFault = 0b0100_0000,
    ModeCommand = 0b0101_000,
    LSDFault = 0b0110_0000,
    DRVREnable = 0b0111_0000,
    StartSparkFilter = 0b1000_0000,
    EndSparkFilter = 0b1001_0000,
    DACRegister = 0b1010_0000,
}

#[derive(Serialize,Debug)]
#[repr(u8)]
pub enum InjectorStatus {
    Ok = 0x00,
    GenericError,
    TLIMFault,
    BatteryShortFault,
    OffOpenFault,
    OnOpenFault,
    CORError,
}

#[derive(Serialize,Debug)]
#[repr(u8)]
pub enum CoilStatus {
    Ok,
    GenericError,
    MAXIFault,
    MaxDwellFault,
    OpenSecondaryFault,
}

#[derive(Serialize, Debug)]
pub struct FullStatus<T> {
    pub cor: bool,
    pub over_voltage: bool,
    pub under_voltage: bool,
    pub cil_1: T,
    pub cil_2: T,
    pub cil_3: T,
    pub cil_4: T,
}

#[derive(Serialize, Debug)]
pub struct FastCil<T> {
    pub cil_2: T,
    pub cil_3: T,
    pub cil_4: T,
    pub cil_1: T,
}

#[derive(Serialize, Debug)]
pub struct FastStatus {
    pub cor: bool,
    pub over_voltage: bool,
    pub under_voltage: bool,
    pub ign: FastCil<CoilStatus>,
    pub inj: FastCil<InjectorStatus>,
}

#[derive(Debug)]
pub struct PMIC<SPI, CS> {
    spi: SPI,
    cs: CS,
}

fn match_injector_status(data: u8, shift: u8) -> InjectorStatus {
    if (data >> 3 + shift) & 1 != 0 { return InjectorStatus::TLIMFault; }
    if (data >> 2 + shift) & 1 != 0 { return InjectorStatus::BatteryShortFault; }
    if (data >> 1 + shift) & 1 != 0 { return InjectorStatus::OffOpenFault; }
    if (data >> 0 + shift) & 1 != 0 { return InjectorStatus::OnOpenFault; }

    return InjectorStatus::Ok;
}

fn match_ignition_status(data: u16, shift: u8) -> CoilStatus {
    if (data >> 2 + shift) & 1 != 0 { return CoilStatus::MAXIFault; }
    if (data >> 1 + shift) & 1 != 0 { return CoilStatus::MaxDwellFault; }
    if (data >> 0 + shift) & 1 != 0 { return CoilStatus::OpenSecondaryFault; }

    return CoilStatus::Ok;
}

impl<SPI: Transfer<u8>, CS: OutputPin> PMIC<SPI, CS> {
    pub fn init(spi: SPI, cs: CS) -> Result<Self, Error<SPI, CS>> {
        let this = Self { cs, spi };

        //TODO: usar el comando de "check" para revisar que este vivo el PMIC antes de iniciar la lib

        Ok(this)
    }

    //TODO: evaluar si hace falta el delay entre comando y payload
    fn send_command(&mut self, command: Commands, payload: u8) -> [u8; 2] {
        let mut mock_word = [0xf, 0x0];
        let mut result_word = [0xf, 0x0];
        let mut spi_payload = [command as u8, payload];

        let _ = self.cs.set_low();
        let _ = self.spi.transfer(&mut spi_payload);
        let result_data = self.spi.transfer(&mut mock_word);
        let _ = self.cs.set_high();

        //FIXME: esto le genera infartos a mas de un catolico de rust
        // https://doc.rust-lang.org/std/result/#extracting-contained-values
        unsafe {
            result_word[0] = result_data.as_ref().unwrap_unchecked()[0];
            result_word[1] = result_data.as_ref().unwrap_unchecked()[1];
        }

        return result_word;
    }


    pub fn get_fast_status(&mut self) -> FastStatus {
        let result = self.send_command(Commands::ReadRegisters, Registers::AllStatus as u8);

        return FastStatus {
            cor: (result[1] >> 7) & 1 != 0,
            over_voltage: (result[1] >> 6) & 1 != 0,
            under_voltage: (result[1] >> 5) & 1 != 0,
            ign: FastCil {
                cil_1: if (result[1] >> 0) & 1 != 0 { CoilStatus::GenericError } else { CoilStatus::Ok },
                cil_2: if (result[1] >> 1) & 1 != 0 { CoilStatus::GenericError } else { CoilStatus::Ok },
                cil_3: if (result[1] >> 2) & 1 != 0 { CoilStatus::GenericError } else { CoilStatus::Ok },
                cil_4: if (result[1] >> 3) & 1 != 0 { CoilStatus::GenericError } else { CoilStatus::Ok },
            },
            inj: FastCil {
                cil_1: if (result[0] >> 0) & 1 != 0 { InjectorStatus::GenericError } else { InjectorStatus::Ok },
                cil_2: if (result[0] >> 1) & 1 != 0 { InjectorStatus::GenericError } else { InjectorStatus::Ok },
                cil_3: if (result[0] >> 2) & 1 != 0 { InjectorStatus::GenericError } else { InjectorStatus::Ok },
                cil_4: if (result[0] >> 3) & 1 != 0 { InjectorStatus::GenericError } else { InjectorStatus::Ok },
            },
        };
    }

    pub fn get_injector_status(&mut self) -> FullStatus<InjectorStatus> {
        let bank_a_status = self.send_command(Commands::ReadRegisters, Registers::Out01Fault as u8);
        let bank_b_status = self.send_command(Commands::ReadRegisters, Registers::Out23Fault as u8);

        let cil_1 = match_injector_status(bank_a_status[0], 0);
        let cil_2 = match_injector_status(bank_a_status[0], 4);
        let cil_3 = match_injector_status(bank_b_status[0], 0);
        let cil_4 = match_injector_status(bank_b_status[0], 4);

        // TODO: check COR/Over Voltage/Under voltage

        return FullStatus {
            cor: false,
            over_voltage: false,
            under_voltage: false,
            cil_1,
            cil_2,
            cil_3,
            cil_4,
        };
    }

    pub fn get_ignition_status(&mut self) -> FullStatus<CoilStatus> {
        let status = self.send_command(Commands::ReadRegisters, Registers::IGNModeFault as u8);

        let data = u16::from_le_bytes(status);

        let cil_1 = match_ignition_status(data, 0);
        let cil_2 = match_ignition_status(data, 3);
        let cil_3 = match_ignition_status(data, 6);
        let cil_4 = match_ignition_status(data, 9);

        return FullStatus {
            cor: false,
            over_voltage: false,
            under_voltage: false,
            cil_1,
            cil_2,
            cil_3,
            cil_4,
        };
    }
}