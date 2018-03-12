#ifndef _UART3_CONFIG_H_
#define _UART3_CONFIG_H_

#include "inc/apa_uart.h"



#define RXBUFLEN 32
#define TXQUEUEBUFLEN 1024
#define QUEUELEN 32

struct tm_queue
{
  uint8_t *pData;
  uint16_t Len;
  bool_t bValid;
};
typedef struct tm_queue queue_t;

struct tm_uart
{
  uint8_t   *pWr_RxBuf;
  uint8_t   *pRd_RxBuf;
  uint8_t   *pTranse;
  uint8_t   *pWr_TxQueueBuf;
  queue_t   Queue[QUEUELEN];
  uint8_t   QueueRdIndex;
  uint8_t   QueueWrIndex;
  uint8_t   TxLen;
  uint8_t   RxBuffer[RXBUFLEN];
  uint8_t   TxQueueBuffer[TXQUEUEBUFLEN];
  uint8_t   NbrOfBlocksInQueue;
  bool_t    bDataSent;
};

typedef struct tm_uart uart_t;

#endif /* _TMUART_PRIVATE_H_ */
