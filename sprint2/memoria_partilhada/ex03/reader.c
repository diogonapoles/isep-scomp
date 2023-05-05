#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ARRAY_SIZE 10

typedef struct {
  int array[ARRAY_SIZE];
} integers;

int main(int argc, char *argv[]) {
  int fd, data_size = sizeof(integers);
  integers *shared_data;

  /* Open a new shared memory object named "/shm3" with read and write for the
   * user. If the shared memory already exists, shm_open will fail. */
  if ((fd = shm_open("/shmex3", O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
    perror("Failed to open shared memory!");
    exit(EXIT_FAILURE);
  }

  if ((shared_data = (integers *)mmap(NULL, data_size, PROT_READ | PROT_WRITE,
                                      MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("Failed to map shared memory!");
    exit(EXIT_FAILURE);
  }

  if (shared_data == NULL) {
    perror("Failed to create shared memory!");
    exit(EXIT_FAILURE);
  }

  int average = 0;
  for (size_t j = 0; j < ARRAY_SIZE; j++) {
    while (!shared_data->array[j]) {
      sleep(1);
    }

    printf("array[%ld] = %d\n", j, shared_data->array[j]);
    average += shared_data->array[j];
  }
  printf("average = %d\n", average / ARRAY_SIZE);

  if (munmap(shared_data, data_size) == -1) {
    perror("Failed to unmap shared memory!");
    exit(EXIT_FAILURE);
  }

  if (close(fd) == -1) {
    perror("Failed to close shared memory!");
    exit(EXIT_FAILURE);
  }

  // delete shared memory space
  if (shm_unlink("/shmex3") == -1) {
    perror("Error while deleting memory space");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
