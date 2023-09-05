/*************************************************************************
	> File Name: tempbuf.cpp
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2019年04月24日 星期三 15时35分20秒
 ************************************************************************/
#include "tempbuf.h"

TempBuf::TempBuf(const string &temp, bool style):
    tempstr(temp),
    doStyle(style),
    isNumber(false) {
    funcnum = 0;
    isdef = true;
}

TempBuf::RESULT TempBuf::Parse(int32_t &pos, bool onlyfunc,bool sub)
{
    TempBuf::RESULT res = UNFIN;
    init();
    isdef = onlyfunc?false:true;

    while (pos < tempstr.length()) {
        bool t = false;
        string wrap = wordWrap(tempstr,pos,t);
        if (doStyle)
            styleGrammar.push_back(styleStr(isdef,t,wrap));

        if (wrap[0] == '#') {
            int tp = tempstr.find_first_of('\n',pos);
            if (tp == string::npos) {
                wrap = tempstr.substr(pos,tempstr.length()-pos);
                pos = tempstr.length();
            }
            else{
                wrap = tempstr.substr(pos,tp-pos);
                pos = tp-1;
            }
            if (doStyle) {
                styleGrammar.pop_back();
                styleGrammar.push_back(styleStr(isdef,t,wrap));
            }
        }
        else if (wrap[0] == '\n') {
        }
        else if (isdef) {
            if (t) {
                res = parseDCh(wrap[0]=='\t'?' ':wrap[0]);
            }
            else {
                res = parseDef(wrap);
            }
        }
        else {
#if WORD_BY_WORD
            if (t) {
                res = parseCh(wrap[0]=='\t'?' ':wrap[0],pos);
                if (res == OK)
                    isNumber = false;
            }
            else {

                res = parseFunc(wrap,pos);
                if (res == OK) {
                    if (isNumber)
                        res = ERR;//不能有连续数字
                    else {
                        isNumber = true;
                        if (topOp() != '#')
                            topNum(1);
                    }
                }
            }
#endif
        }
        if (res == TempBuf::FIN) {
            if (!t) {
                pos -= wrap.length()-1;
                if (doStyle)
                    styleGrammar.pop_back();
            }
            if(ERR==popCh('d'))
                return ERR;
            return res;
        }
        else if (res == TempBuf::ERR) {
            if (!onlyfunc && !sub) {
                int tp = tempstr.find("def ",pos+1);
                if (tp == string::npos) {
                    wrap = tempstr.substr(pos+1,tempstr.length()-pos-1);
                    pos = tempstr.length();
                }
                else{
                    wrap = tempstr.substr(pos+1,tp-pos-1);
                    pos = tp-1;
                }
                if (doStyle) {
                    styleGrammar.push_back(wordWrapStyle(wrap));
                }
            }
            pos++;
            return res;
        }
        else{
            if(sub) {
                if (res == OK && topChar() == '#') {
                    if (ERR== popCh('d'))
                        return ERR;
                    return TempBuf::FIN;
                }
            }
        }
        ++pos;
    }

    if (topChar() == '#' ) {
        if (res == OK || res == UNFIN) {
            if (ERR==popCh('d'))
                return ERR;
            return FIN;
        }
        else
            return res;
    }
    else
        return ERR;
}

TempBuf::RESULT TempBuf::SubParse(int32_t &pos)
{
    return Parse(pos,true,true);
}

const string &TempBuf::FuncName()
{
    return funcname;
}

const int TempBuf::FuncNum()
{
    return funcnum;
}

tExpress TempBuf::ResultExpress()
{
    if (exprlist.empty())
        return NULL;
    if (exprlist.size() == 1) {
        tExpress expr = exprlist.top();
        exprlist.pop();
        return expr;
    }
    else {
        tExpress ex = std::make_shared<Express>(OpItem('['));
        while(!exprlist.empty()) {
            tExpress expr = exprlist.top();
            exprlist.pop();
            ex->Insert(expr);
        }
        return ex;
    }
}

