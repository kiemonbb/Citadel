#ifndef DEBUG_UART_H
#define DEBUG_UART_H
#include "stm32l031xx.h"

void uart_open(void);
void uart_write(const char *data);
void uart_printf(const char *fmt, ...);

#endif
