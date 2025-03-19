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

// Create a new iterator for linked list
extern ll_iterator_t ll_iterator_new(linked_list_t list) {
    assert(list);
    linked_list *actual_list = (linked_list *)list;

    // Allocate memory for iterator
    ll_iterator *iter = (ll_iterator *)malloc(sizeof(ll_iterator));
    if (!iter) return NULL; // Return NULL if memory allocation fails

    iter->current = NULL;  // Start iterator before first element
    iter->last_returned = NULL; // No element has been returned yet
    iter->list = actual_list; // Associate iterator with list

    return (ll_iterator_t)iter;
}

// Destroy the iterator and free allocated memory
extern void ll_iterator_destroy(ll_iterator_t iter) {
    assert(iter);
    free((ll_iterator *)iter);
}

// Check if there is a next element available in list
extern int ll_has_next(ll_iterator_t iter) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    // If current is NULL, check if list has elements
    if (it->current == NULL) {
        return it->list->head != NULL;
    }

    // Otherwise, check if current node has a next node
    return it->current->next != NULL;
}

// Retrieve the next element in list
extern void *ll_next(ll_iterator_t iter) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    // If `current` is NULL (possibly due to removal), move to head
    if (it->current == NULL) {
        if (it->list->head != NULL) {
            it->current = it->list->head;  // Start from head
        } else {
            return NULL;  // List is empty
        }
    } else {
        // Move to next node
        if (it->current->next != NULL) {
            it->current = it->current->next;
        } else {
            return NULL;  // End of list reached
        }
    }

    it->last_returned = it->current; // Store last returned node
    return it->current->item; // Return item stored in the node
}

// Add an item before current position of iterator
extern int ll_add(ll_iterator_t iter, void *item) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    // Allocate memory for new node
    ll_node_t *new_node = (ll_node_t *)malloc(sizeof(ll_node_t));
    if (!new_node) return 0; // Return failure if allocation fails
    new_node->item = item;

    // Case 1: If iterator hasn't moved yet, insert at head
    if (it->current == NULL || it->current == it->list->head) {
        new_node->next = it->list->head;
        new_node->prev = NULL;

        if (it->list->head) {
            it->list->head->prev = new_node;
        } else {
            it->list->tail = new_node; // If list was empty
        }

        it->list->head = new_node;
    } else {
        // Case 2: Insert before `current`
        new_node->prev = it->current->prev;
        new_node->next = it->current;

        if (it->current->prev) {
            it->current->prev->next = new_node;
        } else {
            it->list->head = new_node;
        }

        it->current->prev = new_node;
    }

    return 1; // Successfully added
}

// Set the value of last returned element
extern void *ll_set(ll_iterator_t iter, void *item) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    // Ensure `next()` was called before setting a value
    if (!it->last_returned) return NULL;

    void *old_item = it->last_returned->item; // Store old item
    it->last_returned->item = item; // Replace with new item
    return old_item; // Return old item
}

// Remove last returned element from list
extern void *ll_remove(ll_iterator_t iter) {
    assert(iter);
    ll_iterator *it = (ll_iterator *)iter;

    // Ensure `next()` was called before removing
    if (!it->last_returned) {
        return NULL;
    }

    ll_node_t *node = it->last_returned;
    void *item = node->item; // Store item to be returned

    // Move iterator to a valid position before removing node
    if (it->current == node) {
        it->current = node->next ? node->next : node->prev;
    }

    // Update linked list pointers
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        it->list->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        it->list->tail = node->prev;
    }

    free(node); // Free memory of the removed node
    it->last_returned = NULL; // Reset `last_returned`

    return item; // Return removed item
}





