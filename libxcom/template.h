/*************************************************************************
	> File Name: configuration.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月05日 星期一 15时29分59秒
 ************************************************************************/

#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <stdio.h>
#include <stack>
#include <string>
#include <vector>
#include <utils/lock.h>
#include <map>
#include "data.h"
#include "operator.h"

using namespace std;

typedef struct{
    bool success;
    std::string func;
    tExpress expr;
}ExprItem;

class Template
{
public:
    Template(int32_t model);
    Template();
    virtual ~Template();

    bool IsParse();
    bool Parse();
    bool Parse(const string &temp);
    string StyleParse(const string &temp, vector<ExprItem> &listfuncs);
    void ClearAll();

    bool IsEqual(int32_t model);

    virtual tExpress LocateFunction(const char* def, int funcId = -1);

    bool    UpdateTemplate();
private:

private:
    map<std::string,tExpress> m_mapFunctions;
    Lock                      m_iLock;
    int32_t                       m_model;
    std::string               m_strtemp;
};

#endif //__CONFIGURATION_H__
