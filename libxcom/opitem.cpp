/*************************************************************************
	> File Name: opitem.cpp
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月23日 星期五 15时01分24秒
 ************************************************************************/

#include <stdio.h>
#include <string>
#include <defines.h>
#include <stack>
#include <vector>
#include <queue>
#include <utils/logger.h>
#include <string.h>
#include <stdlib.h>
#include <utils/custom.h>
#include "opitem.h"

OpItem::OpItem(const char ch) {
    type = OP_NONE;
    params = 0;
    switch(ch) {
    case '(':
    case ')':
        type = OP_PARENTHESS;
        break;
    case '[':
    case ']':
        type = OP_BRACKET;
        break;
    case '{':
    case '}':
        type = OP_BRACE;
        break;
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '&':
    case '^':
        type = OP_BASE;
        break;
    default:
        break;
    }

    op.ch = ch;
}

OpItem::OpItem(const string &func,const char prefix) {
#define STREQUAL(str) (func==str)

    type = OP_NONE;
    params = 0;
    if (prefix == 0) {
        bool isfloat = false;
        if (utils::isDecimal(func.c_str(),isfloat)) {
            type = OP_NUM;
            if (isfloat) {
                digit =atof(func.c_str());
            }
            else {
                digit =atoi(func.c_str());
            }
        }
        else if (utils::isHexDigit(func.c_str())) {
            type = OP_NUM;
            uint32_t v = utils::strtohex(func.c_str());
            if (v<=255)
                digit = (uint8_t)v;
            else
                digit = v;
        }
    }
    else {
        if (STREQUAL("pos") ||
                STREQUAL("len") ||
                STREQUAL("addr"))
            type = OP_VAR;
        else if(STREQUAL("chk")  ||
                STREQUAL("ref"))
            type = OP_FUNC;
        else if(STREQUAL("ditch") || STREQUAL("fill"))
            type = OP_KEYFUNC;
        else if (utils::isInteger(func.c_str()))
            type = OP_VAR;
        else if (prefix == '&')
            type = OP_VAR;
        else
            type = OP_FUNC;
        memset(op.var,0,MAX_VAR_LEN);
        op.var[0] = prefix;
        strncpy(&op.var[1],func.c_str(),MAX_VAR_LEN-1);
    }


#undef STREQUAL
}

OpItem::OpItem(const OpItem &item)
{
    type = OP_NONE;
    copyitem(item);
}

OpItem::OpItem()
{
    params = 0;
    type = OP_NONE;
}

OpItem::~OpItem()
{
//    if (type == OP_VAR ||type == OP_FUNC ||type == OP_KEYFUNC)
//        free(op.var);
    release();
}

OpItem& OpItem::operator =(const OpItem &item)
{
    release();
    copyitem(item);
    return *this;
}

bool OpItem::isDigtal(const char *var)
{
    bool isfloat = false;
    if (utils::isDecimal(var,isfloat)) {
        return true;
    }
    else if (utils::isHexDigit(var)) {
        return true;
    }
    return false;
}

bool OpItem::isKeyWords(const string &func)
{
#define STREQUAL(var) (func==var)
    if (STREQUAL("pos") ||
            STREQUAL("len") ||
            STREQUAL("addr"))
        return true;
    else if(STREQUAL("chk")  ||
            STREQUAL("ref"))
        return true;
    else if(STREQUAL("ditch") || STREQUAL("fill"))
        return true;
    else
        return false;
}

bool OpItem::isFormatChar(const char c)
{
    return (c=='{' || c == '}'||c=='[' || c == ']'||c == '(' || c == ')' ||
            c == '+' || c == '-' || c== '*' || c == '/' || c=='%' || c == '^'||c=='|'||
            c == '!' || c == '&' || c == ' ' || c == ',');
}

bool OpItem::isSupport(const char c, int np)
{
    if (c == '(' || c==')' || c== '{' ||
            c == '}'||
            c == '['||c==']')
        return true;
    if (np == 1) {
        if (c == '!'||c == '&'||c=='-')
            return true;
    }
    else if (np == 2) {
        if (c == '+' || c == '-' || c== '*' || c == '/' || c=='%' || c == '^'||c == '|'||c=='&')
            return true;
    }
    return false;
}

void OpItem::copyitem(const OpItem &item) {
    params = item.params;
    type = item.type;
    if (type == OP_VAR ||type == OP_FUNC || type == OP_KEYFUNC) {
        memset(op.var,0,MAX_VAR_LEN);
        strncpy(op.var,item.op.var,MAX_VAR_LEN);
    }
    else {
        if (type == OP_NUM) {
            digit = item.digit;
        }
        else
            op.ch = item.op.ch;
    }
}

