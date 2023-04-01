#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NUM_CHILD 5
#define NUM_PIPES 6
#define RANDOM_MAX 500

int main(void) {
  int i;
  int received, generated;
  time_t t;
  pid_t pid;
  int fd[NUM_PIPES][2];

  for (i = 0; i < NUM_PIPES; i++) {
    if (pipe(fd[i]) == -1) {
      perror("Pipe error!");
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < NUM_CHILD; i++) {
    pid = fork();

    if (pid == -1) {
      perror("Fork error!");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child process
      if (close(fd[i][1]) == -1) {
        perror("Close error!");
        exit(EXIT_FAILURE);
      }

      srand((unsigned)time(&t) ^ getpid());
      generated = rand() % RANDOM_MAX + 1;

      if (read(fd[i][0], &received, sizeof(int)) == -1) {
        perror("Read error!");
        exit(EXIT_FAILURE);
      }

      printf("Child  %d | Generated number: %d\n", getpid(), generated);

      int to_write = (generated > received) ? generated : received;
      if (write(fd[i + 1][1], &to_write, sizeof(int)) == -1) {
        perror("Write failed!");
        exit(EXIT_FAILURE);
      }

      if (close(fd[i][0]) == -1) {
        perror("Close error!");
        exit(EXIT_FAILURE);
      }
      if (close(fd[i + 1][1]) == -1) {
        perror("Close error!");
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
  }

  // parent
  if (close(fd[0][0]) == -1) {
    perror("Close error!");
    exit(EXIT_FAILURE);
  }

  srand((unsigned)time(&t) ^ getpid());
  generated = rand() % RANDOM_MAX + 1;
  printf("Parent %d | Generated number %d\n", getpid(), generated);

  if (write(fd[0][1], &generated, sizeof(int)) == -1) {
    perror("Write failed");
    exit(EXIT_FAILURE);
  }

  if (close(fd[0][1]) == -1) {
    perror("Close error!");
    exit(EXIT_FAILURE);
  }

  if (close(fd[NUM_PIPES - 1][1]) == -1) {
    perror("Close error!");
    exit(EXIT_FAILURE);
  }

  if (read(fd[NUM_PIPES - 1][0], &generated, sizeof(int)) == -1) {
    perror("Read failed");
    exit(EXIT_FAILURE);
  }

  printf("Biggest random number: %d\n", generated);

  if (close(fd[NUM_PIPES - 1][0]) == -1) {
    perror("Close error!");
    exit(EXIT_FAILURE);
  }

  return 0;
}
