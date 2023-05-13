#![allow(unsafe_code)]
use core::str;

use arrayvec::ArrayString;
use numtoa::NumToA;
use stm32f4xx_hal::signature::Uid;

pub fn get_serial_str() -> &'static str {
    static mut SERIAL: [u8; 16] = [b' '; 16];
    let serial = unsafe { SERIAL.as_mut() };

    // let sn = read_serial();
    let uid = Uid::get();

    let mut text = ArrayString::<32>::new();
    let mut num_buffer = [0u8; 20];

    text.push_str("C");
    text.push_str(uid.lot_num());
    text.push_str("/");
    text.push_str(uid.waf_num().numtoa_str(16, &mut num_buffer));
    text.push_str(".");
    text.push_str(uid.x().numtoa_str(16, &mut num_buffer));
    text.push_str("-");
    text.push_str(uid.y().numtoa_str(16, &mut num_buffer));

    let sn = text.as_bytes();

    for (i, d) in serial.iter_mut().enumerate() {
        *d = sn[i]
    }

    unsafe { str::from_utf8_unchecked(serial) }
}

pub fn crc16(data: &[u8], length: u8) -> u16 {
    let mut crc: u16 = 0xFFFF;
    let mut iter: usize = 0;
    let mut in_len = length;

    while in_len != 0 {
        let mut x = (crc >> 8) as u8 ^ data[iter];
        x ^= x >> 4;
        crc = (crc << 8) ^ ((x as u16) << 12) ^ ((x as u16) << 5) ^ (x as u16);

        iter += 1;
        in_len -= 1;
    }

    crc
}