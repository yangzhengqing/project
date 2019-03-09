#include<stdio.h>
#include<string.h>
#include"sqlite3.h"
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include"serve.h"


int datbas_open(sqlite3 **db)
{
        char        *errmsg;

        if(access(DATABASE,F_OK)!=-1)
        {
                printf("DATABASE exist.\n");

                if(sqlite3_open(DATABASE,db)!=SQLITE_OK)
                {

                        printf("sqlite3_open error:%s\n",sqlite3_errmsg(*db));
                        return -1;
                }
               
        }
        else
        {
                if(sqlite3_open(DATABASE,db)!=SQLITE_OK)
                {

                        printf("sqlite3_open error:%s\n",sqlite3_errmsg(*db));
                }
                
                printf("sqlite3_open success.\n");

                if(sqlite3_exec(*db,"create table coll_tem (ID char,temp char,time char);",NULL,NULL,&errmsg)!=SQLITE_OK)
                {
                        printf("create table failed:%s\n",errmsg);
                }
                else
                        printf("create table successfully.\n");
        }
        return 0;
}


int datbas_insert(sqlite3 *db,char *ID,char *temp,char *time)
{
        char        *errmsg;
    
        char        sql[128];
                                           
        if(!db)
        {
            printf("Invalid input argument db!\n");
            return -1;
        }

         memset(sql,0,sizeof(sql)); 
         if(snprintf(sql, sizeof(sql), "insert into coll_tem values('%s','%s','%s');",ID,temp,time)<0)
         {
                     printf("sprintf error:%s\n",strerror(errno));                                                                                          return -1;
         }

         printf("sql: [%s]\n",sql);

         if((sqlite3_exec(db,sql,NULL,NULL,&errmsg)) != SQLITE_OK)
          { 
                  printf("insert failed:%s\n",errmsg);
                  return -2;
                                                                                                                                       }
         printf("insert successfully\n");     
   
         return 0;
}


int datbas_close(sqlite3 *db)
{
        char     *errmsg;

        if(sqlite3_close(db)!=SQLITE_OK)
         {
                 printf("close error:%s\n",sqlite3_errmsg(db));
                 return 0;
         }
    
         return 0;
       
}
