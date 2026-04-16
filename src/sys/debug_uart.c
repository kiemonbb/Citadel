#include "config.h"
#include "stm32l031xx.h"
#include "stm32l0xx.h"

void uart_open(void) {
  // UART CONFIG
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
  (void)RCC->IOPENR;

  USART_PORT->MODER &= ~(0x3U << (USART_TX_PIN * 2));
  USART_PORT->MODER |= (0x2U << (USART_TX_PIN * 2));
  USART_PORT->AFR[0] &= ~(0xFU << (USART_TX_PIN * 4));
  USART_PORT->AFR[0] |= (USART_AF << (USART_TX_PIN * 4));

  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  (void)RCC->APB1ENR;

  USART2->CR1 &= ~USART_CR1_UE;

  USART2->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1 | USART_CR1_PCE);
  USART2->CR2 &= ~USART_CR2_STOP;
  USART2->BRR = (uint32_t)(SystemCoreClock / 115200);
  USART2->CR1 |= USART_CR1_UE | USART_CR1_TE;
}

void uart_putchar(char c) {
  while (!(USART2->ISR & USART_ISR_TXE))
    ;
  USART2->TDR = c;
}

void uart_write(const char *data) {
  while (*data) {
    uart_putchar(*data++);
  }
}

void uart_printf(const char *fmt, ...) {
  //   char buf[128];
  //   va_list args;
  //   va_start(args, fmt);
  //   vsnprintf(buf, sizeof(buf), fmt, args);
  //   va_end(args);
  //   uart_write(buf);
  //   uart_putchar('\r');
  //   uart_putchar('\n');
}
