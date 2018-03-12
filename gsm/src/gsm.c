#include "src/gsm_config.h"
extern bool_t tenmilisec;
extern char DebugStr[];
modemdetresp_t ragresp;
static int a=0;
void Gsm_init(gsm_st *pGsm, sysconfig_st *psysconfig)
{
  int i=0;
  bool_t RetVal;
  memset(pGsm,0,sizeof(pGsm));
  pGsm->pSysConfig = psysconfig;
  pGsm->GsmCmdRespIndex=0;
  pGsm->GsmCmdStrLen=0;
  pGsm->GsmTimeCnt=0;
  pGsm->GsmCtrlCallCnt=0;
  pGsm->GsmState=GSM_SENDAT;
  pGsm->GsmRespState=RESP_WAITFORCR;
  pGsm->RespDetected=FALSE;
  RetVal=xCreatTimer(&pGsm->GsmAtRespTimer,NULL,20,NULL);
  if(RetVal==FALSE)
    PRINTF("Error creating attimer\r\n");
  for(i=0;i<ENDOFTASK;i++)
    pGsm->GsmTaskList[i].Timestamp=-1;
  for(i=0;i<MAXNOOFPROFILE;i++)
  {
    pGsm->Profile[i].ProfileIntitialized=FALSE;
  }
}

void GsmControl(gsm_st *pGsm)
{
  int i;
  pGsm->GsmCtrlCallCnt++;
  switch(pGsm->GsmState)
  {
    case GSM_SENDAT:
      if(pGsm->GsmCtrlCallCnt <= (100* 1000))
        break;
      pGsm->GsmCtrlCallCnt=0;
      pGsm->RespDetected=FALSE;
      pGsm->GsmCmdStr[0]='\0';
      strcpy(&pGsm->GsmCmdStr[0],"AT\r");
      pGsm->GsmCmdStrLen=strlen(&pGsm->GsmCmdStr[0]);
      GsmSendOutData(pGsm,&pGsm->GsmCmdStr[0],pGsm->GsmCmdStrLen);
      pGsm->GsmState=GSM_WAITFORATOK;
      xTimerReset(pGsm->GsmAtRespTimer,10);
      break;
    case GSM_WAITFORATOK:
      if(xCheckTimerExpired(pGsm->GsmAtRespTimer)==TRUE)
      {
        pGsm->GsmState=GSM_SENDAT;
        if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
          PRINTF("at timer out\r\n");
        xTimerSuspend(pGsm->GsmAtRespTimer);
      }
      if((pGsm->RespDetected==TRUE) && (pGsm->RespIDDetected==TRUE))
      {
        if(pGsm->ModemResponce==MODEM_OK)
        {
          if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
            PRINTF("FOUND RESP OK\r\n");
          xTimerSuspend(pGsm->GsmAtRespTimer);
          pGsm->GsmState=GSM_SENDCREG;
          pGsm->GsmCregOk=FALSE;
        }
        if(pGsm->ModemResponce==MODEM_ERROR)
        {
          if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
            PRINTF("FOUND RESP ERROR\r\n");
          xTimerSuspend(pGsm->GsmAtRespTimer);
          pGsm->GsmState=GSM_SENDAT;
        }
      }
      break;
    case GSM_SENDCREG:
      if(pGsm->GsmCtrlCallCnt <= (100* 1000))
        break;
      pGsm->GsmCtrlCallCnt=0;
      pGsm->GsmCmdStr[0]='\0';
      pGsm->RespDetected=FALSE;
      strcpy(&pGsm->GsmCmdStr[0],"AT+CREG?\r");
      pGsm->GsmCmdStrLen=strlen(&pGsm->GsmCmdStr[0]);
      GsmSendOutData(pGsm,&pGsm->GsmCmdStr[0],pGsm->GsmCmdStrLen);
      pGsm->GsmState=GSM_WAITFORCREG;
      xTimerReset(pGsm->GsmAtRespTimer,20);
      break;
    case GSM_WAITFORCREG:
      if((pGsm->RespDetected==TRUE) && (pGsm->RespIDDetected==TRUE))
      {
        if(pGsm->ModemResponce==MODEM_CREG)
        {
          break;
        }
        if(pGsm->ModemResponce==MODEM_OK)
        {
          if(pGsm->GsmCregOk==TRUE)
          {
            if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
              PRINTF("FOUND creg OK\r\n");
            xTimerSuspend(pGsm->GsmAtRespTimer);
            pGsm->GsmState=GSM_INITMODULE;
            pGsm->GsmModemIntitialized=FALSE;
          }
          else
          {
            xTimerSuspend(pGsm->GsmAtRespTimer);
            if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
              PRINTF("Go Back to Creg\r\n");
            pGsm->GsmState=GSM_SENDCREG;
          }
        }
        if(pGsm->ModemResponce==MODEM_ERROR)
        {
          if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
            PRINTF("FOUND creg ERROR\r\n");
          xTimerSuspend(pGsm->GsmAtRespTimer);
          pGsm->GsmState=GSM_SENDCREG;
        }
      }
      if(xCheckTimerExpired(pGsm->GsmAtRespTimer)==TRUE)
      {
        pGsm->GsmState=GSM_SENDCREG;
        if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
          PRINTF("at timer out\r\n");
        xTimerSuspend(pGsm->GsmAtRespTimer);
      }
      break;
    case GSM_INITMODULE:
      InitializeModule(pGsm);
      break;
    case GSM_GETTASK:
      pGsm->CurrentTask=ENDOFTASK;
      for(i=0;i<ENDOFTASK;i++)
      {
        if(pGsm->GsmTaskList[i].Timestamp!=-1)
        {
          pGsm->CurrentTask=i;
          break;
        }
      }
      if(pGsm->CurrentTask!=ENDOFTASK)
      {
        /*write program to change state*/
        //PRINTF("Found task\r\n");
        GetGsmCtrlState(pGsm);
      }
      break;
    case GSM_INITPROFILE:
      InitProfile(pGsm);
      break;
    case GSM_CONNECTPROFILE:
      ConnectProfile(pGsm);
      break;
    case GSM_SENDDATA:
      //SendProfileData(pGsm);
    case GSM_IDLE:
      break;
    default:
      break;
  }
  if(pGsm->RespDetected==TRUE)
  {
    pGsm->RespDetected=FALSE;
    pGsm->RespIDDetected=FALSE;
    pGsm->ModemResponce=MODEMMAXRESP;
  }
}


