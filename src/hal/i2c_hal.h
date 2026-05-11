/**
 * @file i2c_hal.h
 * @brief Blocking I2C HAL.**
 */
#ifndef I2C_HAL_H
#define I2C_HAL_H
#include "stm32l031xx.h"
#include <stddef.h>

/**
 * @brief Configuration of the I2C1 Timing register to work at ~87kHz for 2MHz
 * clock.
 */
#define I2C_TIMINGR_CFG 0x00000709 // for 2MHZ ~87kHz

/**
 * @brief Current state of the I2C peripheral - tracks whether a transfer is in
 * progress.
 */
typedef enum { I2C_STATE_IDLE, I2C_STATE_BUSY } i2c_state_t;

/**
 * @brief Return codes for all I2C HAL functions.
 */
typedef enum {
  I2C_OK,
  I2C_ERROR_INVALID_PARAM,
  I2C_ERROR_NACK,
  I2C_ERROR_BUS,
  I2C_ERROR_TIMEOUT,
  I2C_ERROR_BUSY
} i2c_status_t;
/**
 * @brief Initialize I2C1 peripheral and configure its GPIO pins.
 *
 * Enables clocks, configures SCL (PA9), SDA (PA10) as alternate function
 * outputs and applies timing from @p I2C_TIMINGR_CFG.
 *
 * @return I2C_OK always.
 **/
i2c_status_t i2c_hal_open(void);

/**
 * @brief Transmit a single byte over I2C.
 *
 * If the driver is IDLE, initiates a new write transfer with START condition.
 * If the driver is BUSY, continues the existing RELOAD transaction.
 *
 * @code
 * i2c_hal_write_byte(0x44, 0xAA, 1, 100); //
 * i2c_hal_write_byte(0x44, 0x01, 0, 100); //last byte sends STOP
 * @endcode
 *
 * @param slave_addr 7-bit slave addres.
 * @param data Byte to transmit.
 * @param not_last 1 to keep the transfer going, 0 to send STOP after this byte.
 * @param timeout_ms Timeout per operation in ms.
 *
 * @return I2C_OK on success, error code on failure.
 **/
i2c_status_t i2c_hal_write_byte(uint8_t slave_addr, const uint8_t data,
                                uint8_t is_last, uint32_t timeout_ms);

/**
 * @brief Receive a single byte over I2C.
 *
 * If the driver is IDLE, initiates a new read transfer with START condition.
 * If the driver is BUSY, continues the existing RELOAD transaction.
 *
 * @code
 * uint8_t buf[2];
 * i2c_hal_read_byte(0x44, &buf[0], 1, 100); //
 * i2c_hal_read_byte(0x44, &buf[1], 0, 100); //last byte sends NACK
 * @endcode
 *
 * @param slave_addr 7-bit slave addres.
 * @param data Pointer to store received byte. Must not be NULL.
 * @param ack 1 to ACK this byte, 0 to NACK.
 * @param timeout_ms Timeout per operation in ms.
 *
 * @return I2C_OK on success, error code on failure.
 **/
i2c_status_t i2c_hal_read_byte(uint8_t slave_addr, uint8_t *data, uint8_t ack,
                               uint32_t timeout_ms);

/**
 * @brief Write or read to/from a register over I2C.
 *
 * Sends slave and register address, then writes or reads @p length bytes.
 *
 * @code
 * // Transmit 1 byte to 0xAA register.
 * uint8_t byte = 0x01;
 * i2c_hal_reg(0x44 ,0xAA ,&byte ,1 ,0 ,100);
 *
 * // Read 2 byte from 0xCD register.
 * uint8_t buf[2];
 * i2c_hal_reg(0x44 ,0xCD ,buf ,2 ,1 ,100);
 * @endcode
 *
 * @param slave_addr 7-bit slave addres.
 * @param reg_addr Slave's register address.
 * @param data Buffer to transmit from or read into.
 * @param length Number of bytes to transmit/read after register address.
 * @param read 1 for read, 0 for write
 * @param timeout_ms Timeout per operation in ms.
 *
 * @return I2C_OK on success, error code on failure.
 **/
i2c_status_t i2c_hal_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data,
                         size_t length, uint8_t read, uint32_t timeout_ms);

i2c_status_t i2c_hal_close(void);

#endif
