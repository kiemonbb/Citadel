#ifndef I2C_HAL_H
#define I2C_HAL_H
#include "stm32l031xx.h"
#include <stddef.h>

#define I2C_TIMINGR_CFG 0x00000709 // for 2MHZ ~87kHz

typedef enum { I2C_STATE_IDLE, I2C_STATE_BUSY } i2c_state_t;

typedef enum {
  I2C_OK,
  I2C_ERROR_INVALID_PARAM,
  I2C_ERROR_NACK,
  I2C_ERROR_BUS,
  I2C_ERROR_TIMEOUT,
  I2C_ERROR_BUSY
} i2c_status_t;

i2c_status_t i2c_hal_open(void);

i2c_status_t i2c_hal_write_byte(uint8_t addr, const uint8_t data,
                                uint8_t is_last, uint32_t timeout_ms);

i2c_status_t i2c_hal_read_byte(uint8_t slave_addr, uint8_t *data, uint8_t ack,
                               uint32_t timeout_ms);

i2c_status_t i2c_hal_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data,
                         size_t length, uint8_t read, uint32_t timeout_ms);

i2c_status_t i2c_hal_close(void);

#endif
