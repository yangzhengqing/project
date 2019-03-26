#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"../cliser.h"


int serve_init(int *serv_port)
{
        int         sockfd;
        int         on = 1;
        struct          sockaddr_in serv_addr;
        
        if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        {
                printf("serve socket failed: %s\n",strerror(errno));
        }
        memset(&serv_addr,0,sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(*serv_port);
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

        if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        {
                printf("bind failed: %s\n",strerror(errno));
                close(sockfd);
                return -1;
        }

       if(listen(sockfd,10) < 0)
       {
               printf("listen sockfd failed.\n");
               return -1;
       }

       return sockfd;

}
