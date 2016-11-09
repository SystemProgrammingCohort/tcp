#ifndef __DOUBLE_LIST_H__
#define __DOUBLE_LIST_H__

struct dnode                                                                                                                                                                                                 
{ 
  int group_number; // added much later, when realised that it is required to modify stats in that groupnumbers main node.
  void *val;                                                                                                                                                                                                
  struct dnode *prev;
  struct dnode *next;                                                                                                                                                                                        
};
                                                                                                                                                                                                            
struct double_list
{                                                                                                                                                                                                           
  struct dnode *first;
  struct dnode *last;                                                                                                                                                                                        
};

struct dnode* add_dnode(struct double_list *list,int group_number,void *val);
void init_list(struct double_list *list);
int delete_dnode(struct double_list *list, struct dnode **target_node);
void display_list(struct double_list *list);

#endif /* __DOUBLE_LIST_H__ */
