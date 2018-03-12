
#ifndef _DEVICEPARAMS_H_
#define _DEVICEPARAMS_H_
#include "../common/inc/tmtypes.h"

#define ACKDATALEN 256
#define CANDATALENGTH 256
#define DTCDATALENGTH 128

#define LAT_SIZE   14
#define LONG_SIZE  15
#define TIME_SIZE  8
#define SPEED_SIZE 8
#define HEAD_SIZE  8
#define DIST_SIZE  10
#define NOOFSAT_SIZE  3
#define SIGSTRNGTH_SIZE 4
#define ANALOG_SIZE 8
#define FUELDATASIZE 10
#define AGPS_SIZE         32

#define APN_SIZE          30
#define SERIPADDR_SIZE    20
#define SERIPPORT_SIZE    30
#define SERVERNAME_SIZE   40
#define PORTNO_SIZE       8
#define DNS_SIZE          18
#define ID_SIZE           30
#define GEOFENCES_MAX     28
#define SERIALTOGPRS_BUFFER_SIZE  64
#define SERIALBAUD_SIZE   8
#define NUBERSSUPPORTED   4
#define PHNBR_SIZE        15

#define MAXDIGINPUTS         7
#define IGNITION_REF_2SEC    200     
#define MAINPWR_REF_4SEC     400
#define DIP1_REF_50MSEC      5
#define DIP2_REF_50MSEC      5
#define TAMPDET_REF_2SEC     200
#define CHARGSTATUS_REF_2SEC 200
#define RI_REF_100MSEC       10
#define WAKEUP_THR_DEFAULT   0x09
#define WAKEUP_THR_MAX      32
//#define ACC_50HZ  1
#ifdef  ACC_50HZ
/* FOR 50MSEC ACC.DATA */
#define ACCIDENT_THRESHOLD		  99.0 /* 990mg */
#define BREAK_THRESHOLD			    46.0 /* 460mg */
#define ACCELERATION_THRESHOLD  49.0 /* 490mg */
#else
    /* fOR 10MSEC ACC. DATA */
#define ACCIDENT_THRESHOLD		  70.0  /* 700mg */
#define BREAK_THRESHOLD			    33.0  /* 350mg */
#define ACCELERATION_THRESHOLD  29.0  /* 330mg */
#endif
#define WAKEUP_GAURDTIME     5

#define INTERNALANTENNA  1

#define PASSWORD_SIZE  40

enum canmode
{
   BIT11_250KBPS =0                /* CAN 11 BIT, 250kBPS */
   ,BIT11_500KBPS                 /* CAN 11 BIT, 500kBPS */
   ,BIT29_250KBPS                 /* CAN 29 BIT, 250kBPS */
   ,BIT29_500KBPS                 /* CAN 29 BIT, 500kBPS */
   ,PROTOCOL_AUTO                 /* Auto scan mode */
};
typedef enum canmode tm_canmode_t;

enum klinemode
{
   PROTOCOL_ISO9141=5           /* ISO9141-2 */
   ,PROTOCOL_KWPSLOW           /* ISO14230-4 KWP SLOW */
   ,PROTOCOL_KWPFAST           /* ISO14230-4 KWP FAST */
   ,KLINE_AUTO                 /* Auto scan mode */
};
typedef enum klinemode tm_klinemode_t;

enum serialfunctype
{
   SERIALFUNC_CONFIG=0               /* Serial port as configuartion port */
  ,SERIALFUNC_SMO                    /* Send serial data to server */
  ,SERIALFUNC_PROTECHFUEL            /* Serial port connected to protech fuel sensor */
  ,SERIALFUNC_TECHNOTON              /* Serial port connected to Technoton fuel sensor, Infinium is using this sensor */
  ,SERIALFUNC_JOINTECH               /* Jointech Fuel Sensor */
  ,SERIALFUNC_OBD                    /* Serial port connected to STN based OBD Hardware */
};
typedef enum serialfunctype tm_serfunc_t;

enum gpsfixstatus
{
   NOFIX
  ,FOUNDFIX
  ,LOSTFIX
};
typedef enum gpsfixstatus tm_gpsfixstatus_t;

