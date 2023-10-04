// Logging to the host (if a debugger is plugged)
#[macro_use]
pub mod host {
    // TODO: Remove these macros on RELEASE builds!
    #[macro_export]
    macro_rules! debug {
        ($fmt:expr $(, $($arg:tt)*)?) => {

            if stm32f4xx_hal::pac::DCB::is_debugger_attached() {
                use cortex_m_semihosting::hprintln;

                hprintln!(concat!("[{:0>12.3}] DEBUG: ", $fmt), stm32f4xx_hal::pac::DWT::cycle_count() as f32 / 120000000f32, $($($arg)*)?);
            }
        };
    }

    #[macro_export]
    macro_rules! trace {
        ($fmt:expr $(, $($arg:tt)*)?) => {

            if stm32f4xx_hal::pac::DCB::is_debugger_attached() {
                use cortex_m_semihosting::hprintln;

                hprintln!(concat!("[{:0>12.3}] TRACE: ", $fmt), stm32f4xx_hal::pac::DWT::cycle_count() as f32 / 120000000f32, $($($arg)*)?);
            }
        };
    }

    #[macro_export]
    macro_rules! info {
        ($fmt:expr $(, $($arg:tt)*)?) => {

            // if (stm32f4xx_hal::pac::DCB::is_debugger_attached()) {
            //     use cortex_m_semihosting::hprintln;
            //
            //     hprintln!(concat!("[{:0>12.3}] INFO: ", $fmt), stm32f4xx_hal::pac::DWT::cycle_count() as f32 / 120000000f32, $($($arg)*)?);
            // }
        };
    }

    #[macro_export]
    macro_rules! error {
        ($fmt:expr $(, $($arg:tt)*)?) => {

            // if (stm32f4xx_hal::pac::DCB::is_debugger_attached()) {
            //     use cortex_m_semihosting::hprintln;
            //
            //     hprintln!(concat!("[{:0>12.3}] ERROR: ", $fmt), stm32f4xx_hal::pac::DWT::cycle_count() as f32 / 120000000f32, $($($arg)*)?);
            // }
        };
    }

    pub use debug;
    pub use trace;
    pub use error;
    pub use info;
}
