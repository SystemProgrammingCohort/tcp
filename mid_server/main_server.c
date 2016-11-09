#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h> 
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include "socklib.h"
#include "serverlib.h"
#include "epolllib.h"
#include "serverclientlib.h"

#define MAX_CON 1200


int main(int argc, char *argv[])
{
  int sockfd,efd,tcpfd;
  struct epoll_event event;
  epoll_event_handler *midserver_sock_handle, *server_sock_handle;
  struct addrinfo *serveraddr=NULL;
  uint16_t *groups,num_groups,actual_num_groups=0;
  int i,j;

  if(argc < 2)
  {
    printf("usage: midserver mainserver-addr num_groups groups\n");
    return 1;
  }

  i=3;
  num_groups = atoi(argv[3]);
  ++i;

  printf("\n number of groups: %d",num_groups);
  groups = (uint16_t *)malloc(sizeof(uint16_t)*num_groups);
  for(j=0;i<argc;++i,++j)
  {
    groups[j] = atoi(argv[i]);
    ++actual_num_groups;
    printf(" %d ",groups[j]);
  }

  if(actual_num_groups!=num_groups)
    printf("\n number of groups entered mismatch than required");

  if(actual_num_groups == 0)
    return 1;


  if(serverInit(&serveraddr,&sockfd) == -1)
  {
    printf("\n error");
    return 1;
  }
  if((tcpfd = initMidserver(argv[1]))==-1)                                                                                                                                                                    
    return 1;
  efd = epoll_create(MAX_CON);
  if (efd == -1)
  {
    perror ("epoll_create");
    return -1;
  }

  midserver_sock_handle = create_midserver_socket_handler(efd, sockfd);
  server_sock_handle = create_server_socket_handler(efd, tcpfd);
  add_epoll_handler(efd, midserver_sock_handle, EPOLLIN);
  add_epoll_handler(efd, server_sock_handle, EPOLLIN);
  init_groups();
  do_reactor_loop(efd);

  close(sockfd);
  freeaddrinfo(serveraddr);
  return 0;
}
