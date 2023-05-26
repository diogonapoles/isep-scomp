
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define THREAD_COUNT 10
#define SEARCH_SIZE 100

int array[ARRAY_SIZE];
int target_number;
int found_thread = -1;
int number_found = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread function
void *search_thread(void *arg) {
  int thread_num = *(int *)arg;
  int start_index = thread_num * SEARCH_SIZE;
  int end_index = start_index + SEARCH_SIZE;

  for (int i = start_index; i < end_index; i++) {
    if (array[i] == target_number) {
      pthread_mutex_lock(&mutex);
      if (!number_found) {
        found_thread = thread_num;
        number_found = 1;
      }
      pthread_mutex_unlock(&mutex);
      return (void *)(intptr_t)thread_num;
    }
  }

  return NULL;
}

int is_duplicate(int *arr, int size, int value) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == value) {
      return 1;
    }
  }
  return 0;
}

void generate_unique_random_array(int *arr, int size, int limit) {
  for (int i = 0; i < size; i++) {
    int num;
    do {
      num = rand() % limit;
    } while (is_duplicate(arr, i, num));
    arr[i] = num;
  }
}

int main() {
  srand(time(NULL));

  printf("Enter the number to search for: ");
  scanf("%d", &target_number);

  pthread_t threads[THREAD_COUNT];

  generate_unique_random_array(array, ARRAY_SIZE, 1000);

  for (int i = 0; i < THREAD_COUNT; i++) {
    int *thread_num = malloc(sizeof(int));
    *thread_num = i;
    pthread_create(&threads[i], NULL, search_thread, thread_num);
  }

  for (int i = 0; i < THREAD_COUNT; i++) {
    void *thread_result;
    pthread_join(threads[i], &thread_result);

    if (thread_result != NULL && number_found) {
      int found_thread_num = (intptr_t)thread_result;
      printf("Thread %d found the number.\n", found_thread_num);
    }
  }

  if (!number_found) {
    printf("Number not found.\n");
  }

  pthread_mutex_destroy(&mutex);

  return 0;
}
