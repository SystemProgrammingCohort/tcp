#ifndef __PATRICIA_H__
#define __PATRICIA_H__

#define MAX_BIT_VALUE 0x200
#define MAX_BITS 10

typedef struct patricia_node
{
  unsigned int level;
  int key;
  void *val;
  struct patricia_node *left;
  struct patricia_node *right;
}patricia_node;

int kth_bit_value(int n,int k);
int differing_bit(int key1,int key2);
patricia_node* search_pnode(patricia_node *tree, int key);
int add_pnode(patricia_node **tree,int key, void *val);
int delete_pnode(struct patricia_node **tree, int key, void *val);
void display_ptree_inorder(struct patricia_node *tree,int previous_level);

#endif /* __PATRICIA_H__ */
