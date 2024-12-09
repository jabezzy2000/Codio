#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Allocate a new linked list
list_t *list_alloc() {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    list->head = NULL;
    return list;
}

// Free the memory used by the linked list
void list_free(list_t *list) {
    node_t *current = list->head;
    while (current != NULL) {
        node_t *temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
}

// Add a value to the back of the list
void list_add_to_back(list_t *list, int value) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = NULL;

    if (list->head == NULL) {
        list->head = new_node;
    } else {
        node_t *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Add a value to the front of the list
void list_add_to_front(list_t *list, int value) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = list->head;
    list->head = new_node;
}

// Add a value at a specific index in the list
void list_add_at_index(list_t *list, int index, int value) {
    if (index < 0) return;

    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;

    if (index == 0) {
        new_node->next = list->head;
        list->head = new_node;
        return;
    }

    node_t *current = list->head;
    for (int i = 0; i < index - 1 && current != NULL; i++) {
        current = current->next;
    }

    if (current == NULL) {
        free(new_node);
        return;
    }

    new_node->next = current->next;
    current->next = new_node;
}

// Remove a value from the back of the list
int list_remove_from_back(list_t *list) {
    if (list->head == NULL) return -1;

    node_t *current = list->head;
    if (current->next == NULL) {
        int value = current->value;
        free(current);
        list->head = NULL;
        return value;
    }

    while (current->next->next != NULL) {
        current = current->next;
    }

    int value = current->next->value;
    free(current->next);
    current->next = NULL;
    return value;
}

// Remove a value from the front of the list
int list_remove_from_front(list_t *list) {
    if (list->head == NULL) return -1;

    node_t *temp = list->head;
    int value = temp->value;
    list->head = temp->next;
    free(temp);
    return value;
}

// Remove a value at a specific index in the list
int list_remove_at_index(list_t *list, int index) {
    if (index < 0 || list->head == NULL) return -1;

    if (index == 0) {
        return list_remove_from_front(list);
    }

    node_t *current = list->head;
    for (int i = 0; i < index - 1 && current->next != NULL; i++) {
        current = current->next;
    }

    if (current->next == NULL) return -1;

    node_t *temp = current->next;
    int value = temp->value;
    current->next = temp->next;
    free(temp);
    return value;
}

// Get the element at a specific index in the list
int list_get_elem_at(list_t *list, int index) {
    if (index < 0) return -1;

    node_t *current = list->head;
    for (int i = 0; i < index && current != NULL; i++) {
        current = current->next;
    }

    return current ? current->value : -1;
}

// Get the length of the list
int list_length(list_t *list) {
    int length = 0;
    node_t *current = list->head;
    while (current != NULL) {
        length++;
        current = current->next;
    }
    return length;
}

// Convert the list to a string
char *listToString(list_t *list) {
    static char buffer[1024];
    buffer[0] = '\0';

    node_t *current = list->head;
    while (current != NULL) {
        char temp[32];
        sprintf(temp, "%d -> ", current->value);
        strcat(buffer, temp);
        current = current->next;
    }
    strcat(buffer, "NULL");
    return buffer;
}
