#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define N 1000000

typedef struct {
  int number;
} shared_memory;

volatile sig_atomic_t flag = 0; // atomic so it can be safely accessed by signal
                                // handler and main function

void handler(int signum) { flag = 1; }

int main(void) {
  int fd, shared_memory_size = sizeof(shared_memory);
  shared_memory *block;

  if ((fd = shm_open("/shmab", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
    perror("Error creating shared memory");
    return EXIT_FAILURE;
  }

  if (ftruncate(fd, shared_memory_size) == -1) {
    perror("Error allocating memory");
    return EXIT_FAILURE;
  }

  if ((block = (shared_memory *)mmap(NULL, shared_memory_size,
                                     PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                     0)) == MAP_FAILED) {
    perror("Error mapping memory");
    return EXIT_FAILURE;
  }

  block->number = 100;

  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("Error setting signal handler");
    return EXIT_FAILURE;
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("Error creating process");
    return EXIT_FAILURE;
  }

  if (pid == 0) {
    size_t i;
    pid_t parent = getppid();

    for (i = 0; i < N; i++) {
      while (!flag) {
      }
      flag = 0;
      block->number++;
      block->number--;
      kill(parent, SIGUSR1);
    }
    exit(EXIT_SUCCESS);
  } else {
    size_t i;

    for (i = 0; i < N; i++) {
      block->number++;
      block->number--;
      kill(pid, SIGUSR1); // signal child
      while (!flag) {
      }
      flag = 0;

      printf("\x1B[1A\x1B[2K"); // move cursor up
      printf("Working... %.2f%%\n", (float)i / N * 100);
    }

    printf("Final value: %d\n", block->number);
    wait(NULL);
  }

  if (munmap(block, shared_memory_size) == -1) {
    perror("Error unmapping memory");
    return EXIT_FAILURE;
  }

  if (close(fd) == -1) {
    perror("Error closing shared memory");
    return EXIT_FAILURE;
  }

  if (shm_unlink("/shmab") == -1) {
    perror("Error unlinking shared memory");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
