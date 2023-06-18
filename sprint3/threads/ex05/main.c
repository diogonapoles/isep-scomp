#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_pthread/_pthread_t.h>
#include <time.h>

#define ARR_SIZE 1000
#define THREADS 3

static int global_min, global_max;
static float global_avg;

void * min_balance(void * data) {
    size_t i;
    int min = INT_MAX;
    const int * balances = data;

    for (i = 0; i < ARR_SIZE; i++) {
        if (balances[i] < min) {
            min = balances[i];
        }
    }

    global_min = min;
    return NULL;
}

void * max_balance(void * data) {
    size_t i;
    int max = INT_MIN;
    const int * balances = data;

    for (i = 0; i < ARR_SIZE; i++) {
        if (balances[i] > max) {
            max = balances[i];
        }
    }

    global_max = max;
    return NULL;
}

void * avg_balance(void * data) {
    size_t i;
    float avg = 0;
    const int * balances = data;

    for (i = 0; i < ARR_SIZE; i++) {
        avg += balances[i];
    }

    global_avg = avg / ARR_SIZE;
    return NULL;
}

int main () {
    pthread_t threads[THREADS];
    int balances[ARR_SIZE];

    srand(time(NULL));

    for (size_t i = 0; i < ARR_SIZE; i++) {
        balances[i] = rand() % 10000000;
    }

    for (size_t i = 0; i < THREADS; i++) {
        switch (i){
            case 0:
                if (pthread_create(&threads[i], NULL, min_balance, balances) != 0) {
                    perror("pthread_create");
                    return EXIT_FAILURE;
                }
                break;
            case 1:
                if (pthread_create(&threads[i], NULL, max_balance, balances) != 0) {
                    perror("pthread_create");
                    return EXIT_FAILURE;
                }
                break;
            case 2:
                if (pthread_create(&threads[i], NULL, avg_balance, balances) != 0) {
                    perror("pthread_create");
                    return EXIT_FAILURE;
                }
                break;
        }
    }

    for (size_t i = 0; i < THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    printf("Min: %d$\n", global_min);
    printf("Max: %d$\n", global_max);
    printf("Avg: %.2f$\n", global_avg);

}