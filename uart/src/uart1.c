#include "src/uart1_config.h"

static uart_t Uart1;

extern bool_t uart1rxevent;
extern bool_t uart1txevent;

void UART_1_Init(uint32_t BaudRate)
{
  /* Enable the peripherals used by this example */
  memset((void *)&Uart1,0,sizeof(Uart1));
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

  Uart1.pRd_RxBuf = Uart1.RxBuffer;
  Uart1.pWr_RxBuf = Uart1.RxBuffer;

  Uart1.pWr_TxQueueBuf = Uart1.TxQueueBuffer;

  Uart1.QueueRdIndex = 0;
  Uart1.QueueWrIndex = 0;

  Uart1.pWr_TxQueueBuf = Uart1.TxQueueBuffer;
  Uart1.TxLen = 0;
  Uart1.bDataSent = TRUE;

  /* Enable processor interrupts */
  /* TODO: Move this statement to main loop */
  ROM_IntMasterEnable();

  /* Set GPIO A0 and A1 as UART pins */
  ROM_IntMasterEnable();

  ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
  ROM_GPIOPinConfigure(GPIO_PB1_U1TX);

  ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  switch (BaudRate)
  {
    case 2400:
    case 4800:
    case 9600:
    case 19200:
    case 38400:
    case 57600:
      ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), BaudRate,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
      break;
    case 115200:
    default:
      ROM_UARTConfigSetExpClk(UART1_BASE, ROM_SysCtlClockGet(), 115200,
      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
      break;
  }
  /*HWREG(UART1_BASE + UART_O_CTL) = ((HWREG(UART1_BASE + UART_O_CTL) &
                                     ~(UART_FLOWCONTROL_TX |
                                       UART_FLOWCONTROL_RX)) | (UART_FLOWCONTROL_TX | UART_FLOWCONTROL_RX));*/

  ROM_UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX1_8,UART_FIFO_RX4_8);
  ROM_IntEnable(INT_UART1);
  ROM_UARTIntEnable(UART1_BASE, (UART_INT_RX | UART_INT_RT | UART_INT_TX));
}


void UART1IntHandler(void)
{
  uint32_t ui32Status;
  uint8_t ch;

  /* Get the interrrupt status */
  ui32Status = ROM_UARTIntStatus(UART1_BASE, true);

  /* Clear the asserted interrupts */
  ROM_UARTIntClear(UART1_BASE, ui32Status);

  /* Check Tx or Rx Interrupt */
  /* Loop while there are characters in the receive FIFO */
  if(ui32Status & UART_INT_RX || ui32Status & UART_INT_RT)
  {
    while(ROM_UARTCharsAvail(UART1_BASE))
    {
      /* Read the next character from the UART and write it back to the UART */
      ch = ROM_UARTCharGetNonBlocking(UART1_BASE);
      *Uart1.pWr_RxBuf = ch;
      INCCIRCULARPTR(Uart1.pWr_RxBuf, Uart1.RxBuffer, RXBUFLEN);
      //ROM_UARTCharPutNonBlocking(UART1_BASE,ch);
    }
    uart1rxevent = TRUE;
    /* Set event */
  }

  if(ui32Status & UART_INT_TX)
  {
    if(Uart1.TxLen != 0)
    {
      /* Check if room available to write data */
      //while(1)
      {
        while(ROM_UARTSpaceAvail(UART1_BASE))
        {
          ROM_UARTCharPutNonBlocking(UART1_BASE, *Uart1.pTranse++);
          Uart1.TxLen--;
          if(Uart1.TxLen == 0)
          {
            /* Set tx done event */
            uart1txevent = TRUE;
            Uart1.bDataSent = TRUE;
            break;
          }
        }
      }
    }
  }
}

