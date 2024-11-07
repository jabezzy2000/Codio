#include <stdio.h>
#include <stdlib.h>

/* IMPLEMENT ME: Declare your functions here */
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);

int main(void)
{
    // Predefined integers
    int a = 6, b = 3;

    // Array of function pointers
    int (*operations[])(int, int) = {add, subtract, multiply, divide};

    // Input character from user
    char input;
    printf("Operand 'a' : %d | Operand 'b' : %d\n", a, b);
    printf("Specify the operation to perform (0 : add | 1 : subtract | 2 : multiply | 3 : divide | 4 : exit): ");
    scanf(" %c", &input);

    // Convert input to an integer index
    int operation = input - '0';

    // Perform the operation if valid
    if (operation >= 0 && operation <= 3) {
        printf("Result = %d\n", operations[operation](a, b));
    } else if (operation == 4) {
        printf("Exiting program.\n");
        return 0;
    } else {
        printf("Invalid input.\n");
    }

    return 0;
}

/* IMPLEMENT ME: Define your functions here */
int add(int a, int b) { printf("Adding 'a' and 'b'\n"); return a + b; }
int subtract(int a, int b) { printf("Subtracting 'b' from 'a'\n"); return a - b; }
int multiply(int a, int b) { printf("Multiplying 'a' and 'b'\n"); return a * b; }
int divide(int a, int b) { printf("Dividing 'a' by 'b'\n"); return a / b; }
