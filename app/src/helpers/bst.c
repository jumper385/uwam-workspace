#include "bst.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bst_sys);

struct bst_node * bst_node_create (uint32_t key, struct ll_obj *data) {
    struct bst_node *tmp = malloc(sizeof(struct bst_node));
    tmp->key = key;
    tmp->data = data;
    tmp->left = NULL;
    tmp->right = NULL;
    return tmp;
}

struct bst_node * bst_node_insert(struct bst_node *node, uint32_t key, int data ) {
    if (node == NULL) {
        struct ll_obj *newList = malloc(sizeof(struct ll_obj));
        ll_obj_init(newList);
        ll_obj_push(newList, data);
        return bst_node_create(key, newList);
    } else if (key < node->key) {
        node->left = bst_node_insert(node->left, key, data);
    } else if (key > node -> key){
        node->right = bst_node_insert(node->right, key, data);
    } else if (key == node->key) {
        ll_obj_push(node->data, data);
    }
    return node;
}

struct bst_node * bst_node_search(struct bst_node *node, uint32_t key) {
    if (node == NULL || node->key == key) {
        return node;
    }

    if (key < node->key) {
        return bst_node_search(node->left, key);
    } else if (key > node->key){
        return bst_node_search(node->right, key);
    } else {
        return NULL;
    }
}
