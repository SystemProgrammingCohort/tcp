#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include"cli.h"
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include "socklib.h"
#include "serverlib.h"
#include "serverclientlib.h"

/*Extern */
extern FILE * stdin;
extern pthread_t cli_pid;
extern void display_clients_groups(struct client_group_ptree *p);
extern void display_list(struct double_list *list);
extern group_info _global_groups_[MAX_MULTICAST_GROUPS];
/*Global varible*/

#define BUFFERSIZE 1000
char *argv[BUFFERSIZE]={NULL};
int argc = 0;

/*Local */
static int man_fun(int c,char *v[]);
static int debug_fun(int c,char *v[]);
static int exit_fun(int c,char *v[]); 
static int send_msg_fun(int c,char *v[]);
static int display_client_group(int c ,char *v[]);

static void search_exe_cmd();
static void clear_argv();

char man_des[] = "Man page -----";
char *man_syn[] = {NULL,NULL};

char send_msg_des[] = "Send messages";
char *send_msg_syn[] = {NULL,NULL};

char debug_des[] = "Enable debug messages level";
char *debug_syn[] = {NULL,NULL};

char exit_des[] = "Exit Program";
char *exit_syn[] = {NULL,NULL};

char display_clientgroup_des[] = "List of client in given group : cgr <group number>";
char *display_clientgroup_syn[] = {NULL,NULL};

COMMAND_TABLE_t cmd_info_table[] = {
    {"MAN",man_des,man_syn,man_fun},
    {"SEND",send_msg_des,send_msg_syn,send_msg_fun},
    {"DEBUG",debug_des,debug_syn,debug_fun},
    {"EXIT",exit_des,exit_syn,exit_fun},
    {"display",display_clientgroup_des,display_clientgroup_syn,display_client_group},
    {NULL ,NULL,NULL,0} };

int size_cmd_info_table = ( ( sizeof(cmd_info_table) / sizeof(COMMAND_TABLE_t)) - 1);


int send_msg_fun (int c,char *v[]) 
{
    int i = 0;
    printf("send message ,%d ,%s\n",c,&v[0]);
    for(i=0 ; i <= c ; i++) {
        printf("%s ,%d \n",&v[i],i);
    }     

    return 0;
}

int debug_fun (int c,char *v[]) 
{
    int level = 0;
    level = atoi((char *)&v[1]);
    printf("Enable debug level ==> %d\n",level);

    return 0;
}
int exit_fun(int c,char *v[])
{
    printf("EXIT Program......\n");
    exit(0);
}

int man_fun (int c,char *v[]) 
{
    int count = 0 ,err = 0;
    if ( c < 1 ) {
        printf("List all commands \n");
        while (count < size_cmd_info_table) {

        if ( !strcmp_nocase(cmd_info_table[count].command,"MAN") ) {
            count++;
            continue;
        }    
        printf("%s :- %s \n",cmd_info_table[count].command,
                            cmd_info_table[count].description);
        count++;
       }
    } else {
    
    while ( count < size_cmd_info_table) {
         
         if ( strcmp_nocase(cmd_info_table[count].command,(char *)&argv[c])) {
              err++;
         } else {
              printf("%s :- %s \n",cmd_info_table[count].command,
                               cmd_info_table[count].description);  
         }
        count++;
    }
    if (err >= size_cmd_info_table ) {
       printf("Command is not found\n");
    }
   }
   
   return 0; 
}

void search_exe_cmd()
{
    int count = 0 ,err = 0;

    while ( count < size_cmd_info_table) {
         
         if ( strcmp_nocase(cmd_info_table[count].command,(char *)&argv[0])) {
              err++;
         } else {
          
             (cmd_info_table[count].procedure ) (argc ,&argv[0]);   
         }
        count++;
    }
    if (  (err >= size_cmd_info_table)  ) {
       printf("Command is not found\n");
    }
}  


void* cli_handler(void *arg )
{
    char ch,str[BUFFERSIZE],buff[BUFFERSIZE];
    int len = 0,i = 0 ,buff_cnt = 0;
    pthread_t id = pthread_self();
    while(1) {
      
      __fpurge(stdin);
      memset(str,0,(len));
      len = 0;
      i=0;
      printf("$");
      fgets(str,BUFFERSIZE,stdin);
      len = strlen(str);

      while(i <= (len))
      {
         buff[buff_cnt] = str[i];   
         if(str[i] == '\0') {
            memcpy(&argv[argc],buff,(buff_cnt-1));
            memset(buff,0,BUFFERSIZE);
            buff_cnt=0;
            if( (argc == 0) && (buff_cnt <=2 ) ) {
                buff_cnt=0; 
                clear_argv();
            } else {
                buff_cnt=0; 
                search_exe_cmd();
                clear_argv();
           }
          
         } else if (str[i] == ' ') {
            memcpy(&argv[argc],buff,buff_cnt);
            memset(buff,0,BUFFERSIZE);
            buff_cnt=0; 
            argc++;
         } else {
            buff_cnt++;
         }
         i++; 

      }
      
    }

   return NULL;
}


void clear_argv()
{
    int i = 0;
    while ( i <= argc)
    {    
         memset(&argv[i],0,BUFFERSIZE);
         i++;
    }
    memset(&argv[0],0,BUFFERSIZE);
    argc = 0;
}

int strcmp_nocase (char *str1,char	*str2)
{
	char		c1, c2;
	if (!str1 || !str2)
	   return(STRCMP_FAIL);

	while (*str1 != '\0' && *str2 != '\0') {
	   c1 = *str1++; c2 = *str2++;
	   if (isalpha(c1) && isupper(c1))
	      c1 = tolower(c1);
	   if (isalpha(c2) && isupper(c2))
	      c2 = tolower(c2);
	   if (c1 != c2)
	      return(STRCMP_FAIL);
	}

	if (*str1 != '\0' || *str2 != '\0')
	   return(STRCMP_FAIL);
	return(STRCMP_OK);
}   


static int display_client_group(int c ,char *v[])
{
    int group_num = 0;
    if ( strcmp_nocase((char *)&v[1],"cgr") ) {
        
       printf("command not found ,%s, %s\n",&v[1],&v[2]);
       
       return 1; 
    }
    
    group_num = atoi((char *)&v[2]);
    if(group_num > MAX_MULTICAST_GROUPS ) {
        printf("The group is not exit reached to maximum limit\n");
        return 1;
    }     
    display_list(&(_global_groups_[group_num-1].clients_list));    
      
}     
