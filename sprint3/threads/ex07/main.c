#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_pthread/_pthread_t.h>

#define DATABASE 8000
#define THREADS 16

#define PICKS 5
#define MIN_NUM 1
#define MAX_NUM 49
#define PARTIAL (DATABASE / THREADS)

static int database[DATABASE][PICKS];
static pthread_mutex_t mtx[MAX_NUM];
static int global_statistics[MAX_NUM] = {0};

void *work(void *arg) {
  size_t start = *(size_t *)arg;
  const size_t end = start + PARTIAL;

  int stats[MAX_NUM] = {0};

  for (size_t i = start; i < end; i++) {
    for (size_t j = 0; j < PICKS; j++) {
      int num = database[i][j];
      if (num >= MIN_NUM && num <= MAX_NUM) {
        stats[num - 1]++;
      }
    }
  }

  for (size_t i = 0; i < MAX_NUM; i++) {
    pthread_mutex_lock(&mtx[i]);
    global_statistics[i] += stats[i];
    pthread_mutex_unlock(&mtx[i]);
  }

  return NULL;
}

int main() {
  pthread_t threads[THREADS];

  for (size_t i = 0; i < DATABASE; i++) {
    int drawn[MAX_NUM] = {0}; /* track which numbers have been drawn */
    for (size_t j = 0; j < PICKS; j++) {
      int num = rand() % MAX_NUM;

      if (!drawn[num]) {
        drawn[num] = 1;
        database[i][j] = num + 1;
      }
    }
  }

  for (size_t i = 0; i < MAX_NUM; i++) {
    if (pthread_mutex_init(&mtx[i], NULL)) {
      perror("pthread_mutex_init");
      return EXIT_FAILURE;
    }
  }

  for (size_t i = 0; i < THREADS; i++) {
    size_t *arg = malloc(sizeof(size_t));
    *arg = i * PARTIAL;

    if (pthread_create(&threads[i], NULL, work, arg)) {
      perror("pthread_create");
      return EXIT_FAILURE;
    }
  }

  for (size_t i = 0; i < THREADS; i++) {
    if (pthread_join(threads[i], NULL)) {
      perror("pthread_join");
      return EXIT_FAILURE;
    }
  }

  printf("Statistics:\n");
  for (size_t i = 0; i < MAX_NUM; i++) {
    printf("%2zu: %d\n", i + 1, global_statistics[i]);
  }

  for (size_t i = 0; i < MAX_NUM; i++) {
    if (pthread_mutex_destroy(&mtx[i])) {
      perror("pthread_mutex_destroy");
      return EXIT_FAILURE;
    }
  }
}