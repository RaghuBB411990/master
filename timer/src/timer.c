#include "../inc/timer.h"
#define MAXTIMERS 25


extern bool_t OneSecEvent;
extern bool_t tenmilisec;
static int oneseccnt=0;
static uint64_t SysTimeCnt=0;
static TimerInt_t Timers[MAXTIMERS];

void OneSecTimer()
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  ROM_IntMasterEnable();
  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PERIODIC|TIMER_CFG_B_PERIODIC);
  ROM_TimerPrescaleSet(TIMER0_BASE, TIMER_A,200);
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A,4000);
  ROM_TimerPrescaleSet(TIMER0_BASE, TIMER_B,200);
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_B,40000);
  ROM_IntEnable(INT_TIMER0A);
  ROM_IntEnable(INT_TIMER0B);
  ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);
  ROM_TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

void Timer0BIntHandler(void)
{
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
  oneseccnt++;
  if(oneseccnt >= 10)
  {
    oneseccnt = 0;
    SysTimeCnt++;
    OneSecEvent = TRUE;
  }
  //tenmilisec=TRUE;
}
static int Tenmilicnt=0;
void Timer0AIntHandler(void)
{
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    tenmilisec=TRUE;
}

void xTimersInit(void)
{
  int i=0;
  SysTimeCnt=0;
  for(i=0;i<MAXTIMERS;i++)
  {
    Timers[i].pUserData=NULL;
    Timers[i].RunnoutTime=0;
    Timers[i].FuncPtr=NULL;
    Timers[i].timerstate=NOTCREATED;
  }
}

void xTimerCheck(void)
{
  int i=0;
  for(i=0;i<MAXTIMERS;i++)
  {
    if(Timers[i].timerstate==RUNNING)
    {
      if(Timers[i].RunnoutTime <= SysTimeCnt)
      {
        Timers[i].timerstate=TIMEOUT;
        if(Timers[i].FuncPtr != NULL)
          (*Timers[i].FuncPtr)(Timers[i].pUserData);
      }
    }
  }
  //PRINTF("TimeCheck\r\n");
}


void xTimerReset(TimerInt_t *timer, uint32_t Time)
{
  timer->RunnoutTime=SysTimeCnt+Time;
  timer->timerstate=RUNNING;
}

bool_t xCreatTimer(TimerInt_t **timer, CallBackfunc pFunc, uint32_t Time, void *userdata)
{
  TimerInt_t *hTimer;
  hTimer=Timers;
  int i=0;
  for(i=0;i<MAXTIMERS;i++)
  {
    if(hTimer[i].timerstate == NOTCREATED)
    {
      *timer=&hTimer[i];
      (*timer)->pUserData=userdata;
      (*timer)->RunnoutTime=SysTimeCnt+Time;
      (*timer)->FuncPtr=pFunc;
      (*timer)->timerstate=SUSPENDED;
      return TRUE;
      break;
    }
  }
  return FALSE;
}

bool_t xCheckTimerExpired(TimerInt_t *timer)
{
  if(timer->timerstate == TIMEOUT)
    return TRUE;
  else
    return FALSE;
}

bool_t xTimerRunning(TimerInt_t *timer)
{
  if(timer->timerstate == RUNNING)
    return TRUE;
  else
    return FALSE;
}

void xTimerSuspend(TimerInt_t *timer)
{
  timer->timerstate=SUSPENDED;
}



