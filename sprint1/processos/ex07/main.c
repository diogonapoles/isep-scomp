#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ARRAY_SIZE 1000

int main() {
  int numbers[ARRAY_SIZE]; /* array to lookup */
  int n;                   /* the number to find */
  time_t t; /* needed to initialize random number generator (RNG) */
  int i, count = 0, status;

  /* intializes RNG (srand():stdlib.h; time(): time.h) */
  srand((unsigned)time(&t));

  /* initialize array with random numbers (rand(): stdlib.h) */
  for (i = 0; i < ARRAY_SIZE; i++)
    numbers[i] = rand() % 10000;

  /* initialize n */
  n = rand() % 10000;

  printf("Número de vezes que %d aparece no array \n", n);
  pid_t pid = fork();

  if (pid == 0) { // child process
    int child_count = 0;
    for (i = 0; i < ARRAY_SIZE / 2; i++) {
      if (numbers[i] == n) {
        child_count++;
      }
    }
    printf("[Processo filho]: %dx\n", child_count);
    exit(0);
  } else if (pid > 0) { // parent process
    int parent_count = 0;
    for (i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
      if (numbers[i] == n) {
        parent_count++;
      }
    }
    printf("[Processo pai]: %dx\n", parent_count);
    wait(&status); // wait for child process to finish

    if (WIFEXITED(status)) { // check if child process exited normally
      count = WEXITSTATUS(status) + parent_count;
      printf("[TOTAL]: %dx\n", count);
    }

  } else { // error
    perror("Fork falhou");
    exit(1);
  }

  return 0;
}
