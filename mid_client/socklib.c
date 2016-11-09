#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "socklib.h"
#define SERVER_PORT "3905"
#define TRUE 1
#define FALSE 0

void make_socket_non_blocking(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
      perror("Couldn't get socket flags");
      exit(1);
  }

  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) == -1) {
      perror("Couldn't set socket flags");
      exit(-1);
  }
}

int getfd(struct addrinfo *hints, struct addrinfo **returnaddr, char *serverstring)
{                                                                                                                                                                                                           
  int status;
  if(hints == NULL || returnaddr == NULL || serverstring == NULL)                                                                                                                                                                   
    return -1;
  if((status = getaddrinfo(serverstring, SERVER_PORT, hints, returnaddr)) != 0){                                                                                                                                    
    fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(status));
    return -1;                                                                                                                                                                                              
  }
  return socket((*returnaddr)->ai_family, (*returnaddr)->ai_socktype, (*returnaddr)->ai_protocol);                                                                                                          
}

int bindSock(int sockfd, struct addrinfo *addr)                                                                                                                                                             
{ 
  if(addr==NULL)                                                                                                                                                                                            
    return -1;
  if(bind(sockfd, addr->ai_addr, addr->ai_addrlen) == -1)                                                                                                                                                   
  { 
    close(sockfd);                                                                                                                                                                                          
    perror("server:bind");
    return -1;                                                                                                                                                                                              
  }
}

int makeSockReusable(int sockfd)                                                                                                                                                                            
{ 
  int yes = TRUE;                                                                                                                                                                                           
  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {                                                                                                                                                                                                         
    perror("setsockopt");
    return -1;                                                                                                                                                                                              
  }
}