/*外 优先级 ,数字优先级设为-1 */
int OpItem::priority() {
    switch (type) {
    case OP_BASE://基本运算符，+-×/等
        if (op.ch == '+' || op.ch == '-')
            return 4;
        else if (op.ch == '*' || op.ch == '/' || op.ch == '%')
            return 6;
        else if (op.ch == '&' || op.ch == '^')
            return 7;
    default:
        return 10;
    }
}

string OpItem::showStr()
{
    char buf[64] = {0};
    if (type == OP_NUM) {
        if (digit.IsInteger()) {
            if (digit.type == TYPE_U8 || digit.type == TYPE_CMD)
                sprintf(buf,"%02XH",(uint8_t)digit.Integer());
            else
                sprintf(buf,"%" PRId64,digit.Integer());
        }
        else {
            sprintf(buf,"%0.3f",digit.Float());
        }
    }
    else if (type == OP_BASE ||
             type == OP_BRACKET ||
             type == OP_BRACE ||
             type == OP_PARENTHESS) {
        if (op.ch == '('|| op.ch == ')')
            strcat(buf,"()");
        else if (op.ch == '['|| op.ch == ']')
            strcat(buf,"[]");
        else if (op.ch == '{'|| op.ch == '}')
            strcat(buf,"{}");
        else
            buf[0] = op.ch;
    }
    else if (type == OP_VAR ||
             type == OP_FUNC ||
             type == OP_KEYFUNC) {
        sprintf(buf, "%s", op.var);
    }
    else
        strcat(buf, "null");
    return buf;
}

string OpItem::leftStr()
{
    if (type == OP_BASE ||
             type == OP_BRACKET ||
             type == OP_BRACE ||
             type == OP_PARENTHESS ) {
        if (op.ch == '('|| op.ch == ')')
            return "(";
        else if (op.ch == '['|| op.ch == ']')
            return "[";
        else if (op.ch == '{'|| op.ch == '}')
            return "{";
        else
            return "";
    }
    else
        return showStr();
}

string OpItem::rightStr()
{
    if (type == OP_BASE ||
             type == OP_BRACKET ||
             type == OP_BRACE ||
             type == OP_PARENTHESS ) {
        if (op.ch == '('|| op.ch == ')')
            return ")";
        else if (op.ch == '['|| op.ch == ']')
            return "]";
        else if (op.ch == '{'|| op.ch == '}')
            return "}";
        else
            return "";
    }
    else
        return showStr();
}

void OpItem::release()
{
//    if (type == OP_NUM && (op.digit != NULL)) {
//        delete op.digit;
//        op.digit = NULL;
//    }
    type = OP_NONE;
}

Express::Express(const OpItem &op){
    opItem = op;
    children.clear();
}

Express::~Express()
{
    children.clear();
}

void Express::Insert(tExpress expr)
{
    if (expr == nullptr) {
        LOG_ERR("null expr");
        return;
    }
    children.insert(children.begin(),expr);
}

void Express::Add(tExpress expr)
{
    if (!expr) {
        LOG_ERR("null expr");
        return;
    }
    children.push_back(expr);
}

tExpress Express::ChildAt(int index)
{
    if (index < children.size()) {
        return children.at(index);
    }
    return NULL;
}

int32_t Express::ChildrenSize()
{
    return children.size();
}

OP_TYPE Express::Type()
{
    return opItem.type;
}

OpItem& Express::Value()
{
    return opItem;
}

char* Express::VarName()
{
    if (opItem.op.var[0] == '$' || opItem.op.var[0] == '&')
        return &opItem.op.var[1];
    else
        return opItem.op.var;
}

char Express::VarPrefix()
{
    return opItem.op.var[0];
}

string Express::exprStr()
{
    if (Type() == OP_NUM) {
        return opItem.showStr();
    }
    else if (Type() == OP_BASE) {
        tExpress child0 = ChildAt(0);
        tExpress child1 = ChildAt(1);

        if (ChildrenSize() == 2) {
            if (child0== nullptr || child1== nullptr) {
                return "null"+opItem.showStr()+"null";
            }
            string str;
            if (opItem.priority()>child0->opItem.priority()) {
                str += "("+child0->exprStr()+")";
            }
            else
                str +=child0->exprStr();
            str += opItem.showStr();
            if (opItem.priority()>child1->opItem.priority()) {
                str += "("+child1->exprStr()+")";
            }
            else
                str +=child1->exprStr();
            return str;
        }
        else if(ChildrenSize() == 1) {
            if (child0== nullptr) {
                return opItem.showStr()+"null";
            }
            string str = opItem.showStr();
            if (opItem.priority()>child0->opItem.priority()) {
                str += "("+child0->exprStr()+")";
            }
            else
                str +=child0->exprStr();
            return str;
        }
        else
        {
            return opItem.showStr();
        }
    }
    else if (Type() == OP_VAR) {
        return opItem.showStr();
    }
    else if (Type() == OP_FUNC || Type() == OP_KEYFUNC) {
        string str = opItem.showStr();
        if (ChildrenSize() == 0)
            return str;
        str += "(";
        for (int i = 0;i < children.size();i++) {
            if (i != 0)
                str += ",";
            str += children.at(i)->exprStr();
        }
        str += ")";
        return str;
    }
    else if (Type() == OP_BRACKET || Type() == OP_BRACE) {
        string str = opItem.leftStr();
        for (int num = 0; num < ChildrenSize();num++) {
            if (num != 0) {
                if (Type() == OP_BRACKET)
                    str +=" ";
                else
                    str +=",";
            }
            tExpress childk = ChildAt(num);
            if (childk== nullptr) {
                str += "null";
            }
            else
                str += childk->exprStr();
        }
        return str+=opItem.rightStr();
    }
    else {
        return "null";
    }
}

