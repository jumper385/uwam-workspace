#include "ll.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ll);

void ll_obj_init(struct ll_obj *list) {
    list->head = NULL;
    list->tail = list->head;
}

void ll_obj_push(struct ll_obj *list, int data) {
    struct ll_node *new = malloc(sizeof(struct ll_node));
    new->data = data;

    if (list->head == NULL) {
        list->head = new;
        list->head->next = NULL;
    } else {
        new->next = list->head;
        list->head = new;
    }
}

void ll_node_append(struct ll_node *node, int data) {
    
    if (node->next == NULL) {
        struct ll_node *new = malloc(sizeof(struct ll_node));
        new->data = data;
        
        node->next = new;
    } else {
        ll_node_append(node->next, data);
    }
}

void ll_obj_append(struct ll_obj *list, int data) {
    ll_node_append(list->head, data);
}

void ll_print(struct ll_node *node) {
    printk("%d", node->data);
    if (node->next == NULL) {
        printk("\n");
    } else {
        printk(",");
        ll_print(node->next);
    }
}
