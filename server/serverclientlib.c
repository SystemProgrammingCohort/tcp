#include <stdio.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include "socklib.h"
#include "serverlib.h"
#include "serverclientlib.h"
#include "double_list.h"
#include "patricia.h"

void close_client_socket(struct epoll_event_handler* self)
{
  printf("close_client_socket");
      close(self->fd);
      free(self->closure);
      free(self);
}

int read_from_socket(struct epoll_event_handler* self,char buffer[],int bytes_to_read)
{
  printf("read_from_socket");
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
  printf("mssg:%s",buffer);
  return TRUE;
}

// below code is only for checkingi whether add to group is working or not... need to remove once the code for encoding/decoding client messages is written
void decode_message_from_client(client_socket_event_data *client_data,char buffer[])
{
  printf("decode_message_from_client\n");
  int group=0,i=0,j;
  char char_group[10]={0};
  while(buffer[i]!='\0')
  {
    while(buffer[i]==' ')
      ++i;
    j=0;
    while(buffer[i]!=' ' && buffer[i]!='\0')
    {
      char_group[j]=buffer[i];
      ++i;
      ++j;
    }
    if(j > 4)
      return;
    char_group[j]='\0';
    group = atoi(char_group);
    printf("group:%d\n",group);
    add_client_to_group(client_data,group);
    printf("\n group list:");
    display_list(&(_global_groups_[group-1].clients_list));
    printf("\n ptree:");
    display_clients_groups(client_data->clients_groups);
  }
}

void handle_client_socket_event(struct epoll_event_handler* self, uint32_t events)
{
  printf("handle_client_socket_event");
  struct client_socket_event_data* closure = (struct client_socket_event_data* ) self->closure;

  char buffer[MAX_MSG];
  int bytes_read=0, bytes_to_read=MAX_MSG-1, read_this_time=0;

  if(events & EPOLLIN)
  {
    if(read_from_socket(self,buffer,bytes_to_read) == TRUE)
    {
//      printf("another:%s ",buffer);
      decode_message_from_client(self->closure,buffer);
    }
  }
  if ((events & EPOLLERR) | (events & EPOLLHUP) /*| (events & EPOLLRDHUP)*/) {
    printf("\n error occured");
    close_client_socket(self);
    return;
  }
}

struct epoll_event_handler* create_client_socket_handler(int client_socket_fd, int epoll_fd)
{
  make_socket_non_blocking(client_socket_fd);

  epoll_event_handler *client_handle = malloc(sizeof(struct epoll_event_handler));
  client_socket_event_data *closure = (client_socket_event_data *)malloc(sizeof(client_socket_event_data));

  closure->fd = client_socket_fd;
  closure->capacity = 0;
  closure->clients_groups = (struct client_group_ptree*)malloc(sizeof(struct client_group_ptree));
  closure->clients_groups->num_groups = 0;
  closure->clients_groups->root = NULL;

  client_handle->fd = client_socket_fd;
  client_handle->handle = handle_client_socket_event;
  client_handle->closure = closure;
  printf("\n created client handle");
  return client_handle;
}

void handle_client_connection(int epoll_fd, int client_socket_fd)
{
  epoll_event_handler *client_socket_event_handler;
  client_socket_event_handler = create_client_socket_handler(client_socket_fd, epoll_fd);
  add_epoll_handler(epoll_fd, client_socket_event_handler, (EPOLLIN /*| EPOLLRDHUP*/));
  printf("\n added client to epoll");
}

void add_client_to_group(client_socket_event_data *client,int group)
{
  struct dnode* group_node=NULL;
  if(group < 0 || group > MAX_MULTICAST_GROUPS)
    return;
  _global_groups_[group-1].number_of_clients += 1;
  group_node = add_dnode(&(_global_groups_[group-1].clients_list),group,client);
  if(!add_group_to_client(client->clients_groups,group,(void *)group_node))
    delete_dnode(&(_global_groups_[(group_node->group_number)-1].clients_list),&group_node);
  printf("\n added client to group");
}

int remove_all_groups_from_client(client_socket_event_data *client)
{
  printf("remove_all_groups_from_client");
  if(client == NULL)
    return 0;
  if(client->clients_groups == NULL)
    return 1;

  struct dnode* group_node=NULL;

  while((group_node = (struct dnode*)delete_rootgroup_from_client(client->clients_groups))!=NULL)
  {
    delete_dnode(&(_global_groups_[(group_node->group_number)-1].clients_list),&group_node);
  }
  return 1;
}

int remove_group_from_client(client_socket_event_data *client,int group_num)
{
  if(client == NULL)
    return 0;
  if(client->clients_groups == NULL)
    return 1;

  struct dnode* group_node=NULL;
  if((group_node = (struct dnode *)delete_group_from_client(client->clients_groups,group_num))!=NULL)
    delete_dnode(&(_global_groups_[(group_node->group_number)-1].clients_list),&group_node);
  return 1;
}

void init_clients_groups(struct client_group_ptree *p)
{
  if(p==NULL)
    return;
  p->num_groups = 0;
  p->root = NULL;
  printf("\n initialised client group");
}

//after creating the group node by adding client in group list, need to add that group pointer in client list
int add_group_to_client(struct client_group_ptree *p, int group, void *group_node)
{
  printf("\n adding group to client");
  if(p==NULL)
    return 0;
  if(p->num_groups == MAX_MULTICAST_GROUPS)
    return 0;
  if(add_pnode(&(p->root),group,group_node))
  {
    printf("\n added node");
    ++(p->num_groups);
    return 1;
  }
  return 0;
}

//returns group node after removing the group node from the group list in client
//need to delete from double list as well
void * delete_group_from_client(struct client_group_ptree *p, int group)
{
  printf("delete_group_from_client");
  if(p==NULL)
    return NULL;
  if(p->root == NULL)
    return NULL;
  void *group_node=NULL;
  if(delete_pnode(&(p->root),group,(void *)group_node))
    --(p->num_groups);
  return group_node;
}

void * delete_rootgroup_from_client(struct client_group_ptree *p)
{
  printf("delete_rootgroup_from_client");
  if(p==NULL)
    return NULL;
  if(p->root == NULL)
    return NULL;
  void *group_node=NULL;
  if(delete_pnode(&(p->root),p->root->key,(void *)group_node))
    --(p->num_groups);
  return group_node;
}

void display_clients_groups(struct client_group_ptree *p)
{
  printf("display_clients_groups");
  if(p==NULL || p->root == NULL)
  {
    printf("\n empty");
    return;
  }
  display_ptree_inorder(p->root->left,0);
  printf("done\n");
}
