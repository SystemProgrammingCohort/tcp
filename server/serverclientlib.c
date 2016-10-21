#include <stdio.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include "socklib.h"
#include "serverlib.h"

void close_client_socket(struct epoll_event_handler* self)
{
      close(self->fd);
      free(self->closure);
      free(self);
}

int read_from_socket(struct epoll_event_handler* self,char buffer[],int bytes_to_read)
{
  int bytes_read=0,read_this_time=0;
  if(bytes_to_read > (MAX_MSG - 1))
  {
    return FALSE;
  }

//  while(bytes_read < bytes_to_read)
//  {
    read_this_time = read(self->fd, buffer + bytes_read, (bytes_to_read - bytes_read));
    if (read_this_time == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      return FALSE;
    }
    if (read_this_time == 0 || read_this_time == -1) {
      close_client_socket(self);
      return FALSE;
    }
    bytes_read+=read_this_time;
//  }
  buffer[bytes_read+1]='\0';
  printf("msg: %s",buffer);
  return TRUE;
}

void handle_client_socket_event(struct epoll_event_handler* self, uint32_t events)
{
  struct client_socket_event_data* closure = (struct client_socket_event_data* ) self->closure;

  char buffer[MAX_MSG];
  int bytes_read=0, bytes_to_read=MAX_MSG-1, read_this_time=0;

  if(events & EPOLLIN)
  {
    if(read_from_socket(self,buffer,bytes_to_read) == TRUE)
    {
//      handle_fromclient_message(self->fd,buffer);
    }
  }
  if ((events & EPOLLERR) | (events & EPOLLHUP) /*| (events & EPOLLRDHUP)*/) {
    close_client_socket(self);
    return;
  }
}

struct epoll_event_handler* create_client_socket_handler(int client_socket_fd, int epoll_fd)
{
  make_socket_non_blocking(client_socket_fd);

  struct epoll_event_handler* result = malloc(sizeof(struct epoll_event_handler));
  result->fd = client_socket_fd;
  result->handle = handle_client_socket_event;
  result->closure = NULL;
}

void handle_client_connection(int epoll_fd, int client_socket_fd)
{
  struct epoll_event_handler* client_socket_event_handler;
  client_socket_event_handler = create_client_socket_handler(client_socket_fd, epoll_fd);
  add_epoll_handler(epoll_fd, client_socket_event_handler, (EPOLLIN /*| EPOLLRDHUP*/));
}
