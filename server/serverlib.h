#ifndef __SERVERLIB_H__
#define __SERVERLIB_H__
#include "epolllib.h"

#define MAX_MSG 20
#define MAX_MULTICAST_GROUPS 1000

typedef struct Groupinfo
{
  unsigned int clientcapacity;
  unsigned int servercapacity;
  unsigned int numclients;
  unsigned int numservers;
//  serving_list *clientlist;
//  serving_list *serverlist;
}Groupinfo;

typedef struct server_socket_event_data
{
  int epoll_fd;
}server_socket_event_data;

int serverInit(struct addrinfo **serveraddr, int * sockfd);
epoll_event_handler * create_server_socket_handler(int efd, int sockfd);
void handle_server_socket_event(struct epoll_event_handler* self, uint32_t events);
void do_reactor_loop(int epoll_fd);
//void initialise_groups(groupinfo groups[],int num_groups);

#endif /* __SERVERLIB_H__ */
