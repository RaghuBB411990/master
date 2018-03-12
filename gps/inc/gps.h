#ifndef _GPS_H_
#define _GPS_H_

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

enum gpsstate
{
  CHECKFORBGNMARK
  ,CHECKFORCR
  ,CHECKFORLF
};
typedef enum gpsstate gpsstate_st;

struct gps
{
  char  GpsStr[256];
  uint32_t  TIME;
  uint32_t  DATE;
  float Latitude;
  float Longitude;
  float Speed;
  float Altitude;
  gpsstate_st GpsState;
  bool_t TimeValid;
  bool_t DateValid;
  uint16_t GpsStrIndex;
  bool_t GpsValid;
  sysconfig_st *pSysConfig;
};
typedef struct gps gps_st;

void GpsInit(gps_st *pGps,sysconfig_st *hSysConfig);
void HandleGPSPort(gps_st *pGps, char *pData, u8_t Len);
bool_t CheckDate(gps_st *hGps, char *pChar);
bool_t CheckTime(gps_st *hGps, char *pChar);
bool_t CheckValid(gps_st *hGps, char *pChar);
bool_t CheckLat(gps_st *hGps, char *pChar);
bool_t CheckLong(gps_st *hGps, char *pChar);
bool_t CheckLatDir(gps_st *hGps, char *pChar);
bool_t CheckLongDir(gps_st *hGps, char *pChar);
bool_t CheckSpeed(gps_st *hGps, char *pChar);
void ParseGpsData(gps_st *pGps);
void GetRMCData(gps_st *pGps);
#endif
