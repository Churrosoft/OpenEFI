use postcard::{from_bytes, to_vec};
use serde_json_core::heapless::Vec;
use stm32f4xx_hal::crc32::Crc32;
use w25q::series25::FlashInfo;

use crate::app::engine::efi_cfg::{EngineConfig, get_default_efi_cfg};
use crate::app::logging::host;
use crate::app::memory::tables::FlashT;

const ENGINE_CONFIG_MEMORY_ADDRESS: u32 = 0;

impl EngineConfig {
    pub fn save(&mut self, flash: &mut FlashT, flash_info: &FlashInfo, crc: &mut Crc32) {
        //TODO: ajustar tamaño del vector
        host::debug!("Guardando cfg");
        let mut output: Vec<u8, 800> = to_vec(&self).unwrap();

        crc.init();
        let calculated_crc = crc.update_bytes(&output);
        let mut crc_arr: [u8; 4] = u32::to_le_bytes(calculated_crc);

        let write_address = flash_info.sector_to_page(&(ENGINE_CONFIG_MEMORY_ADDRESS + 4)) * (flash_info.page_size as u32);
        let crc_addr = flash_info.sector_to_page(&ENGINE_CONFIG_MEMORY_ADDRESS) * (flash_info.page_size as u32);

        {
            flash.erase_sectors(ENGINE_CONFIG_MEMORY_ADDRESS, 1).unwrap();
            flash.write_bytes(crc_addr, &mut crc_arr).unwrap();
            flash.write_bytes(write_address, &mut output).unwrap();
        }
    }
    pub fn read(&mut self, flash: &mut FlashT, flash_info: &FlashInfo, crc: &mut Crc32) {
        // TODO: tirar error en caso de que la palme el crc

        let mut read_buff: [u8; 800] = [0; 800];
        let mut crc_buff: [u8; 4] = [0; 4];
        let read_address = flash_info.sector_to_page(&(ENGINE_CONFIG_MEMORY_ADDRESS + 4)) * (flash_info.page_size as u32);
        let crc_addr = flash_info.sector_to_page(&ENGINE_CONFIG_MEMORY_ADDRESS) * (flash_info.page_size as u32);

        {
            flash.read(read_address, &mut read_buff).unwrap();
            flash.read(crc_addr, &mut crc_buff).unwrap();
        }

        crc.init();
        let calculated_crc = crc.update_bytes(&read_buff);
        let memory_crc = u32::from_le_bytes(crc_buff);

        if memory_crc != calculated_crc {
            // self.ready = true;
            // host::debug!("Checksum config no coincide {:?}  {:?}", memory_crc,calculated_crc);
            //
            // let default= get_default_efi_cfg();
            // self.injection = default.injection.clone();
            // self.engine = default.engine.clone();
            //
            // self.save(flash,flash_info,crc);
            host::debug!("Checksum config no coincide {:?}  {:?}", memory_crc,calculated_crc);
            return;
        }

        {
            let mut memory_config: EngineConfig = from_bytes(&read_buff).unwrap();

            self.injection = memory_config.injection.clone();
            self.engine = memory_config.engine.clone();
            self.ready = true;
        }
    }
}