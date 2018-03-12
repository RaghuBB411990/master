#include "src/parse_config.h"
bool_t Found=FALSE;
extern char DebugStr[];
extern u8_t SoftwareVer[];
extern u8_t Customer[];



void ParseInit(ParseCmd_st *parser, sysconfig_st *hSysconfig)
{
  //memset(parser,0,sizeof(parser));
  parser->pSysConfig = hSysconfig;
  parser->CmdWordIndex=0;
  parser->CmdWordLen=0;
  parser->CmdStrIndex=0;
}

cmdresp_t ParsePortData(ParseCmd_st *parser, u8_t *pChar, u8_t Len)
{
  u8_t i;
  cmdresp_t returntype = CMDSERV;
  for(i=0;i<Len;i++)
  {
    if(pChar[i]==BACKSPACE)
    {
      if(parser->CmdStrIndex)
        parser->CmdStrIndex--;
     break;
    }
    else
    {
      parser->CmdStr[parser->CmdStrIndex++]=pChar[i];
    }
    if(pChar[i] == '\n')
    {
      parser->CmdStr[parser->CmdStrIndex]='\0';
      returntype=SplitCmd(parser,parser->CmdStr,parser->CmdStrIndex);
      parser->CmdStrIndex=0;
      return returntype;
    }
  }
  if(parser->CmdStrIndex>60)
  {
    parser->CmdStrIndex=0;
  }
  return returntype;
}


cmdresp_t SplitCmd(ParseCmd_st *parser, u8_t *pChar,u8_t Len)
{
  u8_t tmplen;
  u8_t i=0;
  parser->CmdWordIndex=0;
  parser->CmdWordLen=0;
  while(pChar[i] == SPACE || pChar[i]== TAB)
    i++;
  if(pChar[i] == NEWLINE || pChar[i]=='\0')
    return NOCMD;
  do
  {
    while(pChar[i] != SPACE && pChar[i] != TAB && pChar[i]!='\0' && pChar[i]!='\r')// && pChar[i]!='\n')
    {
      parser->CmdWord[parser->CmdWordIndex][parser->CmdWordLen++]=pChar[i];
      i++;
    }
    parser->CmdWord[parser->CmdWordIndex][parser->CmdWordLen]='\0';
    parser->CmdWordIndex++;
    parser->CmdWordLen=0;
    while(pChar[i] == SPACE || pChar[i] == TAB || pChar[i] == '\r' || pChar[i] == '\n' )
      i++;
  }while(pChar[i] != '\0');
  parser->CmdWordIndex=0;
  return SearchCmd(parser,&CmdStrList[0]);
}

cmdresp_t SearchCmd(ParseCmd_st *parser, CmdParse_t *pWord)
{
  u8_t i=0;
  while(pWord[i].pCmd != NULL)
  {
    if(!strcmp(pWord[i].pCmd,&parser->CmdWord[parser->CmdWordIndex][0]))//, pWord[i].Len))
    {
      if(pWord[i].next!=NULL)
      {
        parser->CmdWordIndex++;
        return SearchCmd(parser,pWord[i].next);
      }
      else
      {
        return (*pWord[i].ptrfunc)(parser,&parser->CmdWord[++parser->CmdWordIndex][0]);
      }
    }
    i++;
  }
  return CMDNOK;
}

cmdresp_t DebugEnable(ParseCmd_st *cmdparser, char *str)
{
  if(!strncmp(str,"ENABLE",5))
    cmdparser->pSysConfig->SysDebugEnable=TRUE;
  else if(!strncmp(str,"DISABLE",6))
    cmdparser->pSysConfig->SysDebugEnable=FALSE;
  return CMDOK;
}
cmdresp_t GsmEnable(ParseCmd_st *cmdparser, char *str)
{
  if(!strncmp(str,"ENABLE",5))
  {
    cmdparser->pSysConfig->GsmDebugEnable=TRUE;
    return CMDOK;
  }
  else if(!strncmp(str,"DISABLE",6))
  {
    cmdparser->pSysConfig->GsmDebugEnable=FALSE;
    return CMDOK;
  }
  else
    return CMDNOK;
}

cmdresp_t GpsEnable(ParseCmd_st *cmdparser, char *str)
{
  if(!strncmp(str,"ENABLE",5))
    cmdparser->pSysConfig->GpsDebugEnable=TRUE;
  else if(!strncmp(str,"DISABLE",6))
    cmdparser->pSysConfig->GpsDebugEnable=FALSE;
  return CMDOK;
}

cmdresp_t Gsmsendcmd(ParseCmd_st *cmdparser, char *str)
{
  cmdparser->pSysConfig->SendGsmCmd=TRUE;
  return CMDOK;
}

cmdresp_t GsmCmdSend(ParseCmd_st *cmdparser, char *str)
{
  UART_1_WrToQueue("SET TEST\r\n",strlen("SET TEST\r\n"));
  return CMDOK;
}


cmdresp_t GetConfig(ParseCmd_st *cmdparser, char *str)
{
  PRINTF("SoftwareVer: ");
  PRINTF(SoftwareVer);
  PRINTF("\r\nCustomer: ");
  PRINTF(Customer);
  PRINTF("\r\n");
  return CMDOK;
}
