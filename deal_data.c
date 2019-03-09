#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include"serve.h"
#include<ctype.h>

int deal_data(char *buf,char *ID,char *temp,char *time)
{
        int                  i=0;
        char                 *ptr,*ptr_start,*ptr_end;

        ptr = strstr(buf,"28");
        ptr_start = ptr;
        while(i<3)
        {
                while(!isblank(*ptr))
                {
                        ptr++;
                }
                ptr_end = ptr;
                i++;
                if(i==1)
                {
                        memset(ID,0,sizeof(ID));
                        memcpy(ID,ptr_start,ptr_end-ptr_start);
                        printf("ID=%s\n",ID);
                }
                if(i==2)
                {
                        memset(temp,0,sizeof(temp));
                        memcpy(temp,ptr_start,ptr_end-ptr_start);
                        printf("temp=%s\n",temp);
                }
                if(i==3)
                {
                        memset(time,0,sizeof(time));
                        memcpy(time,ptr_start,ptr_end-ptr_start);
                        printf("time=%s\n",time);
                }

                ptr_start = NULL;

                ptr_start=++ptr_end;
                ptr++;
        }
            
       /* if((p = strtok(buf,"|")) == NULL)
        {
                printf("strtok failed:%s\n",strerror(errno));
        }
        while(p!=NULL)
        {
                printf("%s\n",p);
                p = strtok(NULL,"|");
        }
     */

           return 0;

}
