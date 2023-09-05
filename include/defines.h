/*************************************************************************
	> File Name: defines.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年01月17日 星期三 15时36分49秒
 ************************************************************************/
#ifndef __GLOBAL_DEFINES__
#define __GLOBAL_DEFINES__

#include "macro.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define floatcmp(f,d) ((f-d)<0.000001 && (f-d)>-0.000001)

typedef void*            HANDLE;

typedef enum
{
    TYPE_NONE = 0,
    TYPE_U8,
    TYPE_S8,
    TYPE_U16,
    TYPE_S16,
    TYPE_U32,
    TYPE_S32,
    TYPE_U64,
    TYPE_S64,
    TYPE_F32,
    TYPE_F64,
    TYPE_STR,
    TYPE_POINTER,
    TYPE_CMD
}TYPE_DEFINE;

typedef struct
{
    TYPE_DEFINE type;
    union{
        int8_t   s8;
        uint8_t   u8;
        int16_t  s16;
        uint16_t  u16;
        int32_t  s32;
        uint32_t  u32;
        int64_t  s64;
        uint64_t  u64;
        float  f32;
        double  f64;
        int8_t   str[8];
    }data;
}UnionData;
#define CONFIG_LOG_ENV          "LOG_ENV"
#define MAX_ID_LEN              20
#define MAX_TYPE_LEN            20
#define MAX_VAR_LEN             32
#define MAX_CMD_LEN             256
#define MAX_COND_LEN            256

#define MAX_NAME_LEN            128
#define MAX_SECRETKEY_LEN       128

#define MAX_RECV_SIZE        512
#define MAX_NETADDR_LEN      128
#define MAX_SERIALADDR_LEN   10
#define MAX_REQUEST_SIZE     4096

#include "devid.h"

#endif//__GLOBAL_DEFINES__