string TempBuf::StyleGrammar(int32_t &pos)
{
    styleGrammar.clear();
    doStyle = true;
    TempBuf::RESULT res = Parse(pos);
    return GetStyle(res);
}

string TempBuf::GetStyle(TempBuf::RESULT res)
{
    string style;
    for (vector<string>::iterator iter = styleGrammar.begin();
         iter != styleGrammar.end();iter++) {
        if (res==ERR && iter+1 == styleGrammar.end()) {
            style += setErrStyle(*iter);
            break;
        }
        else
            style += *iter;
    }
    return style;
}

void TempBuf::init()
{
    while (!exprlist.empty())
        exprlist.pop();
//    funclist.clear();
    while(!chlist.empty())
        chlist.pop();
    while(!oplist.empty())
        oplist.pop();
    styleGrammar.clear();
    funcnum = 0;
    isNumber = false;
}

string TempBuf::wordWrap(const string &str, int &pos,bool &t)
{
    int orig = pos;
    t = false;
    for (;pos < str.length();pos++) {
        char c = str[pos];

        if (utils::isTerminator(c)) {
            if (pos == orig) {
                if (c == ' ' || c == '\t') {
                    int d = pos+1;
                    while (d < str.length()) {
                        if (str[d] != ' ' &&
                                str[d] != '\t') {
                            break;
                        }
                        pos = d++;
                    }
                }

                if (c != '$' && c != '&') {
                    t = true;
                    return str.substr(orig,pos-orig+1);
                }
            }
            else {
                if (c != '.')
                    return str.substr(orig,--pos-orig+1);
                string sub = str.substr(orig,pos-orig);

                if (!utils::isInteger(sub.c_str())) {
                    --pos;
                    return sub;
                }
            }
        }
    }
    return str.substr(orig,--pos-orig+1);
}

TempBuf::RESULT TempBuf::parseDCh(const char ch)
{
    if (ch == ' ' || ch == '\n') {
        return OK;
    }
    else if (ch == '[') {
        if (topChar() != 'd')
            return ERR;
//        chlist.push(ch);
        topNum(0,ch);
        funcnum = -1;
        return OK;
    }
    else if (ch == ']') {
        if (topChar() != '[')
            return ERR;
        topNum(-1);
        return OK;
    }
    else if (ch == ':') {
        if (topChar() != 'd')
            return ERR;
        topNum(-1);
        isdef = false;
        return OK;
    }
    else {
        return ERR;
    }
}

TempBuf::RESULT TempBuf::parseDef(const string &def)
{
    if (topChar() == '#') {
        if (!exprlist.empty()) {
            return FIN;
        }
        if (def == "def") {
            topNum(0,'d');
        }
        else {
            return ERR;
        }
    }
    else {
        if (topChar() == 'd') {
//            chlist.pop();
            funcnum = 1;
            funcname = def;
        }
        else if (topChar() == '[') {
            funcnum = utils::strtoint(def.c_str());
            if (funcnum <= 0)
                return ERR;
        }
        else
            return ERR;
    }
    return OK;
}

#if WORD_BY_WORD
TempBuf::RESULT TempBuf::parseCh(const char c,int32_t &pos)
{
    if (c == ' ') {
        if (topChar() != '[')
            return OK;
    }
    else if (c == ',') {
        if (topChar() != '(' && topChar() != '{')
            return ERR;
    }
    else if (!OpItem::isFormatChar(c))
        return ERR;

    if (ERR == popCh(c))
        return ERR;

    if (c == '(' || c == '[' ||c == '{') {
        topNum(0,c);
        return OK;
    }
    else if(c == ')') {
        if (topChar() != '(')
            return ERR;
        topNum(-1);
        topNum(1);//把底部加1
        return OK;
    }
    else if (c == '}') {
        if (topChar() != '{')
            return ERR;
        int pn = topNum(-1);
        if (composeExpr(c,pn) == ERR)
            return ERR;
        topNum(1);
        return OK;
    }
    else if(c == ']') {
        if (topChar() != '[')
            return ERR;
        int pn = topNum(-1);
        if (composeExpr(c,pn) == ERR)
            return ERR;
        topNum(1);
        return OK;
    }
    else if (c == '!') {//单运算符
        topNum(0,c);
    }
    else if (c == '+' || c == '-' || c=='*'||c =='/'||
             c=='%'||c=='^' || c == '|' || c=='&') {//双运算符
        if (!isNumber) {
            if (c != '-') {
                return ERR;
            }
            topNum(0,c);
            return OK;
        }
        topNum(0,c);
        topNum(1);
    }
    return OK;
}