string Express::funcName()
{
     //关键函数,ditch,fill)
    return opItem.showStr();
}

OpValue Express::GenCmd(Operator &iOperator)
{
    return ParseCmd(iOperator);
}

OpValue Express::ParseCmd(Operator &iOperator,int32_t pos,const CmdValue &recvCmd)
{
    if (Type() == OP_NUM) {
        return Value().digit;
    }
    else if (Type() == OP_BASE) {
        OpValue v1;
        OpValue v2 ;
        tExpress child0 = ChildAt(0);
        tExpress child1 = ChildAt(1);

        do{
            if (ChildrenSize() == 2) {
                if (child0== nullptr || child1== nullptr) {
                    LOG_ERR("child 0 & 1 has been deleted");
                    return OpValue::Null();
                }
                v1 = child0->ParseCmd(iOperator,pos);
                v2 = child1->ParseCmd(iOperator,pos);
                return iOperator.BaseCall(Value().op.ch,v1,v2);
            }
            else if(ChildrenSize() == 1) {
                if (child0== nullptr) {
                    LOG_ERR("child 0 has been deleted");
                    return OpValue::Null();
                }
                v1 = 0;
                v2 = child0->ParseCmd(iOperator,pos);
                return iOperator.SingleCall(Value().op.ch,v2);
            }
            else
            {
                LOG_ERR("%c:error params count",Value().op.ch);
                return OpValue::Null();
            }
        }while(0);
    }
    else if (Type() == OP_VAR) {
        if (VarPrefix() == '&')
            return VarName();
        else if (utils::isInteger(VarName())) {
            int32_t v = atoi(VarName());
            if (v == 0)
                return iOperator.GetParam(0);
            return iOperator.GetParam(v-1);
        }
        else{
            tExpress expr = iOperator.FindDefinition(VarName());
            if (expr != nullptr) {
                return expr->ParseCmd(iOperator,pos,recvCmd);
            }
            else {
                return iOperator.VarCall(VarName(),pos,recvCmd);
            }
        }

    }
    else if (Type() == OP_FUNC) {
        Operator::AutoOperator autoOp(iOperator,Operator::AUTO_PARAM,1);
        for (int32_t i = 0;i < ChildrenSize();i++) {
            OpValue value;
            tExpress childi = ChildAt(i);
            if (childi == nullptr) {
                LOG_ERR("child %d has been deleted",i);
                return OpValue::Null();
            }
            if (childi->Type() != OP_KEYFUNC)
                value =
                    childi->ParseCmd(iOperator,pos);
            else
                value =
                    childi->ParseCmd(iOperator,pos,recvCmd);
            if (value.IsEmpty()){
                LOG_DEBUG("func %s:param %d caculate error",VarName(),i);
                return value;
            }
            autoOp.AddList(value);
        }

        autoOp.StackAll();

        tExpress expr = iOperator.FindDefinition(VarName());
        if (expr != nullptr) {
            return expr->ParseCmd(iOperator,pos,recvCmd);
        }
        else
            return iOperator.FunctionCall(VarName(),pos,recvCmd);
    }
    else if (Type() == OP_KEYFUNC ) {
        if ( ChildrenSize() < 1) {
            LOG_ERR("keyfunc parameter is less");
            return OpValue::Null();
        }

        OpValue v1;
        tExpress child0 = ChildAt(0);
        tExpress child1 = ChildAt(1);

        if (child0== nullptr ) {
            LOG_ERR("child 0 has been deleted");
            return OpValue::Null();
        }
        v1 =child0->ParseCmd(iOperator,pos);

        if (!v1.IsInteger()) {
            LOG_ERR("ditch first parameter must be a integer");
            return OpValue::Null();
        }

        Operator::AutoOperator autoOp(iOperator,Operator::AUTO_PARAM);
        if (!strcmp(VarName(),"ditch")) {
            OpValue get = iOperator.KeyFunctionCall(VarName(),v1.Integer(),recvCmd);
            iOperator.AddParam(get);
            if (ChildrenSize() > 1 ) {
                if (child1 == nullptr) {
                    LOG_ERR("child 1 has been deleted");
                    return OpValue::Null();
                }
                child1->ParseCmd(iOperator,pos,recvCmd);
            }
            return get;
        }
        else if (!strcmp(VarName(),"fill")) {
            OpValue fill;
            if (ChildrenSize() > 1 ) {
                if (child1== nullptr) {
                    LOG_ERR("child 1 has been deleted");
                    return OpValue::Null();
                }
                fill = child1->ParseCmd(iOperator,pos,recvCmd);
            }
            iOperator.AddParam(fill);
            if (ChildrenSize() >= 3) {
                tExpress child2 = ChildAt(2);
                if (child2== nullptr) {
                    LOG_ERR("child 2 has been deleted");
                    return OpValue::Null();
                }
                OpValue v2 =
                        child2->ParseCmd(iOperator,pos);
                if (v2.IsInteger())
                    iOperator.AddParam(v2);
            }
            return iOperator.KeyFunctionCall(VarName(),v1.Integer(),recvCmd);
        }

        return OpValue::Null();
    }
    else if (Type() == OP_BRACKET) {
        int32_t start = 0;
        Operator::AutoOperator autoOp(iOperator,Operator::AUTO_CMD);
        CmdValue recv = recvCmd;
        bool exit = false;
        /*
         * [] 表达式逆向运算
         *
         */
        for (int num = 0; num < ChildrenSize();num++) {
            OpValue value;
            tExpress childk = ChildAt(num);
            if (childk == nullptr) {
                LOG_ERR("child %d has been deleted",num);
                return OpValue::Null();
            }

            value =childk->ParseCmd(iOperator,start,recv);

            if (value.IsEmpty()) {
                LOG_DEBUG("[]:param %d caculate error",num);
                return OpValue::Null();
            }

            /* [] 逆向计算 */
            if (!recv.IsEmpty()) {
                CmdValue cmdValue = value.GenCmd();
                int32_t shift = recv.Contains(cmdValue);

                if (shift == -1) {
                    LOG_DEBUG("[]:param %d not match",num);
                    return OpValue::Null();
                }
                if (num != 0) {
                    if (shift == cmdValue.Length())
                        recv.TrimLeft(shift);
                }
                else
                    recv.TrimLeft(shift);
                if (recv.IsEmpty())
                    exit = true;
            }
            start += iOperator.InsertCmd(num,value);
            if (exit)
                break;

        }
        OpValue result = iOperator.GetCmd();
        return result;
    }
    else if (Type() == OP_BRACE) {
        OpValue result;
        for (int32_t i = 0;i < ChildrenSize();i++) {
            OpValue value;
            tExpress childk = ChildAt(i);

            if (childk == nullptr) {
                LOG_ERR("child %d has been deleted",i);
                return OpValue::Null();
            }

            value = childk->ParseCmd(iOperator,pos,recvCmd);
            if (value.IsEmpty()){
                LOG_DEBUG("func %s:param %d caculate error",VarName(),i);
                return result;
            }
            if (result.IsEmpty())
                result = value;
            else {
                result.AllocNext(value);
            }
        }
        return result;
    }
    else {
        LOG_ERR("error operator %u",Type());
        return OpValue::Null();
    }
}

TYPE_DEFINE Express::ResultType()
{
    if (Type() == OP_NUM) {
        return Value().digit.type;
    }
    else if (Type() == OP_BASE) {
        tExpress child0 = ChildAt(0);
        tExpress child1 = ChildAt(1);

        TYPE_DEFINE t1 = child0 == nullptr?child0->ResultType():TYPE_S32;
        TYPE_DEFINE t2 = child1 == nullptr?child1->ResultType():TYPE_S32;
        return t1>t2?t1:t2;
    }
    else if (Type() == OP_VAR) {
        if (VarPrefix() == '&')
            return TYPE_S32;
        else if (utils::isInteger(VarName())) {
            return TYPE_CMD;
        }
        else{
            return Operator::VarType(VarName());
        }
    }
    else if (Type() == OP_FUNC) {
        return Operator::FuncType(VarName());
    }
    else if (Type() == OP_KEYFUNC ) {

        return TYPE_CMD;
    }
    else if (Type() == OP_BRACKET) {
        return TYPE_CMD;
    }
    else if (Type() == OP_BRACE) {
        return TYPE_CMD;
    }
    else {
        LOG_ERR("error operator %u",Type());
        return TYPE_NONE;
    }
}
