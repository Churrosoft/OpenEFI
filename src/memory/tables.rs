use stm32f4xx_hal::gpio::{Alternate, Output, Pin};
use stm32f4xx_hal::pac::SPI2;
use stm32f4xx_hal::spi::Spi;
use w25q::series25::{Flash, FlashInfo};

type DataT = [[i32; 17]; 17];

pub struct TableData {
    pub data: Option<DataT>,
    pub address: u32,
    pub max_x: u16,
    pub max_y: u16,
}

type FlashT = Flash<
    Spi<
        SPI2,
        (
            Pin<'B', 10, Alternate<5>>,
            Pin<'B', 14, Alternate<5>>,
            Pin<'B', 15, Alternate<5>>,
        ),
        false,
    >,
    Pin<'E', 13, Output>,
>;

impl TableData {
    pub fn read_from_memory(&self, flash: &mut FlashT, fi: &FlashInfo) -> Option<DataT> {
        let mut vv_32 = [[0i32; 17]; 17];
        let read_address = fi.sector_to_page(&self.address) * (fi.page_size as u32) + 4;
        let mut datarow = 0;
        let mut buf: [u8; 4 * 17 * 17] = [0; 4 * 17 * 17];

        {
            flash.read(read_address, &mut buf).unwrap();
        }

        for matrix_y in 0..self.max_y {
            for matrix_x in 0..self.max_x {
                let u8buff = [
                    buf[datarow + 3],
                    buf[datarow + 2],
                    buf[datarow + 1],
                    buf[datarow],
                ];

                let value = i32::from_be_bytes(u8buff);

                vv_32[matrix_y as usize][matrix_x as usize] = value;

                datarow += 4;
            }
        }

        return Some(vv_32);
    }

    pub fn validate(&self, flash: &mut FlashT, fi: &FlashInfo) -> bool {
        let read_address = fi.sector_to_page(&self.address) * (fi.page_size as u32);
        let mut buf: [u8; 4] = [0; 4];

        {
            flash.read(read_address, &mut buf).unwrap();
        }

        let u8buff = [buf[3], buf[2], buf[1], buf[0]];

        let memory_crc = u32::from_be_bytes(u8buff);

        return false;
    }

    pub fn validate_wcrc(&self, flash: &mut FlashT, fi: &FlashInfo, crc: u32) -> bool {
        return false;
    }
}
