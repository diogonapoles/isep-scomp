#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/semaphore.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SEM "/e9M"

void up(sem_t *sem) {
  if (sem_post(sem) == -1) {
    perror("Sem_post failed!");
    exit(EXIT_FAILURE);
  }
}

void down(sem_t *sem) {
  if (sem_wait(sem) == -1) {
    perror("Sem_wait failed!");
    exit(EXIT_FAILURE);
  }
}

void print_text(char *text) {
  printf("[PID: %d] %s", getpid(), text);
  fflush(stdout);
}

unsigned int parse_execution_time(char *value, char *unit) {
  unsigned int time = 0;
  int multiplier = 0;

  if (strcmp(unit, "us") == 0) {
    multiplier = 1;
  } else if (strcmp(unit, "ms") == 0) {
    multiplier = 1000;
  } else if (strcmp(unit, "s") == 0) {
    multiplier = 1000000;
  } else {
    printf("Invalid time unit. Please use 'us', 'ms', or 's'.\n");
    exit(EXIT_FAILURE);
  }

  time = atoi(value) * multiplier;

  if (time == 0) {
    printf("Invalid time format. Please use the format 'X us', 'X ms', or 'X "
           "s' where X is an integer.\n");
    exit(EXIT_FAILURE);
  }

  // check range
  if (time < 100 || time > 1000000) {
    printf("Invalid time value. Please ensure the time is greater than 100 us "
           "and less than 1 s.\n");
    exit(EXIT_FAILURE);
  }

  return time / 1000000;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Usage: %s n time\n", argv[0]);
    printf("n - number of child processes\n");
    printf("time - execution time for each child process\n");
    exit(EXIT_FAILURE);
  }
  int num_processes = atoi(argv[1]);
  unsigned int execution_time = parse_execution_time(argv[2], argv[3]);

  sem_t *sem;
  if ((sem = sem_open(SEM, O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
    perror("Error creating semaphore!");
    exit(EXIT_FAILURE);
  }

  struct timeval start, finish;
  if (gettimeofday(&start, NULL) == -1) {
    perror("Error getting start time!");
    exit(EXIT_FAILURE);
  }

  int i;
  for (i = 0; i < num_processes; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      perror("Error creating child process!");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      print_text("Child process created!\n");

      sleep(execution_time);
      up(sem);

      print_text("Child process finished!\n");
      exit(EXIT_SUCCESS);
    }
  }

  for (i = 0; i < num_processes; i++) {
    down(sem);
  }

  if (sem_close(sem) == -1) {
    perror("Error closing semaphore!");
    exit(EXIT_FAILURE);
  }

  if (sem_unlink(SEM) == -1) {
    perror("Error unlinking semaphore!");
    exit(EXIT_FAILURE);
  }

  gettimeofday(&finish, NULL);

  long long start_micros = (start.tv_sec * 1000000) + start.tv_usec;
  long long end_micros = (finish.tv_sec * 1000000) + finish.tv_usec;
  long long execution_micros = end_micros - start_micros;

  printf("Execution time: %lld microseconds\n", execution_micros);

  return EXIT_SUCCESS;
}
