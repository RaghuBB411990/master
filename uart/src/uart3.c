#include "src/uart3_config.h"

static uart_t Uart3;

extern bool_t uart3rxevent;
extern bool_t uart3txevent;

void UART_3_Init(uint32_t BaudRate)
{
  /* Enable the peripherals used by this example */
  memset((void *)&Uart3,0,sizeof(Uart3));
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

  Uart3.pRd_RxBuf = Uart3.RxBuffer;
  Uart3.pWr_RxBuf = Uart3.RxBuffer;

  Uart3.pWr_TxQueueBuf = Uart3.TxQueueBuffer;

  Uart3.QueueRdIndex = 0;
  Uart3.QueueWrIndex = 0;

  Uart3.pWr_TxQueueBuf = Uart3.TxQueueBuffer;
  Uart3.TxLen = 0;
  Uart3.bDataSent = TRUE;

  /* Enable processor interrupts */
  /* TODO: Move this statement to main loop */
  ROM_IntMasterEnable();

  /* Set GPIO A0 and A1 as UART pins */
  ROM_GPIOPinConfigure(GPIO_PC6_U3RX);
  ROM_GPIOPinConfigure(GPIO_PC7_U3TX);
  ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

  /* Configure the UART for 115,200, 8-N-1 operation */
  switch (BaudRate)
  {
    case 2400:
    case 4800:
    case 19200:
    case 38400:
    case 57600:
    case 115200:
      ROM_UARTConfigSetExpClk(UART3_BASE, ROM_SysCtlClockGet(), BaudRate,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
       UART_CONFIG_PAR_NONE));
	  break;
    case 9600:
	default:
      ROM_UARTConfigSetExpClk(UART3_BASE, ROM_SysCtlClockGet(), 9600,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
       UART_CONFIG_PAR_NONE));
	  break;
  }

  ROM_UARTFIFOLevelSet(UART3_BASE, UART_FIFO_TX1_8,UART_FIFO_RX4_8);
  /* Enable the UART interrupt */
  ROM_IntEnable(INT_UART3);
  //ROM_UARTEnable(UART3_BASE);
  ROM_UARTIntEnable(UART3_BASE, (UART_INT_RX | UART_INT_RT | UART_INT_TX));
  //ROM_UARTCharPutNonBlocking(UART3_BASE, 'z');
}


void UART3IntHandler(void)
{
  uint32_t ui32Status;
  uint8_t ch;

  /* Get the interrrupt status */
  ui32Status = ROM_UARTIntStatus(UART3_BASE, true);

  /* Clear the asserted interrupts */
  ROM_UARTIntClear(UART3_BASE, ui32Status);

  /* Check Tx or Rx Interrupt */
  /* Loop while there are characters in the receive FIFO */
  if(ui32Status & UART_INT_RX || ui32Status & UART_INT_RT)
  {
    while(ROM_UARTCharsAvail(UART3_BASE))
    {
      /* Read the next character from the UART and write it back to the UART */
      ch = ROM_UARTCharGetNonBlocking(UART3_BASE);
      *Uart3.pWr_RxBuf = ch;
      INCCIRCULARPTR(Uart3.pWr_RxBuf, Uart3.RxBuffer, RXBUFLEN);
      //ROM_UARTCharPutNonBlocking(UART3_BASE,ch);
    }
    uart3rxevent = TRUE;
    /* Set event */
  }

  if(ui32Status & UART_INT_TX)
  {
    if(Uart3.TxLen != 0)
    {
      /* Check if room available to write data */
      //while(1)
      {
        while(ROM_UARTSpaceAvail(UART3_BASE))
        {
          ROM_UARTCharPutNonBlocking(UART3_BASE, *Uart3.pTranse++);
          Uart3.TxLen--;
          if(Uart3.TxLen == 0)
          {
            /* Set tx done event */
            uart3txevent = TRUE;
            Uart3.bDataSent = TRUE;
            break;
          }
        }
      }
    }
  }
}

