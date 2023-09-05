#ifndef __DATA_H__
#define __DATA_H__

#include <stdio.h>
#include <defines.h>
#include <time.h>
#include <sys/time.h>
#include <utils/logger.h>
#include <string>
#include <vector>
#include <memory>

#define TRANS_GET_LEN(d)    (strlen(d))
#define TRANS_GET_V16(p)    (((p)[0]<<8)|(p)[1])
#define TRANS_GET_V32(p)    ((p)[0]<<24|(p)[1]<<16|(p)[2]<<8|(p)[3])
#define TRANS_PRINT(p,len)  \
{\
    char *info=(char*)malloc(3*len+1);\
    for (int i = 0; i < len; i++){\
    sprintf(&info[3 * i], "%02X ", (p)[i]); \
    }\
    info[3*len]='\0';\
    LOG_DEBUG("%s",info); \
    free(info);\
}while (0)

// 状态值定义
#define RECV_COMPLETE       0
#define RECV_NOT_COMPLETE   1
#define RECV_NOT_AVAILIABLE 2
#define RECV_FAILED         3
typedef int8_t  RECV_RESULT;

class OpValue;
class Express;
typedef std::shared_ptr<Express> tExpress;
class ParseOperator;
class OpInterface;
typedef std::shared_ptr<OpInterface> tOpInterface;

class OpInterface
{
public:
    virtual tExpress LocateFunction(const std::string &def,int funcId = -1) = 0;
    virtual OpValue GetVar(const std::string& var) = 0;
    virtual const ID_DEV& DevAddr() = 0;
    virtual const int32_t RegLength() = 0;
};

//template<typename T>
class CmdValue
{
public:
    CmdValue() {
        len = 0;
        cmd = NULL;
    }

    CmdValue(uint8_t *cmd_,int32_t len_){
        construct(cmd_,len_);
    }

    CmdValue(const CmdValue &value) {
        construct(value.Cmd(),value.Length());
    }

    virtual ~CmdValue() {
        distruct();
    }

    static CmdValue Null() {
        return CmdValue();
    }

    bool IsEmpty() const{
        return len == 0;
    }

    bool operator !=(const CmdValue &value) const {
        return !operator ==(value);
    }

    bool operator ==(const CmdValue &value) const {
        if (len == value.Length()) {
            if (len == 0)
                return true;
            return cmpdata(cmd,value.Cmd(),len);
        }
        return false;
    }

    void getNext(int32_t *next,uint8_t sub[],int32_t len) {
        int32_t i = 0;
        int32_t j = -1;

        next[0] = -1;

        while (i < len-1) {
            if (j == -1 || sub[i] == sub[j]) {
                ++i;++j;
                next[i] = j;
            }
            else
                j = next[j];
        }
    }

    int32_t Contains(const CmdValue &value) {
        int32_t ipos,jpos;
        int32_t *next = NULL;

        if (len < value.Length() || value.Length() == 0)
            return -1;
        next = (int32_t *)malloc(value.Length()*sizeof(int32_t));
        getNext(next,value.Cmd(),value.Length());
        ipos = 0;
        jpos = 0;
        while (ipos < len &&
               jpos < value.Length()) {
            if (jpos == -1 || cmd[ipos] == value[jpos]) {
                ipos++;jpos++;
            }
            else
                jpos = next[jpos];
        }
        free(next);

        if (jpos == value.Length())
            return ipos;
        return -1;
    }

    inline void Show() {
        LOG_INFO("len = %d,cmd:",len);
        if (len<=0)
            return;
//        TRANS_PRINT(cmd,len);
        LOG_INFO("%s",toString().c_str());
    }

    inline std::string toString() {
        std::string str;
        for (int i = 0; i < len; i++){
            char buf[10];
            sprintf(buf, "%02X ", cmd[i]);
            str += buf;
        }
        return str;
    }

    inline uint8_t* Cmd() const{
        return cmd;
    }

    inline int32_t Length() const {
        return len;
    }

