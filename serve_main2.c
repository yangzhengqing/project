#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include"../cliser.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define MAX_EVENTS 512

int main(int argc,char **argv)
{
	    int             rv=-1;
        int             serv_port;
    	int             sockfd=-1,clien_fd=-1;
        char            buf[1024];
	    int             LISTEN_PORT=0;
	    struct          sockaddr_in cli_addr;
	    socklen_t       cliaddr_len;
        int             i;

        struct epoll_event             event_array[1024];
        struct epoll_event             event;
        int                            events;
        int                            epollfd;
 
     if(DNSopt(argc,argv,&serv_port) < 0)
     {
             printf("DNS failed:%s\n",strerror(errno));
             return -1;
     }

     if((sockfd=serve_init(&serv_port)) < 0)
     {
             printf("serve init failed:%s\n",strerror(errno));
             return -1;
     }

     if((epollfd = epoll_create(MAX_EVENTS)) < 0)
     {
             printf("epoll_create failed:%s\n",strerror(errno));
             return -1;
     }

     event.events = EPOLLIN;
     event.data.fd = sockfd;

     if(epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&event) < 0)
     {
             printf("epoll_ctl failed:%s\n",strerror(errno));
             return -1;
     }

     while(1)
     {
             events = epoll_wait(epollfd,event_array,MAX_EVENTS,-1);
             if(events < 0)
             {
                     printf("epoll_wait failed:%s\n",strerror(errno));
                     break;
             }

             else if(events == 0)
             {
                     printf("epoll_wait get timeout\n");
                     continue;
             }

             for(i =0 ;i<events;i++)
             {
                     if((event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLIN))
                     {
                             printf("epoll_wait get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
                             epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
                             close(event_array[i].data.fd);
                     }

                     if(event_array[i].data.fd == sockfd)
                     {
                             clien_fd=accept(sockfd,(struct sockaddr *)&cli_addr,&cliaddr_len);

                             if(clien_fd<0)
	                         {

	                                 printf("accept client socket failure!:%s\n",strerror(errno));
		                             continue;
	                         }

                             event.data.fd = clien_fd;
                             event.events = EPOLLIN;
                             
                             if(epoll_ctl(epollfd,EPOLL_CTL_ADD,clien_fd,&event) < 0)
                             {
                                     printf("epoll_ctl clien_fd failed:%s\n",strerror(errno));
                                     close(event_array[i].data.fd);
                                     continue;
                             }

                             printf("epoll add new client socket[%d]\n",clien_fd);

                     }
                     else
                     {
                             memset(buf,0,sizeof(buf));

	                         rv=read(event_array[i].data.fd,buf,sizeof(buf));
	                         if(rv<0)
	                         { 
		                             printf("read failure!",strerror(errno));
                                     close(event_array[i].data.fd);
                                     continue;
	    
	    
	                         }
	    
	                         if(rv==0)
	                         {
		                             printf("client disconnect!");
		                             close(event_array[i].data.fd);
                                     continue;
	                         }
	                         if(rv>0)
	                         {
		                             printf("read %d bytes from client:%s\n",rv,buf);
	                         }


	                         if((rv= write(event_array[i].data.fd,buf,rv))<0)
	                         {
		                             printf("write failure!\n",strerror(errno));
		                             close(event_array[i].data.fd);
                                     continue;
	                         }

	                         printf("write into client successfully!\n");
	                 
                     }
                
             }

     }
}
   

