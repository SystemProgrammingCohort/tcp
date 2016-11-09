#ifndef __SOCKLIB_H_
#define __SOCKLIB_H_ 
#include <netdb.h>

#define MIDDLE_SERVER_PORT "3905"
#define SERVER_PORT "3904"
#define MAX_LISTEN_BACKLOG 4096
#define TRUE 1
#define FALSE 0

void make_socket_non_blocking(int fd);
int getfd(struct addrinfo *hints, struct addrinfo **returnaddr,char *serverstring, const char *port);
int bindSock(int sockfd, struct addrinfo *addr);
int makeSockReusable(int sockfd);

#endif /* __SOCKLIB_H_ */