void GsmSendOutData(gsm_st *pGsm, uint8_t *pChar, uint16_t Len)
{
  if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
    UART_3_WrToQueue(pChar,Len);
  UART_1_WrToQueue(pChar,Len);
}

void HandleGsmData(gsm_st *pGsm, uint8_t *pData, uint8_t Len)
{
  int i;
  for(i=0;i<Len;i++)
  {
    if(FindResponse(pGsm,pData[i])==TRUE)
    {
      pGsm->RespDetected=TRUE;
      pGsm->GsmRespState=RESP_WAITFORCR;
      tenmilisec=TRUE;
    }
  }
}


bool_t FindResponse(gsm_st *pGsm, uint8_t pChar)
{
  int i;
  bool_t responsedet = FALSE;
  responseList_st *presponselist = ResponseList;
  switch(pGsm->GsmRespState)
  {
    case RESP_WAITFORCR:
      if(pChar==CARRIAGERETURN)
      {
        pGsm->GsmRespState=RESP_WAITFORLF;
      }
      break;
    case RESP_WAITFORLF:
      if(pChar==NEWLINE)
      {
        pGsm->GsmCmdRespIndex=0;
        pGsm->GsmRespState=RESP_WAITFORCR_2;
      }
      break;
    case RESP_WAITFORCR_2:
      if (pChar != CARRIAGERETURN)
      {
        pGsm->GsmCmdRespIndex=0;
        pGsm->GsmCmdResp[pGsm->GsmCmdRespIndex++]=pChar;
        pGsm->GsmRespState=RESP_DETECT;
      }
      else if(pChar==CARRIAGERETURN)
      {
        pGsm->GsmRespState=RESP_WAITFORLF_2;
      }
      break;
    case RESP_WAITFORLF_2:
      if(pChar==NEWLINE)
      {
        pGsm->GsmCmdRespIndex=0;
        pGsm->GsmRespState=RESP_DETECT;
      }
      else
      {
        pGsm->GsmRespState=RESP_WAITFORCR;
      }
      break;
    case RESP_DETECT:
      if(pChar==CARRIAGERETURN)
      {
        pGsm->GsmCmdResp[pGsm->GsmCmdRespIndex]='\0';
        pGsm->GsmRespState=RESP_WAITFORSECLF;
      }
      else
      {
        pGsm->GsmCmdResp[pGsm->GsmCmdRespIndex++]=pChar;
      }
      break;
    case RESP_WAITFORSECLF:
       if(pChar==NEWLINE)
       {
         for(i=0;i<MODEMMAXRESP;i++)
         {
           if(!(strncmp(presponselist[i].pResp,&pGsm->GsmCmdResp[0],presponselist[i].ResponseLen)))
           {
              responsedet = TRUE;
              pGsm->RespIDDetected=TRUE;
              pGsm->ModemResponce = presponselist[i].ResponseType;
              switch(pGsm->ModemResponce)
              {
                case MODEM_OK:
                  //PRINTF("foundok\r");
                  break;
                case MODEM_ERROR:
                  break;
                case MODEM_CREG:
                  ParseCreg(pGsm,&pGsm->GsmCmdResp[presponselist[i].ResponseLen]);
                  break;
                default:
                  break;
              }
           }
           if(responsedet == TRUE)
             break;
         }
       }
       break;
    default:
       break;
  }
  return responsedet;
}



