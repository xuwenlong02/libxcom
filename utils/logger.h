/************************************************************************* 
 * > File Name: logger.h 
 * > Author: xuwenlong 
 * > Mail: myxuan475@126.com 
 * > Created Time: 2018年01月17日 星期三 10时17分14秒
 ************************************************************************/
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <defines.h>
#include "custom.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/statfs.h>


void set_enable(bool enable);
bool get_enable();

#define LOG_ENABLE(enable)  set_enable(enable)

#define __FILENAME__(x) (strrchr(x,'/')?strrchr(x,'/')+1:x)

inline static int get_current_exename(char name[256]) {
    char *path_end;
    char dir[256] = {0};
    if (readlink("/proc/self/exe",dir,sizeof(dir)) <= 0)
        return -1;
    path_end = strrchr(dir,'/');
    if (!path_end)
        return -1;
    ++path_end;
    strcpy(name,path_end);
    return 0;
}

inline static bool log_is_full()
{
    struct statfs diskinfo;
    statfs("/",&diskinfo);

    double ffree = diskinfo.f_bfree/1024.0*diskinfo.f_bsize/1024.0;
    double ftotal = diskinfo.f_blocks/1024.0*diskinfo.f_bsize/1024.0;

#if HTNICE_K4
    if (ffree < 0.35*ftotal)
        return true;
#else
    if (ffree < 0.25*ftotal)
        return true;
#endif
    return false;
}

inline static std::string check_disk_log(const std::string &logdir)
{
    if (log_is_full()) {
        std::string log;
        const std::string filename = utils::logFile(logdir.c_str());
        if (filename.empty())
            return "";
        log = logdir+"/" + filename;
        remove(log.c_str());
        return filename;
    }
    return "";
}

inline static FILE* openhandle() {
    FILE* fp;

    if (getenv("LOG")) {
        if (!strcmp(getenv("LOG"),"stdout")) {
            fp = stdout;
            return fp;
        }
    }

    if (!get_enable())
        return NULL;

    char path[MAX_NAME_LEN];
    char exename[MAX_NAME_LEN] = {0};
    get_current_exename(exename);

    sprintf(path,"%s/%s-libxcom.log",
            getenv(CONFIG_LOG_ENV),
            utils::todaytostr().c_str());

    fp = fopen(path,"a+");
    return fp;
}

inline static void closehandle(FILE *fp) {
    if (getenv("LOG")) {
        if (!strcmp(getenv("LOG"),"stdout")) {
            return;
        }
    }
    if (fp) {
        fflush(fp);
        fclose(fp);
    }
}

#define LOG_INFO(format,...)  do{\
    FILE *fp = openhandle(); \
    if (fp) {\
    fprintf(fp,"[INFO] %s,%d,%s,%s:" format,\
    __FILENAME__(__FILE__),__LINE__,__FUNCTION__,utils::nowtostr().c_str(),##__VA_ARGS__);\
    fprintf(fp,"\n");\
    closehandle(fp);\
    }\
    }while(0)

#define LOG_ERR(format,...)   do{\
    FILE *fp = openhandle(); \
    if (fp) {\
    fprintf(fp,"[ERROR] %s,%d,%s,%s:" format,\
    __FILENAME__(__FILE__),__LINE__,__FUNCTION__,utils::nowtostr().c_str(),##__VA_ARGS__);\
    fprintf(fp,"\n");\
    closehandle(fp);\
    }\
    }while(0)

#define LOG_WARN(format,...)  do{\
    FILE *fp = openhandle(); \
    if (fp) {\
    fprintf(fp,"[WARN] %s,%d,%s,%s:" format,\
    __FILENAME__(__FILE__),__LINE__,__FUNCTION__,utils::nowtostr().c_str(),##__VA_ARGS__);\
    fprintf(fp,"\n");\
    closehandle(fp);\
    }\
    }while(0)

#define LOG_DEBUG(format,...)  do {\
    FILE *fp = openhandle(); \
    if (fp) {\
    fprintf(fp, "[DEBUG] %s,%d,%s,%s:" format,\
    __FILENAME__(__FILE__),__LINE__,__FUNCTION__,utils::nowtostr().c_str(),##__VA_ARGS__);\
    fprintf(fp,"\n");\
    closehandle(fp);\
    }\
    }while(0)

#define LOG_FIXME(format,...)  do{\
    FILE *fp = openhandle(); \
    if (fp) {\
    fprintf(fp,"[FIXME] %s,%d,%s,%s:" format,\
    __FILENAME__(__FILE__),__LINE__,__FUNCTION__,utils::nowtostr().c_str(),##__VA_ARGS__);\
    fprintf(fp,"\n");\
    closehandle(fp);\
    }\
    }while(0)

class LogTrace
{
public:

    LogTrace(const char *_file,int _line,const char *_function){
        int funlen = strlen(_function);
        int filelen = strlen(_file);

        loginfo = (char*)malloc(filelen+funlen+10);
        sprintf(loginfo,"%s,%d,%s",
                _file,
                  _line,
                  _function);

        FILE *fp = openhandle();
        if (fp) {
            fprintf(fp,"[TRACE] %s Enter\n",loginfo);
            closehandle(fp);
        }

    }

    LogTrace(const char *_module){
        int funlen = strlen(_module);

        loginfo = (char*)malloc(funlen+1);
        strcpy(loginfo,_module);
        FILE *fp = openhandle();
        if (fp) {
            fprintf(fp,"[TRACE] %s Enter\n",loginfo);
            closehandle(fp);
        }
    }

    ~LogTrace(){
        FILE *fp = openhandle();
        if (fp) {
            fprintf(fp,"[TRACE] %s Exit\n",loginfo);
            closehandle(fp);
        }
        free(loginfo);
    }
private:
    char *loginfo;
};

#define LOG_MODULE_TRACE(name) class LogInfo { \
    public:\
    LogTrace *logTrace; \
    LogInfo() { logTrace = new LogTrace(#name);}\
    ~LogInfo(){delete logTrace;}\
    }logInfo

#define LOG_TRACE() LogTrace logTrace(__FILENAME__(__FILE__),\
    __LINE__,\
    __FUNCTION__)

#endif//__LOGGER_H__
