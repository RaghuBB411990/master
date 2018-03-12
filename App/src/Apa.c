#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "apa.h"


#define RCC2_USERCC2     0x80000000  // Use RCC2
#define RCC2_DIV400      0x40000000  // Divide PLL as 400 MHz vs. 200
#define RCC2_SYSDIV2_M   0x1F800000  // System Clock Divisor 2

bool_t uart1rxevent=FALSE;
bool_t uart1txevent=TRUE;
bool_t uart3rxevent=FALSE;
bool_t uart3txevent=TRUE;
bool_t uart7rxevent=FALSE;
bool_t uart7txevent=TRUE;
bool_t  OneSecEvent=FALSE;
bool_t tenmilisec=FALSE;

apa_aparture_st App;

uint8_t SoftwareVer[]="1.00";
uint8_t Customer[]="Aparture";

char DebugStr[256];
uint8_t Pass[15],PassIndex=0;
bool_t ReturnVal;
static int a=0;
int main(void)
{
  /*When lazy stacking is enabled, space is reserved on the stack for
   * the floating-point context, but the floating-point state is not saved*/
#if 1
  ROM_FPULazyStackingEnable();
  /* Set the clocking to run from internal clock and set system clock to 80 MHz*/
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_OSC_INT |  RCC2_USERCC2 | RCC2_DIV400 );
  ROM_SysCtlMOSCConfigSet(SYSCTL_MOSC_NO_XTAL);
#endif
  OneSecTimer();
  xTimersInit();
  App.MainAppState=-1;
  UART_3_Init(115200);
  UART_7_Init(9600);
  UART_1_Init(115200);
  ParseInit(&App.parse,&App.SysConfig);
  GpsInit(&App.Gps,&App.SysConfig);
  Gsm_init(&App.Gsm,&App.SysConfig);
  App.Password=FALSE;
  App.TimeStamp=0;
  PrevResetCause();
  ReturnVal=xCreatTimer(&App.CheckTimerFunc,checktimerag,10,&App);
  if(ReturnVal==FALSE)
    PRINTF("TimerNotCreated\r\n");
  xTimerReset(App.CheckTimerFunc,10);
  //PRINTF("Jai Shree Ram, Jai Shree Ram, Jai Shree Ram, Jai Shree Ram, Jai Shree Ram, Jai Shree Ram\r\n");
  //my_itoa(12345678,&DebugStr[0]);
  //PRINTF(DebugStr);
  //PRINTF("\r\n");
  while(1)
  {
    ROM_IntMasterDisable();
    if(uart1rxevent==TRUE)
    {
      uart1rxevent=FALSE;
      App.MainAppState=COM1RXEVENT;
      goto Execute;
    }
    if(uart3rxevent==TRUE)
    {
      uart3rxevent=FALSE;
      App.MainAppState=COM3RXEVENT;
      goto Execute;
    }
    if(uart3txevent==TRUE)
    {
      uart3txevent=FALSE;
      App.MainAppState=COM3TXEVENT;
      goto Execute;
    }
    if(tenmilisec==TRUE)
    {
      tenmilisec=FALSE;
      App.MainAppState=TENMILIEVENT;
      goto Execute;
    }
    if(OneSecEvent==TRUE)
    {
      OneSecEvent=FALSE;
      App.MainAppState=ONESECEVENT;
      goto Execute;
    }
    if(uart7rxevent==TRUE)
    {
      uart7rxevent=FALSE;
      App.MainAppState=COM7RXEVENT;
      goto Execute;
    }
    if(uart1txevent==TRUE)
    {
      uart1txevent=FALSE;
      App.MainAppState=COM1TXEVENT;
      goto Execute;
    }
    if(uart7txevent==TRUE)
    {
      uart7txevent=FALSE;
      App.MainAppState=COM7TXEVENT;
      goto Execute;
    }
    Execute:
    ROM_IntMasterEnable();
    switch(App.MainAppState)
    {
      case COM1RXEVENT:
        UART_1_GetData(App.Uart1RxBuf,&App.uart1rxLen);
        if(App.SysConfig.GsmDebugEnable==TRUE)
          UART_3_WrToQueue(App.Uart1RxBuf,App.uart1rxLen);
        HandleGsmData(&App.Gsm,App.Uart1RxBuf,App.uart1rxLen);
        break;
      case COM3RXEVENT:
        UART_3_GetData(App.Uart3RxBuf,&App.uart3rxLen);
        if(App.Password==TRUE)
          UART_3_WrToQueue(App.Uart3RxBuf,App.uart3rxLen);
        /*else
          UART_3_WrToQueue("*",1);*/
        HandleDebugPortData(&App, App.Uart3RxBuf, App.uart3rxLen);
        break;
      case COM3TXEVENT:
        UART_3_CheckQueue();
        break;
      case COM7RXEVENT:
        UART_7_GetData(App.Uart7RxBuf,&App.uart7rxLen);
        if(App.SysConfig.GpsDebugEnable==TRUE)
          UART_3_WrToQueue(App.Uart7RxBuf,App.uart7rxLen);
        HandleGPSPort(&App.Gps, App.Uart7RxBuf, App.uart7rxLen);
        break;
      case COM1TXEVENT:
        UART_1_CheckQueue();
        break;
      case COM7TXEVENT:
        UART_7_CheckQueue();
        break;
      case TENMILIEVENT:
        xTimerCheck();
        a++;
        if(App.SysConfig.SendGsmCmd==TRUE)
        {
          //App.SysConfig.SendGsmCmd = FALSE;
          GsmControl(&App.Gsm);
          //SetConnTask(&App.Gsm,PROFILE0_CONNECT);
        }
        if(App.SysConfig.SysDebugEnable==TRUE)
        {
          App.SysConfig.SysDebugEnable=FALSE;
          SetConnTask(&App.Gsm,PROFILE0_CONNECT);
          //PRINTF("Raghavendra\r\n");
        }
        break;
      case ONESECEVENT:
        App.TimeStamp++;
        GsmIncrementTimeCnt(&App.Gsm);
#ifdef APARTURE
        //PRINTF("Ganesh\r\n");
#endif
        /*if(App.SysConfig.GsmDebugEnable==TRUE)
        {
          my_itoa(a,DebugStr);
          PRINTF("a=");
          PRINTF(DebugStr);
          PRINTF("\r\n");
          a=0;
        }*/
        /*if(App.SysConfig.SysDebugEnable==TRUE)
        {
          PRINTF("Raghavendra\r\n");
        }*/
        break;
      default:
        break;
    }
  }
}

