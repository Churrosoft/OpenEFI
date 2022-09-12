#ifndef PMIC_HPP
#define PMIC_HPP

#include "defines.h"
#include "ll_spi.h"

/** @defgroup PMIC
 *  @brief Namespace con I/O hacia el PMIC para manejar las bobinas e inyectores
 *
 * de a poco el codigo de CPWM va a caer aca \n
 * para poder manejar todo por SPI, y agregar compatibilidad con el PMIC mas \n
 * grande https://www.nxp.com/docs/en/data-sheet/MC33800.pdf para uEFI se \n
 * utiliza: https://www.nxp.com/docs/en/data-sheet/MC33810.pdf
 * @{
 */

namespace PMIC {

/*! @ingroup PMIC */
/*     void loop(void); */
void init(void);

/**
 * @ingroup PMIC
 * @brief enable PMIC outputs
 */
void enable(void);

/**
 * @ingroup PMIC
 * @brief tarea periodica, revisa el estado del PMIC para grabar DTC's en caso
 * de falla
 */
void dtc_check(void);

/**
 * @ingroup PMIC
 * @brief envia data por SPI para configurar los tiempos de dWell y la
 * resistencia de los mofet
 */
void setup_spark(void);

/**
 * @ingroup PMIC
 * @brief demo en 2 bobinas para comprobar funcionamiento
 */
void demo_spark(void);

// internal:
namespace {

#define PMIC_READ_ALL 0x0A00
#define PMIC_READ_IGNITION 0x0A40
#define PMIC_READ_INJECTION_A 0x0A10
#define PMIC_READ_INJECTION_B 0x0A20

  static inline uint16_t pmic_receive(void) {
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AUX_CS_1_GPIO_Port, AUX_CS_1_Pin, GPIO_PIN_RESET);
    volatile uint16_t data;
    if ((SPI2->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
      SET_BIT(SPI2->CR1, SPI_CR1_SPE);
    }
    while (!(SPI2->SR & SPI_SR_TXE))
      ;

    // purge all the shit
    while (LL_SPI_IsActiveFlag_RXNE(SPI2)) LL_SPI_ReceiveData16(SPI2);

    LL_SPI_TransmitData16(SPI2, 0);
    data = LL_SPI_ReceiveData16(SPI2);
    while (SPI2->SR & SPI_SR_BSY)
      ;
    return data;
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AUX_CS_1_GPIO_Port, AUX_CS_1_Pin, GPIO_PIN_SET);
  }
  static inline uint8_t spi_send(uint8_t Data) {
    uint8_t ret;
    HAL_SPI_TransmitReceive(&hspi2, &Data, &ret, 1, 100);
    return ret;
  }
  static inline void pmic_send(int16_t command) {

    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AUX_CS_1_GPIO_Port, AUX_CS_1_Pin, GPIO_PIN_RESET);

    spi_send((command & 0xFF00) >> 8);
    spi_send(command & 0xFF);

    /*  if ((SPI2->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        SET_BIT(SPI2->CR1, SPI_CR1_SPE);
    }
    while (!(SPI2->SR & SPI_SR_TXE))
        ;
    LL_SPI_TransmitData16(SPI2, command);
    while (SPI2->SR & SPI_SR_BSY)
        ; */
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AUX_CS_1_GPIO_Port, AUX_CS_1_Pin, GPIO_PIN_SET);
  }
  static inline void empty_pmic_buffer(void) {
    // a seguro se lo llevaron preso asi que:
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_RESET);

    pmic_receive();
    HAL_GPIO_WritePin(PMIC_CS_GPIO_Port, PMIC_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AUX_CS_1_GPIO_Port, AUX_CS_1_Pin, GPIO_PIN_SET);
  }
} // namespace

} // namespace PMIC

/*! @} End of Doxygen PMIC*/

#endif