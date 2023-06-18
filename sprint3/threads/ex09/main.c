#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define TRAVELS 5
#define NUM_TRACKS 3

pthread_mutex_t g_mutex[NUM_TRACKS];

void *travel_ab(void *arg) {
  pthread_mutex_lock(&g_mutex[0]);
  int n = rand() % 3 + 1;
  sleep(n);
  printf("Train %lu | from A to B. Elapsed time: %d. \n",
         (unsigned long)pthread_self(), n);
  pthread_mutex_unlock(&g_mutex[0]);

  pthread_exit((void *)NULL);
}

void *travel_bc(void *arg) {
  pthread_mutex_lock(&g_mutex[1]);
  int n = rand() % 3 + 1;
  sleep(n);
  printf("Train %lu | from B to C. Elapsed time: %d. \n",
         (unsigned long)pthread_self(), n);
  pthread_mutex_unlock(&g_mutex[1]);

  pthread_exit((void *)NULL);
}

void *travel_bd(void *arg) {
  pthread_mutex_lock(&g_mutex[2]);
  int n = rand() % 3 + 1;
  sleep(n);
  printf("Train %lu | from B to D. Elapsed time: %d. \n",
         (unsigned long)pthread_self(), n);
  pthread_mutex_unlock(&g_mutex[2]);

  pthread_exit((void *)NULL);
}

int main() {
  pthread_t trains[TRAVELS];

  for (size_t i = 0; i < NUM_TRACKS; i++) {
    if (pthread_mutex_init(&g_mutex[i], NULL)) {
      perror("Error initializing mutex");
      exit(EXIT_FAILURE);
    }
  }

  unsigned int path;
  for (size_t i = 0; i < TRAVELS; i++) {
    srand(time(NULL) + i);
    path = rand() % 3;
    switch (path) {
    case 0:
      if (pthread_create(&trains[i], NULL, travel_ab, NULL)) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
      }
      break;
    case 1:
      if (pthread_create(&trains[i], NULL, travel_bc, NULL)) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
      }
      break;
    case 2:
      if (pthread_create(&trains[i], NULL, travel_bd, NULL)) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
      }
      break;
    }
  }

  for (size_t i = 0; i < TRAVELS; i++) {
    if (pthread_join(trains[i], NULL)) {
      perror("Error joining thread");
      exit(EXIT_FAILURE);
    }
  }

  for (size_t i = 0; i < NUM_TRACKS; i++) {
    if (pthread_mutex_destroy(&g_mutex[i])) {
      perror("Error destroying mutex");
      exit(EXIT_FAILURE);
    }
  }
}