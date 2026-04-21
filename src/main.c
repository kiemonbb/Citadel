#include "config.h"
#include "hal/gpio_hal.h"
#include "stm32l031xx.h"
#include "systick.h"
int main(void) {
  systick_open();

  gpio_hal_init_output(USER_LED_PORT, USER_LED_PIN, GPIO_PULL_UP);

  while (1) {
    gpio_hal_write(USER_LED_PORT, USER_LED_PIN, 1);
    delay_ms(500);
    gpio_hal_write(USER_LED_PORT, USER_LED_PIN, 0);
    delay_ms(500);
  }
  return 0;
}
