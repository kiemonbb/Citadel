#include "i2c_hal.h"
#include "config.h"
#include "stm32l031xx.h"

static i2c_state_t _state = I2C_STATE_IDLE;
extern uint32_t systick_ms;

static i2c_status_t i2c_wait(uint32_t flag, uint32_t timeout_ms) {
  uint32_t deadline = systick_ms + timeout_ms;
  while (!(I2C1->ISR & flag)) {
    if (I2C1->ISR & I2C_ISR_NACKF)
      return I2C_ERROR_NACK;
    if (I2C1->ISR & I2C_ISR_ARLO)
      return I2C_ERROR_BUS;
    if (I2C1->ISR & I2C_ISR_BERR)
      return I2C_ERROR_BUS;
    if (systick_ms >= deadline)
      return I2C_ERROR_TIMEOUT;
  }
  return I2C_OK;
}

static void i2c_clear_flags(void) {
  I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF | I2C_ICR_ARLOCF | I2C_ICR_BERRCF;
}

i2c_status_t i2c_hal_open(void) {
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  (void)RCC->APB1ENR;

  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
  (void)RCC->IOPENR;

  // SCL PORT
  I2C_PORT->MODER &= ~(0x3U << (I2C_SCL_PIN * 2));
  I2C_PORT->MODER |= (0x2U << (I2C_SCL_PIN * 2));

  I2C_PORT->OSPEEDR &= ~(0x3U << (I2C_SCL_PIN * 2));
  I2C_PORT->OSPEEDR |= (0x3U << (I2C_SCL_PIN * 2));

  I2C_PORT->OTYPER |= (0x1 << I2C_SCL_PIN);
  I2C_PORT->PUPDR &= ~(0x3U << (I2C_SCL_PIN * 2));

  uint32_t shift_scl = (I2C_SCL_PIN - 0x8U) * 0x4U;
  I2C_PORT->AFR[1] &= ~(0xFU << shift_scl);
  I2C_PORT->AFR[1] |= (I2C_AF << shift_scl);

  // SDA PORT
  I2C_PORT->MODER &= ~(0x3U << (I2C_SDA_PIN * 2));
  I2C_PORT->MODER |= (0x2U << (I2C_SDA_PIN * 2));

  I2C_PORT->OSPEEDR &= ~(0x3U << (I2C_SDA_PIN * 2));
  I2C_PORT->OSPEEDR |= (0x3U << (I2C_SDA_PIN * 2));

  I2C_PORT->OTYPER |= (0x1 << I2C_SDA_PIN);
  I2C_PORT->PUPDR &= ~(0x3U << (I2C_SDA_PIN * 2));

  uint32_t shift_sda = (I2C_SDA_PIN - 0x8U) * 0x4U;
  I2C_PORT->AFR[1] &= ~(0xFU << shift_sda);
  I2C_PORT->AFR[1] |= (I2C_AF << shift_sda);

  // I2C PERIPHERAL

  I2C1->CR1 &= ~I2C_CR1_PE;
  I2C1->TIMINGR = I2C_TIMINGR_CFG;

  I2C1->CR1 &= ~I2C_CR1_ANFOFF;
  I2C1->CR1 &= ~I2C_CR1_DNF;

  // I2C1->CR1 |= I2C_CR1_ERRIE;
  I2C1->CR1 |= I2C_CR1_PE;

  return I2C_OK;
}

