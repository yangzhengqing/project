
#include<stdarg.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include<sys/file.h>
#include<sys/types.h>
#include<errno.h>
#include<libgen.h>
#include"serve.h"

#ifndef LOGLEVEL
#define LOGLEVEL DEBUG
#endif

static const char* s_loginfo[] = 
{

        [ERROR] = "ERROR",
        [WARN]  = "WARN",
        [INFO]  = "INFO",
        [DEBUG] = "DEBUG",
};

int mylog(const char *function, int line, enum LogLevel level, const char *fmt, ...)
{
        char           *tmp;
        time_t         t;
        struct tm      *p;
        struct timeval tv;
        int            len;
        char           *progname;
        int            millsec;
        FILE           *fp;
        char           buf[100];
        char           log_buf[200];
        char           time_buf[32];
        va_list        arg_list;
        int            millisec;
        off_t          filesize;
        
        memset(buf,0,sizeof(buf));
        va_start(arg_list,fmt);
        vsnprintf(buf,sizeof(buf),fmt,arg_list);

        t = time(NULL);
        p = localtime(&t);

        gettimeofday(&tv,NULL);
        millisec = (int)(tv.tv_usec/100);

        snprintf(time_buf,32,"%04d-%02d-%02d %02d:%02d:%02d:%03d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,millisec);
        

        if(level > LOGLEVEL)
                return -1;

        

        len = snprintf(log_buf,sizeof(buf),"[%s][%s][%s:%d]:%s\n",time_buf,s_loginfo[level], function, line, buf);
        
        if((fp = fopen("mylog.txt","a+"))==NULL)
        {
                printf("fopen mylog.txt failed:%s\n",strerror(errno));
                return -1;
        }

        
        if(flock(fp->_fileno,LOCK_EX) ==-1 )
        {
            printf("flock LOCK_EX failed:%s\n",strerror(errno));
            return -1;
        }

        if((filesize==lseek(fp->_fileno,0,SEEK_END)) ==-1 )
        {
            printf("lseek failed:%s\n",strerror(errno));
            return -1;
        }
 
        if(filesize == 1024*1024)
        {
            if(ftruncate(fp->_fileno,0) ==-1 )
            {
                printf("ftruncate failed:%s\n",strerror(errno));
                return -1;
            }
        }

        fprintf(fp,"%s\n",log_buf);

        va_end(arg_list);

        fclose(fp);

        if(flock(fp->_fileno,LOCK_UN) ==-1 )
        {
            printf("flock LOCK_UN failed:%s\n",strerror(errno));
            return -1;
        }

        return 0;

}