void UART_3_GetData(uint8_t *pData, uint8_t *Len)
{

  *Len = 0;
  /* Diable interrupts */
  ROM_IntMasterDisable();

  /* Copy data */
  *Len = Uart3.pWr_RxBuf - Uart3.RxBuffer;
  memcpy(pData,Uart3.RxBuffer, *Len);
  Uart3.pWr_RxBuf = Uart3.RxBuffer;

  /* Enable Interrupts */
  ROM_IntMasterEnable();
}

void UART_3_Write(uint8_t *pData, uint8_t Len)
{
  /* Diable interrupts */
  ROM_IntMasterDisable();

  /* Copy data */
  Uart3.TxLen = Len;
  Uart3.pTranse = pData;
  uart3txevent = FALSE;
  while(ROM_UARTSpaceAvail(UART3_BASE))
  {
    ROM_UARTCharPutNonBlocking(UART3_BASE, *Uart3.pTranse++);
    Uart3.TxLen--;
    if(Uart3.TxLen == 0)
    {
      /* Set tx done event */
      uart3txevent = TRUE;
      Uart3.bDataSent = TRUE;
      break;
    }
  }

  /* Enable Interrupts */
  ROM_IntMasterEnable();
}

void UART_3_WrToQueue(uint8_t *pData, uint16_t Len)
{
  uint16_t tmpLen;
  /* Copy data to queue and update queue pts */

  tmpLen = TXQUEUEBUFLEN - (Uart3.pWr_TxQueueBuf - Uart3.TxQueueBuffer);

  if(tmpLen < Len)
  {
    /* Split data in to two queue entries */
    memcpy(Uart3.pWr_TxQueueBuf,pData,tmpLen);
    Uart3.Queue[Uart3.QueueWrIndex].pData = Uart3.pWr_TxQueueBuf;
    Uart3.Queue[Uart3.QueueWrIndex].Len = tmpLen;
    Uart3.Queue[Uart3.QueueWrIndex].bValid = TRUE;
    INCCIRCULARINDEX(Uart3.QueueWrIndex, QUEUELEN);
    Uart3.NbrOfBlocksInQueue++;

    memcpy(Uart3.TxQueueBuffer,pData+tmpLen,Len - tmpLen);
    Uart3.Queue[Uart3.QueueWrIndex].pData = Uart3.TxQueueBuffer;
    Uart3.Queue[Uart3.QueueWrIndex].Len = Len - tmpLen;
    Uart3.Queue[Uart3.QueueWrIndex].bValid = TRUE;
    INCCIRCULARINDEX(Uart3.QueueWrIndex, QUEUELEN);
    Uart3.pWr_TxQueueBuf = Uart3.TxQueueBuffer + Len - tmpLen;
    Uart3.NbrOfBlocksInQueue++;
  }
  else
  {
    memcpy(Uart3.pWr_TxQueueBuf,pData,Len);
    Uart3.Queue[Uart3.QueueWrIndex].pData = Uart3.pWr_TxQueueBuf;
    Uart3.Queue[Uart3.QueueWrIndex].Len = Len;
    Uart3.Queue[Uart3.QueueWrIndex].bValid = TRUE;
    ADDCIRCULARPTR(Uart3.pWr_TxQueueBuf, Len, Uart3.TxQueueBuffer, TXQUEUEBUFLEN);
    INCCIRCULARINDEX(Uart3.QueueWrIndex, QUEUELEN);
    Uart3.NbrOfBlocksInQueue++;
  }
  if(Uart3.bDataSent == TM_TRUE)
  {
    UART_3_CheckQueue();
  }
}

void UART_3_CheckQueue(void)
{
  if(Uart3.NbrOfBlocksInQueue != 0)
  {
    Uart3.bDataSent = FALSE;
    UART_3_Write(Uart3.Queue[Uart3.QueueRdIndex].pData, Uart3.Queue[Uart3.QueueRdIndex].Len);
    INCCIRCULARINDEX(Uart3.QueueRdIndex, QUEUELEN);
    Uart3.NbrOfBlocksInQueue--;
  }
}

#if 0
tm_bool_t Uart3QEmpty(void)
{
  if(g_Uart3.NbrOfBlocksInQueue == 0)
    return TM_TRUE;
  else
    return TM_FALSE;
}
#endif

