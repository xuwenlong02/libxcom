/*************************************************************************
	> File Name: configuration.cpp
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月05日 星期一 15时29分52秒
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iterator>
#include <stdlib.h>
#include <dlfcn.h>
#include <map>

#include <defines.h>
#include <utils/logger.h>
#include <utils/custom.h>
#include "data.h"
#include "tempbuf.h"
#include "template.h"

Template::Template(int32_t model):
    m_iLock(true),
    m_model(model){
    //    Parse(file.c_str(),NULL);
}

Template::Template():
    m_iLock(true),
    m_model(-1)
{
}

Template::~Template()
{
    ClearAll();
}

bool Template::IsParse()
{
    AutoLock autolock(m_iLock);
    return !m_mapFunctions.empty();
}

bool Template::Parse() 
{
    return Parse(m_strtemp);
}

bool Template::Parse(const string &temp)
{
    AutoLock autolock(m_iLock);
    if (temp.empty())
        return false;
    m_strtemp = temp;

    int pos = 0;
    TempBuf tempBuf(temp);
    while (pos < temp.length()) {
        TempBuf::RESULT res = tempBuf.Parse(pos);
        if (!(res == TempBuf::OK || res == TempBuf::FIN))
            return false;
        int num = tempBuf.FuncNum();
        tExpress expr = tempBuf.ResultExpress();
        if (expr == nullptr)
            return false;
        if (expr->Type() == OP_BRACE) {
            if (!((expr->ChildrenSize() == num)||(num == -1))) {
                return false;
            }

            num = expr->ChildrenSize();
            for(int i = 0;i < num; i++) {
                char name[128] = {0};
                sprintf(name,"%s[%d]",tempBuf.FuncName().c_str(),i);
                m_mapFunctions.insert(
                            std::pair<std::string,tExpress>(
                                name,expr->ChildAt(i)));
            }
        }
        else {
            if (num != 1)
                return false;
            m_mapFunctions.insert(
                        std::pair<std::string,tExpress>(tempBuf.FuncName(),expr));
        }
    }
    return true;
}

string Template::StyleParse(const string &temp, vector<ExprItem> &listfuncs)
{
    int pos = 0;
    string outstr;
    TempBuf tempBuf(temp,true);

    while (pos < temp.length()) {
        TempBuf::RESULT res = tempBuf.Parse(pos);
        outstr += tempBuf.GetStyle(res);
        ExprItem exprItem;
        exprItem.success = res != TempBuf::ERR;
        exprItem.func = tempBuf.FuncName();
        exprItem.expr = tempBuf.ResultExpress();
        listfuncs.push_back(exprItem);
    }
    return outstr;
}

void Template::ClearAll()
{
    AutoLock autolock(m_iLock);
    m_mapFunctions.clear();
}

bool Template::IsEqual(int32_t model)
{
    return m_model == model;
}

tExpress Template::LocateFunction(const char *def,int funcId)
{
    AutoLock autolock(m_iLock);
    std::map<std::string,tExpress>::iterator iter;

    if (funcId == -1) {
        if ((iter = m_mapFunctions.find(def)) !=
                m_mapFunctions.end()) {
            return (tExpress)iter->second;
        }
    }
    else {
        char func[64] = {0};
        sprintf(func,"%s[%d]",def,funcId);
        if ((iter = m_mapFunctions.find(func)) !=
                m_mapFunctions.end()) {
            return (tExpress)iter->second;
        }

        if (funcId == 0) {
            if ((iter = m_mapFunctions.find(def)) !=
                    m_mapFunctions.end()) {
                return (tExpress)iter->second;
            }
        }
    }
    return NULL;
}

bool Template::UpdateTemplate()
{
    return Parse();
}

