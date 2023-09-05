/*************************************************************************
	> File Name: tempbuf.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2019年04月24日 星期三 15时35分31秒
 ************************************************************************/
#ifndef __TEMPBUF_H__
#define __TEMPBUF_H__
#include <defines.h>
#include <vector>
#include <stack>
#include <stdio.h>
#include <string>
#include <map>
#include "opitem.h"

using namespace std;
#define WORD_BY_WORD 1

class TempBuf
{
public:
    TempBuf(const string &temp,bool style = false);

    //UNFIN:无需处理项
    typedef enum {OK = 0,UNFIN,FIN,ERR}RESULT;

    RESULT Parse(int32_t &pos,
            bool onlyfunc = false, bool sub = false);
    RESULT SubParse(int32_t &pos);
    const string &FuncName();
    const int FuncNum();
    tExpress ResultExpress();

    string StyleGrammar(int32_t &pos);
    string GetStyle(RESULT res);
private:
    void init();
    string wordWrap(const string &str,int &pos, bool &t);
    RESULT parseDCh(const char ch);
    RESULT parseDef(const string &def);
#if WORD_BY_WORD
    RESULT parseCh(const char c, int32_t &pos);
    RESULT parseFunc(const string &func, int32_t &pos);
    RESULT popCh(const char ch);
    RESULT composeExpr(const char ch,int np);
#endif

    int left_priority(const char ch);
    int right_priority(const char ch);
    int topNum(int add, char c=' ');
    char topChar();
    char topOp();
    tExpress subExpress(const OpItem &item);
    //style string
    string wordWrapStyle(const string &str);
    string styleStr(bool isdef, bool isterm, const string &str);
    string setStyle(const string& str,bool span = true,const string &color = "black");
    string setErrStyle(const string& str);
private:
#if WORD_BY_WORD
    const string &tempstr;
    stack<tExpress> exprlist;//结果
    typedef struct __ch
    {
        char ch;
        int num;
    }chm;
    stack<chm> oplist;//运算符,同palist
    stack<char> chlist;
    bool   isNumber;
//    stack<char> oplist;
    bool doStyle;
    vector<string> styleGrammar;
#endif
    string  funcname;
    int     funcnum;
    bool    isdef;
};

#endif
