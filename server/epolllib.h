#ifndef __EPOLLLIB_H__
#define __EPOLLLIB_H__

typedef struct epoll_event_handler                                                                                                                                                                          
{ 
  int fd;                                                                                                                                                                                                   
  void (*handle)(struct epoll_event_handler*, uint32_t);
  void* closure;                                                                                                                                                                                            
}epoll_event_handler;

void add_epoll_handler(int epoll_fd, epoll_event_handler * handler, uint32_t event_mask);

#endif /* __EPOLLLIB_H__ */
