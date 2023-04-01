#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 80

int main() {
  char read_msg1[BUFFER_SIZE];
  char read_msg2[BUFFER_SIZE];
  char write_msg1[] = "Hello World!\n";
  char write_msg2[] = "Goodbye!\n";
  int fd[2];
  pid_t pid;

  if (pipe(fd) == -1) {
    perror("Pipe failed!");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid == -1) {
    perror("Fork failed!");
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    // parent
    if (close(fd[0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    write(fd[1], write_msg1, strlen(write_msg1) + 1);
    write(fd[1], write_msg2, strlen(write_msg2) + 1);

    if (close(fd[1]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    int status;
    wait(&status);
    printf("Child PID = %d, Exit value = %d\n", pid, WEXITSTATUS(status));
  } else {
    // child
    if (close(fd[1]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    if (read(fd[0], read_msg1, strlen(write_msg1) + 1) == -1) {
      perror("Read failed!");
      exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, read_msg1, strlen(read_msg1));

    if (read(fd[0], read_msg2, strlen(write_msg2) + 1) == -1) {
      perror("Read failed!");
      exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, read_msg2, strlen(read_msg2));

    if (close(fd[0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }

  return 0;
}