void UART_1_GetData(uint8_t *pData, uint8_t *Len)
{

  *Len = 0;
  /* Diable interrupts */
  ROM_IntMasterDisable();

  /* Copy data */
  *Len = Uart1.pWr_RxBuf - Uart1.RxBuffer;
  memcpy(pData,Uart1.RxBuffer, *Len);
  Uart1.pWr_RxBuf = Uart1.RxBuffer;

  /* Enable Interrupts */
  ROM_IntMasterEnable();
}

void UART_1_Write(uint8_t *pData, uint8_t Len)
{
  /* Diable interrupts */
  ROM_IntMasterDisable();

  /* Copy data */
  Uart1.TxLen = Len;
  Uart1.pTranse = pData;
  uart1txevent = FALSE;
  while(ROM_UARTSpaceAvail(UART1_BASE))
  {
    ROM_UARTCharPutNonBlocking(UART1_BASE, *Uart1.pTranse++);
    Uart1.TxLen--;
    if(Uart1.TxLen == 0)
    {
      /* Set tx done event */
      uart1txevent = TRUE;
      Uart1.bDataSent = TRUE;
      break;
    }
  }

  /* Enable Interrupts */
  ROM_IntMasterEnable();
}

void UART_1_WrToQueue(uint8_t *pData, uint16_t Len)
{
  u16_t tmpLen;
  /* Copy data to queue and update queue pts */

  tmpLen = TXQUEUEBUFLEN - (Uart1.pWr_TxQueueBuf - Uart1.TxQueueBuffer);

  if(tmpLen < Len)
  {
    /* Split data in to two queue entries */
    memcpy(Uart1.pWr_TxQueueBuf,pData,tmpLen);
    Uart1.Queue[Uart1.QueueWrIndex].pData = Uart1.pWr_TxQueueBuf;
    Uart1.Queue[Uart1.QueueWrIndex].Len = tmpLen;
    Uart1.Queue[Uart1.QueueWrIndex].bValid = TRUE;
    INCCIRCULARINDEX(Uart1.QueueWrIndex, QUEUELEN);
    Uart1.NbrOfBlocksInQueue++;

    memcpy(Uart1.TxQueueBuffer,pData+tmpLen,Len - tmpLen);
    Uart1.Queue[Uart1.QueueWrIndex].pData = Uart1.TxQueueBuffer;
    Uart1.Queue[Uart1.QueueWrIndex].Len = Len - tmpLen;
    Uart1.Queue[Uart1.QueueWrIndex].bValid = TRUE;
    INCCIRCULARINDEX(Uart1.QueueWrIndex, QUEUELEN);
    Uart1.pWr_TxQueueBuf = Uart1.TxQueueBuffer + Len - tmpLen;
    Uart1.NbrOfBlocksInQueue++;
  }
  else
  {
    memcpy(Uart1.pWr_TxQueueBuf,pData,Len);
    Uart1.Queue[Uart1.QueueWrIndex].pData = Uart1.pWr_TxQueueBuf;
    Uart1.Queue[Uart1.QueueWrIndex].Len = Len;
    Uart1.Queue[Uart1.QueueWrIndex].bValid = TRUE;
    ADDCIRCULARPTR(Uart1.pWr_TxQueueBuf, Len, Uart1.TxQueueBuffer, TXQUEUEBUFLEN);
    INCCIRCULARINDEX(Uart1.QueueWrIndex, QUEUELEN);
    Uart1.NbrOfBlocksInQueue++;
  }
  if(Uart1.bDataSent == TM_TRUE)
  {
    UART_1_CheckQueue();
  }
}

void UART_1_CheckQueue(void)
{
  if(Uart1.NbrOfBlocksInQueue != 0)
  {
    Uart1.bDataSent = FALSE;
    UART_1_Write(Uart1.Queue[Uart1.QueueRdIndex].pData, Uart1.Queue[Uart1.QueueRdIndex].Len);
    INCCIRCULARINDEX(Uart1.QueueRdIndex, QUEUELEN);
    Uart1.NbrOfBlocksInQueue--;
  }
}
