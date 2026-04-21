#include "gpio_hal.h"
#include "stm32l031xx.h"

static uint32_t gpio_hal_enable_rcc(GPIO_TypeDef *port) {
  if (port == GPIOA) {
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    (void)RCC->IOPENR;
    return 0;
  } else if (port == GPIOB) {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    (void)RCC->IOPENR;
    return 0;
  } else if (port == GPIOC) {
    RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
    (void)RCC->IOPENR;
    return 0;
  } else if (port == GPIOH) {
    RCC->IOPENR |= RCC_IOPENR_GPIOHEN;
    (void)RCC->IOPENR;
    return 0;
  }
  return -1;
}

uint32_t gpio_hal_init_output(GPIO_TypeDef *port, uint32_t pin,
                              gpio_pull_t pull) {
  if (gpio_hal_enable_rcc(port) == -1)
    return -1;

  port->MODER &= ~(0x3U << (pin * 2));
  port->MODER |= (0x1U << (pin * 2));

  port->OTYPER &= ~(0x1U << pin);

  port->OSPEEDR &= ~(0x3U << (pin * 2));
  port->OSPEEDR |= (0x2U << (pin * 2));

  port->PUPDR &= ~(0x3U << (pin * 2));
  port->PUPDR |= (pull << (pin * 2));

  return 0;
}

uint32_t gpio_hal_init_input(GPIO_TypeDef *port, uint32_t pin,
                             gpio_pull_t pull) {
  if (gpio_hal_enable_rcc(port) == -1)
    return -1;

  port->MODER &= ~(0x3U << (pin * 2));
  port->MODER |= (0x1U << (pin * 2));

  port->PUPDR &= ~(0x3U << (pin * 2));
  port->PUPDR |= (pull << (pin * 2));

  return 0;
}

uint32_t gpio_hal_init_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af,
                          gpio_pull_t pull) {
  if (gpio_hal_enable_rcc(port) == -1)
    return -1;

  port->MODER &= ~(0x3U << (pin * 2));
  port->MODER |= (0x2U << (pin * 2));

  port->OSPEEDR &= ~(0x3U << (pin * 2));
  port->OSPEEDR |= (0x2U << (pin * 2));

  port->PUPDR &= ~(0x3U << (pin * 2));
  port->PUPDR |= (pull << (pin * 2));

  if (pin < 8) {
    port->AFR[0] &= ~(0xFU << (pin * 4));
    port->AFR[0] |= (af << (pin * 4));
  } else {
    uint32_t shift = (pin - 0x8U) * 0x4U;
    port->AFR[1] &= ~(0xFU << shift);
    port->AFR[1] |= (af << shift);
  }
  return 0;
}

uint32_t gpio_hal_read(GPIO_TypeDef *port, uint32_t pin) {
  uint32_t value = ((port->IDR >> pin) & 0x1U);
  return value;
}

void gpio_hal_write(GPIO_TypeDef *port, uint32_t pin, uint32_t value) {
  if (value)
    port->BSRR = (0x1U << pin);
  else
    port->BSRR = (0x1U << (pin + 16U));
}
