#include <sys/epoll.h>
#include <stdlib.h>
#include "epolllib.h"

void add_epoll_handler(int epoll_fd, epoll_event_handler * handler, uint32_t event_mask)
{   
    struct epoll_event event;

    event.data.ptr = (void *)handler;
    event.events = event_mask;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->fd, &event) == -1) {
        perror("Couldn't register server socket with epoll");
        exit(-1);
    }
}
