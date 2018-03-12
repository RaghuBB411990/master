#ifndef _APA_H_
#define _APA_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "../utilities/inc/apatypes.h"
#include "../utilities/inc/utilities.h"
#include "../uart/inc/apa_uart.h"
#include "../parse/inc/parse.h"
#include "../gps/inc/gps.h"
#include "../timer/inc/timer.h"
#include "../gsm/inc/gsm.h"
enum apastate
{
  COM1RXEVENT
  ,COM1TXEVENT
  ,COM3RXEVENT
  ,COM3TXEVENT
  ,COM7RXEVENT
  ,COM7TXEVENT
  ,ONESECEVENT
  ,TENMILIEVENT
};
typedef enum apastate apastate_t;

struct APARTURE_APP
{
  uint32_t      Ganesh;
  bool_t        DebugEnable;
  apastate_t    MainAppState;
  uint8_t       Uart1RxBuf[32];
  char          Uart3RxBuf[32];
  char          Uart7RxBuf[32];
  uint32_t      uart1rxLen;
  uint32_t      uart3rxLen;
  uint32_t      uart7rxLen;
  ParseCmd_st   parse;
  gps_st        Gps;
  gsm_st        Gsm;
  sysconfig_st  SysConfig;
  bool_t        Password;
  uint32_t      TimeStamp;
  TimerInt_t   *CheckTimerFunc;
};
typedef struct APARTURE_APP apa_aparture_st;
void PrevResetCause();
void HandleDebugPortData(apa_aparture_st *App, u8_t *pChar, u8_t Len);
void OneSecTimer();
bool_t CheckPassword(apa_aparture_st *pApp, u8_t *pChar, int Len);
void checktimerag(void *App);
#endif
