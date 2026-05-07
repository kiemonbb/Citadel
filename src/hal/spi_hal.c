#include "spi_hal.h"
#include "config.h"
#include "stm32l031xx.h"
#include <stddef.h>
#include <stdint.h>

extern uint32_t systick_ms;

hal_status_t spi_hal_open(spi_mode_t mode, spi_baud_divider_t baud_divider) {
  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
  (void)RCC->IOPENR;

  /* Setup MOSI as PB5 pull-up AF.  */
  SPI_PORT->MODER &= ~(0x3U << (SPI_MOSI_PIN * 2));
  SPI_PORT->MODER |= (0x2U << (SPI_MOSI_PIN * 2));

  SPI_PORT->OSPEEDR &= ~(0x3U << (SPI_MOSI_PIN * 2));
  SPI_PORT->OSPEEDR |= (0x2U << (SPI_MOSI_PIN * 2));

  SPI_PORT->PUPDR &= ~(0x3U << (SPI_MOSI_PIN * 2));
  SPI_PORT->PUPDR |= (0x1U << (SPI_MOSI_PIN * 2));

  SPI_PORT->AFR[0] &= ~(0xFU << (SPI_MOSI_PIN * 4));
  SPI_PORT->AFR[0] |= (SPI_AF << (SPI_MOSI_PIN * 4));

  /* Setup SCK as PB3 pull-up AF.  */
  SPI_PORT->MODER &= ~(0x3U << (SPI_SCK_PIN * 2));
  SPI_PORT->MODER |= (0x2U << (SPI_SCK_PIN * 2));

  SPI_PORT->OSPEEDR &= ~(0x3U << (SPI_SCK_PIN * 2));
  SPI_PORT->OSPEEDR |= (0x2U << (SPI_SCK_PIN * 2));

  SPI_PORT->PUPDR &= ~(0x3U << (SPI_SCK_PIN * 2));
  SPI_PORT->PUPDR |= (0x1U << (SPI_SCK_PIN * 2));

  SPI_PORT->AFR[0] &= ~(0xFU << (SPI_SCK_PIN * 4));
  SPI_PORT->AFR[0] |= (SPI_AF << (SPI_SCK_PIN * 4));

  /* TODO: Test pins voltage level */
  /* Setup CS as PB6.  */
  SPI_PORT->MODER &= ~(0x3U << (SPI_CS_PIN * 2));
  SPI_PORT->MODER |= (0x1U << (SPI_CS_PIN * 2));

  SPI_PORT->OTYPER &= ~(1U << SPI_CS_PIN);

  SPI_PORT->OSPEEDR &= ~(0x3U << (SPI_CS_PIN * 2));
  SPI_PORT->OSPEEDR |= (0x2U << (SPI_CS_PIN * 2));

  SPI_PORT->PUPDR &= ~(0x3U << (SPI_CS_PIN * 2));

  SPI_PORT->BSRR = (1U << SPI_CS_PIN);

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  (void)RCC->APB2ENR;

  /* Reset configuration register */
  SPI1->CR1 = 0;

  /* Set baud rate divider and clock phase and polarity. Device works in master
   * mode, sends MSB first and uses Software Slave Managment via CS pin */
  SPI1->CR1 |= (baud_divider << SPI_CR1_BR_Pos);
  SPI1->CR1 |= (mode | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR);
  SPI1->CR2 &= ~(SPI_CR2_FRF);

  /* Enable SPI peripheral */
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
  /* Wait until data shifted out Data register */
  while (!(SPI1->SR & SPI_SR_TXE))
    ;
  /* Wait until  SPI is ready */
  while (SPI1->SR & SPI_SR_BSY)
    ;

  return HAL_OK;
}

hal_status_t spi_hal_transfer_byte(uint8_t byte, uint32_t timeout_ms) {
  uint32_t deadline = systick_ms + timeout_ms;
  while (!(SPI1->SR & SPI_SR_TXE)) {
    if ((timeout_ms == SPI_NO_DELAY) ||
        (timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline))
      return HAL_ERROR_TIMEOUT;
  }
  /* Fill the data register with the provided byte */
  *(volatile uint8_t *)&SPI1->DR = byte;

  while (!(SPI1->SR & SPI_SR_RXNE)) {
    if ((timeout_ms == SPI_NO_DELAY) ||
        (timeout_ms != SPI_MAX_DELAY && systick_ms >= deadline))
      return HAL_ERROR_TIMEOUT;
  }

  /* Remove received data */
  (void)SPI1->DR;

  return HAL_OK;
}
void spi_hal_cs_set(uint8_t state) {
  if (state)
    SPI_PORT->BRR = (1U << SPI_CS_PIN);
  else
    SPI_PORT->BSRR = (1U << SPI_CS_PIN);
}
