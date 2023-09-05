/*************************************************************************
	> File Name: opitem.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月23日 星期五 15时01分34秒
 ************************************************************************/
#ifndef __OPITEM_H__
#define __OPITEM_H__

#include <stdio.h>
#include <defines.h>
#include <string>
#include <stack>
#include <vector>
#include <queue>
#include <memory>
#include <utils/logger.h>
#include "operator.h"
#include "data.h"

using namespace std;

typedef enum
{
    OP_NONE =0,
    OP_NUM ,
    OP_BASE,//基本运算符，+-×/,&,^ 运算符
    OP_VAR,
    OP_FUNC,
    OP_KEYFUNC,  //关键函数,ditch,fill
    OP_BRACE,
    OP_BRACKET, /* 中括号 */
    OP_PARENTHESS /* 小括号 */
}OP_TYPE;

class OpItem
{
public:
    OpItem(const char ch);

    OpItem(const string& func, const char prefix = 0);

    OpItem(const OpItem &item);

    OpItem();

    virtual ~OpItem();

    OpItem& operator=(const OpItem& item);

    static bool isDigtal(const char *var);

    static bool isKeyWords(const string &func);
    static bool isFormatChar(const char c);
    static bool isSupport(const char c,int np);

    /*外 优先级 ,数字优先级设为-1 */
    int priority();

    void show();

    string showStr();
    string leftStr();
    string rightStr();

    union OpData{
        OpData(){}
        ~OpData(){}
        int8_t ch;
        char var[MAX_VAR_LEN];
    };

    OpValue digit;
    OpData op;
    OP_TYPE     type;
    int32_t         params;/* 参数个数 */

private:
    void release();
    void copyitem(const OpItem &item);
private:
};

class Express;
typedef shared_ptr<Express> tExpress;

class Operator;

class Express
{
public:
    Express(const OpItem &op);
    virtual ~Express();

    void Insert(tExpress expr);
    void Add(tExpress expr);
    tExpress ChildAt(int index);

    int32_t ChildrenSize();

    OP_TYPE Type();

    inline OpItem &Value();
    inline char *VarName();
    inline char VarPrefix();
    std::string exprStr();
    std::string funcName();
    OpValue GenCmd(Operator& iOperator);

    OpValue ParseCmd(Operator& iOperator,
                     int32_t pos = 0,
                     const CmdValue &recvCmd = CmdValue::Null());
    TYPE_DEFINE ResultType();

private:
    OpItem    opItem;
    vector<tExpress> children;
};


#endif //__OPITEM_H__
