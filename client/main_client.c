/******************
 * main(argc,multicast_groups,server_ip address)
 ******************/

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


int main(int argc, char *argv[])
{
  // For socket
  int sockfd;

  //message
  char msg[MAX_MSG];
  int bytes_sent=0;

  if(argc < 2)
  {
    printf("usage: client server-addr\n");
    return 1;
  }
  if((sockfd = initClient(argv[1]))==-1)
    return 1;

  while(1)
  {
    fgets(msg, MAX_MSG, stdin);
    bytes_sent = send(sockfd, msg, strlen(msg), 0);
    if(bytes_sent == -1)
    {
      perror("sendto");
      return 1;
    }
  }

  close(sockfd);
  return 0;
}
