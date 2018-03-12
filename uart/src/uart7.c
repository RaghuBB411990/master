#include "src/uart7_config.h"

static uart_t Uart7;

extern bool_t uart7rxevent;
extern bool_t uart7txevent;

void UART_7_Init(uint32_t BaudRate)
{
  /* Enable the peripherals used by this example */
  memset((void *)&Uart7,0,sizeof(Uart7));
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);

  HWREG(GPIO_PORTE_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTE_BASE + GPIO_O_CR) = GPIO_PIN_1;
  HWREG(GPIO_PORTE_BASE + GPIO_O_LOCK) = 0;

  Uart7.pRd_RxBuf = Uart7.RxBuffer;
  Uart7.pWr_RxBuf = Uart7.RxBuffer;

  Uart7.pWr_TxQueueBuf = Uart7.TxQueueBuffer;

  Uart7.QueueRdIndex = 0;
  Uart7.QueueWrIndex = 0;

  Uart7.pWr_TxQueueBuf = Uart7.TxQueueBuffer;
  Uart7.TxLen = 0;
  Uart7.bDataSent = TRUE;
  /* Enable processor interrupts */
  /* TODO: Move this statement to main loop */
  ROM_IntMasterEnable();

  /* Set GPIO A0 and A1 as UART pins */
  ROM_GPIOPinConfigure(GPIO_PE0_U7RX);
  ROM_GPIOPinConfigure(GPIO_PE1_U7TX);

  ROM_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  /* Configure the UART for 115,200, 8-N-1 operation */
  switch (BaudRate)
  {
    case 2400:
    case 4800:
    case 19200:
    case 38400:
    case 57600:
    case 115200:
      ROM_UARTConfigSetExpClk(UART7_BASE, ROM_SysCtlClockGet(), BaudRate,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
       UART_CONFIG_PAR_NONE));
      break;
    case 9600:
    default:
      ROM_UARTConfigSetExpClk(UART7_BASE, ROM_SysCtlClockGet(), 9600,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
       UART_CONFIG_PAR_NONE));
	  break;
  }

  ROM_UARTFIFOLevelSet(UART7_BASE, UART_FIFO_TX1_8,UART_FIFO_RX4_8);
  /* Enable the UART interrupt */
  ROM_IntEnable(INT_UART7);
  ROM_UARTIntEnable(UART7_BASE, (UART_INT_RX | UART_INT_RT | UART_INT_TX));
}

void UART7IntHandler(void)
{
  uint32_t ui32Status;
  uint8_t ch;

  /* Get the interrrupt status */
  ui32Status = ROM_UARTIntStatus(UART7_BASE, true);

  /* Clear the asserted interrupts */
  ROM_UARTIntClear(UART7_BASE, ui32Status);

  /* Check Tx or Rx Interrupt */
  /* Loop while there are characters in the receive FIFO */
  if(ui32Status & UART_INT_RX || ui32Status & UART_INT_RT)
  {
    while(ROM_UARTCharsAvail(UART7_BASE))
    {
      /* Read the next character from the UART and write it back to the UART */
      ch = ROM_UARTCharGetNonBlocking(UART7_BASE);
      *Uart7.pWr_RxBuf = ch;
      INCCIRCULARPTR(Uart7.pWr_RxBuf, Uart7.RxBuffer, RXBUFLEN);
      //ROM_UARTCharPutNonBlocking(UART7_BASE,ch);
    }
    uart7rxevent = TRUE;
    /* Set event */
  }

  if(ui32Status & UART_INT_TX)
  {
    if(Uart7.TxLen != 0)
    {
      /* Check if room available to write data */
      //while(1)
      {
        while(ROM_UARTSpaceAvail(UART7_BASE))
        {
          ROM_UARTCharPutNonBlocking(UART7_BASE, *Uart7.pTranse++);
          Uart7.TxLen--;
          if(Uart7.TxLen == 0)
          {
            /* Set tx done event */
            uart7txevent = TRUE;
            Uart7.bDataSent = TRUE;
            break;
          }
        }
      }
    }
  }
}

