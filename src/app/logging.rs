// Logging to the host (if a debugger is plugged)
#[macro_use]
pub mod host {
    // TODO: Remove these macros on RELEASE builds!
    #[macro_export]
    macro_rules! debug {
        ($fmt:expr $(, $($arg:tt)*)?) => {
            use stm32f4xx_hal::pac::DCB;
            if (DCB::is_debugger_attached()) {
                use cortex_m_semihosting::hprintln;
    
                hprintln!(concat!("DEBUG: ", $fmt), $($($arg)*)?);
            }
        };
    }

    #[macro_export]
    macro_rules! info {
        ($fmt:expr $(, $($arg:tt)*)?) => {
            use stm32f4xx_hal::pac::DCB;
            if (DCB::is_debugger_attached()) {
                use cortex_m_semihosting::hprintln;
    
                hprintln!(concat!("INFO: ", $fmt), $($($arg)*)?);
            }
        };
    }

    #[macro_export]
    macro_rules! error {
        ($fmt:expr $(, $($arg:tt)*)?) => {
            use stm32f4xx_hal::pac::DCB;
            if (DCB::is_debugger_attached()) {
                use cortex_m_semihosting::hprintln;
    
                hprintln!(concat!("ERROR: ", $fmt), $($($arg)*)?);
            }
        };
    }
    
    pub use debug;
    pub use info;
    pub use error;
}