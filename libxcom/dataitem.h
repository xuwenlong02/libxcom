/*************************************************************************
	> File Name: dataitem.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月24日 星期六 16时00分52秒
 ************************************************************************/
#ifndef __DATAITEM_H__
#define __DATAITEM_H__

#include <stdio.h>
#include <string>
#include <memory>
#include "operator.h"
#include "opitem.h"
#include "template.h"

using namespace std;
class Operator;
class DataItem;
typedef shared_ptr<DataItem> tDataItem;

class DataItem:public OpInterface
{
public:
    DataItem(const ID_DEV &devId, const std::string& templ);
    virtual ~DataItem();

    bool IsEmpty();

    /*
     * 解析表达式
     */
    virtual void SetAddr(ID_DEV &devId);
    virtual void SetRegLength(int32_t reglen);
    virtual void SetRegAddr(uint16_t reg);
    virtual void ParseParams(
            const vector<string>& params);

    static vector<OpValue> ParseString(
            const string &str);
    virtual void CalcParams(const vector<OpValue>& params);
    virtual void ParseDataExpr(const string& expr,float rate);

    //采集
    virtual tExpress LocateFunction(const string &def, int funcId = -1);
    virtual const ID_DEV &DevAddr();
    virtual const int32_t RegLength();
    virtual const uint16_t RegAddr();
    virtual OpValue GetVar(const std::string& var) {
        return m_iOperator[var.c_str()];
    }
    void SetVar(const std::string& var,const OpValue& v) {
        m_iOperator.SetVar(var.c_str(),v);
    }

    virtual OpValue Result();
    virtual TYPE_DEFINE DataType();

    OpValue FuncCall();
    OpValue RateCall(OpValue &result, float rate);
    OpValue GenerateNext();

    RECV_RESULT ParseRecvCmd(
        const CmdValue &recvCmd,
        const std::string& func,
        int32_t funcId = -1);

    RECV_RESULT ParseCmd(OpValue& result,const CmdValue &recvCmd,const string& func,
            int32_t funcId = -1);

    OpValue GenerateCmd(const string &func);

    void ResetCmd();
private:
    static tExpress wrapExpress(const string &str);

    class ParamsLock
    {
      public:
        ParamsLock(Operator &iop_):
            iop(iop_) {
            iop.PushStack(Operator::AUTO_PARAM);
        }
        ~ParamsLock() {
            iop.PopStack(Operator::AUTO_PARAM);
        }
    private:
        Operator &iop;
    };
private:
    vector<tExpress>  m_listExprs;
    vector<OpValue>   m_listValues;

    Operator    m_iOperator;
    Template    m_template;
    ID_DEV      m_iDevAddr;
    std::string m_strname;
    std::string m_strdot;
    int32_t         m_reglen;
    tExpress    m_tDataExpr;
    float         m_fRate;
    uint16_t         m_linkReg;
};

#endif