void ParseCreg(gsm_st *pGsm, u8_t *pData)
{
  if(pData[2]=='1' || pData[2]=='5')
    pGsm->GsmCregOk=TRUE;
  else
    pGsm->GsmCregOk=FALSE;
}

void InitializeModule(gsm_st *pGsm)
{
  switch(pGsm->InitModuleState)
  {
    case INITMDM_SENDCMD:
      if(pGsm->GsmCtrlCallCnt <= (100*1000))
      {
        return;
      }
      pGsm->GsmCtrlCallCnt=0;
      strcpy(&pGsm->GsmCmdStr[0],&GsmInitCmds[pGsm->GsmMDMInitCmdCnt++][0]);
      pGsm->GsmCmdStrLen=strlen(&pGsm->GsmCmdStr[0]);
      GsmSendOutData(pGsm,&pGsm->GsmCmdStr[0],pGsm->GsmCmdStrLen);
      xTimerReset(pGsm->GsmAtRespTimer,10);
      pGsm->InitModuleState=INITMDM_WAITFORRESP;
      break;
    case INITMDM_WAITFORRESP:
      if((pGsm->RespDetected==TRUE) && (pGsm->RespIDDetected==TRUE))
      {
         if(pGsm->ModemResponce==MODEM_OK)
         {
           xTimerSuspend(pGsm->GsmAtRespTimer);
           if(GsmInitCmds[pGsm->GsmMDMInitCmdCnt]==NULL)
           {
             pGsm->InitModuleState=INITMDM_SENDCMD;
             pGsm->GsmState=GSM_GETTASK;
             pGsm->GsmModemIntitialized=TRUE;
             //pGsm->pProfile->ProfileIntitialized=FALSE;
             pGsm->GsmMDMInitCmdCnt=0;
             if(pGsm->pSysConfig->GsmDebugEnable=TRUE)
               PRINTF("Modem Initialized\r\n");
           }
           else
           {
             pGsm->InitModuleState=INITMDM_SENDCMD;
           }
         }
         else if(pGsm->ModemResponce==MODEM_ERROR)
         {
            xTimerSuspend(pGsm->GsmAtRespTimer);
            pGsm->GsmState=GSM_INITMODULE;
            pGsm->InitModuleState=INITMDM_SENDCMD;
            pGsm->GsmMDMInitCmdCnt=0;
         }
      }
      if(xCheckTimerExpired(pGsm->GsmAtRespTimer)==TRUE)
      {
        pGsm->GsmState=GSM_INITPROFILE;
        pGsm->InitModuleState=INITMDM_SENDCMD;
        if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
          PRINTF("modem init tieout\r\n");
        xTimerSuspend(pGsm->GsmAtRespTimer);
        pGsm->GsmMDMInitCmdCnt=0;
      }
      break;
    default:
      break;
  }
}

void InitProfile(gsm_st *pGsm)
{
  switch(pGsm->Profile[PROFILE0].InitProfileState)
  {
    case PROFILE_SENDCMD:
     /*if(pGsm->GsmCtrlCallCnt <= (100* 10))
      {
        return;
      }*/
      pGsm->GsmCtrlCallCnt=0;
      //pGsm->GsmCmdStr[0]='\0';
      //pGsm->GsmCmdStrLen=0;
      my_strcpy(&pGsm->GsmCmdStr[0],&GsmInitProfile0[pGsm->Profile[PROFILE0].GsmInitCmdCnt++][0]);
      pGsm->GsmCmdStrLen = strlen(&pGsm->GsmCmdStr[0]);
      GsmSendOutData(pGsm,&pGsm->GsmCmdStr[0],pGsm->GsmCmdStrLen);
      xTimerReset(pGsm->GsmAtRespTimer,20);
      //pGsm->Profile[PROFILE0].GsmInitCmdCnt++;
      pGsm->Profile[PROFILE0].InitProfileState=PROFILE_WAITFORRESP;
      break;
    case PROFILE_WAITFORRESP:
      if((pGsm->RespDetected==TRUE) && (pGsm->RespIDDetected==TRUE))
      {
         if(pGsm->ModemResponce==MODEM_OK)
         {
           xTimerSuspend(pGsm->GsmAtRespTimer);
           if(GsmInitProfile0[pGsm->Profile[PROFILE0].GsmInitCmdCnt]==NULL)
           {
             //pGsm->InitProf0State=PROFILE0_OPENCMD;
             pGsm->GsmState=GSM_CONNECTPROFILE;
             pGsm->Profile[PROFILE0].ProfileStatus=PROFILE0_INITIALIZED;
             pGsm->Profile[PROFILE0].ConnectprofileState=PROFILE_SENDSISO;
             pGsm->Profile[PROFILE0].ProfileIntitialized=TRUE;
             pGsm->Profile[PROFILE0].GsmInitCmdCnt=0;
             if(pGsm->pSysConfig->GsmDebugEnable=TRUE)
               PRINTF("Profile0 Initialized\r\n");
           }
           else
           {
             pGsm->Profile[PROFILE0].InitProfileState=PROFILE_SENDCMD;
           }
         }
         else if(pGsm->ModemResponce==MODEM_ERROR)
         {
           xTimerSuspend(pGsm->GsmAtRespTimer);
            pGsm->GsmState=GSM_INITPROFILE;
            pGsm->Profile[PROFILE0].InitProfileState=PROFILE_SENDCMD;
            pGsm->Profile[PROFILE0].GsmInitCmdCnt=0;
         }
      }
      if(xCheckTimerExpired(pGsm->GsmAtRespTimer)==TRUE)
      {
        pGsm->GsmState=GSM_INITPROFILE;
        pGsm->Profile[PROFILE0].InitProfileState=PROFILE_SENDCMD;
        if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
          PRINTF("modem init tieout\r\n");
        xTimerSuspend(pGsm->GsmAtRespTimer);
        pGsm->Profile[PROFILE0].GsmInitCmdCnt=0;
      }
      break;
    default:
      break;
  }
}


