#ifndef SPI_HAL_H
#define SPI_HAL_H
#include "stm32l031xx.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
  SPI_CLOCK_MODE_0 = 0,                           // CPOL=0 CPHA=0
  SPI_CLOCK_MODE_1 = SPI_CR1_CPHA,                // CPOL=0 CPHA=1
} spi_mode_t;

typedef enum {
  SPI_BAUD_DIV_2 = 0L,
  SPI_BAUD_DIV_4 = 1L,
  SPI_BAUD_DIV_8 = 2L,
  SPI_BAUD_DIV_16 = 3L,
  SPI_BAUD_DIV_32 = 4L,
  SPI_BAUD_DIV_64 = 5L,
  SPI_BAUD_DIV_128 = 6L,
  SPI_BAUD_DIV_256 = 7L,
} spi_baud_divider_t;

// Simplex 8bit transmission 
void spi_hal_open(spi_mode_t mode, spi_baud_divider_t baud_divider);

void spi_hal_write(const uint8_t *data, size_t length);

void spi_hal_transfer_byte(uint8_t byte);

#endif
