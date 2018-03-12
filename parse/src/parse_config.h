#ifndef _PARSE_CONFIG_H
#define _PARSE_CONFIG_H

#include "../inc/parse.h"

cmdresp_t DebugEnable(ParseCmd_st *cmdparser, char *str);
cmdresp_t GsmEnable(ParseCmd_st *cmdparser, char *str);
cmdresp_t GpsEnable(ParseCmd_st *cmdparser, char *str);
cmdresp_t Gsmsendcmd(ParseCmd_st *cmdparser, char *str);
cmdresp_t GetConfig(ParseCmd_st *cmdparser, char *str);
cmdresp_t GsmCmdSend(ParseCmd_st *cmdparser, char *str);

CmdParse_t Sys_t[]={
                      {"DEBUG",NULL,DebugEnable,sizeof("ENABLE")-1}
                      ,{NULL,NULL,0}
                      };
CmdParse_t Gsm_t[]={
                      {"DEBUG",NULL,GsmEnable,sizeof("ENABLE")-1}
                      ,{"SENDCMD",NULL,Gsmsendcmd,sizeof("SENDCMD")-1}
                      ,{"CMD",NULL,GsmCmdSend,sizeof("CMD")-1}
                      ,{NULL,NULL,0}
                      };
CmdParse_t Gps_t[]={
                      {"DEBUG",NULL,GpsEnable,sizeof("ENABLE")-1}
                      ,{NULL,NULL,0}
                      };
CmdParse_t Get_t[]={
                      {"CONFIG",NULL,GetConfig,sizeof("ENABLE")-1}
                      ,{NULL,NULL,0}
                      };
CmdParse_t CmdStrList[]={
                         {"SYSTEM",Sys_t,NULL,sizeof("DEBUG")-1}
                         ,{"GSM",Gsm_t,NULL,sizeof("GSM")-1}
                         ,{"GPS",Gps_t,NULL,sizeof("GPS")-1}
                         ,{"GET",Get_t,NULL,sizeof("GPS")-1}
                         ,{NULL,NULL,0}
                        };

#endif

