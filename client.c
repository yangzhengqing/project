#include<stdio.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<syslog.h>
#include<time.h>
#include<stdarg.h>

//void journal(char *argv);

void DS_get_tem(float *,char *);

void print_usage(char *order)
{
        printf("%s usages:\n",order);
        printf("-I(IP):server IP\n");
        printf("-p(port):server port\n");
        printf("-h(help):help information\n");

        exit(0);
}


int main(int argc,char **argv)
{
        char                   *serv_IP = NULL;
        int                    serv_port = 0;
        int                    opt;
        struct sockaddr_in     serv_addr;
        char                   buf[100];
        char                   buf1[100];
        char                   chip[20];
        int                    sockfd;
        int                    rv = -1;
        int                    on = 1;
        float                  temp;
        struct timespec        tout;
        struct tm              *tmp;
        struct option long_options[]=
        {
                
                {"IP",1,NULL,'i'},
                {"port",1,NULL,'p'},
                {"help",0,NULL,'h'},
                {0,0,0,0}
        
        };

        while((opt=getopt_long(argc,argv,"i:p:h",long_options,NULL)) > 0)
        {
                switch(opt)
                {

                        case 'i':
                                serv_IP = optarg;
                                break;
                        case 'p':
                                serv_port = atoi(optarg);
                                break;
                        case 'h':
                                print_usage(argv[0]);
                                break;
                        default:
                                break;

                }
      
        }

        if((!serv_IP)|(!serv_port))
        {
                print_usage(argv[0]);
        }
        

        if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        {
                printf("socket failed: %s\n",strerror(errno));
        }

        setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

        memset(&serv_addr,0,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(serv_port);
        inet_aton(serv_IP,&serv_addr.sin_addr);
         
        memset(buf,0,sizeof(buf));

        printf("client is connecting serve...\n");

        if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("connect failed: %s\n",strerror(errno));
        }
        else
        {

                printf("Connect successfully...\n");
        }


       // journal(argv[0]);

        
        
        


        while(1)
        {
                 
                DS_get_tem(&temp,chip);
                tmp = localtime(&tout.tv_sec);
                strftime(buf1,sizeof(buf1),"%r",tmp);

                sprintf(buf,"%s: temperature=%f  time=%s \n",chip,temp,buf1);

                if(write(sockfd,buf,strlen(buf)) <0 )
                {

                        printf("write to serve unsuccessfully: %s\n",strerror(errno));
                        goto cleanup;
                }

                memset(buf,0,sizeof(buf));

                if((rv = read(sockfd,buf,sizeof(buf))) < 0)
                {   
                        printf("read from serve unsuccessfully: %s\n",strerror(errno));
                        goto cleanup;
                }

                if(rv == 0)
                { 
                        printf("disconnect serve\n");
                        goto cleanup;
                }

                printf(" %s\n",buf);

                memset(buf,0,sizeof(buf));

                sleep(10);          
           }

        close(sockfd);

cleanup:
        close(sockfd);
        return 0;

}


void DS_get_tem(float *temp,char *chip)
{
        DIR                *dp;
        struct dirent      *dirp;
        char               w1_path[30] = "/sys/bus/w1/devices/";
        int                found = 0;
        int                fd;
        char               *ptr;
        char               buf[100];
        if((dp = opendir(w1_path)) < 0)
        {
                printf("opendir w1_path failed: %s\n",strerror(errno));
        }

        while((dirp = readdir(dp)) != NULL)
        {
                if(strstr(dirp->d_name,"28-") > 0)
                {
                        strcpy(chip,dirp->d_name);
                        found = 1;
                        break;
                }
        }
        closedir(dp);
        strncat(w1_path,chip,sizeof(w1_path));
        strncat(w1_path,"/w1_slave",sizeof(w1_path));
   
        if(!found)
        {
                printf("readdir failed and can't find %s\n",dirp->d_name);
        } 
        
        
        if((fd=open(w1_path,O_RDONLY)) < 0)
        {
                printf("open w1_path failed: %s\n",strerror(errno));
                close(fd);
        }


        if(read(fd,buf,sizeof(buf)) < 0)
        {
                printf("read from w1_path failed: %s\n",strerror(errno));
                close(fd);
        }
       
        ptr = strstr(buf,"t=");
        if(!ptr)
                printf("ERROR:can't get temperature\n");
        ptr+=2;
        *temp = atof(ptr)/1000;
}


/*
void journal(char *argv)
{
        char          *program = basename(argv);

        if(daemon(0,0) < 0)
        {
                printf("program '%s' daemon failed: %s\n",program,strerror(errno));
        }

        openlog("daemon",LOG_CONS | LOG_PID,0);

        syslog(LOG_NOTICE,"Program '%s' start running\n",program);
        syslog(LOG_WARNING,"Program '%s' running with a warnning message\n",program);
        syslog(LOG_EMERG,"Program '%S' ruuning with a emergency message\n",program);

}
*/


