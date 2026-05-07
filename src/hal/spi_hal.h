/**
 * @file spi_hal.h
 * @brief Blocking SPI HAL for a single master.
 *
 * Full duplex transmission with manual CS control. Designed with displays in
 * mind, so for now it voids received data. The CS line is configured in
 * spi_hal_open().
 *
 */
#ifndef SPI_HAL_H
#define SPI_HAL_H
#include "config.h"
#include "stm32l031xx.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @brief SPI clock phase and polarity mode.
 */
typedef enum {
  SPI_CLOCK_MODE_0 = 0,                           // CPOL=0 CPHA=0
  SPI_CLOCK_MODE_1 = SPI_CR1_CPHA,                // CPOL=0 CPHA=1
  SPI_CLOCK_MODE_2 = SPI_CR1_CPOL,                // CPOL=1 CPHA=0
  SPI_CLOCK_MODE_3 = SPI_CR1_CPOL | SPI_CR1_CPHA, // CPOL=1 CPHA=1
} spi_mode_t;

/**
 * @brief SPI baud rate control mode.
 */
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

/**
 * @brief Initialize SPI1 peripheral and configure its GPIO pins.
 *
 * Enables clocks, configures SCK (PB3), MOSI (PB5) and CS (PB6) as alternate
 * function outputs. Sets 8-bit frame format, MSB first and disables CRC.
 *
 * @param mode Clock polarity and phase mode.
 * @param baud_divider Baud rate divider for the peripheral configuration.
 * @return HAL_OK always.
 *
 */
hal_status_t spi_hal_open(spi_mode_t mode, spi_baud_divider_t baud_divider);

/**
 * @brief Transmit a buffer of bytes.
 *
 *  Tranfers @p length bytes from @p data.
 *
 * @code
 * uint8_t data[] = {0xA5, 0xFF, 0x12, 0x53};
 * spi_hal_cs_set(1);
 * spi_hal_write(data,sizeof(data),100); // transfer one byte
 * spi_hal_cs_set(0);
 * @endcode
 *
 * @param data Buffer to transmit.
 * @param length Number of bytes to transmit.
 * @param timeout_ms Timeout per byte in ms.
 *
 * @return HAL_OK on success, error code on failure.
 *
 */
hal_status_t spi_hal_write(const uint8_t *data, size_t length,
                           uint32_t timeout_ms);

/**
 * @brief Transmit one byte.
 *
 *  Tranfers 1 @p byte.
 *
 * @code
 * uint8_t byte = 0xA5;
 * spi_hal_cs_set(1);
 * spi_hal_transfer_byte(byte,100); // transfer one byte
 * spi_hal_cs_set(0);
 * @endcode
 *
 * @param byte Byte to transmit.
 * @param timeout_ms Operations timeout in ms.
 *
 * @return HAL_OK on success, error code on failure.
 *
 */
hal_status_t spi_hal_transfer_byte(uint8_t byte, uint32_t timeout_ms);

/**
 * @brief Assert or deassert chip select pin.
 *
 * CS is active low, so pass 1 to assert (pull low) and 0 to deassert (pull
 * high)
 *
 * @code
 * spi_hal_cs_set(1);						// assert CS
 * spi_hal_write(data,sizeof(data),100);
 * spi_hal_cs_set(0);    					// deassert CS
 * @endcode
 *
 * @param state 1 to assert CS, 0 to deassert.
 */
void spi_hal_cs_set(uint8_t state);

#endif
