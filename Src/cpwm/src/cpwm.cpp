#include "../include/cpwm.hpp"
#ifdef TESTING
#include <trace.h>
#include <unity.h>
#include <string>
#include <stdio.h>

#endif
uint16_t CPWM::iny_time = 0;
uint16_t CPWM::iny_pin = 0;

uint16_t CPWM::eng_pin = 0;

uint16_t CPWM::avc_deg = 15;
uint16_t CPWM::avc_time = 0;

uint16_t CPWM::ckp_tick = 0;
float CPWM::ckp_deg = 4.56f;

void CPWM::set_iny(uint16_t value)
{
    iny_time = value;
}

void CPWM::set_avc(uint16_t deg, uint16_t time)
{
    CPWM::avc_deg = deg;
    CPWM::avc_time = time;
}

void CPWM::write_iny(uint8_t chanel, uint8_t pinState)
{
    uint16_t ign_sec[CIL] = ING_SECUENCY;
// esto luego se pasa a SPI viteh'
#ifdef TESTING
    switch (chanel)
    {
    case 0:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(49, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
        break;
    case 1:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(99, CPWM::ckp_tick, "Check AVCPER / ckp_deg");

        break;
    case 2:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(149, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
        break;
    case 3:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(199, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
        break;
    default:
        break;
    }
#else
    switch (ign_sec[chanel])
    {
    case 0:
        HAL_GPIO_WritePin(INY1_GPIO_Port, INY1_Pin, (GPIO_PinState)pinState);
        break;
    case 1:
        HAL_GPIO_WritePin(INY2_GPIO_Port, INY2_Pin, (GPIO_PinState)pinState);
        break;
    case 2:
        HAL_GPIO_WritePin(INY3_GPIO_Port, INY3_Pin, (GPIO_PinState)pinState);
        break;
    case 3:
        HAL_GPIO_WritePin(INY4_GPIO_Port, INY4_Pin, (GPIO_PinState)pinState);
        break;
    default:
        break;
    }
#endif
}

void CPWM::write_ecn(uint8_t chanel, uint8_t pinState)
{
    uint16_t ecn_sec[CIL] = EGN_SECUENCY;
#ifdef TESTING
    switch (chanel)
    {
    case 0:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(56, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
        break;
    case 1:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(115, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
        break;
    case 2:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(173, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
        break;
    case 3:
        TEST_ASSERT_EQUAL_INT16_MESSAGE(232, CPWM::ckp_tick, "Check AVCPER / ckp_deg");
        break;
    default:
        break;
    }
#else
    switch (ecn_sec[chanel])
    {
    case 0:
        HAL_GPIO_WritePin(ECN1_GPIO_Port, ECN1_Pin, (GPIO_PinState)pinState);
        break;
    case 1:
        HAL_GPIO_WritePin(ECN2_GPIO_Port, ECN2_Pin, (GPIO_PinState)pinState);
        break;
    case 2:
        HAL_GPIO_WritePin(ECN3_GPIO_Port, ECN3_Pin, (GPIO_PinState)pinState);
        break;
    case 3:
        HAL_GPIO_WritePin(ECN4_GPIO_Port, ECN4_Pin, (GPIO_PinState)pinState);
        break;
    default:
        break;
    }
#endif
}

void CPWM::interrupt()
{
    CPWM::ckp_deg = ROUND_16((360 / LOGIC_DNT) * CPWM::ckp_tick);

    if (abs(CPWM::ckp_deg - (AVCPER - AVCI) * (CPWM::iny_pin + 1)) <= 5)
    {
#ifdef TESTING

        trace_printf("------------------------------------\n");
        trace_printf("INY on cil:%d\nDEG: %d.%d \nTICK: %d \n",
                     CPWM::iny_pin,
                     (int16_t)CPWM::ckp_deg,
                     // odio formatear numeros en C
                     ((int16_t)(CPWM::ckp_deg - (int16_t)CPWM::ckp_deg) * 100),
                     CPWM::ckp_tick);
        trace_printf("------------------------------------\n");
#endif

        CPWM::write_iny(iny_pin, GPIO_PIN_SET);
        if (CPWM::iny_pin < L_CIL)
            CPWM::iny_pin++;
        else
            CPWM::iny_pin = 0;
    }

    if (abs(CPWM::ckp_deg - (AVCPER - CPWM::avc_deg) * (CPWM::eng_pin + 1)) <= 5)
    {
#ifdef TESTING
        trace_printf("------------------------------------\n");
        trace_printf("EGN on cil:%d\nDEG: %d.%d \nTICK: %d \n",
                     CPWM::eng_pin,
                     (int16_t)CPWM::ckp_deg,
                     // odio formatear numeros en C
                     ((int16_t)(CPWM::ckp_deg - (int16_t)CPWM::ckp_deg) * 100),
                     CPWM::ckp_tick);
        trace_printf("------------------------------------\n");
#endif
        CPWM::write_ecn(eng_pin, GPIO_PIN_SET);
        if (CPWM::eng_pin < L_CIL)
            CPWM::eng_pin++;
        else
            CPWM::eng_pin = 0;
    }

    if (CPWM::ckp_tick >= LOGIC_DNT * 2)
    {
        CPWM::ckp_tick = 0;
    }
    else
        CPWM::ckp_tick++;
}