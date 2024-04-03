#ifndef __LL_OBJ_H_
#define __LL_OBJ_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct ll_obj {
    struct ll_node *head;
    struct ll_node *tail;
};

struct ll_node {
    int data;
    struct ll_node *next;
};

void ll_obj_init(struct ll_obj *list);
void ll_obj_push(struct ll_obj *list, int data);
void ll_obj_append(struct ll_obj *list, int data);
void ll_print(struct ll_node *node);
#endif
