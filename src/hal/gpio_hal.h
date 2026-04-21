#ifndef GPIO_HAL_H
#define GPIO_HAL_H
#include "stm32l031xx.h"
#include <stdint.h>

typedef enum {
  GPIO_PULL_NONE = 0x0,
  GPIO_PULL_UP = 0x1,
  GPIO_PULL_DOWN = 0x2
} gpio_pull_t;

uint32_t gpio_hal_init_output(GPIO_TypeDef *port, uint32_t pin,
                             gpio_pull_t pull);

uint32_t gpio_hal_init_input(GPIO_TypeDef *port, uint32_t pin, gpio_pull_t pull);

uint32_t gpio_hal_init_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af,
                         gpio_pull_t pull);

uint32_t gpio_hal_read(GPIO_TypeDef *port, uint32_t pin);

void gpio_hal_write(GPIO_TypeDef *port, uint32_t pin, uint32_t value);

#endif
