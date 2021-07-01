#include "../include/cpwm.hpp"

uint16_t CPWM::iny_time = 0;
uint16_t CPWM::iny_pin = 0;

uint16_t CPWM::eng_pin = 0;

uint16_t CPWM::avc_deg = 15;
uint16_t CPWM::avc_time = 0;

uint16_t CPWM::ckp_tick = 0;
float CPWM::ckp_deg = 0;

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
}

void CPWM::write_ecn(uint8_t chanel, uint8_t pinState)
{
    uint16_t ecn_sec[CIL] = ING_SECUENCY;

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
}

void CPWM::interrupt()
{
    CPWM::ckp_deg = ROUND_16((360 / LOGIC_DNT) * CPWM::ckp_tick);

    if (abs(CPWM::ckp_deg - (AVCPER - AVCI) * (CPWM::iny_pin + 1)) <= 5)
    {
        if (CPWM::iny_pin < L_CIL)
            CPWM::iny_pin++;
        else
            CPWM::iny_pin = 0;
    }

    if (abs(CPWM::ckp_deg - (AVCPER - CPWM::avc_deg) * (CPWM::eng_pin + 1)) <= 5)
    {
        if (CPWM::eng_pin < L_CIL)
            CPWM::eng_pin++;
        else
            CPWM::eng_pin = 0;
    }

    if (CPWM::ckp_tick >= LOGIC_DNT * 2)
    {
        CPWM::ckp_tick = 0;
    }
    CPWM::ckp_tick++;
}