#ifndef _APP_UART_H_
#define _APP_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../App/inc/hw_memmap.h"
#include "../App/inc/hw_types.h"
#include "../App/driverlib/sysctl.h"
#include "../App/driverlib/gpio.h"
#include "../App/driverlib/uart.h"
#include "../App/driverlib/pin_map.h"
#include "../App/inc/hw_ints.h"
#include "../App/inc/hw_gpio.h"
#include "../App/driverlib/pin_map.h"
#include "../App/driverlib/debug.h"
#include "../App/driverlib/fpu.h"
#include "../App/driverlib/interrupt.h"
#include "../App/driverlib/rom.h"
#include "../App/inc/hw_uart.h"
#include "../utilities/inc/apatypes.h"
#include "../utilities/inc/utilities.h"

void UART_1_Init(uint32_t BaudRate);
void UART1IntHandler(void);
void UART_1_GetData(uint8_t *pData, uint8_t *Len);
void UART_1_PutData(uint8_t *pData, uint8_t Len);
void UART_1_WrToQueue(uint8_t *pData, uint16_t Len);
void UART_1_CheckQueue(void);

void UART_3_Init(uint32_t BaudRate);
void UART3IntHandler(void);
void UART_3_GetData(uint8_t *pData, uint8_t *Len);
void UART_3_PutData(uint8_t *pData,uint8_t Len);
void UART_3_WrToQueue(uint8_t *pData, uint16_t Len);
void UART_3_CheckQueue(void);

void UART_7_Init(uint32_t BaudRate);
void UART7IntHandler(void);
void UART_7_GetData(uint8_t *pData, uint8_t *Len);
void UART_7_PutData(uint8_t *pData, uint8_t Len);
void UART_7_WrToQueue(uint8_t *pData, uint16_t Len);
void UART_7_CheckQueue(void);

#endif

