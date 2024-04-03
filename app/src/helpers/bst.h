#ifndef __BST_H_
#define __BST_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ll.h"

/**
 * struct bst_node *root = NULL;
 * root = bst_node_insert(root, 3, 33);
 */

struct bst_node {
    uint32_t key; 
    struct ll_obj *data;
    struct bst_node *left, *right; 
};

struct bst_node * bst_node_create (uint32_t key, struct ll_obj *data);
struct bst_node * bst_node_insert(struct bst_node *node, uint32_t key, int data );
struct bst_node * bst_node_search(struct bst_node *node, uint32_t key);

#endif
