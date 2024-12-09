#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include <signal.h>
#include "list.h"
#include <unistd.h>


#define PORT 9001
#define ACK "ACK"

list_t *mylist = NULL;  // Global list for signal handler

void signal_handler(int sig) {
    printf("\nServer shutting down...\n");
    if (mylist) {
        list_free(mylist);  // Free linked list memory
        printf("Linked list memory freed.\n");
    }
    exit(0);  // Exit gracefully
}

int main(int argc, char const* argv[]) {
    signal(SIGINT, signal_handler);  // Register signal handler

    int n, val, idx;
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);

    char buf[1024];
    char sbuf[1024];
    char* token;

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr));
    listen(servSockD, 1);

    int clientSocket = accept(servSockD, NULL, NULL);

    mylist = list_alloc();  // Initialize linked list

    while (1) {
        n = recv(clientSocket, buf, sizeof(buf), 0);
        if (n <= 0) break;  // Exit loop if client disconnects

        buf[n] = '\0';  // Null-terminate the received string
        token = strtok(buf, " ");

        if (strcmp(token, "exit") == 0) {
            list_free(mylist);
            close(clientSocket);
            close(servSockD);
            exit(0);
        } else if (strcmp(token, "get_length") == 0) {
            val = list_length(mylist);
            sprintf(sbuf, "Length = %d", val);
        } else if (strcmp(token, "add_front") == 0) {
            token = strtok(NULL, " ");
            if (!token) {
                sprintf(sbuf, "Error: Missing value for add_front");
            } else {
                val = atoi(token);
                list_add_to_front(mylist, val);
                sprintf(sbuf, "%s%d", ACK, val);
            }
        } else if (strcmp(token, "add_back") == 0) {
            token = strtok(NULL, " ");
            if (!token) {
                sprintf(sbuf, "Error: Missing value for add_back");
            } else {
                val = atoi(token);
                list_add_to_back(mylist, val);
                sprintf(sbuf, "%s%d", ACK, val);
            }
        } else if (strcmp(token, "add_position") == 0) {
            token = strtok(NULL, " ");
            if (!token) {
                sprintf(sbuf, "Error: Missing index for add_position");
            } else {
                idx = atoi(token);
                token = strtok(NULL, " ");
                if (!token) {
                    sprintf(sbuf, "Error: Missing value for add_position");
                } else {
                    val = atoi(token);
                    list_add_at_index(mylist, idx, val);
                    sprintf(sbuf, "%s%d", ACK, val);
                }
            }
        } else if (strcmp(token, "remove_front") == 0) {
            val = list_remove_from_front(mylist);
            sprintf(sbuf, "Removed %d", val);
        } else if (strcmp(token, "remove_back") == 0) {
            val = list_remove_from_back(mylist);
            sprintf(sbuf, "Removed %d", val);
        } else if (strcmp(token, "remove_position") == 0) {
            token = strtok(NULL, " ");
            if (!token) {
                sprintf(sbuf, "Error: Missing index for remove_position");
            } else {
                idx = atoi(token);
                val = list_remove_at_index(mylist, idx);
                sprintf(sbuf, "Removed %d", val);
            }
        } else if (strcmp(token, "get") == 0) {
            token = strtok(NULL, " ");
            if (!token) {
                sprintf(sbuf, "Error: Missing index for get");
            } else {
                idx = atoi(token);
                val = list_get_elem_at(mylist, idx);
                sprintf(sbuf, "Element at index %d = %d", idx, val);
            }
        } else if (strcmp(token, "print") == 0) {
            sprintf(sbuf, "%s", listToString(mylist));
        } else {
            sprintf(sbuf, "Error: Unknown command");
        }

        send(clientSocket, sbuf, strlen(sbuf), 0);  // Send response
        memset(buf, '\0', sizeof(buf));
        memset(sbuf, '\0', sizeof(sbuf));
    }

    close(clientSocket);
    close(servSockD);
    return 0;
}
