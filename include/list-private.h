#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#include "list.h"

struct node_t {
    struct entry_t *value;
    struct node_t *next;
};

struct list_t {
    struct node_t *head;
};

#endif
