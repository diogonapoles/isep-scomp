/*
 * Copyright (c) 2023 Diogo Nápoles
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <unistd.h>

#define BUFFER_SIZE 80

int main() {
  char read_msg[BUFFER_SIZE];
  char write_msg[BUFFER_SIZE];
  int fd[2];
  pid_t pid;

  if (pipe(fd) == -1) {
    perror("Pipe failed!");
    exit(EXIT_FAILURE);
  }

  pid = fork();

  if (pid < 0) {
    perror("Fork failed!");
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    // parent
    if (close(fd[0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    pid_t p_pid = getpid();
    snprintf(write_msg, sizeof(write_msg), "%d",
             p_pid); // convert pid to a string
    printf("Parent PID: %s\n", write_msg);

    if (write(fd[1], write_msg, strlen(write_msg)) == -1) {
      perror("Write failed!");
      exit(EXIT_FAILURE);
    }

    if (close(fd[1]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }
  } else {
    // child
    if (close(fd[1]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    if (read(fd[0], read_msg, BUFFER_SIZE) == -1) {
      perror("Read failed!");
      exit(EXIT_FAILURE);
    }

    printf("Child read parent pid: %s\n", read_msg);

    if (close(fd[0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
  }

  return 0;
}
