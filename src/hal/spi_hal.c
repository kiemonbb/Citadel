#include "spi_hal.h"
#include "config.h"
#include "stm32l031xx.h"
#include <stddef.h>
#include <stdint.h>

extern uint32_t systick_ms;

// TODO: implement extensive error handling

hal_status_t spi_hal_open(spi_mode_t mode, spi_baud_divider_t baud_divider) {

  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
  (void)RCC->IOPENR;

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
  SPI_PORT->PUPDR |= (0x1U << (SPI_SCK_PIN * 2));

  SPI_PORT->AFR[0] &= ~(0xFU << (SPI_SCK_PIN * 4));
  SPI_PORT->AFR[0] |= (SPI_AF << (SPI_SCK_PIN * 4));

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  (void)RCC->APB2ENR;

  SPI1->CR1 |= (baud_divider << SPI_CR1_BR_Pos);

  SPI1->CR1 |= (mode | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR);

  SPI1->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_LSBFIRST | SPI_CR1_CRCEN);

  SPI1->CR1 &= ~SPI_CR1_DFF;
  SPI1->CR2 &= ~SPI_CR2_FRF;

  SPI1->CR1 |= SPI_CR1_SPE;

  return HAL_OK;
}

hal_status_t spi_hal_write(const uint8_t *data, size_t length,
                           uint32_t timeout_ms) {
  for (uint32_t i = 0; i < length; i++) {
    hal_status_t byte_status = spi_hal_transfer_byte(data[i], timeout_ms);
    if (byte_status != HAL_OK)
      return byte_status;
  }
  while (!(SPI1->SR & SPI_SR_TXE))
    ;
  while (SPI1->SR & SPI_SR_BSY)
    ;
}

hal_status_t spi_hal_transfer_byte(uint8_t byte, uint32_t timeout_ms) {
  uint32_t deadline = systick_ms + timeout_ms;
  while (!(SPI1->SR & SPI_SR_TXE)) {
    if ((timeout_ms == SPI_NO_DELAY) ||
        (timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline))
      return HAL_ERROR_TIMEOUT;
  }
  *(volatile uint8_t *)&SPI1->DR = byte;

  while (!(SPI1->SR & SPI_SR_RXNE)) {
    if ((timeout_ms == SPI_NO_DELAY) ||
        (timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline))
      return HAL_ERROR_TIMEOUT;
  }
  (void)SPI1->DR;

  return HAL_OK;
}