void UART_7_GetData(uint8_t *pData, uint8_t *Len)
{

  *Len = 0;
  /* Diable interrupts */
  ROM_IntMasterDisable();

  /* Copy data */
  *Len = Uart7.pWr_RxBuf - Uart7.RxBuffer;
  memcpy(pData,Uart7.RxBuffer, *Len);
  Uart7.pWr_RxBuf = Uart7.RxBuffer;

  /* Enable Interrupts */
  ROM_IntMasterEnable();
}

void UART_7_Write(uint8_t *pData, uint8_t Len)
{
  /* Diable interrupts */
  ROM_IntMasterDisable();

  /* Copy data */
  Uart7.TxLen = Len;
  Uart7.pTranse = pData;
  uart7txevent = FALSE;
  while(ROM_UARTSpaceAvail(UART7_BASE))
  {
    ROM_UARTCharPutNonBlocking(UART7_BASE, *Uart7.pTranse++);
    Uart7.TxLen--;
    if(Uart7.TxLen == 0)
    {
      /* Set tx done event */
      uart7txevent = TRUE;
      Uart7.bDataSent = TRUE;
      break;
    }
  }

  /* Enable Interrupts */
  ROM_IntMasterEnable();
}

void UART_7_WrToQueue(uint8_t *pData, uint16_t Len)
{
  u16_t tmpLen;
  /* Copy data to queue and update queue pts */

  tmpLen = TXQUEUEBUFLEN - (Uart7.pWr_TxQueueBuf - Uart7.TxQueueBuffer);

  if(tmpLen < Len)
  {
    /* Split data in to two queue entries */
    memcpy(Uart7.pWr_TxQueueBuf,pData,tmpLen);
    Uart7.Queue[Uart7.QueueWrIndex].pData = Uart7.pWr_TxQueueBuf;
    Uart7.Queue[Uart7.QueueWrIndex].Len = tmpLen;
    Uart7.Queue[Uart7.QueueWrIndex].bValid = TRUE;
    INCCIRCULARINDEX(Uart7.QueueWrIndex, QUEUELEN);
    Uart7.NbrOfBlocksInQueue++;

    memcpy(Uart7.TxQueueBuffer,pData+tmpLen,Len - tmpLen);
    Uart7.Queue[Uart7.QueueWrIndex].pData = Uart7.TxQueueBuffer;
    Uart7.Queue[Uart7.QueueWrIndex].Len = Len - tmpLen;
    Uart7.Queue[Uart7.QueueWrIndex].bValid = TRUE;
    INCCIRCULARINDEX(Uart7.QueueWrIndex, QUEUELEN);
    Uart7.pWr_TxQueueBuf = Uart7.TxQueueBuffer + Len - tmpLen;
    Uart7.NbrOfBlocksInQueue++;
  }
  else
  {
    memcpy(Uart7.pWr_TxQueueBuf,pData,Len);
    Uart7.Queue[Uart7.QueueWrIndex].pData = Uart7.pWr_TxQueueBuf;
    Uart7.Queue[Uart7.QueueWrIndex].Len = Len;
    Uart7.Queue[Uart7.QueueWrIndex].bValid = TRUE;
    ADDCIRCULARPTR(Uart7.pWr_TxQueueBuf, Len, Uart7.TxQueueBuffer, TXQUEUEBUFLEN);
    INCCIRCULARINDEX(Uart7.QueueWrIndex, QUEUELEN);
    Uart7.NbrOfBlocksInQueue++;
  }
  if(Uart7.bDataSent == TM_TRUE)
  {
    UART_7_CheckQueue();
  }
}

void UART_7_CheckQueue(void)
{
  if(Uart7.NbrOfBlocksInQueue != 0)
  {
    Uart7.bDataSent = FALSE;
    UART_7_Write(Uart7.Queue[Uart7.QueueRdIndex].pData, Uart7.Queue[Uart7.QueueRdIndex].Len);
    INCCIRCULARINDEX(Uart7.QueueRdIndex, QUEUELEN);
    Uart7.NbrOfBlocksInQueue--;
  }
}


