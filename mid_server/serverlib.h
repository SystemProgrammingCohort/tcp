#ifndef __SERVERLIB_H__
#define __SERVERLIB_H__
#include "epolllib.h"
#include "double_list.h"

#define MAX_MSG 20
#define MAX_MULTICAST_GROUPS 1000

typedef struct group_info
{
  unsigned int number_of_clients;
  struct double_list clients_list;
}group_info;

group_info _global_groups_[MAX_MULTICAST_GROUPS];

typedef struct server_socket_event_data
{
  int epoll_fd;
}server_socket_event_data;

int serverInit(struct addrinfo **serveraddr, int * sockfd);
epoll_event_handler * create_midserver_socket_handler(int efd, int sockfd);
void handle_midserver_socket_event(struct epoll_event_handler* self, uint32_t events);
epoll_event_handler * create_server_socket_handler(int efd, int sockfd);
void do_reactor_loop(int epoll_fd);
void init_groups(void);

#endif /* __SERVERLIB_H__ */
