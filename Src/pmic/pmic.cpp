#include "./pmic.hpp"
#include "trace.h"

void PMIC::init()
{
    // disable PMIC until setup
    HAL_GPIO_WritePin(PMIC_ENABLE_GPIO_Port, PMIC_ENABLE_Pin, GPIO_PIN_SET);
}

void PMIC::enable()
{
    HAL_GPIO_WritePin(PMIC_ENABLE_GPIO_Port, PMIC_ENABLE_Pin, GPIO_PIN_RESET);
}

void PMIC::dtc_check()
{
    /*   aca tendria que:
    *   # revisar inyectores 1/4, revisar estado (open/corto/vcc)
    *   # revisar encendido 1/4
    */
    empty_pmic_buffer();
    pmic_send(PMIC_READ_INJECTION_A);
    volatile uint16_t pmic_injection_a = pmic_receive();

    empty_pmic_buffer();
    pmic_send(PMIC_READ_INJECTION_B);
    volatile uint16_t pmic_injection_b = pmic_receive();

    empty_pmic_buffer();
    pmic_send(PMIC_READ_IGNITION);
    volatile uint16_t pmic_ignition = pmic_receive();

    // si el canal 0 de inyeccion tiene falla, esto tendria que tener 1
    uint8_t iny_err = GET_BIT(pmic_injection_a, 0);

#if PMIC_DEBUG

    trace_printf("Error on INY0: %d, INY1 %d, all: %d \n", iny_err, GET_BIT(pmic_injection_a, 0), pmic_injection_a);
    volatile uint8_t test = GET_BIT(0b11101111, 5);

    trace_printf("PMIC INJECTION A: " BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n",
                 BYTE_TO_BINARY(pmic_injection_a >> 8), BYTE_TO_BINARY(pmic_injection_a));

    trace_printf("PMIC INJECTION B: " BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n",
                 BYTE_TO_BINARY(pmic_injection_b >> 8), BYTE_TO_BINARY(pmic_injection_b));

    trace_printf("PMIC IGNITION: " BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n",
                 BYTE_TO_BINARY(pmic_ignition >> 8), BYTE_TO_BINARY(pmic_ignition));

    BREAKPOINT
#endif
}

// from: https://github.com/ECUality/ECUality/blob/master/core/SPICommands.h#L39
void PMIC::setup_spark()
{
    // configuracion inicial del PMIC, por ahora dejo al maximo dwell porque no lo reviso:
    empty_pmic_buffer();

    //  0100 (spark cmd)
    //  1001 (max dwell = 32ms, max dwell protect enabled)
    //  0011 (overlap dwell disabled, gain for 40mohm, soft shutdown enabled, open secondary clamp enabled)
    //  1101 (open 2nd fault = 100us, end spark threshold = Vpwr + 5.5V)
    //  FIXME: would it have to be +7v?
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
    spi_send_byte(0b01001011);
    spi_send_byte(0b00111101);
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);

    empty_pmic_buffer();

    //  0110 (DAC cmd)
    //  1100 (Max current = 19A) // solo con la bobina que uso, pasar luego a memoria para cambiar desde OpenEFI Tuner
    //  100 (overlap setting: 50%)
    //  00100 (Nominal current = 4A) // no hace mucha falta cambiarla a cada rato

    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
    spi_send_byte(0b01101100);
    spi_send_byte(0b10000100);
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);

    empty_pmic_buffer();
}

// demo pequeña sobre la bobina 1-2
void PMIC::demo_spark()
{
    for (uint8_t i = 0; i < 50; i++)
    {
        HAL_Delay(50);

        HAL_GPIO_WritePin(INY1_GPIO_Port, INY1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ECN1_GPIO_Port, ECN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ECN2_GPIO_Port, ECN2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);

        HAL_Delay(20);

        HAL_GPIO_WritePin(INY1_GPIO_Port, INY1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(ECN1_GPIO_Port, ECN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(ECN2_GPIO_Port, ECN2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);

        HAL_Delay(50);
    }
}