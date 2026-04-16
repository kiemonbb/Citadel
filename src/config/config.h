#ifndef CONFIG_H
#define CONFIG_H
#include "stm32l031xx.h"

/* USART */
#define USART_TX_PIN 2
#define USART_AF 4

#define USART_PORT GPIOA

/* BOARD LED */
#define USER_LED_PIN 3

#define USER_LED_PORT GPIOB

/* SPI */
#define SPI_MOSI_PIN 5
#define SPI_SCK_PIN 3
#define SPI_AF 0

#define SPI_NO_DELAY 0
#define SPI_MAX_DELAY 0xFFFFFFFF

#define SPI_PORT GPIOB

/* I2C */

#define I2C_SDA_PIN 10
#define I2C_SCL_PIN 9

#define I2C_PORT GPIOA

/* OTHERS */

#define DEBOUNCE_MS 50
#define DISPLAY_ON_MS 5000

#endif
