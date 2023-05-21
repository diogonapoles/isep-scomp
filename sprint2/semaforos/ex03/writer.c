#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_DELAY 5
#define MESSAGE "I'm the Father - with PID"
#define MAX_STRS 5
#define MAX_STR_LEN 80

#include "common.h"

int main(void) {
  shared *shared_data;
  size_t len;
  const pid_t pid = getpid();

  sem_t *mutex;
  if ((mutex = sem_open(SEM_NAME, O_CREAT, 0640, 1)) == SEM_FAILED) {
    perror("sem_open");
    return EXIT_FAILURE;
  }

  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("Shared memory: ");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(fd, sizeof(shared)) == -1) {
    perror("Ftruncate: ");
    exit(EXIT_FAILURE);
  }

  shared_data =
      mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared_data == MAP_FAILED) {
    perror("Error mapping shared memory");
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
  }

  srand(time(NULL) * pid);

  struct timespec start_time, current_time;
  if (clock_gettime(CLOCK_REALTIME, &start_time) == -1) {
    perror("Error getting current time");
    return EXIT_FAILURE;
  }

  while (1) {
    if (sem_trywait(mutex) == -1) {
      if (clock_gettime(CLOCK_REALTIME, &current_time) == -1) {
        perror("Error getting current time");
        return EXIT_FAILURE;
      }

      // Check if the timeout of 12 seconds has passed
      if ((current_time.tv_sec - start_time.tv_sec) >= 12) {
        printf("Timeout occurred. Failed to acquire the mutex within 12 "
               "seconds\n");
        return EXIT_FAILURE;
      }

      // Sleep for a short interval before retrying
      usleep(100000); // Sleep for 100 milliseconds (0.1 seconds)
      continue;
    }

    len = shared_data->len;
    /*exit if no more msg were possible to write*/
    if (len == MAX_STRS) {
      sem_post(mutex);
      break;
    }
    snprintf(shared_data->arr[len++], MAX_STR_LEN, MESSAGE " %d", pid);
    shared_data->len = len;

    srand(time(0));
    int random_number = rand() % 100;

    if (random_number <= 30) {
      strcpy(shared_data->arr[len - 1], "");
    }

    sleep((rand() % MAX_DELAY) + 1);
    sem_post(mutex);

    printf("Writer %d: Wrote message no. %zu\n", getpid(), len);
  }

  return EXIT_SUCCESS;
}
