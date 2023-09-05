/*************************************************************************
	> File Name: custom.cpp
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月01日 星期四 13时47分45秒
 ************************************************************************/

#include <stdio.h>
#include <malloc.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include "custom.h"

namespace utils {
using namespace std;
vector<string> Split(const char *src,const char *delim)
{
    char * strc = (char *)malloc(strlen(src)+1);
    strcpy(strc, src);
    vector<string> resultVec;
    char* tmpStr = strtok(strc, delim);
    while (tmpStr != NULL)
    {
        resultVec.push_back(string(tmpStr));
        tmpStr = strtok(NULL, delim);
    }
    free(strc);
    return resultVec;
}

vector< string> Split(const string &src,const string &pattern)
{
    vector<string> ret;
    string str = src;
    if(pattern.empty() || str.empty())
        return ret;
    size_t start=0;
    size_t index=str.find_first_of(pattern,0);

    while(index!=str.npos)
    {
        if(start!=index)
            ret.push_back(str.substr(start,index-start));
        start=index+1;
        index=str.find_first_of(pattern,start);
    }

    string sub = str.substr(start);
    if(!sub.empty())
        ret.push_back(sub);
    return ret;
}

bool StartWith(const string& str,const string &substr)
{
    return 0 == str.find(substr);
}

/*
 * 去除字符串右端空格
 */
char *strtrimr(char *pstr)
{
    int i;

    i = strlen(pstr) - 1;
    while (isspace(pstr[i]) && (i >= 0))
        pstr[i--] = '\0';

    return pstr;
}

/*
 * 去除字符串左端空格
 */
char *strtriml(char *pstr)
{
    int i = 0,j;

    j = strlen(pstr) - 1;
    while (isspace(pstr[i]) && (i <= j))
        i++;

    if (0<i)
        strcpy(pstr, &pstr[i]);

    return pstr;

}

char *strtrim(char *pstr)
{
    char *p;

    p  = strtriml(pstr);
    return strtriml(p);
}

using namespace std;

bool isillformat(char c) {
    return (isspace(c)||c== '\n' || c== '\r' || c== '\0');
}

string& ltrim(string &str) {
//    string::iterator p = find_if(str.begin(), str.end(),illegalchar());// not1(ptr_fun<int, int>(isillegal))
    int i = 0;
    for (;i < str.length();i++) {
        if (isillformat(str[0])) {
            str.erase(0,1);
        }
        else
            break;
    }

    return str;
}

string& rtrim(string &str) {
    int i = str.length()-1;
    for (;i >= 0;i--) {
        if (isillformat(str[i])) {
            str.erase(i,1);
        }
        else
            break;
    }
    return str;
}

string& trim(string &str) {
    ltrim(rtrim(str));
    return str;
}

int strtoint(const char *str, int len) {
    int result = 0;
    const char *p = str;

    while(*p != '\0' && (len--) != 0) {
        if (*p >= '0' && *p <='9')
            result = *p - '0' +result*10;
        ++p;
    }

    return result;
}

int strtohex(const char *str,int len) {
    int result = 0;
    const char *p = str;

    while(*p != '\0' && (len--) != 0) {
        if (*p >= 'A' && *p <= 'F')
            result = *p - 'A'+10 +result*16;
        else if(*p >= 'a' && *p <= 'f')
            result = *p - 'a' +10 +result*16;
        else if (*p >= '0' && *p <='9')
            result = *p - '0' +result*16;
        ++p;
    }

    return result;
}

char digittochar(unsigned char digit) {
    if (digit <= 9)
        return digit+'0';
    else
        return (digit-10+'A');
}

inline int chartobcd(const char c) {
    return (c & 0x0f);
}

inline int chartohex(const char c) {
    if (c >= 'a' && c <= 'f')  {
        return (c-'a'+10);
    }
    else if (c >= 'A' && c <= 'F')  {
        return (c-'A'+10);
    }
    else {
        return (c-'0');
    }
}

/*
 * 默认10进制
 */
int strtocmd(const char *str,unsigned char *& cmd) {
    int olen = 0;

    int ilen = strlen(str);

    int il=ilen-1;
    while(il>=0 && (str[il] == ' ' || str[il] == '\t') ) {
        --il;
    }

    ilen = il+1;
    if (ilen < 1)
        return olen;
    cmd = (unsigned char *)malloc(ilen+1);
    bzero(cmd,ilen+1);

    if (str[ilen-1] == 'B' || str[ilen-1] == 'b') {
        int flag = 0;
        for (int i = ilen-2;i >= 0;i--) {
            if (isdigit(str[i])) {
                if (++flag == 1) {
                    cmd[olen] = chartobcd(str[i]);
                    if (i == 0)
                        olen++;
                }
                else {
//                    cmd[olen] = cmd[olen]<<4;
                    cmd[olen] |= chartobcd(str[i])<<4;
                    olen++;
                    flag = 0;
                }
            }
            else {
                if (str[i]==' ')
                  olen++;
                flag = 0;
            }
        }
        return olen;
    }
    else if (str[ilen-1] == 'H' || str[ilen-1] == 'h') {
        int flag = 0;
        for (int i = 0;i < ilen-1;i++) {
            if (isxdigit(str[i])) {
                if (++flag == 1) {
                    cmd[olen] = chartohex(str[i]);
                }
                else {
                    cmd[olen] = cmd[olen]<<4;
                    cmd[olen] |= chartohex(str[i]);
                    olen++;
                    flag = 0;
                }
            }
            else {
                olen++;
                flag = 0;
            }
        }
        return olen;
    }
    else {
        int64_t digit = atoll(str);
        union {
            int64_t d;
            uint8_t a[8];
        }ud;
        ud.d=digit;

        if (digit <= 256){
            cmd[0]=ud.a[0];
            return 1;
        }
        else if(digit <= 65536) {
            cmd[0]=ud.a[1];
            cmd[1]=ud.a[0];
            return 2;
        }
        else {
            cmd[0]=ud.a[3];
            cmd[1]=ud.a[2];
            cmd[2]=ud.a[1];
            cmd[3]=ud.a[0];
            return 4;
        }
    }

    return olen;
}

/*
 * 默认16进制，不支持其他数字格式
 */
int strtohexcmd(const char *str,unsigned char*& cmd) {
    int length = 0;
    int index = 0;

    const char *p = str;
    while(*p != '\0' && *(p+1) != '\0') {
        if (isxdigit(*p) && isxdigit(*(p+1))){
            length++;
            p +=2;
        }
        else
            p++;
    }
    if (length == 0)
        return 0;
    cmd = (unsigned char*)malloc(length);

    p = str;

    while(*p != '\0' && *(p+1) != '\0') {
        if (isxdigit(*p) && isxdigit(*(p+1))){
            cmd[index++] = strtohex(p,2);
            p += 2;
        }
        else
            p++;
    }
    return length;
}

string hexcmdtostr(
        const unsigned char *cmd,
        const int byte,
        const char postfix) {
    string result;

    for (int i = 0;i < byte;i++) {
        result += digittochar((cmd[i]&0xf0)>>4);
        result += digittochar(cmd[i]&0x0f);
        if (i != byte-1) {
            if (postfix != '\0')
                result +=postfix;
            result +=' ';
        }
    }
    return result;
}

/*
 * 字符串转成压缩bcd码
 */
int asc_to_bcd(char * dest,const char *src)
{
    unsigned char temp;
    while(*src !='\0')
    {
        temp = *src;
        *dest = ((temp&0x0f)<<4);
        src++;
        temp = *src;
        *dest |= (temp&0x0f);
        src++;
        dest++;
    }
    return 0;
}

/* 参数dest为申请的存放得到压缩bcd码的字符串
 * src为需要进行转换的字符串
 * src_len为src的长度即strlen(src)
 */
int asc_to_bcd_right(char *dest,const char *src,int src_len)
{
    unsigned char temp;
    if((src_len %2) !=0)
    {
    *dest = 0;
    temp = *src;
    *dest |= (temp&0xf);
    src++;
    dest++;
    }
    return asc_to_bcd(dest,src);
}

/* 参数dest为申请的存放得到压缩bcd码的字符串，
 * src为需要进行转换的字符串
 * src_len为src的长度即strlen(src)
 */
int asc_to_bcd_left(char *dest,const char *src,int src_len)
{
    unsigned char temp;
    if((src_len %2) !=0)
    {
        dest[src_len-1] &=0;
    }
    return asc_to_bcd(dest,src);
}

/*************************************************************************************************************************
*函数         :  void HextoBCD(u8 *pBuff,u8 len)
*功能         :  十六进制转为BCD码
*参数         :  pBuff:输入的十六进制数组,len:数组长度
*返回         :  无
*依赖 : 底层宏定义
* 作者 :  li_qcxy@126.com
* 时间 :  2017-1-5
* 最后修改时间:
*说明         :
*************************************************************************************************************************/
void hextobcd(unsigned char *pBuff,unsigned char len) //十六进制转为BCD码
{
unsigned char i,temp;
for(i=0;i<len;i++)
{
temp=pBuff[i]/10;
pBuff[i]=pBuff[i]%10+temp*16;
}
}

//时间字符串转BCD
uint32_t strtimetobcd(const string &daytime)
{
    if (daytime == "undefined")
        return 0xffffffff;
    const char *pStr = daytime.c_str();
    int len = daytime.length();
    uint32_t bcd = 0;
    for (int i = 0;i < len;i++) {
        if (!isdigit(pStr[i])) {
            if (pStr[i] != ':'&& pStr[i] != ' ')
                return 0xffffffff;
            continue;
        }
        bcd <<= 4;
        bcd |= (pStr[i]&0x0f);
    }
    return bcd;
}


/*************************************************************************************************************************
*函数         :  void BCDtoHex(u8 *pBuff,u8 len)
*功能         :  BCD码转为十六进制
*参数         :  pBuff:输入的十六进制数组,len:数组长度
*返回         :  无
*依赖 : 底层宏定义
* 作者 :  li_qcxy@126.com
* 时间 :  2017-1-5
* 最后修改时间:
*说明         :
*************************************************************************************************************************/
void bcdtohex(unsigned char *pBuff,unsigned char len) //BCD码转为十六进制
{
unsigned char i,temp;
for(i = 0;i < len;i ++)
{
temp =pBuff[i] / 16;
pBuff[i] = temp * 10 + pBuff[i] % 16;
}
}

void InvertUint8(uint8_t *dBuf,uint8_t *srcBuf)
{
    int32_t i;
    uint8_t tmp[4];
    tmp[0] = 0;
    for(i=0;i< 8;i++)
    {
      if(srcBuf[0]& (1 << i))
        tmp[0]|=1<<(7-i);
    }
    dBuf[0] = tmp[0];

}
void InvertUint16(uint16_t *dBuf,uint16_t *srcBuf)
{
    int32_t i;
    uint16_t tmp[4];
    tmp[0] = 0;
    for(i=0;i< 16;i++)
    {
      if(srcBuf[0]& (1 << i))
        tmp[0]|=1<<(15 - i);
    }
    dBuf[0] = tmp[0];
}

bool isHexDigtal(char c)
{
//    return ((c >= '0' && c <= '9') || (c >='a' && c<= 'f') ||
//            (c >= 'A' && c <= 'F')) ;
    return isxdigit(c);
}

bool isTerminator(const char c)
{
//    return (c == ')' || c == '(' || c == '[' || c == ']' ||c == '{' || c == '}'||
//            c == '$' || c == ' ' || c== '+' || c== '-' ||
//            c == '*' || c == '/' || c == '%' || c== ',' || c == '&');
    return !(isalpha(c) || isalnum(c) || c == '_');
}

bool isCharactor(const char c)
{
//    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
    return isalpha(c) || c=='_';
}

bool isHexDigit(const char *str) {
    int len = strlen(str);

    int il=len-1;
    while(il>=0 && (str[il] == ' ' || str[il] == '\t') ) {
        --il;
    }

    len = il+1;
    if (len < 2)
        return false;
    if (str[len-1] == 'H' || str[len-1] == 'h') {
        for (int i = 0;i < len-1;i++) {
            if (!isxdigit(str[i]))
                return false;
        }
        return true;
    }
    else if (len >= 3 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        for (int i = 2;i < len;i++) {
            if (!isxdigit(str[i]))
                return false;
        }
        return true;
    }
    return false;
}

bool isBCD(const char *str) {
    int len = strlen(str);

    int il=len-1;
    while(il>=0 && (str[il] == ' ' || str[il] == '\t') ) {
        --il;
    }

    len = il+1;
    if (len < 2)
        return false;
    if (str[len-1] == 'B' || str[len-1] == 'b') {
        for (int i = 0;i < len-1;i++) {
            if (!isdigit(str[i]))
                return false;
        }
        return true;
    }
    return false;
}

bool isDecimal(const char *str,bool &f) {
    int flag = 0;
    int len = strlen(str);

    f = false;
    if (len == 0)
        return false;
    for (int i = 0;i < len;i++) {
        if ( str[i] == '.') {
            flag += 1;
            if (flag == 2 || i == 0)
                return false;
            f = true;
            continue;
        }
        if (!(isdigit(str[i]) || str[i] == '-' || str[i] == '.'))
            return false;
    }
    return true;
}

bool isInteger(const char *str) {
    bool f;
    if (isDecimal(str,f))
        return !f;
    return false;
}

bool existFile(const char *file) {
//    FILE* fp = fopen(file,"r");
    int acs = access(file,F_OK);
    return acs==0;
//    if (!fp)
//        return false;
//    fclose(fp);
//    return true;

//    int acs = access(file,F_OK);
//    int err = errno;

//    return (-1 != access(file,F_OK));
}

string logFileNotoday(const char *path)
{
    DIR *dir;
    struct dirent *file;
    string filename;
    string today = utils::todaytostr();

    if ((dir=opendir(path)) == NULL)
    {
        return filename;
    }

    while ((file = readdir(dir)) != NULL)
    {
        if(strcmp(file->d_name,".")==0 || strcmp(file->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        if(file->d_type != 8)    ///file
            continue;
        if (utils::StartWith(file->d_name,today))
            continue;
        filename = file->d_name;
        break;
    }
    closedir(dir);
    return filename;
}

string logFile(const char *path)
{
    DIR *dir;
    struct dirent *file;
    string filename;
    vector<string> filelist;

    if ((dir=opendir(path)) == NULL)
    {
        return filename;
    }

    while ((file = readdir(dir)) != NULL)
    {
        if(strcmp(file->d_name,".")==0 || strcmp(file->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        if(file->d_type != 8)    ///file
            continue;
        filelist.push_back(file->d_name);
        break;
    }
    closedir(dir);
    if (filelist.size()>0) {
        sort(filelist.begin(),filelist.end(),less<string>());
        filename = filelist.at(0);
    }
    return filename;
}

string todaytostr()
{
    char szTime[50];
    struct tm *tm2;
    time_t today = time(NULL);

    tm2 = localtime(&today);
//    tm2=gmtime(&time);
    strftime(szTime,sizeof(szTime),"%Y-%m-%d",tm2);

    return string(szTime);
}

string timetostr(const time_t &time,int flag)
{
    char szTime[50];
    struct tm *tm2;

    tm2=localtime(&time);
//    tm2=gmtime(&time);strftime(szTime,sizeof(szTime),"%Y-%m-%d %H:%M:%S",tm2);
    switch(flag) {
    case 0:
        strftime(szTime,sizeof(szTime),"%Y-%m-%d %H:%M:%S",tm2);
        break;
    case 1:
        strftime(szTime,sizeof(szTime),"%Y-%m-%d %H:%M",tm2);
        break;
    case 2:
        strftime(szTime,sizeof(szTime),"%Y-%m-%d %H:%M",tm2);
        break;
    case 3:
        strftime(szTime,sizeof(szTime),"%Y-%m-%d %H",tm2);
        break;
    case 9:
        strftime(szTime,sizeof(szTime),"%04Y%02m%02d%02H%02M%02S",tm2);
        break;
    default:
        strftime(szTime,sizeof(szTime),"%Y-%m-%d",tm2);
        break;
    }

    return string(szTime);
}

string nowtostr()
{
    return timetostr(time(NULL));
}

}