TempBuf::RESULT TempBuf::parseFunc(const string &func,int32_t &pos)
{
    if (func[0] == '$') {
        if (func.length() == 1)
            return ERR;
        string fn = func.substr(1);
        if (OpItem::isDigtal(fn.c_str())) {//参数变量 $1,$2等等
            OpItem op(fn,'$');
            exprlist.push(std::make_shared<Express>(op));
            return OK;
        }
        else {//函数或变量 $addr,$data($1,05H)类似等等
            bool t;
            int32_t tp = pos+1;
            string wrap = wordWrap(tempstr,tp,t);

            if (wrap[0] == '(') {//函数或变量 $int($data)类似等等
                TempBuf buf(tempstr,doStyle);
                TempBuf::RESULT res = buf.SubParse(++pos);
                if (doStyle)
                    styleGrammar.push_back(buf.GetStyle(res));
                if (res != TempBuf::FIN) {
                    return res;
                }
                OpItem op(fn,'$');
                tExpress expr = buf.subExpress(op);
                exprlist.push(expr);
                return OK;
            }
            else {
                OpItem op(fn,'$');
                exprlist.push(std::make_shared<Express>(op));
                return OK;
            }
        }
    }
    else if (func[0] == '&') {
        if (func.length() == 1)
            return ERR;
        string fn = func.substr(1);
        if (OpItem::isDigtal(fn.c_str())) {//参数变量 4&5等与运算
            if (ERR== parseCh('&',pos))
                return ERR;
            OpItem op(fn);
            exprlist.push(std::make_shared<Express>(OpItem(op)));
            return OK;
        }
        else {//函数或变量 &dl 类似等等
            if (OpItem::isKeyWords(fn))
                return ERR;
            OpItem op(fn,'&');
            tExpress expr = std::make_shared<Express>(op);
            exprlist.push(expr);
            return OK;
        }
    }
    if (!OpItem::isDigtal(func.c_str())) {
        if (func == "def") {
            if (topChar() == '#') {
                if (ERR == popCh('d'))//请出全部
                    return ERR;
                return FIN;
            }
        }
        return ERR;
    }
    OpItem op(func.c_str(),false);
    exprlist.push(std::make_shared<Express>(op));
    return OK;
}

TempBuf::RESULT TempBuf::popCh(const char ch)
{
    char top;
    while ((top = topOp()) != '#') {
        if (left_priority(top) <= right_priority(ch))
            break;
        int pn = topNum(-1);

        //top?: +-*/%^ ] } ) 等运算,如果单符号运算？暂时不支持
        if (composeExpr(top,pn) == ERR)
            return ERR;
    }
    return TempBuf::OK;
}

TempBuf::RESULT TempBuf::composeExpr(const char ch, int np)
{
    if (!OpItem::isSupport(ch,np))
        return ERR;
    OpItem op(ch);
    tExpress expr = std::make_shared<Express>(op);
    for (int i = 0;i < np;i++) {
        if (exprlist.empty())
            return ERR;
        tExpress child = exprlist.top();
        exprlist.pop();
        expr->Insert(child);
    }
    exprlist.push(expr);
    return OK;
}

int TempBuf::left_priority(const char ch)
{
    if (ch == '+' || ch == '-')
        return 3;
    else if (ch == '*' || ch == '/' || ch == '%'||
             ch == '|')
        return 5;
    else if ( ch == '^')
        return 7;
    else if (ch == '!'||ch == '&')
        return 9;
    else if (ch == '(' || ch == '[' || ch == '{')
        return 1;
    else if (ch == ')' || ch == ']' || ch == '}')
        return 12;
    else
        return 0;
}

