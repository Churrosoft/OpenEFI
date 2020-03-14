#include <SRAM.h>

bool SRAM::Init(){
     /* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, SS |  SCK | MOSI );

  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
          MISO);

  /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
  spi_reset(SPI1);

  /* Set up SPI in Master mode with:
   * Clock baud rate: 1/64 of peripheral clock frequency
   * Clock polarity: Idle High
   * Clock phase: Data valid on 2nd clock pulse
   * Data frame format: 8-bit
   * Frame format: MSB First
   */
  spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
  spi_enable_software_slave_management(SPI1);
  spi_set_nss_high(SPI1);
  /* Enable SPI1 periph. */
  spi_enable(SPI1);
    return true;
}

bool SRAM::Write(uint32_t addr, char value) {

    SRAM::SetAddr(addr, WRITE_BYTE);    //seteamos modo escritura
    spi_send(SPI1, (uint8_t) value);    //ahora enviamos la data
    if (SRAM::ReadChar(addr) == value){ //por las dudas, revisamos que en esa direccion efectivamente esta la data que grabamos
      return true;
    }
    return false;
}

bool SRAM::Write(uint32_t addr, char * value){
  bool status = false
  uint32_t length = strlen(value);

  for(uint32_t i = 0; i < length; i++){

    SRAM::SetAddr(addr, WRITE_BYTE);    //seteamos modo escritura
    spi_send(SPI1, (uint8_t) value);    //ahora enviamos la data
    if (SRAM::ReadChar(addr) == value){ 
      //por las dudas, revisamos que en esa dirr efectivamente esta la data que grabamos
      status = true;
    }else {
      //por algun motivo no se grabo bien, abortamos y devolvemos false
      status = false;
      break;
    }

  }
  return status;
}

void SRAM::SetAddr(uint32_t addr, byte mode){
  spi_send(SPI1, mode);
  spi_send(SPI1, (uint8_t)(addr >> 16) );
  spi_send(SPI1, (uint8_t)(addr >> 8) );
  spi_send(SPI1, (uint8_t) addr );
}