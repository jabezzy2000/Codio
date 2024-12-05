// list.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

// ===================== User List Functions =====================

// Insert a user at the beginning of the user list
struct node* insertFirstU(struct node *head, int socket, char *username) {
    if(findU(head, username) == NULL) {
        // Create a new user node
        struct node *link = (struct node*) malloc(sizeof(struct node));
        if(!link){
            perror("Failed to allocate memory for new user");
            exit(EXIT_FAILURE);
        }
        link->socket = socket;
        strncpy(link->username, username, sizeof(link->username) - 1);
        link->username[sizeof(link->username) - 1] = '\0'; // Ensure null-termination
        link->next = head;
        head = link;
    }
    else {
        printf("Duplicate user: %s\n", username);
    }
    return head;
}

// Find a user by username
struct node* findU(struct node *head, char* username) {
    struct node* current = head;
    while(current != NULL) {
        if(strcmp(current->username, username) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Remove a user from the user list
struct node* removeUser(struct node *head, char *username) {
    struct node *current = head;
    struct node *prev = NULL;
    while(current != NULL) {
        if(strcmp(current->username, username) == 0) {
            if(prev == NULL) {
                head = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            printf("User '%s' removed from user list.\n", username);
            return head;
        }
        prev = current;
        current = current->next;
    }
    printf("User '%s' not found in user list.\n", username);
    return head;
}

// ===================== Room List Functions =====================

// Insert a room at the beginning of the room list
struct room_node* insertFirstR(struct room_node *head, char *roomname) {
    if(findR(head, roomname) == NULL) {
        // Create a new room node
        struct room_node *link = (struct room_node*) malloc(sizeof(struct room_node));
        if(!link){
            perror("Failed to allocate memory for new room");
            exit(EXIT_FAILURE);
        }
        strncpy(link->roomname, roomname, sizeof(link->roomname) - 1);
        link->roomname[sizeof(link->roomname) - 1] = '\0'; // Ensure null-termination
        link->users = NULL;
        link->next = head;
        head = link;
    }
    else {
        printf("Duplicate Room: %s\n", roomname);
    }
    return head;
}

// Find a room by roomname
struct room_node* findR(struct room_node *head, char* roomname) {
    struct room_node* current = head;
    while(current != NULL) {
        if(strcmp(current->roomname, roomname) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Add a user to a room
void addUserToRoom(struct room_node *room, char *username, int socket) {
    if(findU(room->users, username) == NULL) {
        struct node *link = (struct node*) malloc(sizeof(struct node));
        if(!link){
            perror("Failed to allocate memory for user in room");
            exit(EXIT_FAILURE);
        }
        link->socket = socket;
        strncpy(link->username, username, sizeof(link->username) - 1);
        link->username[sizeof(link->username) - 1] = '\0';
        link->next = room->users;
        room->users = link;
        printf("User '%s' added to room '%s'.\n", username, room->roomname);
    }
    else {
        printf("User '%s' already in room '%s'.\n", username, room->roomname);
    }
}

// Remove a user from a room
void removeUserFromRoom(struct room_node *room, char *username) {
    struct node *current = room->users;
    struct node *prev = NULL;
    while(current != NULL) {
        if(strcmp(current->username, username) == 0) {
            if(prev == NULL) { // First node
                room->users = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            printf("User '%s' removed from room '%s'.\n", username, room->roomname);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("User '%s' not found in room '%s'.\n", username, room->roomname);
}

// ===================== DM List Functions =====================

// Insert a DM at the beginning of the DM list
struct dm_node* insertFirstDM(struct dm_node *head, char *user1, char *user2) {
    if(findDM(head, user1, user2) == NULL && findDM(head, user2, user1) == NULL) {
        // Create a new DM node
        struct dm_node *link = (struct dm_node*) malloc(sizeof(struct dm_node));
        if(!link){
            perror("Failed to allocate memory for new DM");
            exit(EXIT_FAILURE);
        }
        strncpy(link->user1, user1, sizeof(link->user1) - 1);
        link->user1[sizeof(link->user1) - 1] = '\0';
        strncpy(link->user2, user2, sizeof(link->user2) - 1);
        link->user2[sizeof(link->user2) - 1] = '\0';
        link->next = head;
        head = link;
        printf("DM created between '%s' and '%s'.\n", user1, user2);
    }
    else {
        printf("DM between '%s' and '%s' already exists.\n", user1, user2);
    }
    return head;
}

// Find a DM between two users
struct dm_node* findDM(struct dm_node *head, char *user1, char *user2) {
    struct dm_node* current = head;
    while(current != NULL) {
        if( (strcmp(current->user1, user1) == 0 && strcmp(current->user2, user2) == 0) ||
            (strcmp(current->user1, user2) == 0 && strcmp(current->user2, user1) == 0) ) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Remove a DM between two users
void removeDM(struct dm_node **head_ref, char *user1, char *user2) {
    struct dm_node *current = *head_ref;
    struct dm_node *prev = NULL;
    while(current != NULL) {
        if( (strcmp(current->user1, user1) == 0 && strcmp(current->user2, user2) == 0) ||
            (strcmp(current->user1, user2) == 0 && strcmp(current->user2, user1) == 0) ) {
            if(prev == NULL) { // First node
                *head_ref = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            printf("DM between '%s' and '%s' removed.\n", user1, user2);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("DM between '%s' and '%s' not found.\n", user1, user2);
}

// List all users
void list_all_users(struct node *head, char *buffer) {
    struct node *current = head;
    strcat(buffer, "Users:\n");
    while(current != NULL) {
        strcat(buffer, current->username);
        strcat(buffer, "\n");
        current = current->next;
    }
}

// List all rooms
void list_all_rooms(struct room_node *room_head, char *buffer) {
    struct room_node *current = room_head;
    strcat(buffer, "Rooms:\n");
    while(current != NULL) {
        strcat(buffer, current->roomname);
        strcat(buffer, "\n");
        current = current->next;
    }
}
