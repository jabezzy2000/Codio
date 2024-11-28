#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "BENSCHILLIBOWL.h"

// Feel free to play with these numbers! This is a great way to
// test your implementation.
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable for the restaurant.
BENSCHILLIBOWL *bcb;

/**
 * Thread funtion that represents a customer. A customer should:
 *  - allocate space (memory) for an order.
 *  - select a menu item.
 *  - populate the order with their menu item and their customer ID.
 *  - add their order to the restaurant.
 */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = *(int*)tid;
    free(tid); // Free the allocated memory for customer ID

    Order* order = malloc(sizeof(Order));
    if (!order) {
        perror("Failed to allocate memory for order");
        pthread_exit(NULL);
    }

    order->menu_item = PickRandomMenuItem();
    order->customer_id = customer_id;
    order->next = NULL;

    printf("Customer %d: Placing order for %s\n", customer_id, order->menu_item);
    AddOrder(bcb, order); // Add the order to the restaurant queue

    pthread_exit(NULL);
}


/**
 * Thread function that represents a cook in the restaurant. A cook should:
 *  - get an order from the restaurant.
 *  - if the order is valid, it should fulfill the order, and then
 *    free the space taken by the order.
 * The cook should take orders from the restaurants until it does not
 * receive an order.
 */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = *(int*)tid;
    free(tid); 

    while (1) {
        Order* order = GetOrder(bcb);
        if (!order) {
            printf("Cook %d: No more orders. Exiting.\n", cook_id);
            break;
        }

        printf("Cook %d: Fulfilling order for customer %d (%s)\n",
               cook_id, order->customer_id, order->menu_item);
        free(order); // Free the order memory
    }

    pthread_exit(NULL);
}


/**
 * Runs when the program begins executing. This program should:
 *  - open the restaurant
 *  - create customers and cooks
 *  - wait for all customers and cooks to be done
 *  - close the restaurant.
 */
int main() {
    int num_customers = 5; // Example number of customers
    int num_cooks = 2;     // Example number of cooks

    bcb = OpenRestaurant(10, num_customers);

    pthread_t customers[num_customers];
    pthread_t cooks[num_cooks];

    // Create customers
    for (int i = 0; i < num_customers; i++) {
        int* customer_id = malloc(sizeof(int));
        *customer_id = i + 1;
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, customer_id);
    }

    // Create cooks
    for (int i = 0; i < num_cooks; i++) {
        int* cook_id = malloc(sizeof(int));
        *cook_id = i + 1;
        pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, cook_id);
    }

    // Wait for customers to finish
    for (int i = 0; i < num_customers; i++) {
        pthread_join(customers[i], NULL);
    }

    // Signal cooks to stop
    for (int i = 0; i < num_cooks; i++) {
        pthread_join(cooks[i], NULL);
    }

    CloseRestaurant(bcb);
    return 0;
}