    uint8_t* CmdAt(int32_t pos) const{
        ASSERT((pos >=0) && (pos < len));
        return &cmd[pos];
    }

    uint8_t& operator [](int32_t pos) const{
        ASSERT((pos >= 0) && (pos < len));
        return cmd[pos];
    }

    CmdValue & operator=(const CmdValue & value) {
        distruct();
        construct(value.Cmd(),value.Length());
        return *this;
    }

    CmdValue & Insert(int32_t index,const uint8_t value) {
        if (cmd) {
            int32_t len_ = len;
            uint8_t* newcmd = new uint8_t[len_+1];
            if (index > 0)
                copydata(&newcmd[0],cmd,index);
            newcmd[index] = value;
            copydata(&newcmd[index+1],&cmd[index],len_-index);
            distruct();
            cmd = newcmd;
            len = len_+1;
        }
        else {
            cmd = new uint8_t[1];
            cmd[0] = value;
            len = 1;
        }
        return *this;
    }

    CmdValue & operator +=(const CmdValue &value) {
        if (value == Null())
            return *this;

        if (cmd) {
            int32_t len_ = len;
            uint8_t* newcmd = new uint8_t[len_+value.Length()];
            copydata(newcmd,cmd,len_);
            copydata(&newcmd[len_],value.Cmd(),value.Length());
            distruct();
            cmd = newcmd;
            len = len_+ value.Length();
        }
        else {
            cmd = new uint8_t[value.Length()];
            copydata(cmd,value.Cmd(),value.Length());
            len = value.Length();
        }

        return *this;
    }

    CmdValue & operator +=(const uint8_t value) {
        if (cmd) {
            int32_t len_ = len;
            uint8_t* newcmd = new uint8_t[len_+1];
            copydata(newcmd,cmd,len_);
            copydata(&newcmd[len_],&value,1);
            distruct();
            cmd = newcmd;
            len = len_+1;
        }
        else {
            cmd = new uint8_t[1];
            cmd[0] = value;
            len = 1;
        }
        return *this;
    }

    CmdValue & operator -=(const CmdValue &value) {
        if (value == Null() || !cmd)
            return *this;

        int32_t pos = Contains(value);
        return TrimLeft(pos);
    }

    CmdValue & operator -=(const uint8_t value) {
        if (!cmd)
            return *this;

        if (cmd[0] == value)
            return TrimLeft(1);
        return *this;
    }

    CmdValue & operator^(const CmdValue &value) {
        if (len != value.len) {
            LOG_ERR("length:%d not equal %d",len,value.len);
            return *this;
        }
        for (int i = 0;i < len;i++) {
            cmd[i] ^= value.cmd[i];
        }
        return *this;
    }

    CmdValue & operator^(const uint8_t ch) {
        if (len > 0) {
            LOG_ERR("length equal %d",len);
            return *this;
        }
        for (int i = 0;i < len;i++) {
            cmd[i] ^= ch;
        }
        return *this;
    }

    CmdValue& TrimLeft(int32_t length) {
        if (length <= 0 )
            return *this;
        if (length >= len) {
            distruct();
            return *this;
        }
        copydata(cmd,&cmd[length],len-length);
        len -= length;
        return *this;
    }

private:
    inline void copydata(uint8_t* dst,const uint8_t* src,int32_t len) {
        for (int i = 0;i < len;i++) {
            dst[i] = src[i];
        }
    }

    bool cmpdata(const uint8_t* dst,const uint8_t* src,int32_t len) const{
        for (int i = 0;i < len;i++) {
            if (dst[i] != src[i])
                return false;
        }
        return true;
    }

    inline void construct(const uint8_t *cmd_,int32_t len_) {
        len = len_;
        if (len_ <= 0) {
            cmd = NULL;
            return;
        }
        cmd = new uint8_t[len];
        copydata(cmd,cmd_,len);
    }

    inline void distruct() {
        if (cmd && len>0)
            delete[] cmd;
        cmd = NULL;
        len = 0;
    }

private:
    int32_t len;
    uint8_t  *cmd;
};

