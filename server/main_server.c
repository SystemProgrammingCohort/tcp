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
  int sockfd,efd;
  struct epoll_event event;
  epoll_event_handler *server_sock_handle;
  struct addrinfo *serveraddr=NULL;

  if(serverInit(&serveraddr,&sockfd) == -1)
  {
    printf("\n error");
    return 1;
  }
  efd = epoll_create(MAX_CON);
  if (efd == -1)
  {
    perror ("epoll_create");
    return -1;
  }

  server_sock_handle = create_server_socket_handler(efd, sockfd);
  add_epoll_handler(efd, server_sock_handle, EPOLLIN | EPOLLOUT);
  init_groups();
  do_reactor_loop(efd);

  close(sockfd);
  freeaddrinfo(serveraddr);
  return 0;
}
