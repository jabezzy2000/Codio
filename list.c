// list/list.c
// 
// Implementation for linked list.
//
// Jabez Agyemang-Prempeh

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* mylist =  (list_t *) malloc(sizeof(list_t)); 
  mylist->head = NULL;
}

void list_free(list_t *l) {
    node_t *current = l->head;  
    node_t *next_node;

  
    while (current != NULL) {
        next_node = current->next;  
        free(current);              
        current = next_node;    
    }
}

void list_print(list_t *l) {
    node_t *curr_node = l->head;
    while (curr_node != NULL) {
        printf("%d -> ", curr_node->value);  
    }
    printf("NULL\n"); 
}


char * listToString(list_t *l) {
  char* buf = (char *) malloc(sizeof(char) * 1024);
  char tbuf[20];

	node_t* curr = l->head;
  while (curr != NULL) {
    sprintf(tbuf, "%d->", curr->value);
    curr = curr->next;
    strcat(buf, tbuf);
  }
  strcat(buf, "NULL");
  return buf;
}

int list_length(list_t *l) {
  if (l == NULL){
    return -1;
  }
   int counter = 0;
   node_t* curr_node = l->head;
   while (curr_node!= NULL){
    counter++;
    curr_node= curr_node->next;
   }
   return counter;
   }

void list_add_to_back(list_t *l, elem value) {
  node_t* new_node = (node_t*)malloc(sizeof(node_t));
  node_t* curr_node = l->head;
  while (curr_node->next != NULL){
    curr_node = curr_node->next;
  }
  curr_node->next = new_node;
}


void list_add_to_front(list_t *l, elem value) {
     node_t* cur_node = (node_t *) malloc(sizeof(node_t));
     cur_node->value = value;
     cur_node->next = NULL;

     /* Insert to front */

     node_t* head = l->head;  // get head of list

     cur_node->next = head;
     head = cur_node;
}

// void list_add_at_index(list_t *l, elem value, int index) {
// }

void list_add_at_index(list_t *l, elem value, int index) {
    if (index < 0) {
        return;  // Invalid index
    }

    node_t *new_node = (node_t*)malloc(sizeof(node_t));
    if (new_node == NULL) {
        printf("Memory allocation failed\n");
        return;
    }
    new_node->value = value;
    new_node->next = NULL;

    if (index == 0) {
        // Add to the front
        new_node->next = l->head;
        l->head = new_node;
        return;
    }

    node_t *curr = l->head;
    int counter = 0;

    // Traverse the list to find the position just before the index
    while (curr != NULL && counter < index - 1) {
        curr = curr->next;
        counter++;
    }

    if (curr == NULL) {
        free(new_node);  // Index out of bounds
        return;
    }

    // Insert the new node at the correct index
    new_node->next = curr->next;
    curr->next = new_node;
}



// elem list_remove_from_back(list_t *l) { return -1; }
elem list_remove_from_back(list_t *l) {
    if (l->head == NULL) {
        return -1;  // Empty list
    }

    node_t *curr = l->head;
    node_t *prev = NULL;

    // If there is only one element
    if (curr->next == NULL) {
        elem value = curr->value;
        free(curr);
        l->head = NULL;  // List becomes empty
        return value;
    }

    // Traverse to the second-to-last node
    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }

    elem value = curr->value;
    free(curr);              // Free the last node
    prev->next = NULL;        // Set second-to-last node's next to NULL
    return value;
}


// elem list_remove_from_front(list_t *l) { return -1; }

elem list_remove_from_front(list_t *l) {
    if (l->head == NULL) {
        return -1;  // Empty list
    }

    node_t *temp = l->head;
    elem value = temp->value;
    l->head = temp->next;  // Update the head to the next node
    free(temp);            // Free the old head
    return value;
}



// elem list_remove_at_index(list_t *l, int index) { return -1; }
elem list_remove_at_index(list_t *l, int index) {
    if (l->head == NULL || index < 0) {
        return -1;  // Empty list or invalid index
    }

    if (index == 0) {
        return list_remove_from_front(l);  // Use the function for removing from the front
    }

    node_t *curr = l->head;
    node_t *prev = NULL;
    int counter = 0;

    // Traverse to the node just before the specified index
    while (curr != NULL && counter < index) {
        prev = curr;
        curr = curr->next;
        counter++;
    }

    if (curr == NULL) {
        return -1;  // Index out of bounds
    }

    prev->next = curr->next;  // Remove the node by updating the previous node's next
    elem value = curr->value;
    free(curr);               // Free the removed node
    return value;
}



// bool list_is_in(list_t *l, elem value) { return false; }
bool list_is_in(list_t *l, elem value) {
    node_t *curr = l->head;
    while (curr != NULL) {
        if (curr->value == value) {
            return true;  // Element found
        }
        curr = curr->next;
    }
    return false;  // Element not found
}




// elem list_get_elem_at(list_t *l, int index) { return -1; }
elem list_get_elem_at(list_t *l, int index) {
    if (l->head == NULL || index < 0) {
        return -1;  // Empty list or invalid index
    }

    node_t *curr = l->head;
    int counter = 0;

    while (curr != NULL && counter < index) {
        curr = curr->next;
        counter++;
    }

    if (curr == NULL) {
        return -1;  // Index out of bounds
    }

    return curr->value;  // Return the value at the given index
}



//int list_get_index_of(list_t *l, elem value) { return -1; }
int list_get_index_of(list_t *l, elem value) {
    node_t *curr = l->head;
    int index = 0;

    while (curr != NULL) {
        if (curr->value == value) {
            return index;  // Element found at index
        }
        curr = curr->next;
        index++;
    }

    return -1;  // Element not found
}



