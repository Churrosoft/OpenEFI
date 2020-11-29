#include "spi_common.h"

void spi_init() {
  /* Enable SPI2 Periph and gpio clocks */
  rcc_periph_clock_enable(RCC_SPI2);
  /* Configure GPIOs: SS=PE13, SCK=PB10, MISO=PB14 and MOSI=PB15 */
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                GPIO4 | GPIO10 | GPIO15);
  gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                GPIO13);
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO14);

  /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
  spi_reset(SPI2);

  /* Set up SPI in Master mode with:
   * Clock baud rate: 1/4 of peripheral clock frequency => 10.5Mhz
   * Clock polarity: Idle High
   * Clock phase: Data valid on 2nd clock pulse
   * Data frame format: 8-bit
   * Frame format: MSB First
   */
  spi_init_master(
      SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_4, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
      SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
  spi_enable_software_slave_management(SPI2);
  spi_set_nss_high(SPI2);

  /* Enable SPI2 periph. */
  spi_enable(SPI2);
  spi_disable_crc(SPI2);
  spi_enabled = true;
}

void send_command(uint8_t command) {
  if (spi_enabled) {
    // CS pa abajo como el perreo
    gpio_clear(GPIOE, GPIO13);
    spi_send(SPI2, (uint8_t)command);
  }
}

uint8_t read_byte() {
  if (spi_enabled) {
    uint8_t data = spi_read(SPI2);
    close_connection();
    return data;
  }
  return 0xFF;
}

void close_connection() {
  // CS pa arriba como el dolar
  gpio_set(GPIOE, GPIO13);
}
