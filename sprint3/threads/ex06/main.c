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

#define DIMENSION 3

int matrix[DIMENSION][DIMENSION];

void* findPrimes(void* arg) {
    int* maxValue = (int*)arg;

    int is_prime;

    printf("STARTING\n");

    for (int i = 0; i < *maxValue; i++) {
        is_prime = 1;

        if (i % 2 == 0) {
            is_prime = 0;
        } else {
            for (int j = 3; j < i; j++) {
                if ((i % j) == 0) {
                    is_prime = 0;
                    break;
                }
            }
        }
        if (is_prime == 1) {
            printf("%d\n", i);
        }
    }
    pthread_exit((void*)NULL);
}

int main(void) {
    int maxValue;

    scanf("%d", &maxValue);

    pthread_t thread;

    pthread_create(&thread, NULL, findPrimes, (void*)&maxValue);

    pthread_join(thread, NULL);

    fflush(stdout);
    return 0;
}