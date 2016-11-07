#ifndef __SERVERCLIENTLIB_H__
#define __SERVERCLIENTLIB_H__
#include "patricia.h"
#include "tlv.h"

typedef enum MssgType
{
  JOIN_GROUP,
  EXIT_GROUP,
  DATA
}MssgType;

typedef enum ClientErrorType
{                                                                                                                                                                                                           
  LIMITED_RESOURCES,
  INCAPABLE                                                                                                                                                                                                 
}ClientError;

struct client_group_ptree
{
  unsigned int num_groups;
  patricia_node *root;
};

typedef struct client_socket_event_data
{
  int fd;
  int capacity;
  struct client_group_ptree *clients_groups;
}client_socket_event_data;

void close_client_socket(struct epoll_event_handler* self);
void handle_client_socket_event(struct epoll_event_handler* self, uint32_t events);
struct epoll_event_handler* create_client_socket_handler(int client_socket_fd, int epoll_fd);
void handle_client_connection(int epoll_fd, int client_socket_fd);
int add_client_to_group(client_socket_event_data *client,int group);

void init_clients_groups(struct client_group_ptree *p);
int add_group_to_client(struct client_group_ptree *p, int group, void *val);
void * delete_group_from_client(struct client_group_ptree *p, int group);
void display_clients_groups(struct client_group_ptree *p);
void * delete_rootgroup_from_client(struct client_group_ptree *p);

void process_signal_type(tlv *tlv,client_socket_event_data *client_data);
#endif /* __SERVERCLIENTLIB_H__ */