i2c_status_t i2c_hal_write_byte(uint8_t slave_addr, const uint8_t data,
                                uint8_t not_last, uint32_t timeout_ms) {
  i2c_status_t err;
  // NEW TRANSFER
  if (_state == I2C_STATE_IDLE) {
    i2c_clear_flags();
    if (I2C1->ISR & I2C_ISR_BUSY)
      return I2C_ERROR_BUSY;

    I2C1->CR2 = ((uint32_t)(slave_addr & 0x7FU) << 1U) |
                (1UL << I2C_CR2_NBYTES_Pos) | I2C_CR2_RELOAD | I2C_CR2_START;

    _state = I2C_STATE_BUSY;
  } else {
    err = i2c_wait(I2C_ISR_TCR, timeout_ms);

    if (err != I2C_OK) {
      I2C1->ICR = I2C_ICR_NACKCF;
      I2C1->CR2 |= I2C_CR2_STOP;
      i2c_wait(I2C_ISR_STOPF, timeout_ms);
      I2C1->ICR = I2C_ICR_STOPCF;
      _state = I2C_STATE_IDLE;
      return err;
    }

    // Continue transaction
    uint32_t CR2 = I2C1->CR2;
    CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND);
    CR2 |= (1UL << I2C_CR2_NBYTES_Pos);
    CR2 |= not_last ? I2C_CR2_RELOAD : I2C_CR2_AUTOEND;
    I2C1->CR2 = CR2;
  }

  err = i2c_wait(I2C_ISR_TXIS, timeout_ms);
  if (err != I2C_OK) {
    I2C1->ICR = I2C_ICR_NACKCF;
    I2C1->CR2 |= I2C_CR2_STOP;
    i2c_wait(I2C_ISR_STOPF, timeout_ms);
    I2C1->ICR = I2C_ICR_STOPCF;
    _state = I2C_STATE_IDLE;
    return err;
  }

  I2C1->TXDR = data;
  if (!not_last) {
    err = i2c_wait(I2C_ISR_STOPF, timeout_ms);
    I2C1->ICR = I2C_ICR_STOPCF;
    _state = I2C_STATE_IDLE;
    return err;
  }
  return I2C_OK;
}

i2c_status_t i2c_hal_read_byte(uint8_t slave_addr, uint8_t *data, uint8_t ack,
                               uint32_t timeout_ms) {
  i2c_status_t err;
  // NEW READ
  if (_state == I2C_STATE_IDLE) {
    i2c_clear_flags();
    if (I2C1->ISR & I2C_ISR_BUSY)
      return I2C_ERROR_BUSY;

    I2C1->CR2 = ((uint32_t)(slave_addr & 0x7FU) << 1U) |
                (1UL << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN | I2C_CR2_RELOAD |
                I2C_CR2_START;

    _state = I2C_STATE_BUSY;
  } else {

    err = i2c_wait(I2C_ISR_TCR, timeout_ms);
    if (err != I2C_OK) {
      I2C1->ICR = I2C_ICR_NACKCF;
      I2C1->CR2 = I2C_CR2_STOP;
      i2c_wait(I2C_ISR_STOPF, timeout_ms);
      I2C1->ICR = I2C_ICR_STOPCF;
      _state = I2C_STATE_IDLE;
      return err;
    }

    // Continue transaction
    uint32_t CR2 = I2C1->CR2;
    CR2 &= ~(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND);
    CR2 |= (1UL << I2C_CR2_NBYTES_Pos);
    CR2 |= ack ? I2C_CR2_RELOAD : I2C_CR2_AUTOEND;
    I2C1->CR2 = CR2;
  }

  err = i2c_wait(I2C_ISR_RXNE, timeout_ms);
  if (err != I2C_OK) {
    I2C1->ICR = I2C_ICR_NACKCF;
    I2C1->CR2 = I2C_CR2_STOP;
    i2c_wait(I2C_ISR_STOPF, timeout_ms);
    I2C1->ICR = I2C_ICR_STOPCF;
    _state = I2C_STATE_IDLE;
    return err;
  }

  *data = (uint8_t)I2C1->RXDR;
  if (!ack) {
    err = i2c_wait(I2C_ISR_STOPF, timeout_ms);
    I2C1->ICR = I2C_ICR_STOPCF;
    _state = I2C_STATE_IDLE;
    return err;
  }
  return I2C_OK;
}

i2c_status_t i2c_hal_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data,
                         size_t length, uint8_t read, uint32_t timeout_ms) {
  if (data == NULL && length > 0U) {
    return I2C_ERROR_INVALID_PARAM;
  }

  i2c_status_t err;
  err = i2c_hal_write_byte(slave_addr, reg_addr, length == 0U, timeout_ms);
  if (err != I2C_OK)
    return err;

  /* WRITE */
  if (!read) {
    for (size_t i = 0; i < length; i++) {
      err =
          i2c_hal_write_byte(slave_addr, data[i], i == length - 1U, timeout_ms);
      if (err != I2C_OK)
        return err;
    }
    return I2C_OK;
  }
  /* READ */
  else {
    for (size_t i = 0; i < length; i++) {
      err =
          i2c_hal_read_byte(slave_addr, &data[i], i < length - 1U, timeout_ms);
      if (err != I2C_OK)
        return err;
    }
    return I2C_OK;
  }
}
