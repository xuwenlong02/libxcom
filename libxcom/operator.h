/*************************************************************************
	> File Name: operator.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月06日 星期二 12时42分09秒
 ************************************************************************/
#ifndef __OPERATOR_H__
#define __OPERATOR_H__

#include<stdio.h>
#include <string>
#include <defines.h>
#include <vector>
#include <stack>
#include <map>
#include <utils/logger.h>
#include "opitem.h"
#include "data.h"

using namespace std;

class Operator
{
public:
    Operator(OpInterface *interface);

    virtual ~Operator();

    OpInterface *Interface();
    void SetInterface(OpInterface *iface);
    OpValue operator[](const char *var);
    void SetVar(const char* var,const OpValue& v);
    void clearOut();

    /*
     * 命令生产
     */
    int32_t InsertCmd(int index,const OpValue& value);
    OpValue GetCmd(int32_t start = 0, int32_t length = -1);
    int32_t     GetLength();
    /* 必须按顺序添加参数 */
    void AddParam(OpValue value);
    OpValue GetParam(int32_t index);

    /* 相关计算 */
    tExpress FindDefinition(const string &def);
    OpValue  VarCall(const char *var,
                     int32_t pos = 0,
                     const CmdValue &cmdValue = CmdValue::Null());
    OpValue  FunctionCall(const char *func,
                          int32_t pos = 0,
                          const CmdValue &recvCmd = CmdValue::Null());

    OpValue EncodeCalc(const char *func,const CmdValue &recvCmd);

    OpValue CmdCalc(const char *func, OpValue &data);

    OpValue KeyFunctionCall(const char *keyfunc,int32_t byte = 0,
                            const CmdValue& cmdValue = CmdValue::Null());
    OpValue  BaseCall(const char c,OpValue &v1,OpValue &v2);
    OpValue SingleCall(const char c,OpValue &v);
    static TYPE_DEFINE VarType(const char *var);
    static TYPE_DEFINE FuncType(const char *func);
    static TYPE_DEFINE CalcType(const char *func);

private:

    class OperatorData
    {
    public:
        OperatorData() {

        }
        vector<OpValue>     opResult;

    };
    OperatorData &topData();

    class OperatorParam
    {
    public:
        OperatorParam() {}
        std::vector<OpValue>  opParams;
    };
    OperatorParam &topParam();

    OpValue             m_opDitch;

public:
    enum {AUTO_CMD = 0,AUTO_PARAM};
    class AutoOperator {
    public:

        /* mode:0直接入栈，1延迟入栈 */
        AutoOperator(Operator& op,uint32_t type,uint32_t mode = 0):
            opData(op),
            stacktype(type),
            stackmode(mode),
            push(mode == 0){
            if (mode == 0)
                opData.PushStack(stacktype);
        }

        void AddList(const OpValue &value) {
            templist.push_back(value);
        }

        int32_t StackAll() {
            opData.PushStack(stacktype);
            opData.AddtoStack(stacktype,templist);
            push = true;
            return 0;
        }

        virtual ~AutoOperator() {
            if (push)
                opData.PopStack(stacktype);
        }

    private:
        Operator& opData;
        bool      push;
        uint32_t       stacktype;
        uint32_t       stackmode;
        vector<OpValue> templist;
    };

    void PushStack(uint32_t type);
    void PopStack(uint32_t type);
    void AddtoStack(uint32_t type,const vector<OpValue> &list);
private:
    OpInterface *          m_pOperatorInterface;
    map<string,OpValue>    m_mapOut;
    stack<OperatorData>    m_stackData;
    stack<OperatorParam>   m_stackParam;
};

#endif //__OPERATOR_H__
