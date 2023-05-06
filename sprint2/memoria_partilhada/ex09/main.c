#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_VALUES 30

typedef struct {
  int buffer[BUFFER_SIZE];
  int head;
  int tail;
} circular_buffer;

void write_to_buffer(circular_buffer *cb, int value) {
  cb->buffer[cb->head] = value;
  cb->head = (cb->head + 1) % BUFFER_SIZE;
}

void read_from_buffer(circular_buffer *cb, int *value) {
  *value = cb->buffer[cb->tail];
  cb->tail = (cb->tail + 1) % BUFFER_SIZE;
}

int main(void) {
  int fd, data_size = sizeof(circular_buffer);
  circular_buffer *cb;

  if ((fd = shm_open("/shm_ex9", O_CREAT | O_EXCL | O_RDWR,
                     S_IRUSR | S_IWUSR)) == -1) {
    perror("Error creating shared memory");
    return EXIT_FAILURE;
  }

  if (ftruncate(fd, data_size) == -1) {
    perror("Error allocating shared memory");
    return EXIT_FAILURE;
  }

  if ((cb = (circular_buffer *)mmap(NULL, data_size, PROT_READ | PROT_WRITE,
                                    MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("Error mapping shared memory");
    return EXIT_FAILURE;
  }

  cb->head = 0;
  cb->tail = 0;

  pid_t pid = fork();

  if (pid == -1) {
    perror("Error creating child process");
    return EXIT_FAILURE;
  }

  if (pid == 0) { // consumer
    for (size_t i = 0; i < NUM_VALUES; i++) {
      while (cb->head == cb->tail) {
        printf("[WARNING] Buffer empty, waiting...\n");
        if (sleep(1) != 0) {
          perror("Error sleeping");
          return EXIT_FAILURE;
        };
      }
      int value;
      read_from_buffer(cb, &value);
      printf("<- Consumed %d\n", value);
    }
    exit(EXIT_SUCCESS);
  } else { // producer
    int value = 0;
    for (size_t i = 0; i < NUM_VALUES; i++) {
      while ((cb->head + 1) % BUFFER_SIZE == cb->tail) {
        printf("[WARNING] Buffer full, waiting...\n");
        sleep(1);
      }
      write_to_buffer(cb, value);
      printf("-> Produced %d\n", value);
      value++;
    }
  }

  wait(NULL);
  if (munmap(cb, data_size) == -1) {
    perror("Error unmapping shared memory");
    return EXIT_FAILURE;
  }

  if (close(fd) == -1) {
    perror("Error closing shared memory");
    return EXIT_FAILURE;
  }

  if (shm_unlink("/shm_ex9") == -1) {
    perror("Error removing shared memory");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
