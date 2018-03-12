#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../App/inc/hw_ints.h"
#include "../App/inc/hw_memmap.h"
//#include "inc/hw_types.h"
#include "../App/driverlib/debug.h"
#include "../App/driverlib/fpu.h"
#include "../App/driverlib/gpio.h"
#include "../App/driverlib/interrupt.h"
#include "../App/driverlib/sysctl.h"
#include "../App/driverlib/uart.h"
#include "../App/driverlib/rom.h"
#include "../App/driverlib/timer.h"
#include "../utilities/inc/apatypes.h"
#include "../utilities/inc/utilities.h"
#include "../uart/inc/apa_uart.h"
enum TimerState
{
  NOTCREATED
  ,RUNNING
  ,SUSPENDED
  ,TIMEOUT
};
typedef enum TimerState timerstate_t;

typedef void (*CallBackfunc)(void *);

struct TimerInt
{
  void *pUserData;
  uint32_t RunnoutTime;
  CallBackfunc  FuncPtr;
  timerstate_t timerstate;
};
typedef struct TimerInt TimerInt_t;

void OneSecTimer(void);
//void Timer0AIntHandler(void);
bool_t xTimerRunning(TimerInt_t *timer);
bool_t xCheckTimerExpired(TimerInt_t *timer);
bool_t xCreatTimer(TimerInt_t **timer, CallBackfunc FuncPtr, u32_t Time, void *userdata);
void xTimerReset(TimerInt_t *timer, u32_t Time);
void xTimerCheck(void);
void xTimersInit(void);
void xTimerSuspend(TimerInt_t *timer);
//void TenMiliTimer(void);
//void Timer0BIntHandler(void);
#endif
