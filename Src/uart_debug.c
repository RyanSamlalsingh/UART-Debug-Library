/*
 * 
 *  uart_debug.c
 *  for debugging-use print functions of STM32F0s over UART
 *  
 *  Created May 24 2016 by rsamlalsingh
 *
 */

#include <stm32f0xx.h>
#include <stdint.h>
#include <uart_debug.h>
#include <string.h>

UART_HandleTypeDef UARTHandle;

/*
 *  UART CONFIGURATION DETAILS:
 *      -UART mode on USART2
 *      -TX Pin: PA2 (macro in uart_debug.h)
 *      -RX Pin: PA3 (macro in uart_debug.h)
 *      -Baud Rate: 9600 (macro in uart_debug.h)
 *      -Word lenght: 8 bits
 *      -Stop bit count: 1
 *      -Parity bit: None
 *      -Hardware flow control: None
 *      -Timeout: 10000ms (macro in uart_debug.h)
 *         *If messages become obscenely long and do not completely print before 
 *          being cut off by another message, try increasing timeout
 *
 *  Note: A very easy way to test these debug functions is to download and install
 *    the Arduino IDE, select the serial port corresponding to the appropriate
 *    microcontroller board under Tools->Serial Port in that IDE, and opening the 
 *    Serial Monitor by selecting Tools->Serial Monitor in the IDE. Then, set the
 *    Baud Rate in the Serial Monitor window to our configured Baud Rate (9600 
 *    by default) and set the Line Endings to be Unix-styled (Line Feed).
 *    If this setup is performed and the controller is attached by USB to the 
 *    computer, Debug_PrintChar() and Debug_PrintLn() messages will appear properly
 *    formatted on the Serial Monitor.
 *
 */

void Debug_GPIOConfig(){
    __GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef initStruct;
    
    initStruct.Pin = UART_TX_PIN | UART_RX_PIN;
    initStruct.Mode = GPIO_MODE_AF_PP;
    initStruct.Pull = GPIO_PULLUP;
    initStruct.Speed = GPIO_SPEED_HIGH;
    initStruct.Alternate = GPIO_AF1_USART2;

    HAL_GPIO_Init(GPIOA, &initStruct);
}
void Debug_UARTConfig() { 
    UARTHandle.Instance = USART2;
    UARTHandle.Init.BaudRate = UART_BAUDRATE;
    UARTHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UARTHandle.Init.StopBits = UART_STOPBITS_1;
    UARTHandle.Init.Parity = UART_PARITY_NONE;
    UARTHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UARTHandle.Init.Mode = UART_MODE_TX_RX;
    UARTHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    HAL_UART_DeInit(&UARTHandle);
    HAL_UART_Init(&UARTHandle);
}

void Debug_PrintChar(char *c) {
    HAL_UART_Transmit(&UARTHandle, (uint8_t*)c, strlen(c), UART_MSG_TIMEOUT);
}

void Debug_PrintLn(char* c){
    Debug_PrintChar(c);
    Debug_PrintChar("\n");
}


