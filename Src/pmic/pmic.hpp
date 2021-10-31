#ifndef PMIC_HPP
#define PMIC_HPP

#include "ll_spi.h"

// de a poco el codigo de CPWM va a caer aca
// para poder manejar todo por SPI, y agregar compatibilidad con el PMIC mas grande
// https://www.nxp.com/docs/en/data-sheet/MC33800.pdf
// para uEFI se utiliza:
// https://www.nxp.com/docs/en/data-sheet/MC33810.pdf

namespace PMIC
{
    /*     void loop(void); */
    void init(void);
    void enable(void);
    void dtc_check(void);
    void setup_spark(void);
    void demo_spark(void);
    /*     void set_spark(void); */
    // internal:
    namespace
    {
        static inline void empty_pmic_buffer(void)
        {
            // a seguro se lo llevaron preso asi que:
            HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
            spi_read_byte();
            spi_read_byte();
            HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);
        }
    }

} // namespace PMIC

#endif