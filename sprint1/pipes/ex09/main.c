/*
 * Copyright (c) 2023 Diogo Nápoles
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ARRAY_SIZE 50000
#define CHUNK_SIZE 5000
#define SEARCH_THRESHOLD 20

typedef struct {
  int customer_code;
  int product_code;
  int quantity;
} SaleRecord;

void search_sales(int start, int end, SaleRecord *sales, int write_fd) {
  for (int i = start; i < end; i++) {
    if (sales[i].quantity > SEARCH_THRESHOLD) {
      write(write_fd, &sales[i].product_code, sizeof(int));
    }
  }

  if (close(write_fd) == -1) { // close write end
    perror("Close error!");
    exit(EXIT_FAILURE);
  }
}

int main() {
  SaleRecord sales[ARRAY_SIZE];
  int products[ARRAY_SIZE];
  int num_found = 0;
  int pipes[10][2]; // 10 pipes for communication between child and parent

  // Fill the sales array with random values
  srand(time(NULL));
  for (int i = 0; i < ARRAY_SIZE; i++) {
    sales[i].customer_code = rand() % 100;
    sales[i].product_code = rand() % 1000;
    sales[i].quantity = rand() % 22;
  }

  // Create 10 child processes and pipes for communication
  for (int i = 0; i < 10; i++) {
    if (pipe(pipes[i]) < 0) {
      perror("Pipe error!");
      exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
      perror("Fork error!");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
      int start = i * CHUNK_SIZE;
      int end = start + CHUNK_SIZE;

      if (close(pipes[i][0]) == -1) { // close read end of pipe
        perror("Close error!");
        exit(EXIT_FAILURE);
      }

      search_sales(start, end, sales, pipes[i][1]);
      exit(EXIT_SUCCESS);
    }
  }

  // Wait for all child processes to finish and read from pipes
  for (int i = 0; i < 10; i++) {
    int status, product_code;
    pid_t child_pid = wait(&status);
    if (WIFEXITED(status)) {
      printf("-> Child %d exited with status %d!\n", child_pid,
             WEXITSTATUS(status));
    }

    if (close(pipes[i][1]) == -1) { // close write end of pipes
      perror("Close error!");
      exit(EXIT_FAILURE);
    }

    while (read(pipes[i][0], &product_code, sizeof(int)) == sizeof(int)) {
      products[num_found++] = product_code;
    }

    if (close(pipes[i][0]) == -1) { // close read end of pipe
      perror("Close error!");
      exit(EXIT_FAILURE);
    }
  }

  // Print the product codes that have sold more than 20 units
  printf("Products that have sold more than %d units in a single sale:\n",
         SEARCH_THRESHOLD);
  for (int i = 0; i < num_found; i++) {
    printf("%d ", products[i]);
  }
  printf("\n");

  exit(EXIT_SUCCESS);
}