enum powerdownmode
{
   NO_PWRDWN=0
  ,NORMAL_PWRDWN     /* Processor:DeepSleep, GPS power off, GSM Cyclic Sleep*/
                     /* WakeUp events: Ignition ON, Digital I\Ps, Main power Rem\Ins, Acelerometer event, WakeupTimeOut */
  ,ONLYBAT_PWRDWN    /* Processor:DeepSleep, GPS OFF,  GSM Cyclic Sleep */
                     /* WakeUp events: Main Power Insertion, WakeupTimeOut */
};
typedef enum powerdownmode tm_powerdownmode_t;

struct latlong
{
  char  Latitude[LAT_SIZE];
  tm_bool_t LatSign;
  char  Longitude[LONG_SIZE];
  tm_bool_t LongSign;
  tm_bool_t bNew;
};
typedef struct latlong tm_latlong_t;

struct gpsinfo
{
  char  Latitude[LAT_SIZE];
  char  Longitude[LONG_SIZE];
  char  Speed[SPEED_SIZE];
  char Date[8];
  char Time[8];
  char Heading[8];
  char PDOP[8];
  char HDOP[8];
  char NoOfSat[4];
  tm_gpsfixstatus_t FixStatus;
  tm_bool_t bCheckFixStatus;
  tm_bool_t bDopplerValid;
  tm_bool_t bAorV;
  float horizontal_dop;
  float lat_sign;
  float long_sign;
};
typedef struct gpsinfo tm_gpsinfo_t;

struct GeoFence
{
  tm_bool_t  bEnable;
  tm_u8_t  FenceStatus;
  float Lat;
  float Long;
  float Radius;
};
typedef struct GeoFence tm_GeoFence_t;



enum packettype
{
   PERIODIC_PACKET=0
  ,FENCE_PACKET
  ,SMO_PACKET
  ,ALERT_ACCIDENT_PACKET
  ,ALERT_BREAK_PACKET
  ,ALERT_ACCELERATION_PACKET
  ,PANIC_PACKET
  ,IGNON_PACKET
  ,IGNOFF_PACKET
  ,MDMRESTART_PACKET
  ,MAINPWRREM_PACKET
  ,OTASUCCESS_PACKET
  ,OTAFAIL_PACKET
  ,NEWCONNECTION_PACKET
  ,GPRSCMDRESP_PACKET
  ,DTRACK_PACKET
  ,ATRACK_PACKET
  ,MAINPWRINS_PACKET
  ,LOWINTBAT_PACKET
  ,ACTIVERFID_PACKET
  ,ALERT_OVERSPEEDMAX
  ,ALERT_OVERSPEEDMIN
  ,PWRDWN_PACKET
  ,TOWING_PACKET
  ,DTC_PACKET
  ,VIN_PACKET
  ,REG_PACKET
  ,CRNR_PACKET
};
typedef enum packettype tm_packettype_t;

enum ioid
{
   IN_IGNITION=0
  ,IN_MAINPWR
  ,IN_DIP1
  ,IN_DIP2
  ,IN_TAMPDET
  ,IN_CHARGSTATUS
  ,IN_RI
  ,OUT_GPSPWR
  ,OUT_GPSRFPWR
  ,OUT_GSMPWRKEY
  ,OUT_GSMRESET
  ,OUT_DOP1
  ,OUT_DOP2
  ,OUT_RS232SHDWN
  ,OUT_SPKSHDWN
  ,IOEND
};
typedef enum ioid tm_ioid_t;

struct iostatus
{
  tm_u8_t    Status;
  tm_u16_t    Count;
};
typedef struct iostatus tm_iostatus_t;

struct speedlimit
{
  tm_bool_t  bMapToDigOutEnable;
  tm_u8_t    PeriodicBuzzerOnPeriod;      /* On period for periodic buzzer */
  tm_u8_t    PeriodicBuzzerOffPeriod;     /* Off period for periodic buzzer */
  float OverSpeedLimitMin;
  float OverSpeedLimitMax;
};
typedef struct speedlimit tm_speedlimit_t;

struct speedalert
{
  tm_bool_t  bAccidentAlert;
  tm_bool_t  bBreakAlert;
  tm_bool_t  bAccelarationAlert;
  float    AccidentThreshold;           /* Speed threshold at which accident alert triggers */
  float    BreakThreshold;              /* Break threshold at which Break alert triggers */
  float    AccelarationThreshold;       /* Accelaration threshold at which accident alert triggers */
};
typedef struct speedalert tm_speedalert_t;


