#include <stdio.h>
#include <stdlib.h>
#include "patricia.h"

int kth_bit_value(int n,int k)
{
  int i= MAX_BIT_VALUE;
  i=i>>(k-1);
  return (n&i)>0;
}

int differing_bit(int key1,int key2)
{
  int i=MAX_BIT_VALUE,diff,first_bit=0;
  i=i-1;
  i=i<<1;
  i|=1;

  key1 = key1 & i;
  key2 = key2 & i;
  diff = key1 ^ key2;

  while(diff)
  {
    ++first_bit;
    diff = diff >> 1;
  }
  return (MAX_BITS - first_bit +1);
}

patricia_node* search_pnode(patricia_node *tree, int key)
{
  if(tree == NULL)
    return NULL;
  patricia_node *current, *next;
  current = tree;
  next = tree->left;

  while(next->level > current->level)
  {
    current = next;
    if(kth_bit_value(key,next->level))
      next = next->right;
    else
      next = next->left;
  }
  return next;
}

int add_pnode(patricia_node **tree,int key,void *val)
{
  if(tree == NULL)
    return 0;
  patricia_node *new_node=NULL, *t, *last_node=NULL,*next_node = NULL;
  static int num_node = 0;
  int diff_bit = 0;
  new_node = (struct patricia_node*)malloc(sizeof(struct patricia_node));
  new_node->key = key;
  new_node->val = val;
  new_node->left = new_node;
  ++num_node;

  t=*tree;
  if(t==NULL)
  {
    *tree = new_node;
    return 1;
  }

  last_node = search_pnode(t,key);
  if(last_node->key == key)
  {
    free(new_node);
    return 0;
  }
  diff_bit = differing_bit(last_node->key,key);
  new_node->level = diff_bit;

  t = *tree;
  last_node = t;
  next_node = t->left;

  while((last_node->level < next_node->level) && (next_node->level < diff_bit))
  {
    last_node = next_node;
    if(kth_bit_value(key,next_node->level))
      next_node = next_node->right;
    else
      next_node = next_node->left;
  }

  if(kth_bit_value(key,diff_bit))
  {
    new_node->right = new_node;
    new_node->left = next_node;
  }
  else
  {
    new_node->left = new_node;
    new_node->right = next_node;
  }

  if(last_node->left == next_node)
    last_node->left = new_node;
  else
    last_node->right = new_node;

  return 1;
}

int delete_pnode(struct patricia_node **tree, int key, void *val)
{
  if(tree == NULL)
    return 0;
  if(*tree == NULL)
    return 0;

  patricia_node *parent=NULL,*grandparent=NULL,*parent_pointer=NULL,*other_child=NULL,*child=NULL;
  int parent_key=-1;

  parent = *tree;
  child = (*tree)->left;

  while(child->level > parent->level)
  {
    grandparent = parent;
    parent = child;
    if(kth_bit_value(key,child->level))
      child=child->right;
    else
      child=child->left;
  }
  if(child->key != key)
    return 0;

  if(parent == *tree)
  {
    val = (*tree)->val;
    free(*tree);
    *tree = NULL;
    return 1;
  }

  if(child == parent->left)
    other_child = parent->right;
  else
    other_child = parent->left;

  if(other_child == parent)
    other_child = child;
  else
  {
    parent_pointer = (*tree)->left;
    parent_key = parent->key;

    while(parent_pointer->key != parent_key)
    {
      if(kth_bit_value(parent_key,parent_pointer->level))
        parent_pointer = parent_pointer->right;
      else
        parent_pointer = parent_pointer->left;
    }
    if(parent == parent_pointer->left)
      parent_pointer->left = child;
    else
      parent_pointer->right = child;
  }
  //check left first as root only has left node
  if(grandparent->left == parent)
    grandparent->left = other_child;
  else
    grandparent->right = other_child;
  val = child->val;
  child->val = parent->val;
  child->key = parent->key;
  free(parent);
}

void display_ptree_inorder(struct patricia_node *tree,int previous_level)
{
  if(tree == NULL)
  {
    return;
  }
  if(tree->level <= previous_level)
  {
    printf("%d \n",tree->key);
    return;
  }
  display_ptree_inorder(tree->left,tree->level);
  display_ptree_inorder(tree->right,tree->level);
}
