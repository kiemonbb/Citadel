#include "ds3231_driver.h"
#include "../hal/i2c_hal.h"
#include "stm32l031xx.h"

static uint8_t ds3231_dec_to_bcd(uint8_t dec) {
  return ((dec / 10) << 4) | (dec % 10);
}

static uint8_t ds3231_bcd_to_dec(uint8_t bcd) {
  return (bcd >> 4) * 10 + (bcd & 0xF);
}

ds3231_status_t ds3231_set_time(const ds3231_time_t *time,
                                uint32_t timeout_ms) {
  if (time == NULL)
    return DS3231_ERROR_INVALID_PARAM;

  uint8_t data[7] = {
      ds3231_dec_to_bcd(time->seconds), ds3231_dec_to_bcd(time->minutes),
      ds3231_dec_to_bcd(time->hours),   ds3231_dec_to_bcd(time->day),
      ds3231_dec_to_bcd(time->date),    ds3231_dec_to_bcd(time->month),
      ds3231_dec_to_bcd(time->year),
  };
  i2c_status_t err = i2c_hal_reg(DS3231_ADDRESS, DS3231_REG_SECONDS, data,
                                 sizeof(data), 0, timeout_ms);
  if (err != I2C_OK)
    return DS3231_ERROR_I2C;
  return DS3231_OK;
}

ds3231_status_t ds3231_get_time(ds3231_time_t *time, uint32_t timeout_ms) {
  if (time == NULL)
    return DS3231_ERROR_INVALID_PARAM;

  uint8_t data[7] = {0};
  i2c_status_t err = i2c_hal_reg(DS3231_ADDRESS, DS3231_REG_SECONDS, data,
                                 sizeof(data), 1, timeout_ms);

  if (err != I2C_OK)
    return DS3231_ERROR_I2C;

  time->seconds = ds3231_bcd_to_dec(data[0] & 0x7F);
  time->minutes = ds3231_bcd_to_dec(data[1] & 0x7F);
  time->hours = ds3231_bcd_to_dec(data[2] & 0x3F);
  time->day = ds3231_bcd_to_dec(data[3] & 0x07);
  time->date = ds3231_bcd_to_dec(data[4] & 0x3F);
  time->month = ds3231_bcd_to_dec(data[5] & 0x1F);
  time->year = ds3231_bcd_to_dec(data[6]);

  return DS3231_OK;
}

ds3231_status_t ds3231_get_temp(float *temp, uint32_t timeout_ms) {
  if (temp == NULL)
    return DS3231_ERROR_INVALID_PARAM;

  uint8_t data[2] = {0};
  i2c_status_t err =
      i2c_hal_reg(DS3231_ADDRESS, DS3231_REG_TEMP_MSB, data, 2, 1, timeout_ms);

  if (err != I2C_OK)
    return DS3231_ERROR_I2C;

  *temp = (int8_t)data[0] + ((data[1] >> 6) & 0x03) * 0.25f;

  return DS3231_OK;
}
