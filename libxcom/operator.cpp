/*************************************************************************
	> File Name: operator.cpp
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月06日 星期二 12时42分00秒
 ************************************************************************/

#include<stdio.h>
#include "operator.h"
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <map>
#include <utils/logger.h>
#include <utils/custom.h>
#include <vector>
#include <iterator>
#include "data.h"

/* CRC 高位字节值表 */
static unsigned char auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 };
/* CRC低位字节值表*/
static unsigned char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2,
    0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6,
    0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE,
    0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA,
    0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62,
    0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE,
    0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76,
    0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A,
    0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86,
    0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40 };

Operator::Operator(OpInterface *interface):
    m_pOperatorInterface(interface) {
    PushStack(AUTO_CMD);
    PushStack(AUTO_PARAM);
}

Operator::~Operator()
{
    PopStack(AUTO_PARAM);
    PopStack(AUTO_CMD);
}

OpInterface *Operator::Interface()
{
    return m_pOperatorInterface;
}

void Operator::SetInterface(OpInterface *iface)
{
    m_pOperatorInterface = iface;
}

OpValue Operator::operator[](const char *var)
{
    std::map<std::string,OpValue>::iterator iter;
    if ( (iter = m_mapOut.find(var)) != m_mapOut.end()) {
        return (OpValue)iter->second;
    }
    return OpValue::Null();
}

void Operator::SetVar(const char *var, const OpValue &v)
{
    m_mapOut[var] = v;
}

void Operator::clearOut()
{
    m_mapOut.clear();
}

int32_t Operator::InsertCmd(int index,const OpValue& value)
{
    topData().opResult.insert(topData().opResult.begin()+index,value);
    if (value.type == TYPE_CMD)
        return value.cmd.Length();
    else if (value.IsDigital())
        return 1;
    return 0;
}

OpValue Operator::GetCmd(int32_t start,int32_t length)
{
    OpValue opValue;
    for (vector<OpValue>::iterator iter=
         topData().opResult.begin();
         iter != topData().opResult.end();
         iter++) {
        opValue << *iter;
    }
    return opValue.GenCmd(start,length);
}

int32_t Operator::GetLength()
{
    int32_t len = 0;
    for (vector<OpValue>::iterator iter=
         topData().opResult.begin();
         iter != topData().opResult.end();
         iter++) {
        if (((OpValue)*iter).type == TYPE_CMD)
            len += ((OpValue)*iter).cmd.Length();
        else if (((OpValue)*iter).IsDigital())
            len += 1;
    }
    return len;
}

void Operator::AddParam(OpValue value)
{
    topParam().opParams.push_back(value);
}

OpValue Operator::GetParam(int32_t index)
{
    if (topParam().opParams.empty()||index<0 ||
            index >= topParam().opParams.size())
        return OpValue::Null();

    return topParam().opParams.at(index);
}

tExpress Operator::FindDefinition(const string &def)
{
    if (!m_pOperatorInterface)
        return NULL;
    return m_pOperatorInterface->LocateFunction(def);
}

OpValue Operator::VarCall(const char *var,int32_t pos,const CmdValue& cmdValue)
{
    #define STREQUAL(str) !strcmp(var,str)

    /* 先使用局部定义变量 */
    std::map<std::string,OpValue>::iterator iter;

    if ( (iter = m_mapOut.find(var)) != m_mapOut.end()) {
        OpValue &opLen = iter->second;
        return opLen;
    }
    else if (STREQUAL("pos")) {
        return pos;
    }
    else if(STREQUAL("len")) {
        std::map<std::string,OpValue>::iterator iter;

        if ( (iter = m_mapOut.find("len")) != m_mapOut.end()) {
            OpValue &opLen = iter->second;
            return opLen;
        }
        return m_pOperatorInterface->RegLength();
    }
    else if (STREQUAL("addr")) {
        uint8_t *cmd;
        int32_t len;
//
//        char devId[20] ={"7147491B"};
        len = utils::strtocmd(m_pOperatorInterface->DevAddr().c_str(),cmd);
        if (len == 0){
            LOG_ERR("str to hex cmd error");
            return OpValue::Null();
        }
        OpValue value(cmd,len);
        free(cmd);

        return value;
    }
    else {
        return m_pOperatorInterface->GetVar(var);
    }

#undef STREQUAL
}

