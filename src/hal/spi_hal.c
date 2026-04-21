#include "spi_hal.h"
#include "config.h"
#include "stm32l031xx.h"
#include <stddef.h>
#include <stdint.h>

void spi_hal_open(spi_mode_t mode, spi_baud_divider_t baud_divider) {
  // MOSI PORT
  SPI_PORT->MODER &= ~(0x3U << (SPI_MOSI_PIN * 2));
  SPI_PORT->MODER |= (0x2U << (SPI_MOSI_PIN * 2));

  SPI_PORT->OSPEEDR &= ~(0x3U << (SPI_MOSI_PIN * 2));
  SPI_PORT->OSPEEDR |= (0x2U << (SPI_MOSI_PIN * 2));

  SPI_PORT->PUPDR &= ~(0x3U << (SPI_MOSI_PIN * 2));
  SPI_PORT->PUPDR |= (0x1U << (SPI_MOSI_PIN * 2));

  SPI_PORT->AFR[0] &= ~(0xFU << (SPI_MOSI_PIN * 4));
  SPI_PORT->AFR[0] |= (SPI_AF << (SPI_MOSI_PIN * 4));

  // SCK
  SPI_PORT->MODER &= ~(0x3U << (SPI_SCK_PIN * 2));
  SPI_PORT->MODER |= (0x2U << (SPI_SCK_PIN * 2));

  SPI_PORT->OSPEEDR &= ~(0x3U << (SPI_SCK_PIN * 2));
  SPI_PORT->OSPEEDR |= (0x2U << (SPI_SCK_PIN * 2));

  SPI_PORT->PUPDR &= ~(0x3U << (SPI_SCK_PIN * 2));

  SPI_PORT->AFR[0] &= ~(0xFU << (SPI_SCK_PIN * 4));
  SPI_PORT->AFR[0] |= (SPI_AF << (SPI_SCK_PIN * 4));

  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
  (void)RCC->IOPENR;

  SPI1->CR1 |= (baud_divider << SPI_CR1_BR_Pos);

  SPI1->CR1 |= (mode | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR);

  SPI1->CR1 |= SPI_CR1_RXONLY;

  SPI1->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_LSBFIRST | SPI_CR1_CRCEN);

  SPI1->CR2 &= ~SPI_CR1_DFF;
  SPI1->CR2 &= ~SPI_CR2_FRF;

  SPI1->CR1 |= SPI_CR1_SPE;
}

void spi_hal_write(const uint8_t *data, size_t length) {
  for (uint32_t i = 0; i < length; i++) {
    spi_hal_transfer_byte(data[i]);
  }
}

void spi_hal_transfer_byte(uint8_t byte) {
  while (!(SPI1->SR & SPI_SR_TXE))
    ;
  *(volatile uint8_t *)&SPI1->DR = byte;

  while (!(SPI1->SR & SPI_SR_RXNE))
    ;
  *(volatile uint8_t *)&SPI1->DR;
}
