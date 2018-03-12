#include "src/gps_config.h"

extern char DebugStr[];

void GpsInit(gps_st *pGps,sysconfig_st *hSysConfig)
{
  pGps->pSysConfig = hSysConfig;
  pGps->GpsState = CHECKFORBGNMARK;
  pGps->GpsStrIndex = 0;
}

void HandleGPSPort(gps_st *pGps, char *pData, uint8_t Len)
{
  uint8_t Char;
  while(Len--)
  {
    Char=*pData++;
    switch(pGps->GpsState)
    {
      case CHECKFORBGNMARK:
        if(Char=='$')
        {
          pGps->GpsState=CHECKFORCR;
          pGps->GpsStrIndex=0;
        }
        break;
      case CHECKFORCR:
        if(Char=='\r')
        {
          pGps->GpsState=CHECKFORLF;
          pGps->GpsStr[pGps->GpsStrIndex]='\0';
        }
        else
        {
          pGps->GpsStr[pGps->GpsStrIndex++]=Char;
          if(pGps->GpsStrIndex > 256)
            pGps->GpsState=CHECKFORBGNMARK;
        }
        break;
      case CHECKFORLF:
        pGps->GpsState=CHECKFORBGNMARK;
        if(Char=='\n')
        {
          ParseGpsData(pGps);
        }
        break;
    }
  }
}

void ParseGpsData(gps_st *pGps)
{
  if(!strncmp(pGps->GpsStr,"GPRMC",5))
  {
    GetRMCData(pGps);
  }
}

void GetRMCData(gps_st *pGps)
{
 int CommaCnt=0;
 int Len;
 uint8_t *pData;
 uint8_t *str;
 str=pGps->GpsStr;
 Len=strlen(pGps->GpsStr);
 pData=str;
 DebugStr[0]='\0';
 while(Len)
 {
  Len--;
  if( *str == ',')
  {
    *str='\0';
    CommaCnt++;
    if(pData == str)
    {
      pData = str+1;
      str++;
      continue;
    }
    switch(CommaCnt)
    {
      case 2: /*time*/
        if(CheckTime(pGps,pData)==FALSE)
        {
          pGps->TimeValid=FALSE;
        }
        break;
      case 3:/*GpsValid*/
        if(CheckValid(pGps,pData)==FALSE)
        {
           pGps->GpsValid=FALSE;
        }
        break;
      case 4:/*Latitude*/
        if(CheckLat(pGps,pData)==FALSE)
        {
          pGps->Latitude=0.0;
        }
        break;
      case 5:/*Latdir*/
        if(CheckLatDir(pGps,pData)==FALSE)
        {
        }
        break;
       case 6:/*Longitude*/
        if(CheckLong(pGps,pData)==FALSE)
        {
          pGps->Longitude=0.0;
        }
        break;
      case 7:/*LongDir*/
        if(CheckLongDir(pGps,pData)==FALSE)
        {
        }
        break;
      case 8:/*Speed*/
        if(CheckSpeed(pGps,pData)==FALSE)
        {
          pGps->TimeValid=FALSE;
        }
        break;
      case 10: /*Date*/
        if(CheckDate(pGps,pData)==FALSE)
        {
          pGps->DateValid=FALSE;
        }
        break;
      default:
        break;
    }
    pData=str+1;
  }
  str++;
 }
//PRINTF(DebugStr);
}

bool_t CheckDate(gps_st *hGps, char *pChar)
{
  int Len;
  int i;
  Len = strlen(pChar);
  i = 0;
  while(Len)
  {
    Len--;
    if(!isdigit(pChar[i])&& pChar[i] != '.')
      return FALSE;
    i++;
  }
  hGps->DateValid=TRUE;
  pChar[6]='\0';
  /*hGps->DATE=my_atoi(pChar);
  strcat(&DebugStr[strlen(DebugStr)],"Date:");
  my_itoa(hGps->DATE,&DebugStr[strlen(DebugStr)]);
  strcat(&DebugStr[strlen(DebugStr)],"\r\n");*/
  return TRUE;
}

