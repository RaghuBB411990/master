#ifndef _UART7_CONFIG_H_
#define _UART7_CONFIG_H_

#include "inc/apa_uart.h"


#define RXBUFLEN 64
#define TXQUEUEBUFLEN 128
#define QUEUELEN 32

struct tm_queue
{
  u8_t *pData;
  u16_t Len;
  bool_t bValid;
};
typedef struct tm_queue queue_t;

struct tm_uart
{
  u8_t   *pWr_RxBuf;
  u8_t   *pRd_RxBuf;
  u8_t   *pTranse;
  u8_t   *pWr_TxQueueBuf;
  queue_t Queue[QUEUELEN];
  u8_t   QueueRdIndex;
  u8_t   QueueWrIndex;
  u8_t   TxLen;
  u8_t   RxBuffer[RXBUFLEN];
  u8_t   TxQueueBuffer[TXQUEUEBUFLEN];
  u8_t   NbrOfBlocksInQueue;
  bool_t bDataSent;
};

typedef struct tm_uart uart_t;

#endif /* _TMUART_PRIVATE_H_ */
