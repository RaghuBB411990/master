#include <stdint.h>
#ifndef _APATYPES_H_
#define _APATYPES_H_

#define NULL 0
typedef uint32_t u32_t;
typedef int32_t s32_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;
typedef int8_t s8_t;
typedef uint8_t u8_t;

#define TRUE ((bool_t)1)
#define FALSE ((bool_t)0)
#define HIGH 1
#define LOW 0
typedef uint8_t tm_u8_t;
typedef int8_t tm_s8_t;
typedef uint16_t tm_u16_t;
typedef int16_t tm_s16_t;
typedef uint32_t tm_u32_t;
typedef int32_t tm_s32_t;
typedef uint64_t tm_u64_t;
typedef int64_t tm_s64_t;

#define TM_FALSE ((tm_bool_t)0)
#define TM_TRUE  ((tm_bool_t)1)

/** Boolean type: should only be assigned to TL_FALSE or TL_TRUE */
typedef tm_u8_t tm_bool_t;


typedef u8_t bool_t;

#endif


