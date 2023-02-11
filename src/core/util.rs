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