OpValue Operator::FunctionCall(const char *func, int32_t pos, const CmdValue &recvCmd)
{
    #define STREQUAL(str) !strcmp(func,str)

    std::map<std::string,OpValue>::iterator iter;

    if ( (iter = m_mapOut.find(func)) != m_mapOut.end()) {
        OpValue &opLen = iter->second;
        return CmdCalc(func,opLen);
    }
    else if (STREQUAL("group")) {
        if (topParam().opParams.size() < 1 ){
            LOG_ERR("group:parameter error");
            return OpValue::Null();
        }
        OpValue v1;
        OpValue v2;
        if (topParam().opParams.size() >1 ) {
            v1 = GetParam(0);
            v2 = GetParam(1);
        }
        else {
            v1 = 0;
            v2 = GetParam(0);
        }
        if (!v1.IsInteger() || !v2.IsInteger()) {
            LOG_ERR("group:parameter must be integer");
            return OpValue::Null();
        }

        if (v1.Integer() < 0 ||
                v1.Integer()+v2.Integer()>GetLength()) {
            LOG_ERR("group:length is too long" );
            return OpValue::Null();
        }
        return GetCmd(v1.Integer(),v2.Integer());
    }
    else if (STREQUAL("assign")) {
        if (topParam().opParams.size()!=2){
            LOG_ERR("assign:ditch parameter error");
            return OpValue::Null();
        }
        OpValue v1 = GetParam(0);
        OpValue v2 = GetParam(1);

        if (!v1.IsString()) {
            LOG_ERR("assign:parameter must be integer");
            return OpValue::Null();
        }

        if (!strcmp(v1.data.str,"addr")) {
            CmdValue cmd = v2.GenCmd();
            string str;
            for (int i = 0;i<cmd.Length();i++) {
                str += utils::digittochar((cmd[i]&0xf0)>>4);
                str += utils::digittochar(cmd[i]&0x0f);
            }
            m_mapOut.insert(
                        std::pair<string,OpValue>(
                            v1.data.str,
                            OpValue(str.c_str())));
        }
        else if (!strcmp(v1.data.str,"len")) {
            m_mapOut.insert(
                        std::pair<string,OpValue>(
                            v1.data.str,
                            OpValue(v2.Integer())));
        }
        else {
            m_mapOut.insert(
                        std::pair<string,OpValue>(
                            v1.data.str,
                            v2));
        }

        return v2;
    }
    else if (STREQUAL("rbcd")) {
        if (topParam().opParams.size()  != 1){
            LOG_ERR("rbcd:parameter error");
            return OpValue::Null();
        }
        OpValue v1 = GetParam(0);
        if (v1.type == TYPE_STR) {
            LOG_ERR("rbcd:parameter must be string");
            return OpValue::Null();
        }
        CmdValue cmd = v1.GenCmd();
        string str = utils::hexcmdtostr(cmd.Cmd(),cmd.Length());
        return OpValue(str.c_str());
    }
    else if (STREQUAL("cmp")) {
        if (topParam().opParams.size()!=2) {
            LOG_ERR("chk:parameter count error");
            return OpValue::Null();
        }

        OpValue v1 = GetParam(0);
        OpValue v2 = GetParam(1);

        if (v1 != v2) {
            return (uint8_t)0;
        }
        else {
            return (uint8_t)1;
        }
    }
    else if (STREQUAL("chk")) {
        if (topParam().opParams.size()!=2) {
            LOG_ERR("chk:parameter count error");
            return OpValue::Null();
        }

        OpValue v1 = GetParam(0);
        OpValue v2 = GetParam(1);
        OpValue rt;

        if (v1 != v2) {
            rt = 0;
        }
        else {
            rt = 1;
        }

        if (m_mapOut.find("chk") != m_mapOut.end()) {
            m_mapOut["chk"] = rt;
        }
        else
            m_mapOut.insert(
                    std::pair<string,OpValue>(
                        "chk",
                        rt));
        return rt;
    }
    else if (STREQUAL("reverse")) {
        if (topParam().opParams.size()!=1) {
            LOG_ERR("reverse:parameter count error");
            return OpValue::Null();
        }
        CmdValue cmd = GetParam(0).GenCmd();
        for (int i = 0;i < cmd.Length()/2;i++) {
            uint8_t tmp = cmd[i];
            cmd[i] = cmd[cmd.Length()-i-1];
            cmd[cmd.Length()-i-1] = tmp;
        }
        return cmd;
    }
    else if (STREQUAL("crc")) {
        if (topParam().opParams.size() < 5) {
            LOG_ERR("crc:parameter count error");
            return OpValue::Null();
        }
        OpValue v0 = GetParam(0);
        OpValue v1 = GetParam(1);
        OpValue v2 = GetParam(2);
        OpValue v3 = GetParam(3);
        OpValue v4 = GetParam(4);
        if (!v1.IsInteger() || !v2.IsInteger() || !v3.IsInteger() ||!v4.IsInteger()) {
            LOG_ERR("crc:parameter must be 4 integer");
            return OpValue::Null();
        }
        uint8_t  wEnc = 0;
        if (topParam().opParams.size() >= 6) {
            OpValue v5 = GetParam(5);
            if (v5.IsInteger()) {
                wEnc = v5.Integer();
            }
        }

        uint16_t wCRCin = v2.Integer();
        uint16_t wCPoly = v1.Integer();
        bool low = (v4.Integer()==0);
        uint8_t  wChar = 0;

        CmdValue usData = v0.GenCmd();

        for(int i = 0;i < usData.Length();i++)
        {
              wChar = usData[i];
              if (low) {
                  utils::InvertUint8(&wChar,&wChar);
              }
              wCRCin ^= wChar << 8;
//              wCRCin ^= wEnc;
              for(int i = 0;i < 8;i++)
              {
                if(wCRCin & 0x8000)
                  wCRCin = ((wCRCin << 1)) ^ wCPoly;
                else
                  wCRCin = (wCRCin << 1);
              }
        }
        if (low)
            utils::InvertUint16(&wCRCin,&wCRCin);

        wCRCin ^= (uint16_t)v3.Integer();

        CmdValue result;
        for (int i = 0;i < 2;i++) {
            result +=(uint8_t)(wCRCin&0x00ff);
            wCRCin >>= 8;
        }
        return result;
    }
#if 1
    else if (STREQUAL("eram")) {
        if (topParam().opParams.size() < 2) {
            LOG_ERR("eram:parameter count error");
            return OpValue::Null();
        }
        OpValue v0 = GetParam(0);
        OpValue v1 = GetParam(1);
        if (!v1.IsInteger()) {
            LOG_ERR("crc:second parameter must be integer");
            return OpValue::Null();
        }
        CmdValue usData = v0.GenCmd();
        int usDataLen = usData.Length();
        uint8_t uchRand = v1.Integer();

        unsigned char uchCRCHi = 0xFF;
        /* 高CRC字节初始化 */
        unsigned char uchCRCLo = 0xFF;
        /* 低CRC 字节初始化 */
        unsigned uIndex; /* CRC循环中的索引 */
        for (int i = 0;i < usDataLen;i++)/* 传输消息缓冲区 */
        {
            uIndex = uchCRCHi ^ usData[i] ; /* 计算CRC */
            uchCRCHi = uchCRCLo ^ (auchCRCHi[uIndex]^uchRand);
            uchCRCLo = auchCRCLo[uIndex];
        }
        CmdValue result;
        result+=uchCRCHi;
        result+=uchCRCLo;
        return result;
    }
#endif
    else if(STREQUAL("data")) {

        if (topParam().opParams.size() == 2) {
            OpValue v1 = GetParam(0);
            OpValue v2 = GetParam(1);

            if (!v2.IsInteger() || v1.IsInteger()) {
                LOG_ERR("data:parameter must be integer");
                return OpValue::Null();
            }

            CmdValue data = ((*this)["data"]).GenCmd();
            int32_t start = v1.Integer();
            int32_t len = v2.Integer();

            if (data.IsEmpty() || len > data.Length() ||
                    start < 0 || start+len>data.Length()) {
                LOG_ERR("data:len(%d) bigger than cmd(%d)",len,data.Length());
                return OpValue::Null();
            }
            CmdValue hex;
            for (int32_t i = 0;i<len;i++) {
                hex += data[start+i];
            }
            return hex;
        }
        else if(topParam().opParams.size() == 1){
            OpValue v1 = GetParam(0);
            m_mapOut.insert(
                        std::pair<string,OpValue>(
                            "data",
                            v1));
            return v1;
        }
        else{
            return recvCmd;
        }

    }
    else if(STREQUAL("itom")) {
        if (topParam().opParams.size()<2) {
            LOG_ERR("itom:parameter count error");
            return OpValue::Null();
        }

        OpValue v1 = GetParam(0);
        OpValue v2 = GetParam(1);
        uint8_t vec = 0;
        if (!v1.IsInteger()) {
            LOG_ERR("itom:parameter must be integer");
            return OpValue::Null();
        }

        if (v1.Integer()>4 || v1.Integer()<=0) {
            LOG_ERR("itom:first parameter must be 1~4");
            return OpValue::Null();
        }


        return v2.GenCmd(0,v1.Integer());

        if (!v2.IsInteger()) {
            return v2.GenCmd(0,v1.Integer());
        }


        union
        {
            uint32_t u32;
            uint8_t  u8[4];
        }urt;
        urt.u32 = v2.Integer();
        int32_t byte = v1.Integer();

        if (topParam().opParams.size()>2) {
            OpValue v3 = GetParam(2);
            if (!v3.IsInteger())
                LOG_ERR("invalid params,default 0");
            else
                vec = v3.Integer();
        }

        CmdValue result;
        if (vec == 0) {
            for (int32_t i = byte-1;i >= 0;i--) {
                result += urt.u8[i];
            }
        }
        else {
            for (int32_t i = 0;i < byte;i++) {
                result += urt.u8[i];
            }
        }
        return result;
    }
    else if (STREQUAL("bit")) {
        OpValue v1;
        OpValue v2;
        OpValue v3;

        CmdValue data;

        if (m_stackParam.size() > 0 &&
                topParam().opParams.size() == 1) {
            v1 = GetParam(0);
            data = v1.GenCmd();
            v2 = 0;
            v3 = 0;
        }
        else if (m_stackParam.size() > 0 &&
                 topParam().opParams.size() == 3) {
            v1 = GetParam(0);
            v2 = GetParam(1);
            v3 = GetParam(2);
            data = v1.GenCmd();
        }
        else {
            LOG_ERR("bit:%s parameter count error",func);
            return recvCmd;
        }

        if (v1.IsEmpty() || !v2.IsInteger() || !v3.IsInteger()) {
            LOG_ERR("bit:params type error");
            return OpValue::Null();
        }

        int32_t start = v2.Integer();
        uint8_t bit = v3.Integer();

        if (data.IsEmpty() || start >= data.Length() ||
                start < 0 || bit >= 8) {
            LOG_ERR("bit:%d bigger than 8",bit);
            return OpValue::Null();
        }
        data.Show();
        uint8_t k = data[start];
        uint8_t a = (1 << bit);
        uint8_t vt = k & a;
        LOG_DEBUG("k  = %u,a = %u,vt = %u",k,a,vt);
        return (uint8_t)(vt>>bit);
    }
    else if (STREQUAL("rand")) {
        OpValue v1;

        CmdValue data;

        if (topParam().opParams.size() < 1) {
            LOG_ERR("rand:params is less");
            return OpValue::Null();
        }
        v1 = GetParam(0);
        if (!v1.IsInteger()) {
            LOG_ERR("rand:params is not integer");
            return OpValue::Null();
        }

        int num = v1.Integer();
        for (int i = 0;i < num;i++) {
            data +=(uint8_t)rand()&0x000000ff;
        }
        return data;
    }
    else
        return EncodeCalc(func,recvCmd);

#undef STREQUAL
}

