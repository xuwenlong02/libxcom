/*************************************************************************
	> File Name: dataitem.cpp
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月24日 星期六 16时00分30秒
 ************************************************************************/
#include "dataitem.h"
#include <stdio.h>
#include <utils/custom.h>
#include "tempbuf.h"

DataItem::DataItem(const ID_DEV &devId, const std::string& templ):
    m_iDevAddr(devId),
    m_iOperator(this) {
    m_template.Parse(templ);
}

DataItem::~DataItem()
{
    m_listValues.clear();
    m_listExprs.clear();
}

bool DataItem::IsEmpty()
{
    if (m_strname.empty() ||
            m_listExprs.empty() ||
            m_listValues.empty() ||
            m_tDataExpr == nullptr)
        return true;
    return false;
}

void DataItem::SetAddr(ID_DEV &devId)
{
    m_iDevAddr = devId;
}

void DataItem::SetRegLength(int32_t reglen)
{
    m_reglen = reglen;
}

void DataItem::SetRegAddr(uint16_t reg)
{
    m_linkReg = reg;
}

void DataItem::ParseParams(const vector<string> &params)
{
    m_listExprs.clear();
    m_listValues.clear();
    m_iOperator.clearOut();
    for(int i = 0;i<params.size();i++) {
#if 0
        vector<string> list =
                utils::Split(params[i]," ");
        string paralist;
        string paras;
        for (int j = 0;j<list.size();j++) {
            paras += list.at(j);
            if (j != list.size()-1)
                paras += " ";
        }

        if (list.size() > 1)
            paralist +="["+paras+"]";
        else
            paralist += paras;
#endif
        tExpress expr = wrapExpress(params[i]);
        if (expr == nullptr)
            return;

//        Operator iOperator(this);
//        iOperator.PushStack(Operator::AUTO_PARAM);
        m_listValues.push_back(expr->GenCmd(m_iOperator));
        m_listExprs.push_back(expr);
    }
}

vector<OpValue> DataItem::ParseString(const string &str)
{
    vector<OpValue> listValues;
    Operator iOp(NULL);
    vector<string> params = utils::Split(str,";");
    for(int i = 0;i<params.size();i++) {
        vector<string> list =
                utils::Split(params[i],",");
        string paralist;
        string paras;
        for (int j = 0;j<list.size();j++) {
            paras += list.at(j);
            if (j != list.size()-1)
                paras += " ";
        }

        if (list.size() > 1)
            paralist +="["+paras+"]";
        else
            paralist += paras;

        tExpress expr = wrapExpress(paralist);
        if (expr == nullptr)
            return listValues;
        listValues.push_back(expr->GenCmd(iOp));
    }
    return listValues;
}

void DataItem::CalcParams(const vector<OpValue> &params)
{
    m_iOperator.clearOut();
    m_listValues.clear();

    ParamsLock palock(m_iOperator);
    m_iOperator.AddtoStack(Operator::AUTO_PARAM,params);
    for (vector<tExpress>::iterator iter = m_listExprs.begin();iter !=
         m_listExprs.end();iter++) {
        tExpress expr = *iter;
        if (expr == nullptr) {
            LOG_ERR("params expr is null");
            m_listValues.push_back(OpValue::Null());
            continue;
        }
        m_listValues.push_back(expr->GenCmd(m_iOperator));
    }
}

void DataItem::ParseDataExpr(const string &expr, float rate)
{
    m_fRate = rate;
    m_tDataExpr = wrapExpress(expr);
}

tExpress DataItem::LocateFunction(const string &def, int funcId)
{
    if (!m_template.IsParse())
        return nullptr;
    return m_template.LocateFunction(def.c_str(), funcId);
}

const ID_DEV &DataItem::DevAddr()
{
    return m_iDevAddr;
}

RECV_RESULT DataItem::ParseCmd(OpValue& result,const CmdValue &recvCmd, const string &func, int32_t funcId)
{
    if (recvCmd.IsEmpty())
        return RECV_NOT_AVAILIABLE;

    ParamsLock palock(m_iOperator);
    m_iOperator.AddtoStack(Operator::AUTO_PARAM,m_listValues);
    tExpress expr = LocateFunction(func,funcId);

    if (expr == nullptr) {
        LOG_ERR("express is invalid");
        return RECV_NOT_AVAILIABLE;
    }

     result = expr->ParseCmd(m_iOperator,0,recvCmd);

    if (result.IsEmpty())
        return RECV_NOT_AVAILIABLE;
    //校验
    OpValue chk = m_iOperator["chk"];
    if (chk.IsEmpty())
        return RECV_COMPLETE;
    if (chk.Integer() == 0)
        return RECV_NOT_AVAILIABLE;
    return RECV_COMPLETE;
}

OpValue DataItem::GenerateCmd(const string &func)
{
    ParamsLock palock(m_iOperator);
    m_iOperator.AddtoStack(Operator::AUTO_PARAM, m_listValues);
    tExpress expr = LocateFunction(func, -1);
    if (expr == nullptr) {
        LOG_ERR("express is invalid");
        return CmdValue::Null();

    }
    return expr->GenCmd(m_iOperator);
}

RECV_RESULT DataItem::ParseRecvCmd(
        const CmdValue &recvCmd,
        const std::string& func,
        int32_t funcId) {
    OpValue result;
    return ParseCmd(result, recvCmd, func, funcId);
}

OpValue DataItem::Result()
{
    tExpress expr = m_tDataExpr;
    if (expr == nullptr)
        return OpValue::Null();
#ifdef LOCAL_TEST
        return 23.77;
#endif
    OpValue result = expr->ParseCmd(m_iOperator);
    return RateCall(result,m_fRate);
}

void DataItem::ResetCmd()
{
    m_iOperator.clearOut();
}

TYPE_DEFINE DataItem::DataType()
{
    if (!floatcmp(m_fRate,1))
        return TYPE_F32;
    if (m_tDataExpr == nullptr)
        return TYPE_CMD;
    return m_tDataExpr->ResultType();
}

OpValue DataItem::FuncCall()
{
    ParamsLock palock(m_iOperator);
    m_iOperator.AddtoStack(Operator::AUTO_PARAM,m_listValues);

    tExpress expr = m_tDataExpr;
    if (expr == nullptr)
        return OpValue::Null();
    OpValue result = expr->ParseCmd(m_iOperator);
    if (result.IsEmpty())
        return result;
    return RateCall(result,m_fRate);
}

OpValue DataItem::RateCall(OpValue &result,float rate)
{
    float f = rate;
    switch(result.type) {
    case TYPE_S8:
    case TYPE_U8:
    case TYPE_S16:
    case TYPE_U16:
    case TYPE_S32:
    case TYPE_U32:
    case TYPE_S64:
    case TYPE_U64:
    {
        if (floatcmp(f,1))
            return result;
        else
            return result/f;
    }
    case TYPE_F32:
    case TYPE_F64:
        return result/f;
    default:
        return result/f;
    }
}

tExpress DataItem::wrapExpress(const string &str)
{
    TempBuf tempBuf(str);
    int32_t pos = 0;
    TempBuf::RESULT res = tempBuf.Parse(pos,true);
    if ((res != TempBuf::OK && res != TempBuf::FIN))
        return NULL;
    return tempBuf.ResultExpress();
}

const int32_t DataItem::RegLength()
{
    return m_reglen;
}

const uint16_t DataItem::RegAddr()
{
    return m_linkReg;
}