void PrevResetCause()
{
  uint32_t ResetCause;
  ResetCause = ROM_SysCtlResetCauseGet();
  PRINTF("ResetCause:");
  switch(ResetCause)
  {
    case SYSCTL_CAUSE_LDO:
      PRINTF("LDO\r\n");
      break;
    case SYSCTL_CAUSE_SW:
      PRINTF("SoftwareReset\r\n");
      break;
    case SYSCTL_CAUSE_WDOG:
      PRINTF("Watchdog\r\n");
      break;
    case SYSCTL_CAUSE_BOR:
      PRINTF("BurnOut\r\n");
      break;
    case SYSCTL_CAUSE_POR:
      PRINTF("PowerOnReset\r\n");
      break;
    case SYSCTL_CAUSE_EXT:
      PRINTF("ExternalEvent\r\n");
      break;
  }
 ROM_SysCtlResetCauseClear(ResetCause);
}

void HandleDebugPortData(apa_aparture_st *pApp, uint8_t *pChar, uint8_t Len)
{
  cmdresp_t Responsetype;
  if(CheckPassword(pApp,pChar,Len)==TRUE)
  {
    Responsetype=ParsePortData(&pApp->parse, pChar, Len);
    if(Responsetype==CMDOK)
      PRINTF("\rOK\r\n");
    else if(Responsetype==CMDNOK)
      PRINTF("\rERROR\r\n");
  }
}

bool_t CheckPassword(apa_aparture_st *pApp, uint8_t *pChar, int Len)
{
  int i;
  if(pApp->Password==TRUE)
    return TRUE;
  for(i=0;i<Len;i++)
  {
    //UART3QueueData(star,strlen(star));
    if(pChar[i] == '\n' || pChar[i] == '\r')
    {
      if(!strncmp("APARTURE",&Pass[0],strlen("APARTURE")))
      {
        pApp->Password=TRUE;
        PassIndex=0;
        Pass[0]='\0';
        //pChar[0]='\0';
        UART_3_WrToQueue("\rACCEPTED\r\n",my_strlen("\rACCEPTED\r\n"));
        break;
        //return TRUE;
      }
      else
      {
        UART_3_WrToQueue("\rENTER CORRECT PASSWORD\r\n",my_strlen("\rENTER CORRECT PASSWORD\r\n"));
        pApp->Password=FALSE;
        PassIndex=0;
        break;
        //return FALSE;
      }
    }
    else
    {
      Pass[PassIndex++]=pChar[i];
      if(PassIndex > (strlen("APARTURE")+1))
      {
        PRINTF("\rENTER CORRECT PASSWORD\n");
        PassIndex=0;
        pApp->Password=FALSE;
        break;
      }
    }
  }
  return FALSE;
}

void checktimerag(void *App)
{
  apa_aparture_st* pApp;
  pApp=(apa_aparture_st*)App;
  //PRINTF("Calledchecktimerag\r\n");
  xTimerReset(pApp->CheckTimerFunc,10);
}