OpValue Operator::EncodeCalc(const char *func, const CmdValue &recvCmd)
{
#define STREQUAL(str) !strcmp(func,str)
    OpValue v1;
    OpValue v2;
    OpValue v3;

    CmdValue data;

    if (m_stackParam.size() > 0 &&
            topParam().opParams.size() == 1) {
        v1 = GetParam(0);
        data = v1.GenCmd();
        v2 = 0;
        v3 = data.Length();

    }
    else if (m_stackParam.size() > 0 &&
             topParam().opParams.size() == 3) {
        v1 = GetParam(0);
        v2 = GetParam(1);
        v3 = GetParam(2);
        data = v1.GenCmd();
    }
    else {
        LOG_ERR("%s parameter count error",func);
        return recvCmd;
    }

    if (v1.IsEmpty() || !v2.IsInteger() || !v3.IsInteger()) {
        LOG_ERR("params type error");
        return OpValue::Null();
    }

    int32_t start = v2.Integer();
    int32_t len = v3.Integer();

    if (data.IsEmpty() || len > data.Length() ||
            start < 0 || start+len>data.Length()) {
        LOG_ERR("len(%d) bigger than cmd(%d)",len,data.Length());
        return OpValue::Null();
    }

    if (STREQUAL("int")) {
        int64_t v = 0;
        for (int i = 0;i < len;i++) {
            v = (((uint64_t)v)<<8)|(uint8_t)data[start+i];
        }
        return v;
    }
    else if (STREQUAL("uint")) {
        uint64_t v = 0;
        for (int i = 0;i < len;i++) {
            v = (((uint64_t)v)<<8)|(uint8_t)data[start+i];
        }
        return v;
    }
    else if(STREQUAL("float")) {
        int64_t v = 0;
        for (int i = 0;i < len;i++) {
            v = (((uint64_t)v)<<8)|(uint8_t)data[start+i];
        }
        if (len <= 4) {
            return *(float*)&v;
        }
        else {
            return *(double*)&v;
        }
    }
    else if(STREQUAL("sum")) {
        int64_t vt = 0;
        for (int32_t i = 0;i<len;i++) {
            vt += data[start+i];
        }
        return vt;
    }
    else if(STREQUAL("product")) {
        int64_t vt = 1;
        for (int32_t i = 0;i<len;i++) {
            vt *= data[start+i];
        }
        return vt;
    }
    else if(STREQUAL("bcd")) {
        uint32_t vt = 0;
        for (int32_t i = 0;i<len;i++) {
            vt =vt*100+((data[start+i]&0x0f)+((data[start+i]&0xf0)>>4)*10);
        }
        return vt;
    }
    else if (STREQUAL("xor")) {
        uint32_t v = 0;
        for (int i = 0;i < len;i++) {
            v = v^(uint8_t)data[start+i];
        }
        return v;
    }
    else if (STREQUAL("hex")){
        //hex
        CmdValue hex;
        for (int32_t i = 0;i<len;i++) {
            hex += data[start+i];
        }
        return hex;
    }
    else {
        m_mapOut.insert(
                    std::pair<string,OpValue>(
                        func,
                        v1));
        return m_opDitch;
    }
#undef STREQUAL
}

