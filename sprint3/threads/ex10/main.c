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

#define N_THREAD 6
#define N_P 5
#define N_HP 3
#define MAX_PRICE 10
#define ARR_SIZE 30

typedef struct {
    int id_h;
    int id_p;
    int p;
} Element;

typedef struct {
    int price;
    int id_h;
} LowestPrice;

typedef struct {
    int id_h;
    int total;
    int len;
    Element* elements;
} HypermarketDatabase;

typedef struct {
    int n;
    int sum;
} PriceCalculation;

pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
pthread_mutex_t mutexLowest;

LowestPrice lowest;

HypermarketDatabase hypermarket1;
HypermarketDatabase hypermarket2;
HypermarketDatabase hypermarket3;

void* filtering(void* given_data) {
    Element* vec = (Element*)given_data;

    for (int i = 0; i < ARR_SIZE / N_HP; i++) {
        switch (vec[i].id_h) {
            case 1:
                pthread_mutex_lock(&mutex1);
                hypermarket1.elements[hypermarket1.len] = vec[i];
                hypermarket1.len++;
                pthread_mutex_unlock(&mutex1);
                break;
            case 2:
                pthread_mutex_lock(&mutex2);
                hypermarket2.elements[hypermarket2.len] = vec[i];
                hypermarket2.len++;
                pthread_mutex_unlock(&mutex2);
                break;
            case 3:
                pthread_mutex_lock(&mutex3);
                hypermarket3.elements[hypermarket3.len] = vec[i];
                hypermarket3.len++;
                pthread_mutex_unlock(&mutex3);
                break;
            default:
                printf("[WARNING] There are hypermarkets that are not known to the application %d\n", vec[i].id_h);
                break;
        }
    }

    pthread_exit((void*)NULL);
}

void* computing(void* given_data) {
    HypermarketDatabase* data = (HypermarketDatabase*)given_data;

    PriceCalculation prodPrices[N_P];
    memset(prodPrices, 0, sizeof(prodPrices));

    for (int i = 0; i < data->len; i++) {
        int id_p = data->elements[i].id_p;
        prodPrices[id_p].sum += data->elements[i].p;
        prodPrices[id_p].n++;
    }

    for (int i = 0; i < N_P; i++) {
        if (prodPrices[i].n > 0) {
            data->total += (prodPrices[i].sum / prodPrices[i].n);
        }
    }

    pthread_mutex_lock(&mutexLowest);
    if (data->total < lowest.price) {
        lowest.price = data->total;
        lowest.id_h = data->id_h;
    }
    pthread_mutex_unlock(&mutexLowest);

    pthread_exit((void*)NULL);
}

int main(void) {
    pthread_t threads[N_THREAD];
    srand(time(NULL));

    Element* vec = calloc(ARR_SIZE, sizeof(Element));

    for (int i = 0; i < ARR_SIZE; i++) {
        vec[i].id_h = (rand() % N_HP) + 1;
        vec[i].id_p = rand() % N_P;
        vec[i].p = (rand() % MAX_PRICE) + 1;
    }

    hypermarket1.len = 0;
    hypermarket1.id_h = 1;
    hypermarket1.elements = calloc(ARR_SIZE, sizeof(Element));

    hypermarket2.len = 0;
    hypermarket2.id_h = 2;
    hypermarket2.elements = calloc(ARR_SIZE, sizeof(Element));

    hypermarket3.len = 0;
    hypermarket3.id_h = 3;
    hypermarket3.elements = calloc(ARR_SIZE, sizeof(Element));

    lowest.price = INT_MAX;

    pthread_mutex_init(&mutexLowest, NULL);
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);

    for (int i = 0; i < N_HP; i++) {
        pthread_create(&threads[i], NULL, filtering, (void*)&vec[i * (ARR_SIZE / N_HP)]);
    }

    for (int i = 0; i < N_HP; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[INFO] FILTERING DONE!!\n");

    pthread_create(&threads[3], NULL, computing, (void*)&hypermarket1);
    pthread_create(&threads[4], NULL, computing, (void*)&hypermarket2);
    pthread_create(&threads[5], NULL, computing, (void*)&hypermarket3);

    for (int i = N_HP; i < N_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("[INFO] COMPUTING DONE!!\n");

    printf("\n=============================\n");
    printf("\tHypermarket 1:\n");
    printf("\tPrice: %d\n", hypermarket1.total);

    printf("\n=============================\n");
    printf("\tHypermarket 2:\n");
    printf("\tPrice: %d\n", hypermarket2.total);

    printf("\n=============================\n");
    printf("\tHypermarket 3:\n");
    printf("\tPrice: %d\n", hypermarket3.total);

    printf("\n===========SUMMARY===========\n");
    printf("\tLowest price: %d\n", lowest.price);
    printf("\tBy hypermarket: %d\n", lowest.id_h);

    free(vec);
    free(hypermarket1.elements);
    free(hypermarket2.elements);
    free(hypermarket3.elements);

    pthread_mutex_destroy(&mutexLowest);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);

    return 0;
}