#ifndef _GSM_CONFIG_H_
#define _GSM_CONFIG_H_

#include "inc/gsm.h"

const char ModemRespOk[]="OK";
const char ModemRespError[]="ERROR";
const char ModemRespCreg[]="+CREG: ";

struct responseList
{
  const char *pResp;
  u8_t        ResponseLen;
  modemdetresp_t ResponseType;
};
typedef struct responseList responseList_st;

static responseList_st  ResponseList[]={
                                       { ModemRespOk, sizeof(ModemRespOk)-1, MODEM_OK}
                                        ,{ ModemRespError, sizeof(ModemRespError)-1, MODEM_ERROR}
                                        ,{ModemRespCreg,sizeof(ModemRespCreg)-1,MODEM_CREG}
                                       };


char *GsmInitCmds[]={
  "ATE1+CMGF=1+CNMI=2,1\r"
   ,"AT+CFUN=7\r"
   ,"AT+CMEE=2\r"
   ,"AT^SICS=0,contype,GPRS0\r"
   ,"AT^SICS=0,apn,airtelgprs.com\r"
   ,NULL
};

char *GsmInitProfile0[]={
  "AT^SISS=0,srvType,socket\r"
   ,"AT^SISS=0,conId,0\r"
   ,"AT^SISS=0,address,socktcp://157.49.252.141:10232\r"
   ,NULL
};





#endif
