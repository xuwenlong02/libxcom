/*************************************************************************
	> File Name: custom.h
	> Author: xuwenlong
	> Mail: myxuan475@126.com 
	> Created Time: 2018年02月01日 星期四 13时48分08秒
 ************************************************************************/
#ifndef __CUSTOM_H__
#define __CUSTOM_H__

#include<stdio.h>
#include <vector>
#include <string>
#include <string.h>

namespace utils
{
using namespace std;
vector<string> Split(const char *src,const char *delim);
vector<string> Split(const string &src,const string &pattern);
bool StartWith(const string& str, const string &substr);
/*
 * 去除字符串右端空格
 */
char *strtrimr(char *pstr);

/*
 * 去除字符串左端空格
 */
char *strtriml(char *pstr);

char *strtrim(char *pstr);

string& ltrim(string &str);

string& rtrim(string &str);

string& trim(string &str);

int strtoint(const char *str,int len = -1);
int strtohex(const char *str, int len = -1);
char digittochar(unsigned char digit);

/*
 * 默认10进制
 */
int strtocmd(const char *str,unsigned char *& cmd);
/*
 * 字符串转换成16进制命令
 */
int strtohexcmd(const char *str,unsigned char*& cmd);
string hexcmdtostr(const unsigned char *cmd,const int byte,const char postfix ='\0');

/*
 * 字符串转成压缩bcd码
 */
int asc_to_bcd(char * dest,const char *src);

/* 参数dest为申请的存放得到压缩bcd码的字符串
 * src为需要进行转换的字符串
 * src_len为src的长度即strlen(src)
 */
int asc_to_bcd_right(char *dest,const char *src,int src_len);

/* 参数dest为申请的存放得到压缩bcd码的字符串，
 * src为需要进行转换的字符串
 * src_len为src的长度即strlen(src)
 */
int asc_to_bcd_left(char *dest,const char *src,int src_len);

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
void hextobcd(unsigned char *pBuff,unsigned char len); //十六进制转为BCD码
uint32_t strtimetobcd(const string &daytime);

void bcdtohex(unsigned char *pBuff,unsigned char len); //BCD码转为十六进制

void InvertUint8(uint8_t *dBuf,uint8_t *srcBuf);
void InvertUint16(uint16_t *dBuf,uint16_t *srcBuf);

bool isHexDigtal(const char c);
bool isTerminator(const char c);
bool isCharactor(const char c);
bool isHexDigit(const char *str);
bool isDecimal(const char *str,bool &fl);
bool isInteger(const char *str);

bool existFile(const char *file);

string logFileNotoday(const char *path);
string logFile(const char *path);

string todaytostr();
string timetostr(const time_t &time,int flag = 0);
string nowtostr();
}

#endif//__CUSTOM_H__
