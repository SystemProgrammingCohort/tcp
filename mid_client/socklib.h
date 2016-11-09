#ifndef __SOCKLIB_H_
#define __SOCKLIB_H_ 
#include <netdb.h>

void make_socket_non_blocking(int fd);
int getfd(struct addrinfo *hints, struct addrinfo **returnaddr, char *serverstring);
int bindSock(int sockfd, struct addrinfo *addr);
int makeSockReusable(int sockfd);

#endif /* __SOCKLIB_H_ */
