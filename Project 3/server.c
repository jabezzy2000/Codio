// server.c

#include "server.h"
#include <signal.h>
#include <pthread.h>

// Global lists
struct node *head = NULL;              // User list
struct room_node *room_head = NULL;    // Room list
struct dm_node *dm_head = NULL;        // DM list

int chat_serv_sock_fd; // Server socket

/////////////////////////////////////////////
// USE THESE LOCKS AND COUNTER TO SYNCHRONIZE

int numReaders = 0; // Keep count of the number of readers

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // Mutex lock
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER;    // Read/Write lock

/////////////////////////////////////////////

char const *server_MOTD = "Thanks for connecting to the BisonChat Server.\n\nchat>";

int main(int argc, char **argv) {

   // Register the SIGINT handler for graceful shutdown
   signal(SIGINT, sigintHandler);
    
   //////////////////////////////////////////////////////
   // Create the default room "Lobby"
   room_head = insertFirstR(room_head, "Lobby");
   if(room_head == NULL){
       perror("Failed to create Lobby room");
       exit(EXIT_FAILURE);
   }
   printf("Default room 'Lobby' created.\n");
    
   // Open server socket
   chat_serv_sock_fd = get_server_socket(NULL, NULL); // Adjusted to pass NULL as parameters are unused
    
   // Start listening for connections
   if(start_server(chat_serv_sock_fd, BACKLOG) == -1) {
      printf("Failed to start server.\n");
      exit(1);
   }
   
   printf("Server Launched! Listening on PORT: %d\n", PORT);
    
   // Main execution loop
   while(1) {
      // Accept a new client connection
      int new_client = accept_client(chat_serv_sock_fd);
      if(new_client != -1) {
         pthread_t new_client_thread;
         // Allocate memory for the client socket to pass to the thread
         int *client_sock = malloc(sizeof(int));
         if(client_sock == NULL){
             perror("Failed to allocate memory for client socket");
             continue;
         }
         *client_sock = new_client;
         pthread_create(&new_client_thread, NULL, client_receive, (void *)client_sock);
         pthread_detach(new_client_thread); // Detach the thread to reclaim resources when done
      }
   }

   close(chat_serv_sock_fd);
}
// server.c (continued)

// Function to remove and free all users from the user list
void free_user_list() {
    struct node *current = head;
    while(current != NULL) {
        close(current->socket); // Close the client socket
        struct node *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
}

// Function to remove and free all rooms and their user lists
void free_room_list() {
    struct room_node *current_room = room_head;
    while(current_room != NULL) {
        // Free users in the room
        struct node *current_user = current_room->users;
        while(current_user != NULL) {
            struct node *temp_user = current_user;
            current_user = current_user->next;
            free(temp_user);
        }
        // Free the room itself
        struct room_node *temp_room = current_room;
        current_room = current_room->next;
        free(temp_room);
    }
    room_head = NULL;
}

// Function to remove and free all DMs
void free_dm_list() {
    struct dm_node *current_dm = dm_head;
    while(current_dm != NULL) {
        struct dm_node *temp_dm = current_dm;
        current_dm = current_dm->next;
        free(temp_dm);
    }
    dm_head = NULL;
}

/* Handle SIGINT (CTRL+C) */
void sigintHandler(int sig_num) {
   printf("\nSIGINT received. Shutting down the server gracefully.\n");

   // Acquire write lock to ensure no other thread is accessing the lists
   pthread_mutex_lock(&rw_lock);

   // Close and free all client sockets
   free_user_list();

   // Free all rooms and their user lists
   free_room_list();

   // Free all DMs
   free_dm_list();

   pthread_mutex_unlock(&rw_lock);

   printf("All resources have been cleaned up. Exiting.\n");

   close(chat_serv_sock_fd);
   exit(0);
}