OpValue Operator::CmdCalc(const char *func,OpValue &data)
{
    if (data.type == TYPE_CMD) {
        if (m_stackParam.empty() || topParam().opParams.size() == 0) {
            return data;
        }
        else {
            int32_t start = 0;
            int32_t len = 0;
            if (topParam().opParams.size() == 1) {
                OpValue v1 = GetParam(0);
                if (!v1.IsInteger()) {
                    LOG_ERR("%s first parameter error",func);
                    return OpValue::Null();
                }
                len = v1.Integer();
            }
            else {
                OpValue v1,v2;
                v1 = GetParam(0);
                v2 = GetParam(1);

                if (!v1.IsInteger() || !v2.IsInteger()) {
                    LOG_ERR("%s parameters error",func);
                    return OpValue::Null();
                }
                start = v1.Integer();
                len = v2.Integer();
            }

            if (start < 0 || (len!=-1 && start+len > data.cmd.Length())) {
                LOG_ERR("%s parameters is too long or error",func);
                return OpValue::Null();
            }
            return data.GenCmd(start,len);
        }
    }
    else {
        return data;
    }
}

OpValue Operator::KeyFunctionCall(const char *keyfunc, int32_t byte, const CmdValue &recvCmd)
{
    if (!strcmp(keyfunc,"ditch")) {
        if (recvCmd.IsEmpty()) {
            LOG_ERR("ditch parameter is less");
            return OpValue::Null();
        }
        if (byte >recvCmd.Length()||byte <= 0) {
            LOG_ERR("ditch too long or too short");
            return OpValue::Null();
        }

        if (byte == 1)
            m_opDitch = OpValue(recvCmd[0]);
        else
            m_opDitch = OpValue(recvCmd.Cmd(),byte);
        return m_opDitch;
    }
    else if (!strcmp(keyfunc,"fill")) {
        OpValue v1 = GetParam(0);
        OpValue v2 = 0;
        if (v1.IsEmpty())
            return OpValue::Null();
        CmdValue cmd = v1.GenCmd();
        if (byte <= 0) {
            LOG_ERR("fill length cannot be zero");
            return OpValue::Null();
        }
        if (byte == 1)
            return OpValue(cmd[0]);

        if (topParam().opParams.size()>=2)
            v2 = GetParam(1);
        CmdValue result;
        for (int i = 0;i<byte && i<cmd.Length();i++) {
            result += cmd[i];
        }

        for (int i = cmd.Length();i < byte;i++) {
            if (v2.Integer()==0)
                result += 0;
            else
                result.Insert(0,0);
        }
        return result;
    }
    return OpValue::Null();
}

