/*************************************************************************
	> File Name: devid.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月22日 星期四 14时18分47秒
 ************************************************************************/
#ifndef __DEV_ID_H__
#define __DEV_ID_H__

#include <stdio.h>
#include "defines.h"
#include <string.h>

//typedef struct __ID_DEV{
//    int8_t devId[MAX_ID_LEN];
//    bool operator==(struct __ID_DEV &id){
//        return !strncmp(id.devId,devId,MAX_ID_LEN);
//    }
//}ID_DEV;
class ID_DEV
{
public:
    ID_DEV(){
        memset(devId,0,sizeof(devId));
    }

    ID_DEV(const char *str)
    {
        memset(devId,0,sizeof(devId));
        operator =(str);
    }

//    ID_DEV(const ID_DEV &dev)
//    {
//        memset(devId,0,sizeof(devId));
//        operator =(dev);
//    }

    const static ID_DEV Null() {
        return ID_DEV();
    }

    void clear() {
        memset(devId,0,sizeof(devId));
    }

    bool IsEmpty() {
        return devId[0] == 0;
    }

    const char *c_str()
    {
        return devId;
    }

    const char *c_str() const
    {
        return devId;
    }

    const int32_t length() const
    {
        return strlen(devId);
    }

    ID_DEV& operator =(const char * str)
    {
        ASSERT(strlen(str)<=MAX_ID_LEN);
//        memset(devId,0,sizeof(devId));
        strncpy(devId, str, MAX_ID_LEN);

        return *this;
    }

    ID_DEV& operator =(const ID_DEV& dev)
    {
        ASSERT(strlen(dev.c_str())<=MAX_ID_LEN);
//        memset(devId,0,sizeof(devId));
        strncpy(devId,dev.c_str(),MAX_ID_LEN);
        return *this;
    }

    bool operator ==(const char *str) const
    {
        ASSERT(strlen(str)<=MAX_ID_LEN);
        return !strncmp(devId,str,MAX_ID_LEN);
    }

    bool operator ==(const ID_DEV &id) const
    {
        ASSERT(strlen(id.c_str())<=MAX_ID_LEN);
        return !strncmp(devId,id.c_str(),MAX_ID_LEN);
    }

private:
    char devId[MAX_ID_LEN];
};

#endif//__DEV_ID_H__
