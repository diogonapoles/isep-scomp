#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>


#define MATRIX_SIZE 3

typedef struct {
    int *matrixA;
    int **matrixB;
    int index;
} Params;

int result_matrix[MATRIX_SIZE][MATRIX_SIZE];

void* fillMatrix(void* arg) {
    srand(time(NULL));

    int** matrix = (int**)arg;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix[i][j] = rand() % 10;
        }
    }

    pthread_exit(NULL);
}

void* multiplyMatrices(void* arg) {
    Params* params = (Params*)arg;
    int* matrixA = params->matrixA;
    int** matrixB = params->matrixB;
    int index = params->index;

    for (int i = 0; i < MATRIX_SIZE; i++) {
        int sum = 0;

        for (int j = 0; j < MATRIX_SIZE; j++) {
            sum += matrixA[j] * matrixB[j][i];
        }

        result_matrix[index][i] = sum;
    }

    pthread_exit(NULL);
}

int main(void) {
    pthread_t fill_threads[2];
    pthread_t multiply_threads[MATRIX_SIZE];

    int** matrixA = (int**)calloc(MATRIX_SIZE, sizeof(int*));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        matrixA[i] = (int*)calloc(MATRIX_SIZE, sizeof(int));
    }

    int** matrixB = (int**)calloc(MATRIX_SIZE, sizeof(int*));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        matrixB[i] = (int*)calloc(MATRIX_SIZE, sizeof(int));
    }

    pthread_create(&fill_threads[0], NULL, fillMatrix, (void*)matrixA);
    pthread_create(&fill_threads[1], NULL, fillMatrix, (void*)matrixB);

    pthread_join(fill_threads[0], NULL);
    pthread_join(fill_threads[1], NULL);

    printf("Matrices are filled\n");
    fflush(stdout);

    Params* params = (Params*)calloc(MATRIX_SIZE, sizeof(Params));

    for (int i = 0; i < MATRIX_SIZE; i++) {
        params[i].matrixA = matrixA[i];
        params[i].matrixB = matrixB;
        params[i].index = i;

        pthread_create(&multiply_threads[i], NULL, multiplyMatrices, (void*)&params[i]);
    }

    for (int i = 0; i < MATRIX_SIZE; i++) {
        pthread_join(multiply_threads[i], NULL);
    }

    printf("All threads ended\n");
    fflush(stdout);

    printf("Matrix A:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        printf("%d  ", i);
        fflush(stdout);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("[%d] ", matrixA[i][j]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }

    printf("Matrix B:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        printf("%d  ", i);
        fflush(stdout);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("[%d] ", matrixB[i][j]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }

    printf("Multiply matrices A and B:\n");
    fflush(stdout);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        printf("%d  ", i);
        fflush(stdout);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("[%d] ", result_matrix[i][j]);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }

    for (int i = 0; i < MATRIX_SIZE; i++) {
        free(matrixA[i]);
    }
    free(matrixA);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        free(matrixB[i]);
    }
    free(matrixB);

    return 0;
}