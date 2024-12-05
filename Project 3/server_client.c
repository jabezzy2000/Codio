// server_client.c

#include "server.h"
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Define the maximum buffer size
#define MAXBUFF 1024
#define delimiters " \n"

// Declare external variables from server.c
extern struct node *head;               // User list
extern struct room_node *room_head;     // Room list
extern struct dm_node *dm_head;         // DM list

extern int numReaders;
extern pthread_mutex_t mutex;
extern pthread_mutex_t rw_lock;

extern char const *server_MOTD;

// Function prototypes for synchronization
void read_lock();
void read_unlock();
void write_lock();
void write_unlock();

// Utility function to remove a user from the user list
struct node* removeUser(struct node *head, char *username);

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

void *client_receive(void *ptr) {
   int client = *(int *) ptr;  // socket
   free(ptr); // Free the allocated memory for client socket
   
   int received, i;
   char buffer[MAXBUFF], sbuffer[MAXBUFF];  // Data buffers
   char cmd[MAXBUFF], username[50];
   char *arguments[80] = {0}; // Initialize all to NULL

   struct node *currentUser;
    
   send(client, server_MOTD, strlen(server_MOTD), 0); // Send Welcome Message of the Day.

   // Creating the guest user name
   snprintf(username, sizeof(username), "guest%d", client);
   
   // Add the user to the user list
   write_lock();
   head = insertFirstU(head, client, username);
   write_unlock();
   
   // Add the GUEST to the DEFAULT ROOM (i.e., Lobby)
   write_lock();
   struct room_node *lobby = findR(room_head, "Lobby");
   if(lobby != NULL){
       addUserToRoom(lobby, username, client);
   }
   else{
       // This should not happen as Lobby is created in server.c
       lobby = insertFirstR(room_head, "Lobby");
       addUserToRoom(lobby, username, client);
   }
   write_unlock();
   
   while (1) {
      if ((received = read(client, buffer, MAXBUFF - 1)) > 0) {
          buffer[received] = '\0'; 
          strcpy(cmd, buffer);  
          strcpy(sbuffer, buffer);
       
          arguments[0] = strtok(cmd, delimiters);
          i = 0;
          while(arguments[i] != NULL && i < 79) {
              arguments[++i] = strtok(NULL, delimiters); 
              if(arguments[i-1] != NULL){
                  arguments[i-1] = trimwhitespace(arguments[i-1]);
              }
          } 
       
          // Handle commands
          if(strcmp(arguments[0], "create") == 0)
          {
               if(arguments[1] == NULL){
                   snprintf(buffer, sizeof(buffer), "Usage: create <room>\nchat>");
                   send(client, buffer, strlen(buffer), 0);
               }
               else{
                   printf("create room: %s\n", arguments[1]); 
                  
                   // Create the room
                   write_lock();
                   struct room_node *existing_room = findR(room_head, arguments[1]);
                   if(existing_room == NULL){
                       room_head = insertFirstR(room_head, arguments[1]);
                       snprintf(buffer, sizeof(buffer), "Room '%s' created successfully.\nchat>", arguments[1]);
                       printf("Room '%s' created.\n", arguments[1]);
                   }
                   else{
                       snprintf(buffer, sizeof(buffer), "Room '%s' already exists.\nchat>", arguments[1]);
                   }
                   write_unlock();
                  
                   send(client, buffer, strlen(buffer), 0); // send back to client
          }
          }
          else if (strcmp(arguments[0], "join") == 0)
          {
               if(arguments[1] == NULL){
                   snprintf(buffer, sizeof(buffer), "Usage: join <room>\nchat>");
                   send(client, buffer, strlen(buffer), 0);
               }
               else{
                   printf("join room: %s\n", arguments[1]);  
                  
                   // Join the room
                   write_lock();
                   struct room_node *room = findR(room_head, arguments[1]);
                   if(room == NULL){
                       snprintf(buffer, sizeof(buffer), "Room '%s' does not exist.\nchat>", arguments[1]);
                   }
                   else{
                       addUserToRoom(room, username, client);
                       snprintf(buffer, sizeof(buffer), "Joined room '%s'.\nchat>", arguments[1]);
                       printf("User '%s' joined room '%s'.\n", username, arguments[1]);
                   }
                   write_unlock();
                  
                   send(client, buffer, strlen(buffer), 0); // send back to client
          }
          }
          else if (strcmp(arguments[0], "leave") == 0)
          {
               if(arguments[1] == NULL){
                   snprintf(buffer, sizeof(buffer), "Usage: leave <room>\nchat>");
                   send(client, buffer, strlen(buffer), 0);
               }
               else{
                   printf("leave room: %s\n", arguments[1]); 
    
                   // Leave the room
                   write_lock();
                   struct room_node *room = findR(room_head, arguments[1]);
                   if(room == NULL){
                       snprintf(buffer, sizeof(buffer), "Room '%s' does not exist.\nchat>", arguments[1]);
                   }
                   else{
                       removeUserFromRoom(room, username);
                       snprintf(buffer, sizeof(buffer), "Left room '%s'.\nchat>", arguments[1]);
                       printf("User '%s' left room '%s'.\n", username, arguments[1]);
                   }
                   write_unlock();
    
                   send(client, buffer, strlen(buffer), 0); // send back to client
          }
          }
          else if (strcmp(arguments[0], "connect") == 0)
          {
               if(arguments[1] == NULL){
                   snprintf(buffer, sizeof(buffer), "Usage: connect <user>\nchat>");
                   send(client, buffer, strlen(buffer), 0);
               }
               else{
                   printf("connect to user: %s \n", arguments[1]);
    
                   // Connect to the user for DM
                   write_lock();
                   struct node *target_user = findU(head, arguments[1]);
                   if(target_user == NULL){
                       snprintf(buffer, sizeof(buffer), "User '%s' does not exist.\nchat>", arguments[1]);
                   }
                   else{
                       // Check if DM already exists
                       struct dm_node *existing_dm = findDM(dm_head, username, arguments[1]);
                       if(existing_dm == NULL){
                           dm_head = insertFirstDM(dm_head, username, arguments[1]);
                           snprintf(buffer, sizeof(buffer), "Connected to user '%s'.\nchat>", arguments[1]);
                           printf("User '%s' connected to '%s'.\n", username, arguments[1]);
                       }
                       else{
                           snprintf(buffer, sizeof(buffer), "Already connected to user '%s'.\nchat>", arguments[1]);
                       }
                   }
                   write_unlock();
    
                   send(client, buffer, strlen(buffer), 0); // send back to client
          }
          }
          else if (strcmp(arguments[0], "disconnect") == 0)
          {             
               if(arguments[1] == NULL){
                   snprintf(buffer, sizeof(buffer), "Usage: disconnect <user>\nchat>");
                   send(client, buffer, strlen(buffer), 0);
               }
               else{
                   printf("disconnect from user: %s\n", arguments[1]);
                   
                   // Disconnect from the user
                   write_lock();
                   struct node *target_user = findU(head, arguments[1]);
                   if(target_user == NULL){
                       snprintf(buffer, sizeof(buffer), "User '%s' does not exist.\nchat>", arguments[1]);
                   }
                   else{
                       // Check if DM exists
                       struct dm_node *existing_dm = findDM(dm_head, username, arguments[1]);
                       if(existing_dm != NULL){
                           removeDM(&dm_head, username, arguments[1]);
                           snprintf(buffer, sizeof(buffer), "Disconnected from user '%s'.\nchat>", arguments[1]);
                           printf("User '%s' disconnected from '%s'.\n", username, arguments[1]);
                       }
                       else{
                           snprintf(buffer, sizeof(buffer), "No existing connection to user '%s'.\nchat>", arguments[1]);
                       }
                   }
                   write_unlock();
                   
                   send(client, buffer, strlen(buffer), 0); // send back to client
          }
          }
          else if (strcmp(arguments[0], "rooms") == 0)
          {
               printf("List all the rooms\n");
              
               // List all rooms
               buffer[0] = '\0'; // Clear buffer
              
               read_lock();
               list_all_rooms(room_head, buffer);
               read_unlock();
              
               strcat(buffer, "chat>");
               send(client, buffer, strlen(buffer), 0 ); // send back to client                            
          }   
          else if (strcmp(arguments[0], "users") == 0)
          {
               printf("List all the users\n");
              
               // List all users
               buffer[0] = '\0'; // Clear buffer
              
               read_lock();
               list_all_users(head, buffer);
               read_unlock();
              
               strcat(buffer, "chat>");
               send(client, buffer, strlen(buffer), 0 ); // send back to client
          }                           
          else if (strcmp(arguments[0], "login") == 0)
          {
               if(arguments[1] == NULL){
                   snprintf(buffer, sizeof(buffer), "Usage: login <username>\nchat>");
                   send(client, buffer, strlen(buffer), 0);
               }
               else{
                   // Rename their guestID to username
                   write_lock();
                   if(findU(head, arguments[1]) != NULL){
                       snprintf(buffer, sizeof(buffer), "Username '%s' is already taken.\nchat>", arguments[1]);
                   }
                   else{
                       struct node *current_user = findU(head, username);
                       if(current_user != NULL){
                           strncpy(current_user->username, arguments[1], sizeof(current_user->username) - 1);
                           current_user->username[sizeof(current_user->username) - 1] = '\0';
                           
                           // Update username in all rooms
                           struct room_node *room = room_head;
                           while(room != NULL){
                               struct node *room_user = room->users;
                               while(room_user != NULL){
                                   if(room_user->socket == client){
                                       strncpy(room_user->username, arguments[1], sizeof(room_user->username) -1 );
                                       room_user->username[sizeof(room_user->username) -1] = '\0';
                                   }
                                   room_user = room_user->next;
                               }
                               room = room->next;
                           }
                           
                           // Update username in DMs
                           struct dm_node *current_dm = dm_head;
                           while(current_dm != NULL){
                               if(strcmp(current_dm->user1, username) == 0){
                                   strncpy(current_dm->user1, arguments[1], sizeof(current_dm->user1) -1 );
                                   current_dm->user1[sizeof(current_dm->user1) -1] = '\0';
                               }
                               if(strcmp(current_dm->user2, username) == 0){
                                   strncpy(current_dm->user2, arguments[1], sizeof(current_dm->user2) -1 );
                                   current_dm->user2[sizeof(current_dm->user2) -1] = '\0';
                               }
                               current_dm = current_dm->next;
                           }
                           
                           snprintf(buffer, sizeof(buffer), "Logged in as '%s'.\nchat>", arguments[1]);
                           printf("User '%s' logged in as '%s'.\n", username, arguments[1]);
                           
                           // Update local username variable
                           strncpy(username, arguments[1], sizeof(username) - 1);
                           username[sizeof(username) - 1] = '\0';
                       }
                       else{
                           snprintf(buffer, sizeof(buffer), "Login failed. User not found.\nchat>");
                       }
                   }
                   write_unlock();
                   
                   send(client, buffer, strlen(buffer), 0 ); // send back to client
          }
          }
          else if (strcmp(arguments[0], "help") == 0 )
          {
               snprintf(buffer, sizeof(buffer),
                   "Available commands:\n"
                   "login <username> - Login with a username\n"
                   "create <room> - Create a new room\n"
                   "join <room> - Join an existing room\n"
                   "leave <room> - Leave a room\n"
                   "users - List all connected users\n"
                   "rooms - List all available rooms\n"
                   "connect <user> - Connect to a user for direct messaging (DM)\n"
                   "disconnect <user> - Disconnect from a user\n"
                   "exit or logout - Exit the chat\nchat>");
               send(client, buffer, strlen(buffer), 0 ); // send back to client 
          }
          else if (strcmp(arguments[0], "exit") == 0 || strcmp(arguments[0], "logout") == 0)
          {
               // Remove the initiating user from all rooms and DMs, then close the socket
               write_lock();
               
               // Remove user from user list
               head = removeUser(head, username);
               
               // Remove user from all rooms
               struct room_node *room = room_head;
               while(room != NULL){
                   removeUserFromRoom(room, username);
                   room = room->next;
               }
               
               // Remove all DMs involving this user
               struct dm_node *current_dm = dm_head;
               while(current_dm != NULL){
                   if(strcmp(current_dm->user1, username) == 0 || strcmp(current_dm->user2, username) == 0){
                       removeDM(&dm_head, current_dm->user1, current_dm->user2);
                       // Reset to head since list may have changed
                       current_dm = dm_head;
                   }
                   else{
                       current_dm = current_dm->next;
                   }
               }
               
               write_unlock();
               
               snprintf(buffer, sizeof(buffer), "Goodbye!\n");
               send(client, buffer, strlen(buffer), 0 );
               printf("User '%s' has disconnected.\n", username);
               close(client);
               pthread_exit(NULL);
          }                         
          else { 
              
               // Format the message
               char message[MAXBUFF];
               snprintf(message, sizeof(message), "::%s> %s\nchat>", username, sbuffer);
               
               // Temporary list to hold recipients to avoid duplicates
               struct node *recipients = NULL;
               
               // Acquire read lock to access shared data structures
               read_lock();
               
               // Find all rooms the user is part of
               struct room_node *room = room_head;
               while(room != NULL){
                   // Check if user is in this room
                   struct node *room_user = room->users;
                   bool in_room = false;
                   while(room_user != NULL){
                       if(strcmp(room_user->username, username) == 0){
                           in_room = true;
                           break;
                       }
                       room_user = room_user->next;
                   }
                   if(in_room){
                       // Add all users in this room to recipients
                       room_user = room->users;
                       while(room_user != NULL){
                           if(room_user->socket != client){
                               // Check if already in recipients
                               struct node *existing = findU(recipients, room_user->username);
                               if(existing == NULL){
                                   recipients = insertFirstU(recipients, room_user->socket, room_user->username);
                               }
                           }
                           room_user = room_user->next;
                       }
                   }
                   room = room->next;
               }
               
               // Find all DMs involving this user
               struct dm_node *dm = dm_head;
               while(dm != NULL){
                   if(strcmp(dm->user1, username) == 0){
                       // Add user2
                       struct node *target = findU(head, dm->user2);
                       if(target != NULL && target->socket != client){
                           struct node *existing = findU(recipients, target->username);
                           if(existing == NULL){
                               recipients = insertFirstU(recipients, target->socket, target->username);
                           }
                       }
                   }
                   else if(strcmp(dm->user2, username) == 0){
                       // Add user1
                       struct node *target = findU(head, dm->user1);
                       if(target != NULL && target->socket != client){
                           struct node *existing = findU(recipients, target->username);
                           if(existing == NULL){
                               recipients = insertFirstU(recipients, target->socket, target->username);
                           }
                       }
                   }
                   dm = dm->next;
               }
               
               read_unlock();
               
               // Send the message to all recipients
               struct node *current_recipient = recipients;
               while(current_recipient != NULL){
                   send(current_recipient->socket, message, strlen(message), 0 ); 
                   current_recipient = current_recipient->next;
               }
               
               // Free the temporary recipients list
               while(recipients != NULL){
                   struct node *temp = recipients;
                   recipients = recipients->next;
                   free(temp);
               }
               
               // Optionally, send the prompt to the sender
               send(client, "chat>", 6, 0 ); 
          }
   
       }
       else{
           // Handle client disconnection or read error
           printf("Client %d disconnected or read error.\n", client);
           // Perform cleanup similar to exit command
           
           write_lock();
           
           // Remove user from user list
           head = removeUser(head, username);
           
           // Remove user from all rooms
           struct room_node *room = room_head;
           while(room != NULL){
               removeUserFromRoom(room, username);
               room = room->next;
           }
           
           // Remove all DMs involving this user
           struct dm_node *current_dm = dm_head;
           while(current_dm != NULL){
               if(strcmp(current_dm->user1, username) == 0 || strcmp(current_dm->user2, username) == 0){
                   removeDM(&dm_head, current_dm->user1, current_dm->user2);
                   // Reset to head since list may have changed
                   current_dm = dm_head;
               }
               else{
                   current_dm = current_dm->next;
               }
           }
           
           write_unlock();
           
           close(client);
           pthread_exit(NULL);
       }
   }
   return NULL;
}

// Function to remove a user from the user list
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
