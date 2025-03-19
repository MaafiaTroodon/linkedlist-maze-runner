//
// Created by Alex Brodsky on 2023-09-20.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"

typedef struct ll_node {
    struct ll_node *prev;
    struct ll_node *next;
    void *item;
} ll_node_t;

typedef struct linked_list {
    ll_node_t *head;
    ll_node_t *tail;
} linked_list;


// Structure for the iterator
typedef struct ll_iterator {
    ll_node_t *current;
    ll_node_t *last_returned;
    linked_list *list;
} ll_iterator;


extern linked_list_t ll_new() {
    linked_list_t * list = calloc(1,sizeof(linked_list_t));
    return list;
}

extern void ll_destroy(linked_list_t ll) {
    linked_list  *list = (linked_list *)ll;
    assert(list != NULL);

    while (list->head != NULL) {
        ll_node_t *tmp = list->head;
        list->head = list->head->next;
        free(tmp);
    }

    free(list);
}

static ll_node_t *new_node(void *item, ll_node_t *prev, ll_node_t *next) {
    ll_node_t *n = calloc(1, sizeof(ll_node_t));
    if (n != NULL) {
        n->item = item;
        n->prev = prev;
        n->next = next;
    }
    return n;
}


extern int ll_add_front(linked_list_t ll, void *item) {
    linked_list  *list = (linked_list *)ll;
    assert(list);

    ll_node_t *n = new_node(item,NULL, list->head);
    if (n != NULL) {
        if (list->tail == NULL) {
            list->tail = n;
        } else {
            list->head->prev = n;
        }
        list->head = n;
    }
    return n != NULL;
}

extern int ll_add_back(linked_list_t ll, void *item) {
    linked_list  *list = (linked_list *)ll;
    assert(list);

    ll_node_t *n = new_node(item,list->tail, NULL);
    if (n != NULL) {
        if (list->head == NULL) {
            list->head = n;
        } else {
            list->tail->next = n;
        }
        list->tail = n;
    }
    return n != NULL;
}

extern void * ll_remove_front(linked_list_t ll) {
    linked_list  *list = (linked_list *)ll;
    assert(list);

    void *item = NULL;
    if (list->head != NULL) {
        ll_node_t *n = list->head;
        list->head = list->head->next;
        if (list->head == NULL) {
            list->tail = NULL;
        } else {
            list->head->prev = NULL;
        }

        item = n->item;
        free(n);
    }
    return item;
}

extern void * ll_remove_back(linked_list_t ll) {
    linked_list  *list = (linked_list *)ll;
    assert(list);

    void *item = NULL;
    if (list->tail != NULL) {
        ll_node_t *n = list->tail;
        list->tail = list->tail->prev;
        if (list->tail == NULL) {
            list->head = NULL;
        } else {
            list->tail->next = NULL;
        }

        item = n->item;
        free(n);
    }
    return item;
}

extern void * ll_get_front(linked_list_t ll) {
    linked_list  *list = (linked_list *)ll;
    assert(list);

    void *item = NULL;
    if (list->head != NULL) {
        item = list->head->item;
    }
    return item;
}

extern void * ll_get_back(linked_list_t ll) {
    linked_list  *list = (linked_list *)ll;
    assert(list);

    void *item = NULL;
    if (list->head != NULL) {
        item = list->tail->item;
    }
    return item;
}

extern int ll_size(linked_list_t ll) {
    linked_list  *list = (linked_list *)ll;
    assert(list);
    int count = 0;
    for (ll_node_t *ptr = list->head; ptr != NULL; ptr = ptr->next) {
        count++;
    }
    return count;
}

extern int ll_is_empty(linked_list_t ll) {
    linked_list  *list = (linked_list *)ll;
    assert(list);
    return list->head == NULL;
}

/************************************************************************
 * Your code below
 ************************************************************************/

// Create a new iterator for the list
extern ll_iterator_t ll_iterator_new(linked_list_t list) {
    assert(list);
    linked_list *actual_list = (linked_list *)list;
    ll_iterator *iter = (ll_iterator *)malloc(sizeof(ll_iterator));
    if (!iter) return NULL;

    iter->current = NULL;  // Start BEFORE the first element
    iter->last_returned = NULL;
    iter->list = actual_list;

    return (ll_iterator_t)iter;
}


// Destroy the iterator
extern void ll_iterator_destroy(ll_iterator_t iter) {
    assert(iter);
    free((ll_iterator *)iter);
}

// Check if there is a next element
extern int ll_has_next(ll_iterator_t iter) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    // If current is NULL, check if the list is not empty
    if (it->current == NULL) {
        return it->list->head != NULL;
    }

    // Otherwise, check if there's a next element
    return it->current->next != NULL;
}

extern void *ll_next(ll_iterator_t iter) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    if (it->current == NULL) {
        it->current = it->list->head;
    } else {
        it->current = it->current->next;
    }

    if (it->current == NULL) {
        return NULL;  // Prevents invalid access if list ends
    }

    it->last_returned = it->current;
    return it->current->item;
}

// Add an item before the current position of the iterator
extern int ll_add(ll_iterator_t iter, void *item) {
    assert(iter);
    ll_iterator *iter_struct = (ll_iterator *)iter;

    ll_node_t *new_node = (ll_node_t *)malloc(sizeof(ll_node_t));
    if (!new_node) return 0;
    new_node->item = item;

    // Case 1: If iterator hasn't moved yet (before first element), insert at index 1
    if (iter_struct->current == iter_struct->list->head || iter_struct->current == NULL) {
        if (iter_struct->list->head == NULL) {
            // If the list is empty, set new node as head and tail
            new_node->prev = NULL;
            new_node->next = NULL;
            iter_struct->list->head = iter_struct->list->tail = new_node;
        } else {
            // Insert AFTER the head (index 1)
            new_node->prev = iter_struct->list->head;
            new_node->next = iter_struct->list->head->next;

            if (iter_struct->list->head->next) {
                iter_struct->list->head->next->prev = new_node;
            } else {
                iter_struct->list->tail = new_node; // If there was only one element
            }

            iter_struct->list->head->next = new_node;
        }
    }
        // Case 2: Insert before `current`
    else {
        new_node->prev = iter_struct->current->prev;
        new_node->next = iter_struct->current;

        if (iter_struct->current->prev) {
            iter_struct->current->prev->next = new_node;
        } else {
            iter_struct->list->head = new_node; // If adding at the head
        }

        iter_struct->current->prev = new_node;
    }

    return 1;
}


// Set the value of the last returned element
extern void *ll_set(ll_iterator_t iter, void *item) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    if (!it->last_returned) return NULL; // Must have called `next()` before setting

    void *old_item = it->last_returned->item;
    it->last_returned->item = item;
    return old_item;
}

// Remove the last returned element
extern void *ll_remove(ll_iterator_t iter) {
    assert(iter);
    ll_iterator *iter_struct = (ll_iterator *)iter;

    if (!iter_struct->last_returned) {
        return NULL;  // Prevents removing without calling `next()`
    }

    ll_node_t *node = iter_struct->last_returned;
    void *item = node->item;

    // Move iterator forward if needed
    if (iter_struct->current == node) {
        iter_struct->current = node->next;  // Move to the next node if possible
    }

    // Update list pointers
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        iter_struct->list->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        iter_struct->list->tail = node->prev;
    }

    // If list is empty, reset iterator
    if (iter_struct->list->head == NULL) {
        iter_struct->current = NULL;
    }

    iter_struct->last_returned = NULL;  // Reset last_returned
    free(node);

    return item;
}