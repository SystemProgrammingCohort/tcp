#include <stdio.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include "socklib.h"
#include "serverlib.h"
#include "serverclientlib.h"
#include "double_list.h"
#include "patricia.h"
#include "signals.h"
#include "tlv.h"

int tcpfd;
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
    printf("bytes read: %d",bytes_read);
  return bytes_read;
}

void decode_message_from_client(client_socket_event_data *client_data,char buffer[],unsigned int buf_size)
{
  printf("decode_message_from_client\n");
  tlv_chain tc;
  int num_groups,chain_size,i;
  uint16_t *groups=NULL;
  init_tlv_chain(&tc);

  tlv_chain_deserialise(buffer,&tc,buf_size);
  tlv_chain_print(&tc);
  chain_size = tc.used;
  for(i=0;i<chain_size;++i)
    process_signal_type(&(tc.object[i]),client_data);
  tlv_chain_free(&tc);
}

void handle_client_socket_event(struct epoll_event_handler* self, uint32_t events)
{
  printf("handle_client_socket_event");
  struct client_socket_event_data* closure = (struct client_socket_event_data* ) self->closure;

  char buffer[MAX_MSG];
  int bytes_read=0, bytes_to_read=MAX_MSG-1, read_this_time=0;

  if(events & EPOLLIN)
  {
    if((bytes_read = read_from_socket(self,buffer,bytes_to_read)) != FALSE)
    {
      printf("byr rd: %d",bytes_read);
      decode_message_from_client(self->closure,buffer,bytes_read);
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

int  add_client_to_group(client_socket_event_data *client,int group)
{
  struct dnode* group_node=NULL;
  if(group < 0 || group > MAX_MULTICAST_GROUPS)
    return 0;
  _global_groups_[group-1].number_of_clients += 1;
  group_node = add_dnode(&(_global_groups_[group-1].clients_list),group,client);
  if(group_node == NULL)
    return 0;
  if(!add_group_to_client(client->clients_groups,group,(void *)group_node))
  {
    delete_dnode(&(_global_groups_[(group_node->group_number)-1].clients_list),&group_node);
    return 0;
  }
  printf("\n added client to group");
  return 1;
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
//  display_ptree_inorder(p->root->left,0);
  printf("done\n");
}

void process_signal_type(tlv *tlv,client_socket_event_data *client_data)
{
  int i;
  uint16_t *groups=NULL,group,num_groups;
  tlv_chain *tc=NULL;
  uint16_t num_success_groups=0,*success_group_list=NULL;
  char msg[MAX_MSG];
  int num_bytes=0,bytes_sent;

  switch(tlv->type)
  {
    case JOIN_REQUEST: decode_join_request(tlv,&num_groups,&groups);
                       success_group_list = (uint16_t *)malloc(sizeof(uint16_t)*num_groups);
                       printf("\n Number of groups:%d\n",num_groups);
                       for(i=0;i<num_groups;++i)
                       {
                         group = *(groups+i);
                         printf("\n Group: %d\n",group);
                         if(add_client_to_group(client_data,group))
                         {
                         printf("\n group list:");
                         *(success_group_list+num_success_groups) = group;
                         ++num_success_groups;
                         }
                         display_list(&(_global_groups_[group-1].clients_list));
                         printf("\n ptree:");
                         display_clients_groups(client_data->clients_groups);
                       }
                       printf("\n Sending Accept");
                       tc = encode_join(JOIN_ACCEPT,num_success_groups,success_group_list);
                       tlv_chain_serialise(tc,msg,&num_bytes);
                       bytes_sent = send(client_data->fd,msg,num_bytes,0);
                       tlv_chain_free(tc);
                       tc = encode_join(JOIN_REQUEST,num_success_groups, success_group_list);
                       tlv_chain_serialise(tc,msg,&num_bytes);
                       tlv_chain_free(tc);
                       if(num_bytes > MAX_MSG)
                       {
                         //needs to be handled more... needs to send mssg to client that mid-server unable to join that grp in main server.
                         printf("\n mssg too long");
                         free(success_group_list);
                         return;
                       }
                       bytes_sent = 0;
                       printf("sending to main server%s",msg);
                       send(tcpfd, msg, num_bytes, 0);
                       free(success_group_list);
                       printf("\n bytes sent: %d",bytes_sent);
                       break;
    case JOIN_ACCEPT: break;
    case QUES_SOLVE: printf("\n I need to solve this question");
                     send(client_data->fd,"Solve it\0",8,0);
                     break;
    case QUES_ANSWER: printf("\n recieved answer from client");
                      break;
    case QUES_ERROR_INVALID: printf("\n recieved messge from client thet question is invalid");
                             break;
    case QUES_ERROR_LIMITED_RESOURCE: printf("\n client cannot solve it right now");
                                      break;
    case QUES_ERROR_ANSWER_INVALID: printf("\n the answer that I gave was invalid, will probably remove myself from that group");
                                    break;
    case QUES_ERROR_OTHER: printf("\n recieved this error from client");
                           break;
    case INVALID_CLIENT: printf("\n I sent some messge for a group I was not supposed to send");
                         break;
    default: printf("\n received invalid signal");
             break;
  }
}