struct phnumber
{
  char  Phonenumber[PHNBR_SIZE];      
};
typedef struct phnumber pr_phnumber_p; 

struct serverip
{
  tm_bool_t     bServerIpAddrValid;    /* TRUE=IP Address is valid */
  tm_bool_t     bServerNameValid;      /* TRUE=Server name is valid */
  tm_bool_t     bServerDNS1Valid;      /* TRUE=DNS1 is valid */
  tm_bool_t     bServerDNS2Valid;      /* TRUE=DNS2 is valid */
  char     ServerIpAddr[SERIPADDR_SIZE];/* Server IP Address */
  char     ServerName[SERVERNAME_SIZE]; /* Server Name */
  char     DnsServer1IPAddr[DNS_SIZE];  /* DNS server IP1 */
  char     DnsServer2IPAddr[DNS_SIZE];  /* DNS server IP2 */
};
typedef struct serverip tm_serverip_t;

struct otaserverip
{
  tm_bool_t     bServerIpAddrValid;      /* TRUE=IP Address is valid */
  char     ServerIpAddr[SERIPADDR_SIZE];/* Server IP Address */
};
typedef struct otaserverip tm_otaserverip_t;

enum vehiclestatus
{
   VEHICLE_STOPPED
  ,VEHICLE_MOVING
  ,VEHICLE_IDLE
};

struct tm_periodic
{
  //char  Speed[SPEED_SIZE];
 // char  Distance[DIST_SIZE];
  //char  Heading[HEAD_SIZE];
  //char  NoOfSat[NOOFSAT_SIZE];
  //char  GSMSigStrength[SIGSTRNGTH_SIZE];
  tm_u32_t UTCTime;
  tm_u8_t  Resetstatus;
  tm_u16_t X_Data;
  tm_u16_t Y_Data;
  tm_u16_t Z_Data;
  tm_u8_t  VehicleStatus;
  tm_u8_t  FuelData[FUELDATASIZE];
#ifdef CUSTOMER_MAGNASOFT
  tm_u8_t  RFID[32];
#endif
#ifdef CUSTOMER_MTAP
  tm_latlong_t Position[10];
  tm_u8_t    PositionIndex;
#endif
};
typedef struct tm_periodic tm_periodic_t;

struct tm_fence
{
  tm_u8_t  GeoFenceStatus[GEOFENCES_MAX];
};
typedef struct tm_fence tm_fence_t;

struct tm_smo
{
  tm_u8_t SerialToGPRSSize;
  tm_u8_t SerialToGPRS_BUF[SERIALTOGPRS_BUFFER_SIZE];
};
typedef struct tm_smo tm_smo_t;

union tm_packetdata
{
  //tm_periodic_t  periodic;
  //tm_fence_t     fence;
  //tm_smo_t       smo;
  tm_u8_t        AckData[ACKDATALEN];
  tm_u8_t        CanData[CANDATALENGTH];
  tm_u16_t       DTCCodes[DTCDATALENGTH];
};
typedef union tm_packetdata tm_packetdata_t;
struct tm_packet
{
  tm_u8_t WriteInd;
  tm_u8_t Packettype;
  char  Latitude[LAT_SIZE];
  char  Longitude[LONG_SIZE];
  char  Time[TIME_SIZE];
  char  Date[TIME_SIZE];
  char  Speed[SPEED_SIZE];
  char  Heading[HEAD_SIZE];
  char  NoOfSat[NOOFSAT_SIZE];
  char  CarBattVoltage[ANALOG_SIZE];
  //char  AnalogIN[ANALOG_SIZE];
  char  InternalBatVoltage[ANALOG_SIZE];
  float GPSOdometerReading;
  float G_Acc;
  tm_u32_t SeqNumber;
  tm_s16_t X_Data;
  tm_s16_t Y_Data;
  tm_s16_t Z_Data;
  tm_u8_t  bVehicleStatus;
  tm_u8_t  bGPSStatus;
  tm_bool_t  bIgnition;
  tm_bool_t  bMainPower;
  tm_bool_t  bValid;
  tm_bool_t  bStored;
  tm_packetdata_t packet;
  tm_u8_t ChkSumLow;
  tm_u8_t ChkSumHigh;
};
typedef struct tm_packet tm_packet_t;


#endif /* _DEVICEPARAMS_H_ */
