#include "config.h"
#include "stm32l031xx.h"
#include "systick.h"
int main(void) {
  systick_open();

  RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
  (void)RCC->IOPENR;

  USER_LED_PORT->MODER &= ~(0x3U << (USER_LED_PIN * 2));
  USER_LED_PORT->MODER |= (0x1U << (USER_LED_PIN * 2));

  USER_LED_PORT->OTYPER &= ~(0x1U << USER_LED_PIN);

  USER_LED_PORT->OSPEEDR &= ~(0x3U << (USER_LED_PIN * 2));
  USER_LED_PORT->OSPEEDR |= (0x2U << (USER_LED_PIN * 2));

  USER_LED_PORT->PUPDR &= ~(0x3U << (USER_LED_PIN * 2));

  while (1) {
    GPIOB->BSRR = (0x1U << USER_LED_PIN);
    delay_ms(500);
    GPIOB->BSRR = (0x1U << (USER_LED_PIN + 16));
    delay_ms(500);
  }
  return 0;
}
