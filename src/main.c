#include "config.h"
#include "hal/gpio_hal.h"
#include "hal/i2c_hal.h"
#include "stm32l031xx.h"
#include "systick.h"
int main(void) {
  systick_open();
  i2c_hal_open();

  uint8_t test[] = {0xAA, 0xBB, 0xCC, 0xDD};
  uint8_t recv_buffer[4] = {0};

  int value = 1;
  while (1) {
    delay_ms(5000);
    i2c_hal_reg(0x50, 0x00, test, sizeof(test), 0, 100);
    delay_ms(100);
    i2c_hal_reg(0x50, 0x00, recv_buffer, sizeof(recv_buffer), 1, 100);
  }
  return 0;
}
