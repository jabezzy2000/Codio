// list.h

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

// User Node Structure
struct node {
    char username[30];
    int socket;
    struct node *next;
};

// Room Node Structure
struct room_node {
    char roomname[30];
    struct node *users; // Linked list of users in the room
    struct room_node *next;
};

// Direct Message (DM) Node Structure
struct dm_node {
    char user1[30];
    char user2[30];
    struct dm_node *next;
};

// Function Prototypes for Users
struct node* insertFirstU(struct node *head, int socket, char *username);
struct node* findU(struct node *head, char* username);

// Function Prototypes for Rooms
struct room_node* insertFirstR(struct room_node *head, char *roomname);
struct room_node* findR(struct room_node *head, char* roomname);
void addUserToRoom(struct room_node *room, char *username, int socket);
void removeUserFromRoom(struct room_node *room, char *username);

// Function Prototypes for DMs
struct dm_node* insertFirstDM(struct dm_node *head, char *user1, char *user2);
struct dm_node* findDM(struct dm_node *head, char *user1, char *user2);
void removeDM(struct dm_node **head, char *user1, char *user2);

// Utility Function Prototypes
void list_all_users(struct node *head, char *buffer);
void list_all_rooms(struct room_node *room_head, char *buffer);

#endif // LIST_H
