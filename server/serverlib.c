#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include "socklib.h"
#include "serverlib.h"



int serverInit(struct addrinfo **serveraddr, int * sockfd)
{
  struct addrinfo hints;
  int status;

  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((*sockfd = getfd(&hints, serveraddr)) == -1)
  {
    fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(status));
    return -1;
  }
  make_socket_non_blocking(*sockfd);
#ifndef WIN32
  if((status = makeSockReusable(*sockfd)) == -1)
    return -1;
#endif
  if((status = bindSock(*sockfd, *serveraddr)) == -1)
  {
    perror("listener:socket");
    return -1;
  }
  if(listen(*sockfd, MAX_LISTEN_BACKLOG) == -1)
  {
    perror("listen");
    return -1;
  }
}

epoll_event_handler * create_server_socket_handler(int efd, int sockfd)                                                                                                                                     
{ 
  epoll_event_handler *handle=NULL;                                                                                                                                                                         
  server_socket_event_data *closure=NULL;
  closure = (server_socket_event_data *)malloc(sizeof(server_socket_event_data));                                                                                                                           
  closure->epoll_fd = efd;
  handle = (epoll_event_handler *)malloc(sizeof(epoll_event_handler));                                                                                                                                      
  handle->fd = sockfd;
  handle->handle = handle_server_socket_event;;                                                                                                                                                             
  handle->closure = closure;
  return handle;                                                                                                                                                                                            
}

void handle_server_socket_event(struct epoll_event_handler* self, uint32_t events)
{
  printf("\n handle_server_socket_event");
  struct server_socket_event_data* closure = (struct server_socket_event_data*) self->closure;
  int client_socket_fd;
  while (1) {
    client_socket_fd = accept(self->fd, NULL, NULL);
    if (client_socket_fd == -1) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {                                                                                                                                                    
        break;
      }else{                                                                                                                                                                                                
        perror("Could not accept");
        exit(1);                                                                                                                                                                                            
      }
    }                                                                                                                                                                                                       
    handle_client_connection(closure->epoll_fd,
        client_socket_fd);                                                                                                                                                                                  
  }
}

// needs to be moved to main function. is not server specific
void do_reactor_loop(int epoll_fd)                                                                                                                                                                          
{ 
  struct epoll_event current_epoll_event;                                                                                                                                                                   

  while (1) {                                                                                                                                                                                               
      struct epoll_event_handler* handler;
                                                                                                                                                                                                            
      epoll_wait(epoll_fd,&current_epoll_event, 1, -1);
      handler = (struct epoll_event_handler*) current_epoll_event.data.ptr;                                                                                                                                 
      handler->handle(handler, current_epoll_event.events);
  }
}
