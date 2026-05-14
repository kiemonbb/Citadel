#ifndef DS3231_DRIVER
#define DS3231_DRIVER
#include "stm32l031xx.h"

#define DS3231_ADDRESS 0x68

#define DS3231_REG_SECONDS 0x00
#define DS3231_REG_MINUTES 0x01
#define DS3231_REG_HOUR 0x02
#define DS3231_REG_DAY 0x03
#define DS3231_REG_DATE 0x04
#define DS3231_REG_MONTH 0x05
#define DS3231_REG_YEAR 0x06
#define DS3231_REG_TEMP_MSB 0x11
#define DS3231_REG_TEMP_LSB 0x12

#define DS3231_REG_CONTROL 0x0E
#define DS3231_REG_STATUS 0x0F

#define DS3231_CONTROL_NEOSC (1 << 7U)
#define DS3231_STATUS_OSF (1 << 7U)
#define DS3231_STATUS_EN32kHZ (1 << 3U)
#define DS3231_STATUS_BSY (1 << 2U)

typedef enum {
  DS3231_OK = 0,
  DS3231_ERROR_I2C,
  DS3231_ERROR_INVALID_PARAM,
} ds3231_status_t;

typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;

  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint8_t year;
} ds3231_time_t;

ds3231_status_t ds3231_set_time(const ds3231_time_t *time, uint32_t timeout_ms);

ds3231_status_t ds3231_get_time(ds3231_time_t *time, uint32_t timeout_ms);

ds3231_status_t ds3231_get_temp(float *temp, uint32_t timeout_ms);

#endif
