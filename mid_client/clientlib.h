#ifndef __CLIENTLIB_H__
#define __CLIENTLIB_H__

#define MAX_MULTICAST_GROUPS 1000
#define SERVER_PORT "3904"
#define MAX_MSG 20

void print_addr(struct addrinfo *addr);
int initClient(char *serverstring);

#endif /* __CLIENTLIB_H__ */
