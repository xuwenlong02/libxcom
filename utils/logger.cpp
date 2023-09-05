/************************************************************************* 
 * > File Name: logger.h 
 * > Author: xuwenlong 
 * > Mail: myxuan475@126.com 
 * > Created Time: 2018年01月17日 星期三 10时17分14秒
 ************************************************************************/
#include "logger.h"

static bool g_enable = true;

void set_enable(bool enable)
{
    g_enable = enable;
}

bool get_enable()
{
    return g_enable;
}
