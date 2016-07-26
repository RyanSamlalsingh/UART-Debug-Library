#ifndef __DEBUG_H
#define __DEBUG_H

/*
 * 
 *  uart_debug.h
 *  for debugging-use print functions of STM32F0s over UART
 *  
 *  Created May 24 2016 by rsamlalsingh
 *
 */

#include <stm32f0xx.h>

#define UART_TX_PIN           GPIO_PIN_2
#define UART_RX_PIN           GPIO_PIN_3

#define UART_MSG_TIMEOUT      10000
#define UART_BAUDRATE         9600
/*
 *  Usage instructions:
 *      -Uncomment "#define HAL_UART_MODULE_ENABLED" and
 *          "#define HAL_DMA_MODULE_ENABLED" in stm32f0xx_hal_conf.h
 *      -Call Debug_GPIOConfig() and Debug_UARTConfig() at some point AFTER 
 *          configuring system clocks
 *      -Then, use Debug_PrintChar(char *c) to print strings to UART,
 *          or use Debug_PrintLn(char *c) to print strings with a 
 *          unix-styled line ending
 */

void Debug_GPIOConfig();
void Debug_UARTConfig();
void Debug_PrintChar(char *c);
void Debug_PrintLn(char* c);

#endif
