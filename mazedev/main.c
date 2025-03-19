#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "linkedlist/linkedlist.h"

// Structure to represent a room in the maze
typedef struct room {
    char *name;         // Name of the room
    linked_list_t doors; // List of adjacent rooms (doors connecting to other rooms)
} room_t;

// Helper function to find a room by name in the linked list
room_t *find_room(linked_list_t rooms, const char *name) {
    if (ll_is_empty(rooms)) return NULL;  // Prevents iterating an empty list

    ll_iterator_t iter = ll_iterator_new(rooms); // Create an iterator for the room list
    while (ll_has_next(iter)) {
        room_t *room = ll_next(iter);  // Get the next room
        if (strcmp(room->name, name) == 0) { // Compare room names
            ll_iterator_destroy(iter);  // Free iterator memory
            return room;
        }
    }
    ll_iterator_destroy(iter); // Free iterator if not found
    return NULL;
}

int main() {
    char buffer[100];  // Buffer for reading input
    linked_list_t rooms = ll_new(); // Create a new linked list for rooms

    // Reading room names from input until "END"
    while (scanf("%s", buffer) == 1 && strcmp(buffer, "END") != 0) {
        room_t *new_room = malloc(sizeof(room_t));  // Allocate memory for a new room
        assert(new_room);  // Ensure memory allocation succeeded
        new_room->name = strdup(buffer);  // Copy room name
        new_room->doors = ll_new();  // Initialize linked list for doors
        ll_add_back(rooms, new_room);  // Add room to the linked list
    }

    // Reading connections (doors) between rooms
    while (scanf("%s", buffer) == 1 && strcmp(buffer, "END") != 0) {
        char source[100], destination[100];
        strcpy(source, buffer);

        if (scanf("%s", destination) != 1) {  // Read destination room
            printf("Oops: Missing destination room.\n");
            continue;
        }

        room_t *src_room = find_room(rooms, source); // Find source room
        room_t *dest_room = find_room(rooms, destination); // Find destination room

        if (!src_room || !dest_room) {
            if (!src_room) {
                printf("Oops: Cannot find room %s.\n", source);
            }
            if (!dest_room) {
                printf("Oops: Cannot find room %s.\n", destination);
            }
            continue;
        }


        // Ensure door list is initialized
        if (!src_room->doors) {
            src_room->doors = ll_new();
        }

        ll_add_back(src_room->doors, dest_room); // Add door connection to the source room
    }

    // Consume any extra newline after "END"
    while (getchar() != '\n' && !feof(stdin));

    int move_failed = 0;  //  Track if a move has failed

    // Read move sequence
    room_t *current_room = NULL;
    while (scanf("%s", buffer) == 1 && strcmp(buffer, "END") != 0) {
        if (!current_room) {
            current_room = find_room(rooms, buffer); // Set starting room
            if (!current_room) {  // Error handling for missing room
                printf("Oops: Cannot find room %s.\n", buffer);
                move_failed = 1;  //  Move failed
                break;
            }
            printf("Entering room %s.\n", current_room->name);
        } else {
            room_t *next_room = find_room(rooms, buffer); // Find next room
            if (!next_room) {
                printf("Oops: Cannot find room %s.\n", buffer);
                move_failed = 1;  //  Move failed
                continue;  //  Keep checking other inputs instead of breaking immediately
            }


            // Check if there's a valid connection (door) from current_room to next_room
            int valid_move = 0;
            ll_iterator_t iter = ll_iterator_new(current_room->doors);
            while (ll_has_next(iter)) {
                if (ll_next(iter) == next_room) { // If found, move is valid
                    valid_move = 1;
                    break;
                }
            }
            ll_iterator_destroy(iter); // Free iterator

            if (!valid_move) { // If no door exists between rooms, print error
                printf("Oops: Room %s does not connect to room %s.\n", current_room->name, next_room->name);
                move_failed = 1;  //  Move failed
                break;
            }

            printf("Entering room %s.\n", next_room->name);
            current_room = next_room; // Update current room
        }
    }

    // Print "Ended in room X." **only if all moves succeeded**
    if (!move_failed && current_room) {
        printf("Ended in room %s.\n", current_room->name);
    }

    // Free allocated memory and destroy linked lists
    ll_iterator_t iter = ll_iterator_new(rooms);
    while (ll_has_next(iter)) {
        room_t *room = ll_next(iter);
        ll_destroy(room->doors); // Destroy doors list
        free(room->name); // Free room name
        free(room); // Free room structure
    }
    ll_iterator_destroy(iter);
    ll_destroy(rooms); // Destroy main room list

    return 0;
}