//typedef StrandValue<uint8_t> CmdValue;
//typedef StrandValue<CustomValue> AutoValue;

#define AT_VALUE(pos,len) ((pos)*1000+(len))

class OpValue
{
public:
    OpValue() {
        type = TYPE_NONE;
        data.s64 = 0;
        next = NULL;
    }

    OpValue(const OpValue &value) {
        next = NULL;
//        operator =(value);
        *this = value;
    }

    static OpValue Null() {
        return OpValue();
    }

    OpValue& operator =(const OpValue & value) {
//        release();
//        next = value.next;
        type = value.type;
        copydata(value);
        OpValue *p = copynext(value.next);
        if (next)
            delete next;
        next = p;

        return *this;
    }

    bool IsEmpty() const {
        return ((type == TYPE_NONE) ||
                (type == TYPE_CMD && cmd.IsEmpty()) ||
                (type == TYPE_STR && data.str == NULL) ||
                (type == TYPE_POINTER && data.pointer == NULL));
    }

    bool operator!=(const OpValue &value) {
        return !operator ==(value);
    }

    bool operator!=(const TYPE_DEFINE type) const{
        return !operator ==(OpValue(type));
    }

    bool operator==(const OpValue &value) const {
        //�жϺ���Ԫ��
        if ((next != NULL) || (value.next != NULL)) {
            if (next!=NULL && value.next!=NULL) {
                if (*next != *value.next)
                    return false;
            }
            else
                return false;
        }

        if (type == TYPE_NONE ||value.type==TYPE_NONE){
            if (type != TYPE_NONE ||value.type!=TYPE_NONE)
                return false;
            return true;
        }
        else if (IsInteger() && value.IsInteger())
            return Integer() == value.Integer();
        else if (IsFloat() && value.IsFloat())
            return (Float() - value.Float() > -0.000001) &&
                    (Float() - value.Float() < 0.000001);
        else if (type == TYPE_CMD && value.type == TYPE_CMD)
            return cmd == value.cmd;
        else if (type == TYPE_POINTER)
            return data.pointer==value.data.pointer;
        else
            return !strcmp(data.str,value.data.str);
    }

    OpValue(int8_t v) {
        type = TYPE_S8;
        data.s8 = v;
        next = NULL;
    }

    OpValue(uint8_t v) {
        type = TYPE_U8;
        data.u8 = v;
        next = NULL;
    }

    OpValue(int16_t v) {
        type = TYPE_S16;
        data.s16 = v;
        next = NULL;
    }

    OpValue(uint16_t v) {
        type = TYPE_U16;
        data.u16 = v;
        next = NULL;
    }

    OpValue(int32_t v) {
        type = TYPE_S32;
        data.s32 = v;
        next = NULL;
    }

    OpValue(uint32_t v) {
        type = TYPE_U32;
        data.u32 = v;
        next = NULL;
    }

    OpValue(int64_t v) {
        type = TYPE_S64;
        data.s64 = v;
        next = NULL;
    }

    OpValue(uint64_t v) {
        type = TYPE_U64;
        data.u64 = v;
        next = NULL;
    }

    OpValue(float v) {
        type = TYPE_F32;
        data.f32 = v;
        next = NULL;
    }

    OpValue(double v) {
        type = TYPE_F32;
        data.f32 = v;
        next = NULL;
    }

    OpValue(const char* str) {
        type = TYPE_STR;
        int len = strlen(str);
        data.str = (char*)malloc(len+1);
        strcpy(data.str,str);
        data.str[len] = '\0';
        next = NULL;
    }

    OpValue(void *p) {
        type == TYPE_POINTER;
        data.pointer = p;
        next = NULL;
    }

    OpValue(uint8_t *cmd_,int32_t len) {
        type = TYPE_CMD;
        cmd = CmdValue(cmd_,len);
        next = NULL;
    }

    OpValue(const CmdValue& cmd_) {
        type = TYPE_CMD;
        cmd = CmdValue(cmd_.Cmd(),cmd_.Length());
        next = NULL;
    }