OpValue Operator::BaseCall(const char c,OpValue &v1,OpValue &v2)
{
    if (v1.IsEmpty() || v2.IsEmpty())
        return OpValue::Null();
    switch(c) {
    case '+':
        return v1+v2;
    case '-':
        return v1-v2;
    case '*':
        return v1*v2;
    case '/':
        return v1/v2;
    case '%':
        return v1%v2;
    case '^':
        return v1^v2;
    case '|':
//        return v1|v2;
        ASSERT("'|' not support!");
    case '&':
//        return v1&v2;
        ASSERT("'&' not support!");
    default:
        return OpValue::Null();
    }
}

OpValue Operator::SingleCall(const char c, OpValue &v)
{
    if (v.IsEmpty())
        return OpValue::Null();
    switch (c) {
    case '-':
        return OpValue(0)-v;
    case '!':
        ASSERT("'!' not support!");
    default:
        return v;
    }
}

TYPE_DEFINE Operator::VarType(const char *var)
{
#define STREQUAL(str) !strcmp(var,str)
    /* 先使用局部定义变量 */
    if (STREQUAL("pos")) {
        return TYPE_S32;
    }
    else if(STREQUAL("len")) {
        return TYPE_S32;
    }
    else if (STREQUAL("addr")) {
        return TYPE_CMD;
    }
    else {
        LOG_FIXME("var %s not defined",var);

        return TYPE_NONE;
    }
#undef STREQUAL
}

