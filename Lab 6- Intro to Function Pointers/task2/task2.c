#include <stdio.h>
#include <stdlib.h>

/* Function declarations */
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);
int exit_program(int a, int b);
int invalid_operation(int a, int b);

int main(void)
{
    // Predefined integers
    int a = 6, b = 3;

    // Array of function pointers for operations
    int (*operations[])(int, int) = {add, subtract, multiply, divide, invalid_operation, exit_program};

    while (1) { // Infinite loop until the user exits
        // Display operands and menu
        printf("Operand 'a' : %d | Operand 'b' : %d\n", a, b);
        printf("Specify the operation to perform:\n");
        printf("  0 : add\n");
        printf("  1 : subtract\n");
        printf("  2 : multiply\n");
        printf("  3 : divide\n");
        printf("  4 : exit\n");
        printf("Your choice: ");

        // Read user input
        char input;
        scanf(" %c", &input);

        // Convert input to index
        int operation = input - '0';

        // Ensure index is valid
        operation = (operation >= 0 && operation <= 4) ? operation : 4;

        // Perform operation using function pointer array
        int result = operations[operation](a, b);

        // Print the result for valid operations
        if (operation < 4) {
            printf("Result = %d\n", result);
        }
    }

    return 0;
}

/* Function definitions */
int add(int a, int b) { 
    printf("Adding 'a' and 'b'\n"); 
    return a + b; 
}

int subtract(int a, int b) { 
    printf("Subtracting 'b' from 'a'\n"); 
    return a - b; 
}

int multiply(int a, int b) { 
    printf("Multiplying 'a' and 'b'\n"); 
    return a * b; 
}

int divide(int a, int b) { 
    if (b == 0) {
        fprintf(stderr, "Error: Division by zero.\n");
        exit(EXIT_FAILURE);
    }
    printf("Dividing 'a' by 'b'\n"); 
    return a / b; 
}

int exit_program(int a, int b) {
    printf("Exiting program.\n");
    exit(0);
    return 0;  // This is added to satisfy the `int` return type
}

int invalid_operation(int a, int b) {
    printf("Invalid input.\n");
    return 0;
}
