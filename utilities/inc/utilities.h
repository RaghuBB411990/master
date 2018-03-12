#ifndef _UTILITIES_H_
#define _UTILITIES_H_


#include"ctype.h"
#include"apatypes.h"
#include "../uart/inc/apa_uart.h"

#define INCINDEX(x,y) { \
                        if(((x)+1) >= (y)) \
                            (x)=0;\
                        else   \
                            (x)++;\
                    }
#define DECINDEX(x,y) { \
                        if( (x) == 0) \
                            (x)=(y)-1;\
                        else   \
                            (x)--;\
                    }

#define INCPTR(z,x,y) { \
                        if( (z+1) > ((x)+(y))) \
                            (z)=(x);\
                        else   \
                            (z)++;\
                    }

#define ADDCIRPTR(x, y, a, l)  { \
                                        if(((x) + (y)) >= ((a) + (l)))  \
                                            (x) = (x) + (y) - (l); \
                                        else \
                                          (x) += (y); \
                                      }
#define INCCIRCULARINDEX(Index, Len)  { \
                                        if(((Index) + 1) >= (Len))  \
                                            (Index) = 0; \
                                        else \
                                          (Index)++; \
                                      }

#define INCCIRCULARPTR(ptr, Base, Len)  { \
                                        if(((ptr) + 1) >= ((Base) + (Len)))  \
                                            (ptr) = (Base); \
                                        else \
                                          (ptr)++; \
                                      }

#define DECCIRCULARINDEX(Index, Len)  { \
                                        if((Index) == 0)  \
                                            (Index) = (Len) - 1; \
                                        else \
                                          (Index)--; \
                                      }

#define DECCIRCULARPTR(ptr, Base, Len)  { \
                                        if((ptr) == (Base))  \
                                            (ptr) = ((Base) + (Len) - 1); \
                                        else \
                                          (ptr)--; \
                                      }

#define ADDCIRCULARINDEX(Index, AddLen, Len)  { \
                                        if(((Index) + (AddLen)) >= (Len))  \
                                            (Index) = (Index) + (AddLen) - (Len); \
                                        else \
                                          (Index) += (AddLen); \
                                      }

#define ADDCIRCULARPTR(ptr, AddLen, Base, Len)  { \
                                        if(((ptr) + (AddLen)) >= ((Base) + (Len)))  \
                                            (ptr) = (ptr) + (AddLen) - (Len); \
                                        else \
                                          (ptr) += (AddLen); \
                                      }
#define PRINTF(str)  UART_3_WrToQueue((char*)(str),(strlen((char*)str)))

#define SPACE 0x20
#define CARRIAGERETURN 0x0D
#define BACKSPACE 0x08
#define NEWLINE 0x0A
#define TAB 0x0B


struct sysconfig
{
  u8_t   DeviceId[10];
  bool_t SysDebugEnable;
  bool_t GsmDebugEnable;
  bool_t GpsDebugEnable;
  bool_t SendGsmCmd;
};
typedef struct sysconfig sysconfig_st;


u32_t my_strlen(char *str1);
void my_strcpy(char *dst, char *src);
void my_strcat(char *src, char *dst);
void my_strrev(char *str, int j);
int my_atoi(char *str);
void my_itoa(int n, char *a);
//void my_itoa(int a,char *str);
//void my_ftoa(double f, char *str, int a);
void my_ftoa(float fvalue,char *buffer,int MAXno_of_fractional_digits);
int my_atof(char *str,double *number);
u32_t atox( char *str );
char* my_xtoa(u32_t n, char *a);
#endif
