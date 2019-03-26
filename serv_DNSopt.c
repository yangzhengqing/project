#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<getopt.h>
#include"../cliser.h"


void print_usage(char *program)
{
        printf("%s usages: %s\n",program,strerror(errno));
        printf("-p(port):serve listen port\n");
        printf("-h(help):help information\n");
        exit(0);
}


int DNSopt(int argc,char **argv,int *serv_port)
{
        int               opt;

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
                                 *serv_port = atoi(optarg);                                                                                                    break;                                                                                                              case 'h':
                                 print_usage(argv[0]);                                                                                                        break;
                         default:
                                 break;
                 }      
         }

         if(!serv_port)
         {
                 print_usage(argv[0]);
         }
}
