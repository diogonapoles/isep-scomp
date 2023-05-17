#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBERS_COUNT 200

int main() {
  // delete any previous version of the file Output.txt
  remove("Output.txt");

  for (int i = 0; i < 8; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("Fork failed!");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      // Child process
      FILE *numbersFile = fopen("Numbers.txt", "r");
      FILE *outputFile = fopen("Output.txt", "a");

      if (numbersFile == NULL || outputFile == NULL) {
        perror("Fopen failed!");
        exit(EXIT_FAILURE);
      }

      int number;
      for (int j = 0; j < NUMBERS_COUNT; j++) {
        fscanf(numbersFile, "%d", &number);
        fprintf(outputFile, "[%d] %d\n", getpid(), number);
      }

      fclose(numbersFile);
      fclose(outputFile);

      exit(EXIT_SUCCESS);
    }
  }

  // Wait for all child processes to finish
  for (int i = 0; i < 8; i++) {
    if (wait(NULL) == -1) {
      perror("Wait failed!");
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}
