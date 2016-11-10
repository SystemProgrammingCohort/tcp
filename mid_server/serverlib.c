#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include "socklib.h"
#include "serverlib.h"
#include "tlv.h"

void close_server_socket(struct epoll_event_handler* self)
{
  printf("close_server_socket");
  return;
}

int serverInit(struct addrinfo **serveraddr, int * sockfd)
{
  struct addrinfo hints;
  int status;

  memset(&hints,0,sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((*sockfd = getfd(&hints, serveraddr, NULL, MIDDLE_SERVER_PORT)) == -1)
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

int initMidserver(char *serverstring)
{                                                                                                                                                                                                           
  // For server and my info
  int status=-1;                                                                                                                                                                                            
  struct addrinfo hints,*server_addr=NULL,*p=NULL;
                                                                                                                                                                                                            
  // For socket
  int sockfd=-1, yes=1;                                                                                                                                                                                     

  //message                                                                                                                                                                                                 
  char msg[MAX_MSG];
  int bytes_sent=0;                                                                                                                                                                                         

  memset(&hints,0,sizeof hints);                                                                                                                                                                            
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;                                                                                                                                                                          
  hints.ai_flags = 0;
                                                                                                                                                                                                            
  if((sockfd = getfd(&hints, &server_addr, serverstring, SERVER_PORT)) == -1)
  {                                                                                                                                                                                                         
  //  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    printf("nope!");                                                                                                                                                                                        
    exit(-1);
  }                                                                                                                                                                                                         
                                                                                                                                                                                                            
#ifndef WIN32
  if(makeSockReusable(sockfd) == -1)                                                                                                                                                                        
    exit(-1);
#endif                                                                                                                                                                                                      
  printf("\n socket: %d",sockfd);
  if(connect(sockfd, server_addr->ai_addr, server_addr->ai_addrlen) == -1)                                                                                                                                  
  { 
    printf("could not connect");                                                                                                                                                                            
    close(sockfd);
    perror("client: connect");                                                                                                                                                                              
    return 1;
  }                                                                                                                                                                                                         
  make_socket_non_blocking(sockfd);
  freeaddrinfo(server_addr);                                                                                                                                                                                
  return sockfd;
}

void init_groups(void)
{
  int i;
  for(i=0;i<MAX_MULTICAST_GROUPS;++i)
  {
    _global_groups_[i].number_of_clients = 0;
    _global_groups_[i].clients_list.last = NULL;
    _global_groups_[i].clients_list.first = NULL;
  }
  printf("\n server loading done\n");
}

epoll_event_handler * create_midserver_socket_handler(int efd, int sockfd)                                                                                                                                     
{ 
  epoll_event_handler *handle=NULL;                                                                                                                                                                         
  server_socket_event_data *closure=NULL;
  closure = (server_socket_event_data *)malloc(sizeof(server_socket_event_data));                                                                                                                           
  closure->epoll_fd = efd;
  handle = (epoll_event_handler *)malloc(sizeof(epoll_event_handler));                                                                                                                                      
  handle->fd = sockfd;
  handle->handle = handle_midserver_socket_event;;                                                                                                                                                             
  handle->closure = closure;
  printf("\n created server handle");
  return handle;                                                                                                                                                                                            
}

void handle_midserver_socket_event(struct epoll_event_handler* self, uint32_t events)
{
  printf("\n handle_midserver_socket_event");
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
    printf("\n accepted client");
  }
}

void decode_message_from_server(server_socket_event_data *server_data,char buffer[],unsigned int buf_size)
{
  printf("decode_message_from_server\n");
  tlv_chain tc;
  int num_groups,chain_size,i;
  uint16_t *groups=NULL;
  init_tlv_chain(&tc);

  tlv_chain_deserialise(buffer,&tc,buf_size);
  tlv_chain_print(&tc);
  chain_size = tc.used;
  for(i=0;i<chain_size;++i)
    process_signal_type(&(tc.object[i]),server_data);
  tlv_chain_free(&tc);
}

void handle_server_socket_event(struct epoll_event_handler* self, uint32_t events)
{
  printf("handle_server_socket_event");
  struct server_socket_event_data* closure = (struct server_socket_event_data* ) self->closure;

  char buffer[MAX_MSG];
  int bytes_read=0, bytes_to_read=MAX_MSG-1, read_this_time=0;

  if(events & EPOLLIN)
  {
    if((bytes_read = read_from_socket(self,buffer,bytes_to_read)) != FALSE)
    {
      printf("byr rd: %d",bytes_read);
      decode_message_from_server(self->closure,buffer,bytes_read);
   }
  }
  if ((events & EPOLLERR) | (events & EPOLLHUP) /*| (events & EPOLLRDHUP)*/) {
    printf("\n error occured");
    close_server_socket(self);
    return;
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
  printf("\n created server handle");
  return handle;                                                                                                                                                                                            
}

// needs to be moved to main function. is not server specific
void do_reactor_loop(int epoll_fd)                                                                                                                                                                          
{ 
  struct epoll_event current_epoll_event;                                                                                                                                                                   

  while (1) {                                                                                                                                                                                               
      struct epoll_event_handler* handler;
                                                                                                                                                                                                            
      epoll_wait(epoll_fd,&current_epoll_event, 1, -1);
      handler = (struct epoll_event_handler*) current_epoll_event.data.ptr;                                                                                                                                 
      if(handler->handle)
      handler->handle(handler, current_epoll_event.events);
  }
}