void ConnectProfile(gsm_st *pGsm)
{
  switch(pGsm->pProfile->ConnectprofileState)
  {
    case PROFILE_SENDSISO:
      if(pGsm->GsmCtrlCallCnt <= (100* 1000))
      {
        return;
      }
      pGsm->GsmCtrlCallCnt=0;
      strcpy(&pGsm->GsmCmdStr[0],"AT^SISO=0\r");
      pGsm->GsmCmdStrLen=strlen(&pGsm->GsmCmdStr[0]);
      GsmSendOutData(pGsm,&pGsm->GsmCmdStr[0],pGsm->GsmCmdStrLen);
      xTimerReset(pGsm->GsmAtRespTimer,60);
      pGsm->pProfile->ConnectprofileState=PROFILE_WAITFOROPENCMD;
      break;
    case PROFILE_WAITFORSENDSISO:
      if(xCheckTimerExpired(pGsm->GsmAtRespTimer)==TRUE)
      {
        pGsm->GsmState=GSM_IDLE;
        if(pGsm->pSysConfig->GsmDebugEnable==TRUE)
          PRINTF("modem opentimeout\r\n");
        xTimerSuspend(pGsm->GsmAtRespTimer);
      }
    default:
      break;
  }

}

void GsmIncrementTimeCnt(gsm_st *pGsm)
{
  pGsm->GsmTimeCnt++;
}

void GetGsmCtrlState(gsm_st *pGsm)
{
  switch(pGsm->CurrentTask)
  {
    case PROFILE0_CONNECT:
      pGsm->pProfile=&pGsm->Profile[PROFILE0];
      switch(pGsm->pProfile->ProfileStatus)
      {
        case PROFILE0_IDLE:
          pGsm->GsmState=GSM_INITPROFILE;
          pGsm->Profile[PROFILE0].InitProfileState=PROFILE_SENDCMD;
          pGsm->GsmCtrlCallCnt=0;
          pGsm->Profile[PROFILE0].GsmInitCmdCnt=0;
          break;
        case PROFILE0_INITIALIZED:
          PRINTF("Prof0initialized\r\n");
          pGsm->GsmState=GSM_CONNECTPROFILE;
          pGsm->pProfile->ConnectprofileState=PROFILE_SENDSISO;
          pGsm->GsmCtrlCallCnt=0;
          break;
        case PROFILE0_CONNECTED:
          PRINTF("Prof0connected\r\n");
          pGsm->GsmState=GSM_SENDDATA;
          pGsm->pProfile->ProfileDataSendState=PROFILE_SENDWRCMD;
          pGsm->GsmCtrlCallCnt=0;
          break;
        case PROFILE0_DISCONNECTED:
          PRINTF("Prof0disconnected\r\n");
          pGsm->GsmState=GSM_CONNECTPROFILE;
          pGsm->pProfile->InitProfileState=PROFILE_SENDCMD;
          pGsm->GsmCtrlCallCnt=0;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void SetConnTask(gsm_st *pGsm,gsmtasks_t SetTask)
{
  switch(SetTask)
  {
    case PROFILE0_CONNECT:
      if(pGsm->GsmTaskList[PROFILE0_CONNECT].Timestamp == -1)
      {
        pGsm->GsmTaskList[PROFILE0_CONNECT].Timestamp = pGsm->GsmTimeCnt;
      }
      break;
    default:
      break;
  }

}
