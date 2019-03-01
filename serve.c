#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>
#include<arpa/inet.h>

void print_usage(char *program)
{
        printf("%s usages: %s\n",strerror(errno));
        printf("-p(port):serve listen port\n");
        printf("-h(help):help information\n");
        exit(0);
}

int main(int argc,char **argv)
{
        int                  opt;
        int                  serv_port;
        struct sockaddr_in   serv_addr;
        struct sockaddr_in   clien_addr;
        socklen_t            clienaddr_len;
        int                  sockfd;
        int                  clienfd;
        int                  fd;
        int                  rv;
        char                 buf[1024];
        int                  on = 1;
        char                 finish[25]="serve receive finish!";
        pid_t                pid;

        struct option long_options[]=
        {
                {"port",1,NULL,'p'},
                {"h",0,NULL,'h'},
                {0,0,0,0}
        };

        while((opt = getopt_long(argc,argv,"p:h",long_options,NULL)) > 0)
        {
                switch(opt)
                {
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

        if(!serv_port)
        {
                print_usage(argv[0]);
        }
        
        
        if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        {
                printf("serve socket failed: %s\n",strerror(errno));
        }
        memset(&serv_addr,0,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(serv_port);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

        if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("bind failed: %s\n",strerror(errno));
                close(sockfd);
                return -1;
        }

        listen(sockfd,10);

        
        while(1)
        {

                printf("serve start to connect new client...\n");

                if((clienfd = accept(sockfd,(struct sockaddr *)&clien_addr,&clienaddr_len)) <0 )
                {

                        printf("serve accept failed: %s\n",strerror(errno));
                        return -1;
                }

                printf("connecting successfully...\n");

                pid = fork();
                if(pid < 0)
                {
                        printf("fork() create child process failed: %s\n",strerror(errno));
                        close(clienfd);
                        continue;
                }
                if(pid > 0)
                {
                        close(clienfd);
                        continue;
                }
                if(pid == 0)
                {


                        if((fd = open("temp",O_RDWR|O_CREAT|O_APPEND)) < 0)
                        {

                                printf("open 'temp' failed: %s\n",strerror(errno));
                                return -1;
                        }

                
                        while(1) 
                
                        {

                                memset(buf,0,sizeof(buf));

                                if((rv=read(clienfd,buf,sizeof(fd))) < 0)
                                {

                                        printf("read from client failed: %s\n",strerror(errno));
                                        close(clienfd);
                                        close(fd);
                                        return -1;
                                }

                                else if(rv == 0)
                                {
                                        printf("Disconnect with client..\n");
                                        close(clienfd);
                                        close(fd);
                                        exit(0);
                                }

               
                                write(STDOUT_FILENO,buf,rv);

                                if(write(fd,buf,rv) < 0)
                                {
                                        printf("write into 'temp' failed: %s\n",strerror(errno));
                                        close(clienfd);
                                        close(fd);
                                        return -1;
                                }

                
                                write(clienfd,finish,sizeof(finish));

                       }
                }        
        }


        return 0;

}


