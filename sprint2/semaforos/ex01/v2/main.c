#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
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
        flock(fileno(numbersFile), LOCK_EX);
        fscanf(numbersFile, "%d", &number);
        flock(fileno(numbersFile), LOCK_UN);

        flock(fileno(outputFile), LOCK_EX);
        fprintf(outputFile, "[%d] %d\n", getpid(), number);
        flock(fileno(outputFile), LOCK_UN);
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

  FILE *outputFile = fopen("Output.txt", "r");
  if (outputFile == NULL) {
    perror("Fopen failed!");
    exit(EXIT_FAILURE);
  }

  int ch;
  while ((ch = fgetc(outputFile)) != EOF) {
    putchar(ch);
  }

  fclose(outputFile);

  return 0;
}