TYPE_DEFINE Operator::FuncType(const char *func)
{
#define STREQUAL(str) !strcmp(func,str)

    if (STREQUAL("group")) {
        return TYPE_CMD;
    }
    else if (STREQUAL("assign")) {
        return TYPE_CMD;
    }
    else if (STREQUAL("rbcd")) {
        return TYPE_CMD;
    }
    else if (STREQUAL("chk")) {
        return TYPE_U8;
    }
    else if (STREQUAL("reverse")) {
        return TYPE_CMD;
    }
    else if (STREQUAL("crc")) {
        return TYPE_CMD;
    }
    #if 1
    else if (STREQUAL("eram")) {
        return TYPE_CMD;
    }
    #endif
    else if(STREQUAL("data")) {
        return TYPE_CMD;
    }
    else if(STREQUAL("itom")) {
        return TYPE_CMD;
    }
    else if (STREQUAL("rand")) {
        return TYPE_CMD;
    }
    else
        return CalcType(func);
#undef STREQUAL
}

TYPE_DEFINE Operator::CalcType(const char *func)
{
    #define STREQUAL(str) !strcmp(func,str)
    if (STREQUAL("int")) {
        return TYPE_S32;
    }
    else if(STREQUAL("float")) {
        return TYPE_F32;
    }
    else if(STREQUAL("sum")) {
        return TYPE_S32;
    }
    else if(STREQUAL("product")) {
        return TYPE_S32;
    }
    else if(STREQUAL("bcd")) {
        return TYPE_S32;
    }
    else if (STREQUAL("xor")) {
        return TYPE_U32;
    }
    else if (STREQUAL("hex")){
        //hex
        return TYPE_CMD;
    }
    else {
        return TYPE_CMD;
    }
}

Operator::OperatorData &Operator::topData()
{
    ASSERT(!m_stackData.empty());
    return (Operator::OperatorData&)m_stackData.top();
}

Operator::OperatorParam &Operator::topParam()
{
    ASSERT(m_stackParam.size() != 0);
    return (Operator::OperatorParam&)m_stackParam.top();
}

void Operator::PushStack(uint32_t type)
{
    if (type == AUTO_CMD)
        m_stackData.push(OperatorData());
    else if (type == AUTO_PARAM)
        m_stackParam.push(OperatorParam());
}

void Operator::PopStack(uint32_t type)
{
    if (type == AUTO_CMD)
        m_stackData.pop();
    else if (type == AUTO_PARAM)
        m_stackParam.pop();
}

void Operator::AddtoStack(uint32_t type, const vector<OpValue> &list)
{
    if (type == AUTO_CMD)
        topData().opResult.insert(topData().opResult.begin(),
                                  list.begin(),list.end());
    else if (type == AUTO_PARAM) {
        vector<OpValue>::const_iterator iter;
        for (iter = list.begin(); iter != list.end();iter++) {
            topParam().opParams.push_back(*iter);
        }
    }
}
