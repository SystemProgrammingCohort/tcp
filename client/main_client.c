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
#include "tlv.h"
#include <errno.h>


int main(int argc, char *argv[])
{
  // For socket
  int sockfd;
  uint16_t *groups,num_groups,actual_num_groups=0;
  tlv_chain *tc;

  //message
  char msg[MAX_MSG];
  int bytes_sent=0,num_bytes=0,bytes_read,read_this_time,bytes_to_read;
  char buffer[MAX_MSG];

  int i,j;
  if(argc < 2)
  {
    printf("usage: client server-addr\n");
    return 1;
  }

  i=3;
  num_groups = atoi(argv[3]);
  ++i;

  printf("\n number og groups: %d",num_groups);

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

  if((sockfd = initClient(argv[1]))==-1)
    return 1;

  tc = encode_join_request(num_groups, groups);
  tlv_chain_print(tc);
  tlv_chain_serialise(tc,msg,&num_bytes);

  if(num_bytes > MAX_MSG)
  {
    printf("\n mssg too long");
    return 1;
  }
  bytes_sent = 0;
  printf("sending %s",msg);
//  while(bytes_sent != num_bytes)
  {
    bytes_sent += send(sockfd, msg, num_bytes, 0);
//    printf("\n sent bytes %d",bytes_sent);
  }

  bytes_read = 0;
  bytes_to_read = MAX_MSG-1;
  printf("\n Entering while loop");
    listen(sockfd,10);

   while((read_this_time = recv(sockfd, buffer + bytes_read, (bytes_to_read - bytes_read),0)) != 0)
   {
     if(read_this_time > 0)
     {
       printf("\n recieved: %d",read_this_time);
       buffer[read_this_time]='\0';
       printf("message recieved:%s",buffer);
     }
   }
    if(read_this_time == -1)
    {
      printf("recv failed");
    }
    if(read_this_time == 0 || read_this_time == -1) {
      printf("server disconnected");
    }

  close(sockfd);
  return 0;
}
