#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 10

int matA[20][20];
int matB[20][20];
int matSumResult[20][20];
int matDiffResult[20][20];
int matProductResult[20][20];

int MAX;

typedef struct {
    int startRow;
    int endRow;
} ThreadData;

void fillMatrix(int matrix[20][20]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[20][20]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* computeSum(void* args) {
    ThreadData* data = (ThreadData*)args;
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    free(data);  // Free allocated memory
    pthread_exit(0);
}

void* computeDiff(void* args) {
    ThreadData* data = (ThreadData*)args;
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    free(data);  // Free allocated memory
    pthread_exit(0);
}

void* computeProduct(void* args) {
    ThreadData* data = (ThreadData*)args;
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            for (int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    free(data);  // Free allocated memory
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    MAX = atoi(argv[1]);
    if (MAX > 20 || MAX <= 0) {
        printf("Matrix size should be between 1 and 20.\n");
        return 1;
    }

    srand(time(0));  // Initialize random seed

    // Fill matrices with random values
    fillMatrix(matA);
    fillMatrix(matB);

    // Print initial matrices
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    pthread_t threads[MAX_THREADS];
    int rowsPerThread = MAX / MAX_THREADS;

    // Compute Sum
    for (int i = 0; i < MAX_THREADS; i++) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        data->startRow = i * rowsPerThread;
        data->endRow = (i == MAX_THREADS - 1) ? MAX : data->startRow + rowsPerThread;
        pthread_create(&threads[i], NULL, computeSum, data);
    }
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Compute Difference
    for (int i = 0; i < MAX_THREADS; i++) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        data->startRow = i * rowsPerThread;
        data->endRow = (i == MAX_THREADS - 1) ? MAX : data->startRow + rowsPerThread;
        pthread_create(&threads[i], NULL, computeDiff, data);
    }
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Compute Product
    for (int i = 0; i < MAX_THREADS; i++) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        data->startRow = i * rowsPerThread;
        data->endRow = (i == MAX_THREADS - 1) ? MAX : data->startRow + rowsPerThread;
        pthread_create(&threads[i], NULL, computeProduct, data);
    }
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print results
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}
