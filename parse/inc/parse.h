#ifndef _PARSE_H_
#define _PARSE_H_
#include <stdint.h>
#include <stdbool.h>
#include "../App/inc/hw_ints.h"
#include "../App/inc/hw_memmap.h"
#include "../App/inc/hw_types.h"
#include "../App/driverlib/debug.h"
#include "../App/driverlib/fpu.h"
#include "../App/driverlib/gpio.h"
#include "../App/driverlib/interrupt.h"
#include "../App/driverlib/sysctl.h"
#include "../App/driverlib/uart.h"
#include "../App/driverlib/rom.h"
#include "../utilities/inc/apatypes.h"
#include "../utilities/inc/utilities.h"
#include "../uart/inc/apa_uart.h"

#define MAXCMDSTRLEN 50
#define MAXCMDWORDINDEXLEN 3
#define CMDWORDLEN 20

enum cmdresp
{
  CMDOK
  ,CMDNOK
  ,CMDSERV
  ,NOCMD
};
typedef enum cmdresp cmdresp_t;

struct ParseCmd
{
  char      CmdStr[MAXCMDSTRLEN];
  u8_t      CmdStrIndex;
  char      CmdWord[MAXCMDWORDINDEXLEN][CMDWORDLEN];
  u8_t      CmdWordIndex;
  u8_t      CmdWordLen;
  //u8_t      CmdData[10];
  sysconfig_st *pSysConfig;
};
typedef struct ParseCmd ParseCmd_st;

typedef cmdresp_t (*func)(ParseCmd_st *cmdparser, char *str);

struct CmdParse
{
  char *pCmd;
  struct CmdParse *next;
  func  ptrfunc;
  u8_t Len;
  /*u8_t NoOfPara;
  u8_t MaxStrLen;*/
};
typedef struct CmdParse CmdParse_t;


void ParseInit(ParseCmd_st *parser,sysconfig_st *hSysconfig);
cmdresp_t ParsePortData(ParseCmd_st *parser, u8_t *pChar, u8_t Len);
cmdresp_t SplitCmd(ParseCmd_st *parser, u8_t *pChar,u8_t Len);
cmdresp_t SearchCmd(ParseCmd_st *parser, CmdParse_t *pWord);

#endif