int TempBuf::right_priority(const char ch)
{
    if (ch == '+' || ch == '-')
        return 2;
    else if (ch == '*' || ch == '/' || ch == '%'||
             ch == '|')
        return 4;
    else if ( ch == '^')
        return 6;
    else if (ch == '!'||ch == '&')
        return 8;
    else if (ch == '(' || ch == '[' || ch == '{')
        return 12;
    else if (ch == ')' || ch == ']' || ch == '}')
        return 1;
    else if (ch == ',' || ch == ' ')
        return 1;
    else
        return 0;
}

int TempBuf::topNum(int add, char c)
{
    if (add == -1) {
        if (!oplist.empty()) {
            chm ch = oplist.top();
            oplist.pop();

            if (!chlist.empty()) {
                if (ch.ch == chlist.top())
                    chlist.pop();
            }
            return ch.num;
        }
        return -1;
    }
    else if (add == 0) {
        chm ch;
        ch.ch = c;
        ch.num = 0;
        oplist.push(ch);
        if (c == '(' || c == '[' || c == '{'||c=='d')
            chlist.push(c);
        return 0;
    }
    else {
        if (oplist.empty())
            return -1;
        chm &ch = oplist.top();
        ch.num+=add;
        return ch.num;
    }
}

char TempBuf::topChar()
{
    if (chlist.empty())
        return '#';
    return chlist.top();
}

char TempBuf::topOp()
{
    if (oplist.empty())
        return '#';
    return oplist.top().ch;
}

tExpress TempBuf::subExpress(const OpItem &item)
{
    tExpress root = std::make_shared<Express>(item);
    while (!exprlist.empty()) {
        tExpress top = exprlist.top();
        exprlist.pop();
        root->Insert(top);
    }
    return root;
}
#endif

string TempBuf::wordWrapStyle(const string& str) {
    int pos = 0;
    string style;
    while (pos < str.length()) {
        bool t = false;
        string wrap = wordWrap(str,pos,t);

        if (wrap[0] == '#') {
            int tp = str.find_first_of('\n',pos);
            if (tp == string::npos) {
                wrap = str.substr(pos,str.length()-pos);
                pos = str.length();
            }
            else {
                wrap = str.substr(pos,tp-pos);
                pos = tp-1;
            }
        }
        style += styleStr(false,t,wrap);
        pos++;
    }
    return style;
}

string TempBuf::styleStr(bool isdef,bool isterm,const string &str)
{
    if (str[0] == '#') {
        return setStyle(str,true,"grey");
    }
    else if (isterm) {
        if (str[0] == ' ') {
            return setStyle("&nbsp;",false);
        }
        else if (str[0] == '\t') {
            return setStyle("&nbsp;&nbsp;&nbsp;&nbsp;",false);
        }
        else if (str[0] == '\n')
            return setStyle("<br>",false);
        else if(str[0] == '{' || str[0] == '}' ||
                str[0] == '[' || str[0] == ']'||
                str[0] == '(' || str[0] == ')')
            return setStyle(str,true,"rgb(0,103,124)");
         else
            return setStyle(str,true);
    }
    else {
        if (isdef) {
            if (str == "def") {
                return setStyle(str,true,"rgb(0,103,124)");
            }
            else {
                return setStyle(str,true,"rgb(128,0,255)");
            }
        }
        else {
            if (OpItem::isDigtal(str.c_str())) {
                return setStyle(str,false);
            }
            else if (str[0] == '$' || str[0] == '&')
                return setStyle(str.substr(0,1),true,"blue") +
                        setStyle(str.substr(1),false);
            else {
                return setStyle(str,true,"blue");
            }
        }
    }
}

string TempBuf::setStyle(const string &str, bool span, const string &color)
{
    if (!span)
        return str;
    else
        return "<span style='color:"+color+"'>"+str+"</span>";
}

string TempBuf::setErrStyle(const string &str)
{
    return "<span style='background-color:rgba(255,0,0,0.2);'>"+str+"</span>";
}

