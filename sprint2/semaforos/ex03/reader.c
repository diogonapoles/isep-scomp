
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_DELAY 5
#define MESSAGE "I'm the Father - with PID"

#include "common.h"

int main(void) {
  size_t len;
  const pid_t pid = getpid();

  sem_t *mutex;
  shared *shared_data;

  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("Shared memory: ");
    exit(EXIT_FAILURE);
  }

  shared_data =
      mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);

  if (shared_data == MAP_FAILED) {
    perror("Error mapping shared memory");
    shm_unlink(SHM_NAME);
    exit(EXIT_FAILURE);
  }

  if ((mutex = sem_open(SEM_NAME, O_CREAT, 0640, 1)) == SEM_FAILED) {
    perror("sem_open");
    return EXIT_FAILURE;
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
    for (size_t i = 0; i < len; i++) {
      printf("%zu: %s\n", i, shared_data->arr[i]);
      sem_post(mutex);
    }
    printf("Total lines: %zu\n", len);
    break;
  }

  if (munmap(shared_data, sizeof(shared)) == -1) {
    perror("Error unmapping shared memory");
    return EXIT_FAILURE;
  }

  if (close(fd) == -1) {
    perror("Error closing shared memory");
    return EXIT_FAILURE;
  }

  if (sem_unlink(SEM_NAME) == -1) {
    perror("Error closing semaphore");
    return EXIT_FAILURE;
  }

  if (shm_unlink(SHM_NAME) == -1) {
    perror("Error removing shared memory");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
