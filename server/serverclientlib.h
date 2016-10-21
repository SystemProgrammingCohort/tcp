#ifndef __SERVERCLIENTLIB_H__
#define __SERVERCLIENTLIB_H__

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

void close_client_socket(struct epoll_event_handler* self);
void handle_client_socket_event(struct epoll_event_handler* self, uint32_t events);
struct epoll_event_handler* create_client_socket_handler(int client_socket_fd, int epoll_fd);
void handle_client_connection(int epoll_fd, int client_socket_fd);

#endif /* __SERVERCLIENTLIB_H__ */