bool_t CheckTime(gps_st *hGps, char *pChar)
{
  int Len;
  int i;
  int tmp;
  Len = strlen(pChar);
  i = 0;
  while(Len)
  {
    Len--;
    if(!isdigit(pChar[i]) && pChar[i] != '.')
      return FALSE;
    i++;
  }
  pChar[6]='\0';
  hGps->TimeValid=TRUE;
  /*hGps->TIME=my_atoi(&pChar[0]);
  strcat(&DebugStr[strlen(DebugStr)],"Time:");
  my_itoa(hGps->TIME,&DebugStr[strlen(DebugStr)]);
  strcat(&DebugStr[strlen(DebugStr)],"\r\n");*/
  return TRUE;
}
bool_t CheckLat(gps_st *hGps, char *pChar)
{
  int Len;
  int i;
  double deg=0,min=0,dummy=0;
  Len = strlen(pChar);
  i = 0;
  while(Len)
  {
    Len--;
    if(!isdigit(pChar[i]) && pChar[i] != '.')
      return FALSE;
    i++;
  }
  my_atof((char*)&pChar[2],&dummy);
  min=dummy/60;
  pChar[2]='\0';
  my_atof((char*)&pChar[0],&deg);
  hGps->Latitude = deg+min;
  return TRUE;
}
bool_t CheckLong(gps_st *hGps, char *pChar)
{
  int Len;
  int i;
  double deg=0,min=0,dummy=0;
  Len = strlen(pChar);
  i = 0;
  while(Len)
  {
    Len--;
    if(!isdigit(pChar[i]) && pChar[i] != '.')
      return FALSE;
    i++;
  }
  my_atof((char*)&pChar[3],&dummy);
  min = dummy/60;
  pChar[3] = '\0';
  my_atof((char*)&pChar[0],&deg);
  hGps->Longitude=deg+min;
  return TRUE;
}

bool_t CheckValid(gps_st *hGps, char *pChar)
{
  if(strlen(pChar) != 1)
    return FALSE;
  else
  {
    if(pChar[0]=='A')
    {
      hGps->GpsValid=TRUE;
      //strcat(&DebugStr[strlen(DebugStr)],"GpsValid\r\n");
    }
    else
    {
      hGps->GpsValid=FALSE;
      //strcat(&DebugStr[strlen(DebugStr)],"GpsNotValid\r\n");
    }
    return TRUE;
  }
}

bool_t CheckLatDir(gps_st *hGps, char *pChar)
{
  if(strlen(pChar) != 1)
    return FALSE;
  else
  {
    if(pChar[0]=='S')
    {
      hGps->Latitude=-hGps->Latitude;
    }
    else if(pChar[0]=='N')
    {
      hGps->Latitude=hGps->Latitude;
    }
    /*strcat(&DebugStr[strlen(DebugStr)],"Lat:");
    my_ftoa(hGps->Latitude,&DebugStr[strlen(DebugStr)],6);
    strcat(&DebugStr[strlen(DebugStr)],"\r\n");*/
    return TRUE;
  }
}

bool_t CheckLongDir(gps_st *hGps, char *pChar)
{
  if(strlen(pChar) != 1)
    return FALSE;
  else
  {
    if(pChar[0]=='W')
    {
      hGps->Longitude=-hGps->Longitude;
    }
    else if(pChar[0]=='N')
    {
      hGps->Longitude=hGps->Longitude;
    }
    /*strcat(&DebugStr[strlen(DebugStr)],"Lon:");
    my_ftoa(hGps->Longitude,&DebugStr[strlen(DebugStr)],6);
    strcat(&DebugStr[strlen(DebugStr)],"\r\n");*/
    return TRUE;
  }
}
bool_t CheckSpeed(gps_st *hGps, char *pChar)
{
  int Len;
  int i;
  double dummy;
  Len = strlen(pChar);
  i = 0;
  while(Len)
  {
    Len--;
    if(!isdigit(pChar[i]) && pChar[i] != '.')
      return FALSE;
    i++;
  }
  my_atof(pChar,&dummy);
  hGps->Speed=dummy;
  /*strcat(&DebugStr[strlen(DebugStr)],"Speed:");
  my_ftoa(hGps->Speed,&DebugStr[strlen(DebugStr)],1);
  strcat(&DebugStr[strlen(DebugStr)],"\r\n");*/
  return TRUE;
}