    /* operator=*/
    OpValue& operator =(int8_t v) {
        release();
        type = TYPE_S8;
        data.s8 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(uint8_t v) {
        release();
        type = TYPE_U8;
        data.u8 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(int16_t v) {
        release();
        type = TYPE_S16;
        data.s16 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(uint16_t v) {
        release();
        type = TYPE_U16;
        data.u16 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(int32_t v) {
        release();
        type = TYPE_S32;
        data.s32 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(uint32_t v) {
        release();
        type = TYPE_U32;
        data.u32 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(int64_t v) {
        release();
        type = TYPE_S64;
        data.s64 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(uint64_t v) {
        release();
        type = TYPE_U64;
        data.u64 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(float v) {
        release();
        type = TYPE_F32;
        data.f32 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(double v) {
        release();
        type = TYPE_F32;
        data.f32 = v;
        next = NULL;
        return *this;
    }

    OpValue& operator =(char* str) {
        release();
        type = TYPE_STR;
        int len = strlen(str);
        data.str = (char*)malloc(len+1);
        strcpy(data.str,str);
        data.str[len] = '\0';
        next = NULL;
        return *this;
    }

    OpValue& operator =(void *p) {
        release();
        type = TYPE_POINTER;
        data.pointer = p;
        next = NULL;
        return *this;
    }

    OpValue& operator <<(const OpValue &value) {
        ASSERT(type != TYPE_STR && value.type != TYPE_POINTER) ;

        //����Ԫ��
        if (next != NULL) {
            *next << value;
        }

        if (type != TYPE_CMD && type != TYPE_NONE) {
            cmd += (uint8_t)Integer();
        }
//        if (type != TYPE_CMD)
//            data.cmd = new CmdValue();
        type = TYPE_CMD;
        if (value.type == TYPE_CMD)
            cmd += value.cmd;
        else if (value.type != TYPE_NONE)
            cmd += (uint8_t)value.Integer();

        return *this;
    }

    OpValue& operator <<(const uint8_t value) {
        ASSERT(type != TYPE_STR &&
                type != TYPE_POINTER);

        //����Ԫ��
        if (next != NULL) {
            *next << value;
        }
        if (type != TYPE_CMD && type != TYPE_NONE) {
            cmd += (uint8_t)Integer();
        }
        cmd += value;

        return *this;
    }

    OpValue operator >>(const int32_t at) {
        //����Ԫ��
        int32_t pos = at / 1000;
        int32_t len = at % 1000;
        OpValue opValue = OpValue(GenCmd(pos,len));

        if (next != NULL) {
            opValue.next = new OpValue();
           *opValue.next >>at;
        }
        return opValue;
    }

    CmdValue GenCmd(int32_t pos_ = 0,int32_t len_ = -1) {
        ASSERT(type != TYPE_STR &&
                type != TYPE_POINTER);

        if (type == TYPE_NONE)
            return CmdValue::Null();
        if (type != TYPE_CMD) {
//            uint8_t c[1] ={(uint8_t)Integer()};
            if (len_==-1)
                len_ = 1;
            len_ = len_>8?8:len_;
            uint8_t buf[8] = {0};
            for (int i = 0;i < len_;i++) {
                buf[len_-i-1] = data.buf[i];
            }
            return CmdValue((uint8_t*)buf,len_);
        }
        else if (pos_ < 0)
            return CmdValue::Null();
        else if(pos_ == 0 && len_ == -1) {
            return cmd;
        }
        else {
            if (len_ < 0 || pos_+len_ > cmd.Length() )
                len_ = cmd.Length()-pos_;
            return CmdValue(cmd.CmdAt(pos_),len_);
        }
    }

    virtual ~OpValue() {
        release();
    }

    bool IsString() const {
        return type == TYPE_STR;
    }

    bool IsDigital() const{
        return ((type >= TYPE_U8 && type <= TYPE_F64) ||
                (type == TYPE_CMD && cmd.Length() == 1));
    }

    bool IsInteger() const {
        return (IsDigital() && !IsFloat());
    }

    bool IsFloat() const{
        return (type == TYPE_F32 ||
                type == TYPE_F64);
    }

    int64_t Integer() const{
        ASSERT(((type >= TYPE_U8 && type <= TYPE_F64) ||
                (type == TYPE_CMD && cmd.Length() == 1)));

        switch (type) {
        case TYPE_U8:
            return data.u8;
        case TYPE_S8:
            return data.s8;
        case TYPE_U16:
            return data.u16;
        case TYPE_S16:
            return data.s16;
        case TYPE_U32:
            return data.u32;
        case TYPE_S32:
            return data.s32;
        case TYPE_U64:
            return data.u64;
        case TYPE_S64:
            return data.s64;
        case TYPE_CMD:
            return cmd[0];
        }

        if (IsFloat())
            return (int64_t)Float();

        return 0;
    }

    double Float() const {
        ASSERT(type != TYPE_NONE &&
                type != TYPE_STR &&
                type != TYPE_POINTER);

        switch(type) {
        case TYPE_F32:
            return data.f32;
        case TYPE_F64:
            return data.f64;
        }
        if (IsInteger())
            return Integer();
        return 0;
    }

    OpValue *Next() {
        return next;
    }

    OpValue *AllocNext(const OpValue &next_) {
        if (next) {
            next->AllocNext(next_);
        }
        else {
            next = new OpValue(next_);
        }
        return this;
    }

    typedef enum {ADD,SUB,MUL,DIV,MOD,XOR}OP_E;

    static OpValue calc(OP_E op,const OpValue &value1,const OpValue &value2) {
        if (!value1.IsDigital() || !value2.IsDigital()) {
            CmdValue v1 = value1.cmd;
            if (value1.type != TYPE_CMD) {
                LOG_ERR("error value %u",value1.type);
                return OpValue::Null();
            }

            if (value2.IsDigital() ||(value2.cmd.Length() == 1)) {
                for (int32_t i = 0;i<v1.Length();i++) {
                    switch(op) {
                    case ADD:
                        *(v1.CmdAt(i)) += value2.Integer();
                        break;
                    case SUB:
                        *(v1.CmdAt(i)) -= value2.Integer();
                        break;
                    case MUL:
                        *(v1.CmdAt(i)) *= value2.Integer();
                        break;
                    case DIV:
                    {
                        uint8_t disor = value2.Integer();
                        if (disor != 0)
                            *(v1.CmdAt(i)) /= disor;
                        break;
                    }
                    case MOD:
                    {
                        uint8_t disor = value2.Integer();
                        if (disor != 0)
                            *(v1.CmdAt(i)) %= disor;
                        break;
                    }
                    case XOR:
                    {
                        uint8_t disor = value2.Integer();
                        *(v1.CmdAt(i)) ^= disor;
                        break;
                    }
                    }
                }
                return v1;
            }
            else if (value2.type == TYPE_CMD) {
                if (v1.Length() != value2.cmd.Length()) {
                    LOG_ERR("error value %u",value2.type);
                    return OpValue::Null();
                }
                for (int32_t i = 0;i<v1.Length();i++) {
                    switch(op) {
                    case ADD:
                        *(v1.CmdAt(i)) += *(value2.cmd.CmdAt(i));
                        break;
                    case SUB:
                        *(v1.CmdAt(i)) -= *(value2.cmd.CmdAt(i));
                        break;
                    case MUL:
                        *(v1.CmdAt(i)) *= *(value2.cmd.CmdAt(i));
                        break;
                    case DIV:
                    {
                        uint8_t disor = *(value2.cmd.CmdAt(i));
                        if (disor != 0)
                            *(v1.CmdAt(i)) /= disor;
                    }
                        break;
                    case MOD:
                    {
                        uint8_t disor = *(value2.cmd.CmdAt(i));
                        if (disor != 0)
                            *(v1.CmdAt(i)) %= disor;
                        break;
                    }
                    case XOR:
                    {
                        uint8_t disor = value2.Integer();
                        *(v1.CmdAt(i)) ^= disor;
                        break;
                    }
                    }
                }
                return v1;
            }
            else {
                LOG_ERR("error value %u",value2.type);
                return OpValue::Null();
            }
        }

        if (value1.IsInteger() && value2.IsInteger()) {
            switch(op) {
            case ADD:
                return value1.Integer()+value2.Integer();
            case SUB:
                return value1.Integer()-value2.Integer();
            case MUL:
                return value1.Integer()*value2.Integer();
            case DIV:
            {
                int64_t disor = value2.Integer();
                if (disor != 0)
                    return value1.Integer()/disor;
                return value1.Integer();
            }
            case MOD:
            {
                int64_t disor = value2.Integer();
                if (disor != 0)
                   return value1.Integer() % disor;
                return value1.Integer();
            }
            case XOR:
            {
                int64_t disor = value2.Integer();
                return value1.Integer() ^ disor;
            }
            }
        }
        else if (op != MOD && op != XOR) {
            double result = 0;
            if (value1.IsInteger())
                result = value1.Integer();
            else
                result = value1.Float();

            if (value2.IsInteger()) {
                switch(op) {
                case ADD:
                    result +=value2.Integer();
                    break;
                case SUB:
                    result -=value2.Integer();
                    break;
                case MUL:
                    result *=value2.Integer();
                    break;
                case DIV:
                {
                    int32_t disor = value2.Integer();
                    if (disor != 0)
                        result /= disor;
                }
                    break;
                }
            }
            else {
                switch(op) {
                case ADD:
                    result +=value2.Float();
                    break;
                case SUB:
                    result -=value2.Float();
                    break;
                case MUL:
                    result *=value2.Float();
                    break;
                case DIV:
                {
                    if (!floatcmp(value2.Float(),0))
                        result /=value2.Float();
                }
                    break;
                }
            }
            return result;
        }
        return value1;
    }

    OpValue operator +(const OpValue &value) {
        OpValue result = calc(ADD,*this,value);
        if (value.IsEmpty() || !next)
            return result;
        result.next = new OpValue();
        if (value.next) {
            *result.next = calc(ADD,*next,*value.next);
        }
        else
            *result.next = calc(ADD,*next,value);
        return result;
    }

    OpValue operator -(const OpValue &value) {
        OpValue result = calc(SUB,*this,value);
        if (value.IsEmpty() || !next)
            return result;
        result.next = new OpValue();
        if (value.next) {
            *result.next = calc(SUB,*next,*value.next);
        }
        else
            *result.next = calc(SUB,*next,value);
        return result;
    }

    OpValue operator *(const OpValue &value) {
        OpValue result = calc(MUL,*this,value);
        if (value.IsEmpty() || !next)
            return result;
        result.next = new OpValue();
        if (value.next) {
            *result.next = calc(MUL,*next,*value.next);
        }
        else
            *result.next = calc(MUL,*next,value);
        return result;
    }

    OpValue operator /(const OpValue &value) {
        OpValue result = calc(DIV,*this,value);
        if (value.IsEmpty() || !next)
            return result;
        result.next = new OpValue();
        if (value.next) {
            *result.next = calc(DIV,*next,*value.next);
        }
        else
            *result.next = calc(DIV,*next,value);
        return result;
    }

    OpValue operator %(const OpValue &value) {
        OpValue result = calc(MOD,*this,value);
        if (value.IsEmpty() || !next)
            return result;
        result.next = new OpValue();
        if (value.next) {
            *result.next = calc(MOD,*next,*value.next);
        }
        else
            *result.next = calc(MOD,*next,value);
        return result;
    }

    OpValue operator^(const OpValue &value) {
        OpValue result = calc(XOR,*this,value);
        if (value.IsEmpty() || !next)
            return result;
        result.next = new OpValue();
        if (value.next) {
            *result.next = calc(XOR,*next,*value.next);
        }
        else
            *result.next = calc(XOR,*next,value);
        return result;
    }

    void Show() {
        switch (type) {
        case TYPE_U8:
            LOG_DEBUG("%u",data.u8);
            break;
        case TYPE_S8:
            LOG_DEBUG("%d",data.s8);
            break;
        case TYPE_U16:
            LOG_DEBUG("%u",data.u16);
            break;
        case TYPE_S16:
            LOG_DEBUG("%d",data.s16);
            break;
        case TYPE_U32:
            LOG_DEBUG("%u",data.u32);
            break;
        case TYPE_S32:
            LOG_DEBUG("%d",data.s32);
            break;
        case TYPE_U64:
            LOG_DEBUG("%" PRIu64, data.u64);
            break;
        case TYPE_S64:
            LOG_DEBUG("%" PRId64, data.s64);
            break;
        case TYPE_F32:
            LOG_DEBUG("%0.3f",data.f32);
            break;
        case TYPE_F64:
            LOG_DEBUG("%0.3f",data.f64);
            break;
        case TYPE_CMD: {
            cmd.Show();
        }
            break;
        }
    }


    std::string toString() {
        char str[30];
        switch (type) {
        case TYPE_U8:
            sprintf(str,"%u",data.u8);
            break;
        case TYPE_S8:
            sprintf(str,"%d",data.s8);
            break;
        case TYPE_U16:
            sprintf(str,"%u",data.u16);
            break;
        case TYPE_S16:
            sprintf(str,"%d",data.s16);
            break;
        case TYPE_U32:
            sprintf(str,"%u",data.u32);
            break;
        case TYPE_S32:
            sprintf(str,"%d",data.s32);
            break;
        case TYPE_U64:
            sprintf(str,"%" PRIu64, data.u64);
            break;
        case TYPE_S64:
            sprintf(str,"%" PRId64, data.s64);
            break;
        case TYPE_F32:
            sprintf(str,"%0.3f",data.f32);
            break;
        case TYPE_F64:
            sprintf(str,"%0.3f",data.f64);
            break;
        case TYPE_CMD: {
            return cmd.toString();
        }
            break;
        }
        return str;
    }


    TYPE_DEFINE type;
    union Udata{
        Udata() {
            s64 = 0;
        }
        ~Udata() {

        }

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
        char*  str;
        char   buf[8];
        void *pointer;
    };
    Udata data;
    CmdValue cmd;

    OpValue *next;

private:
    void release() {
        if (next) {
            delete next;
            next = NULL;
        }
        if (type == TYPE_STR){
            if (data.str)
                free(data.str);
            data.str = NULL;
        }
//        if(type == TYPE_CMD) {
//            if (data.cmd)
//                delete data.cmd;
//            data.cmd=NULL;
//        }
        type = TYPE_NONE;

    }

    void copydata(const OpValue & value) {
        switch (value.type) {
        case TYPE_U8:
            data.u8 = value.data.u8;
            break;
        case TYPE_S8:
            data.s8 = value.data.s8;
            break;
        case TYPE_U16:
            data.u16 = value.data.u16;
            break;
        case TYPE_S16:
            data.s16 = value.data.s16;
            break;
        case TYPE_U32:
            data.u32 = value.data.u32;
            break;
        case TYPE_S32:
            data.s32 = value.data.s32;
            break;
        case TYPE_U64:
            data.u64 = value.data.u64;
            break;
        case TYPE_S64:
            data.s64 = value.data.s64;
            break;
        case TYPE_F32:
            data.f32 = value.data.f32;
            break;
        case TYPE_F64:
            data.f64 = value.data.f64;
            break;
        case TYPE_STR:{
            if (data.str)
                free(data.str);
            int len = strlen(value.data.str);
            data.str = (char*)malloc(len+1);
            strcpy(data.str,value.data.str);
            data.str[len] = '\0';
            break;
        }
        case TYPE_POINTER:
            data.pointer = value.data.pointer;
            break;
        case TYPE_CMD: {
            cmd = value.cmd;
        }
            break;
        }

    }

    OpValue *copynext(OpValue *p) {
        if (!p)
            return NULL;
        OpValue *b = new OpValue();
        b->type = p->type;
        b->copydata(*p);
        b->next = copynext(p->next);
        return b;
    }
};

#endif
