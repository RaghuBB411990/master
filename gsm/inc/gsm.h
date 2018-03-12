#ifndef _GSM_H_
#define _GSM_H_

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
#include "../timer/inc/timer.h"

enum gsmtasks
{
  PROFILE0_RCVDATA
  ,PROFILE0_CONNECT
  ,PROFILE0_SENDDATA
  ,PROFILE0_DISCONNECT
  ,ENDOFTASK
};
typedef enum gsmtasks gsmtasks_t;

struct gsmtasklist
{
  gsmtasks_t taskname;
  int Timestamp;
};
typedef struct gsmtasklist gsmtasklist_t;


enum gsmstate
{
  GSM_SENDAT
  ,GSM_IDLE
  ,GSM_WAITFORATOK
  ,GSM_SENDCREG
  ,GSM_WAITFORCREG
  ,GSM_INITMODULE
  ,GSM_INITPROFILE
  ,GSM_CONNECTPROFILE
  ,GSM_SENDDATA
  ,GSM_GETTASK
};
typedef enum gsmstate gsmatate_t;

enum initmodule
{
  INITMDM_SENDCMD
  ,INITMDM_WAITFORRESP
};
typedef enum initmodule initmodule_t;

enum gsmrespstate
{
  RESP_WAITFORCR
  ,RESP_WAITFORLF
  ,RESP_WAITFORCR_2
  ,RESP_WAITFORLF_2
  ,RESP_WAITFORSECCR
  ,RESP_WAITFORSECLF
  ,RESP_DETECT
};
typedef enum gsmrespstate gsmrespstate_t;

enum modemdetresp
{
  MODEM_NORESP
  ,MODEM_OK
  ,MODEM_ERROR
  ,MODEM_CREG
  ,MODEMMAXRESP
};
typedef enum modemdetresp modemdetresp_t;

enum InitProfileState
{
  PROFILE_SENDCMD
    ,PROFILE_WAITFORRESP
    ,PROFILE_OPENCMD
    ,PROFILE_WAITFOROPENCMD
};
typedef enum InitProfileState InitProfileState_t;

enum connectprofile
{
  PROFILE_SENDSISO
  ,PROFILE_WAITFORSENDSISO
};
typedef enum connectprofile connectprofilestate_t;

enum profilestate
{
  PROFILE0_IDLE
  ,PROFILE0_INITIALIZED
  ,PROFILE0_CONNECTED
  ,PROFILE0_DISCONNECTED
};
typedef enum profilestate profilestatus_t;

enum profiledatasendstate
{
  PROFILE_SENDWRCMD
  ,PROFILE_WAITFORSENDWRCMD
  ,PRIFILE_SENDACK
  ,PROFILE_WAITFORACK
};
typedef enum profiledatasendstate profiledatasendstate_t;

enum profilename
{
  PROFILE0=0
  ,MAXNOOFPROFILE
};
typedef enum profilename profilename_t;

struct profile
{
  bool_t              ProfileIntitialized;
  profilestatus_t     ProfileStatus;
  bool_t              ProfileConected;
  InitProfileState_t  InitProfileState;
  unsigned char       ConnTryCnt;
  unsigned char       DisConnTryCnt;
  unsigned char       GsmInitCmdCnt;
  connectprofilestate_t  ConnectprofileState;
  profiledatasendstate_t ProfileDataSendState;
  profilename_t       ProfileName;
};
typedef struct profile profile_t;


struct gsm
{
  sysconfig_st     *pSysConfig;
  gsmatate_t        GsmState;
  gsmrespstate_t    GsmRespState;
  modemdetresp_t    ModemResponce;
  initmodule_t      InitModuleState;
  uint16_t          GsmCmdRespIndex;
  uint16_t          GsmCmdStrLen;
  uint8_t           GsmCmdResp[50];
  uint8_t           GsmCmdStr[50];
  bool_t            GsmCregOk;
  unsigned char     GsmCregTryCnt;
  bool_t            RespDetected;
  bool_t            RespIDDetected;
  TimerInt_t       *GsmAtRespTimer;
  unsigned int      GsmCtrlCallCnt;
  bool_t            GsmModemIntitialized;
  unsigned int      GsmMDMInitCmdCnt;
  //connectprofile_t GsmProfile0Connect;
  uint32_t          GsmTimeCnt;
  gsmtasklist_t     GsmTaskList[ENDOFTASK];
  gsmtasks_t        CurrentTask;
  unsigned char     ProfileTxData[256];
  int               Profile0TxLen;
  unsigned char     Profile0RxData[256];
  int               Profile0RxIndex;
  profile_t         Profile[MAXNOOFPROFILE];
  profile_t        *pProfile;
};
typedef struct gsm gsm_st;

void Gsm_init(gsm_st *pGsm, sysconfig_st *psysconfig);
void GsmControl(gsm_st *pGsm);
void GsmSendOutData(gsm_st *pGsm, uint8_t *pChar, uint16_t Len);
bool_t FindResponse(gsm_st *pGsm, uint8_t pChar);
void HandleGsmData(gsm_st *pGsm, uint8_t *pData, uint8_t Len);
void ParseCreg(gsm_st *pGsm, uint8_t *pData);
void InitializeModule(gsm_st *pGsm);
void InitProfile(gsm_st *pGsm);
void GsmIncrementTimeCnt(gsm_st *pGsm);
void ConnectProfile(gsm_st *pGsm);
void GetGsmCtrlState(gsm_st *pGsm);
//void SendProfileData(gsm_st *pGsm);
void SetConnTask(gsm_st *pGsm,gsmtasks_t SetTask);
#endif
