#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "linkedlist.h"

static void clean_list(linked_list_t list) {
    for (ll_iterator_t *it = ll_iterator_new(list); ll_has_next(it); ) {
        free(ll_next(it));
    }
}

int main() {
    char buffer[100];
    linked_list_t list = NULL;
    ll_iterator_t curr = NULL;

    while (scanf("%s", buffer) == 1) {
        if (!strcmp(buffer, "new")) {
            if (list) {
                clean_list(list);
                ll_destroy(list);
            }
            list = ll_new();
            assert(list);
        } else if (!strcmp(buffer, "append")) {
            assert(list);
            scanf("%s", buffer);
            char * string = strdup(buffer);
            int success = ll_add_back(list, string);
            if (!success) {
                printf("Append failed: '%s'\n", string);
                free(string);
            }
        } else if (!strcmp(buffer, "iterator")) {
            assert(list);
            if (curr != NULL) {
                ll_iterator_destroy(curr);
            }
            curr = ll_iterator_new(list);
            assert(curr);
        } else if (!strcmp(buffer, "hasnext")) {
            assert(list);
            assert(curr);
            if (ll_has_next(curr)) {
                printf("Has next\n");
            } else {
                printf("No next\n");
            }
        } else if (!strcmp(buffer, "next")) {
            assert(list);
            assert(curr);
            if (ll_has_next(curr)) {
                printf("Next: %s\n", (char *) ll_next(curr));
            } else {
                printf("Next failed\n");
            }
        } else if (!strcmp(buffer, "destroy")) {
            clean_list(list);
            ll_destroy(list);
            list = NULL;
            if (curr) {
                ll_iterator_destroy(curr);
                curr = NULL;
            }
        } else if (!strcmp(buffer, "find")) {
            assert(list);
            assert(curr);

            char *found  = NULL;
            scanf("%s", buffer);

            while (ll_has_next(curr)) {
                char *item = ll_next(curr);
                if(!strcmp(buffer, item)) {
                    found = item;
                    break;
                }
            }

            if (!found) {
                printf("Found failed: '%s'\n", buffer);
            } else {
                printf("Found: '%s'\n", buffer);
            }
        } else if (!strcmp(buffer, "add")) {
            assert(list);
            assert(curr);

            scanf("%s", buffer);
            char *string = strdup(buffer);
            assert(string);

            if (ll_add(curr, string)) {
                printf("Added: '%s'\n", buffer);
            } else {
                printf("Add failed\n");
                free(string);
            }
        } else if (!strcmp(buffer, "set")) {
            assert(list);
            assert(curr);

            scanf("%s", buffer);
            char *string = strdup(buffer);
            assert(string);

            char *old = ll_set(curr, string);

            if (old) {
                printf("Set: '%s' -> '%s'\n", old, buffer);
                free(old);
            } else {
                printf("Set failed\n");
                free(string);
            }
        } else if (!strcmp(buffer, "remove")) {
            assert(list);
            assert(curr);

            char *old = ll_remove(curr);

            if (old) {
                printf("Removed: '%s'\n", old);
                free(old);
            } else {
                printf("Remove failed\n");
            }
        } else {
            break;
        }

        if (list) {
            printf("[");
            for (ll_iterator_t it = ll_iterator_new(list); ll_has_next(it); ) {
                printf(" %s", (char *) ll_next(it));
            }
            printf(" ]\n");
        } else {
            printf("No list!\n");
        }
    }
    return 0;
}