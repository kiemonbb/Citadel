#include "config.h"
#include "hal/gpio_hal.h"
#include "hal/spi_hal.h"
#include "stm32l031xx.h"
#include "systick.h"
int main(void) {
  systick_open();
  spi_hal_open(SPI_CLOCK_MODE_0, SPI_BAUD_DIV_8);

  uint8_t test[] = {0xA5, 0xDE, 0x55, 0xAA};

  while (1) {
    spi_hal_cs_set(1);
    spi_hal_write(test, 4, SPI_MAX_DELAY);
    spi_hal_cs_set(0);
    delay_ms(1000);
  }
  return 0;
}
