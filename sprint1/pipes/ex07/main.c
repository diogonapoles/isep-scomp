/*
 * Copyright (c) 2023 Diogo Nápoles
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ARRAY_SIZE 1000
#define CHUNK_SIZE 200

void initialize_arrays(int vec1[], int vec2[], int result[]) {
  int i;
  srand(time(NULL));
  for (i = 0; i < ARRAY_SIZE; i++) {
    vec1[i] = rand() % 100;
    vec2[i] = rand() % 100;
    result[i] = 0;
  }
}

int main() {
  int vec1[ARRAY_SIZE], vec2[ARRAY_SIZE], result[ARRAY_SIZE];
  int i, j, sum, status;
  int pipes[5][2]; // first dimension corresponds to the child process

  initialize_arrays(vec1, vec2, result);

  // create pipes
  for (i = 0; i < 5; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("Pipe error!");
      exit(EXIT_FAILURE);
    }
  }

  // child processes
  for (i = 0; i < 5; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("Fork failed!");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child process
      if (close(pipes[i][0]) == -1) {
        perror("Close failed!");
        exit(EXIT_FAILURE);
      }

      for (j = i * CHUNK_SIZE; j < (i + 1) * CHUNK_SIZE; j++) {
        sum = vec1[j] + vec2[j];
        write(pipes[i][1], &sum, sizeof(sum)); // send sum to parent
      }

      if (close(pipes[i][1]) == -1) {
        perror("Close failed!");
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
  }

  // parent process
  for (i = 0; i < 5; i++) {
    if (close(pipes[i][1]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    for (j = i * CHUNK_SIZE; j < (i + 1) * CHUNK_SIZE; j++) {
      if (read(pipes[i][0], &sum, sizeof(sum)) == -1) { // read sum from child
        perror("Read failed!");
        exit(EXIT_FAILURE);
      }
      result[j] = sum; // store sum in result array
    }

    if (close(pipes[i][0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < 5; i++) {
    wait(&status);
  }

  printf("Result:\n");
  // print result array
  for (i = 0; i < ARRAY_SIZE; i++) {
    printf("%d ", result[i]);
  }
  printf("\n");

  return 0;
}
