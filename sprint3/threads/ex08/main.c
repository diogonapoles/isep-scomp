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

#define N_THREAD 5
#define ARR_SIZE 1000
#define PARTITION_SIZE(s, n) s / n

typedef struct {
    int* input_arr;
    int* outputArr;
    int start_index;
    int end_index;
} Data;

pthread_mutex_t mutex;

void* threadWork(void* arg) {
    Data* data = (Data*)arg;

    for (int i = data->start_index; i <= data->end_index; i++) {
        data->outputArr[i] = (data->input_arr[i] * 10) + 2;
    }

    pthread_mutex_lock(&mutex);
    printf("\nPRINTING FROM %d - %d", data->start_index, data->end_index);
    for (int i = data->start_index; i <= data->end_index; i++) {
        if ((i - data->start_index) % 20 == 0) {
            printf("\n");
            fflush(stdout);
        }
        printf("%d ", data->outputArr[i]);
        fflush(stdout);
    }
    printf("\n");
    pthread_mutex_unlock(&mutex);

    pthread_exit((void*)NULL);
}

int main(void) {
    pthread_t threads[N_THREAD];
    srand(time(NULL));

    int* input_arr = calloc(ARR_SIZE, sizeof(int));
    for (int i = 0; i < ARR_SIZE; i++) {
        // input_arr[i] = i;

        input_arr[i] = rand() % 10;
    }

    int* outputArr = calloc(ARR_SIZE, sizeof(int));

    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("Mutex init: ");
        return EXIT_FAILURE;
    }

    Data threadData[N_THREAD];
    for (int i = 0; i < N_THREAD; i++) {
        threadData[i].outputArr = outputArr;
        threadData[i].input_arr = input_arr;
        threadData[i].start_index = i * PARTITION_SIZE(ARR_SIZE, N_THREAD);
        threadData[i].end_index = (i + 1) * PARTITION_SIZE(ARR_SIZE, N_THREAD) - 1;
        pthread_create(&threads[i], NULL, threadWork, (void*)&threadData[i]);
    }

    for (int i = 0; i < N_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

    free(outputArr);
    free(input_arr);
    pthread_mutex_destroy(&mutex);

    return 0;
}