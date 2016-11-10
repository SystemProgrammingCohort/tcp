#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "socklib.h"
#include "clientlib.h"

void print_addr(struct addrinfo *addr)
{
  if(addr == NULL)
    return;
  char ipstr[INET6_ADDRSTRLEN];
  for(;addr!=NULL;addr=addr->ai_next)
  {
    void *address;
    char *ipver;
    if(addr->ai_family == AF_INET)
    {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)addr->ai_addr;
      address = &(ipv4->sin_addr);
      ipver = "IPv4";
    }else
    {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)addr->ai_addr;
      address = &(ipv6->sin6_addr);
      ipver = "IPv6";
    }

    inet_ntop(addr->ai_family, address, ipstr, sizeof ipstr);
    printf("%s:%s\n",ipver,ipstr);
  }
}

int initClient(char *serverstring)
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

  if((sockfd = getfd(&hints, &server_addr, serverstring)) == -1)
  {
  //  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    printf("nope!");
    exit(-1);
  }
  print_addr(server_addr);

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
    exit(-1);
  }
  make_socket_non_blocking(sockfd);
  freeaddrinfo(server_addr);
  return sockfd;
}


