#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    int random_index = rand() % MENU_ITEMS; 
    return BENSCHILLIBOWL_MENU[random_index];
}


/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL* bcb = malloc(sizeof(BENSCHILLIBOWL));
    if (!bcb) {
        perror("Failed to allocate memory for restaurant");
        exit(1);
    }
    bcb->max_size = max_size;
    bcb->expected_num_orders = expected_num_orders;
    bcb->orders = NULL; 
    bcb->current_size = 0;
    bcb->orders_handled = 0;
    bcb->orders_received = 0;

    // Initializing mutex and condition variables
    pthread_mutex_init(&(bcb->mutex), NULL);
    pthread_cond_init(&(bcb->can_add_orders), NULL);
    pthread_cond_init(&(bcb->can_get_orders), NULL);

    printf("Restaurant is now open!\n");
    return bcb;
}



/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));
    if (bcb->orders_received != bcb->orders_handled) {
        printf("Warning: Not all orders were handled.\n");
    }
    pthread_mutex_unlock(&(bcb->mutex));

    pthread_mutex_destroy(&(bcb->mutex));
    pthread_cond_destroy(&(bcb->can_add_orders));
    pthread_cond_destroy(&(bcb->can_get_orders));
    free(bcb);

    printf("Restaurant is now closed!\n");
}


/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&(bcb->mutex));

    while (bcb->current_size == bcb->max_size) {
        pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
    }

    AddOrderToBack(&(bcb->orders), order);
    bcb->current_size++;
    bcb->orders_received++;

    pthread_cond_signal(&(bcb->can_get_orders));
    pthread_mutex_unlock(&(bcb->mutex));

    return 0; 
}


Order* GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));

    while (bcb->current_size == 0) {
        pthread_cond_wait(&(bcb->can_get_orders), &(bcb->mutex));
    }

   
    Order* order = bcb->orders; 
    bcb->orders = bcb->orders->next; 
    bcb->current_size--;
    bcb->orders_handled++;

    pthread_cond_signal(&(bcb->can_add_orders));
    pthread_mutex_unlock(&(bcb->mutex));

    return order;
}


// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == 0;
}

bool IsFull(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == bcb->max_size;
}


/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
    if (*orders == NULL) {
        *orders = order; 
    } else {
        Order *temp = *orders;
        while (temp->next != NULL) {
            temp = temp->next; 
        }
        temp->next = order; 
    }
}


