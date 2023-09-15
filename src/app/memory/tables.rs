#![allow(dead_code)]

use shared_bus_rtic::SharedBus;
use stm32f4xx_hal::{
    crc32::Crc32,
    gpio::{Alternate, Output, Pin},
    pac::SPI2,
    spi::Spi,
};
use w25q::series25::{Flash, FlashInfo};

use crate::app::logging::host;

pub type DataT = [[i32; 17]; 17];
pub type PlotData = [[i32; 2]; 10];

pub struct Tables {
    // injection
    pub tps_rpm_ve: Option<DataT>,
    pub tps_rpm_afr: Option<DataT>,
    pub injector_delay: Option<DataT>,
    pub vbat_correction: Option<PlotData>,
    pub wue: Option<PlotData>,
    pub ase_taper: Option<PlotData>,
    pub ase_intensity: Option<PlotData>,
    //ignition
    pub load_tps_deg: Option<DataT>,
}

pub struct TableData {
    pub data: Option<DataT>,
    pub crc: u32,
    pub address: u32,
    pub max_x: u16,
    pub max_y: u16,
}


pub type SpiT = Spi<
    SPI2,
    (
        Pin<'B', 10, Alternate<5>>,
        Pin<'B', 14, Alternate<5>>,
        Pin<'B', 15, Alternate<5>>,
    ),
    false,
>;

pub type FlashT = Flash<SharedBus<SpiT>, Pin<'E', 13, Output>>;

impl TableData {
    pub fn read_from_memory(
        &mut self,
        flash: &mut FlashT,
        fi: &FlashInfo,
        crc: &mut Crc32,
    ) -> Option<DataT> {
        let mut vv_32 = [[0i32; 17]; 17];
        let read_address = fi.sector_to_page(&self.address) * (fi.page_size as u32);
        let mut datarow = 4;
        let mut buf: [u8; 4 * 17 * 17 + 4] = [0; 4 * 17 * 17 + 4];

        {
            flash.read(read_address, &mut buf).unwrap();
        }
        for matrix_y in 0..self.max_y {
            for matrix_x in 0..self.max_x {
                let u8buff = [
                    buf[datarow],
                    buf[datarow + 1],
                    buf[datarow + 2],
                    buf[datarow + 3],
                ];

                let value = i32::from_le_bytes(u8buff);
                vv_32[matrix_y as usize][matrix_x as usize] = value;

                datarow += 4;
            }
        }
        // CRC Check
        let u8buff = [buf[0], buf[1], buf[2], buf[3]];
        let memory_crc = u32::from_le_bytes(u8buff);

        crc.init();
        let calculated_crc = crc.update_bytes(&buf[4..]);

        if memory_crc != calculated_crc {
            host::debug!("Checksum tablas no coinciden {:?}  {:?}", memory_crc,calculated_crc)
        }


        {
            self.crc = calculated_crc;
            self.data = Some(vv_32);
        }

        return Some(vv_32);
    }

    pub fn write_to_memory(&self, flash: &mut FlashT, fi: &FlashInfo, crc: &mut Crc32) {
        let mut buf: [u8; 4 * 17 * 17 + 4] = [0; 4 * 17 * 17 + 4];
        let mut datarow = 4;

        for matrix_y in 0..self.max_y {
            for matrix_x in 0..self.max_x {
                let value_arr: [u8; 4] =
                    i32::to_le_bytes(self.data.unwrap()[matrix_y as usize][matrix_x as usize]);
                buf[datarow] = value_arr[0];
                buf[datarow + 1] = value_arr[1];
                buf[datarow + 2] = value_arr[2];
                buf[datarow + 3] = value_arr[3];

                datarow += 4;
            }
        }

        crc.init();
        let calculated_crc = crc.update_bytes(&buf[4..]);
        let crc_arr: [u8; 4] = u32::to_le_bytes(calculated_crc);

        buf[0] = crc_arr[0];
        buf[1] = crc_arr[1];
        buf[2] = crc_arr[2];
        buf[3] = crc_arr[3];

        {
            let write_address = fi.sector_to_page(&self.address) * (fi.page_size as u32);
            flash.erase_sectors(write_address, 1).unwrap();
            flash.write_bytes(write_address, &mut buf).unwrap();
        }
    }

    pub fn validate(&self, crc: &mut Crc32, crc_val: u32) -> bool {
        let mut buf: [u8; 4 * 17 * 17] = [0; 4 * 17 * 17];
        let mut datarow = 0;

        for matrix_y in 0..self.max_y {
            for matrix_x in 0..self.max_x {
                let value_arr: [u8; 4] =
                    i32::to_le_bytes(self.data.unwrap()[matrix_y as usize][matrix_x as usize]);
                buf[datarow] = value_arr[0];
                buf[datarow + 1] = value_arr[1];
                buf[datarow + 2] = value_arr[2];
                buf[datarow + 3] = value_arr[3];

                datarow += 4;
            }
        }

        {
            crc.init();
            let calculated_crc = crc.update_bytes(&buf);
            return calculated_crc == crc_val;
        }
    }

    pub fn clear(&mut self, flash: &mut FlashT, fi: &FlashInfo, crc: &mut Crc32 ) {
        let mut buf: [u8; 4 * 17 * 17 + 4] = [0; 4 * 17 * 17 + 4];

        crc.init();
        let calculated_crc = crc.update_bytes(&buf[4..]);
        let crc_arr: [u8; 4] = u32::to_le_bytes(calculated_crc);

        buf[0] = crc_arr[0];
        buf[1] = crc_arr[1];
        buf[2] = crc_arr[2];
        buf[3] = crc_arr[3];

        {
            let write_address = fi.sector_to_page(&self.address) * (fi.page_size as u32);
            flash.erase_sectors(write_address, 1).unwrap();
            flash.write_bytes(write_address, &mut buf).unwrap();
        }
    }

    pub fn on_bounds(&self, x: i16, y: i16) -> bool {
        if x > (self.max_x as i16) || y > (self.max_y as i16) {
            false;
        }

        if x < 0 || y < 0 {
            false;
        }

        return true;
    }
}
