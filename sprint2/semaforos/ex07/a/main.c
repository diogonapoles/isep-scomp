#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>
#include <sys/semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NUM_CHILD 1

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

void buy_chips() {
  print_text("Buying chips...\n");
  sleep(1); // Simulating buying chips
  print_text("Chips acquired!\n");
}

void buy_beer() {
  print_text("Buying beer...\n");
  sleep(1); // Simulating buying beer
  print_text("Beer acquired!\n");
}

void eat_and_drink() {
  print_text("Eating and drinking...\n");
  sleep(2); // Simulating eating and drinking
  print_text("Finished eating and drinking!\n");
}

int main() {
  srand(time(NULL));

  sem_t *chips_sem;
  sem_t *beer_sem;

  // Create and initialize semaphores
  chips_sem = sem_open("/chips_sem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (chips_sem == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }
  beer_sem = sem_open("/beer_sem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (beer_sem == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("Fork failed!");
    exit(EXIT_FAILURE);
  }
  if (pid == 0) {
    // Child process (Process 2)
    sleep(rand() % 5); // Sleep for some random time

    buy_beer();
    up(beer_sem);    // Signal that beer is acquired
    down(chips_sem); // Wait for chips to be acquired

    eat_and_drink();

    if (sem_close(chips_sem) == -1) {
      perror("Sem_close failed!");
      exit(EXIT_FAILURE);
    }
    if (sem_close(beer_sem) == -1) {
      perror("Sem_close failed!");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
  }

  sleep(rand() % 5); // Sleep for some random time

  buy_chips();
  up(chips_sem);  // Signal that chips are acquired
  down(beer_sem); // Wait for beer to be acquired

  eat_and_drink();

  if (sem_close(chips_sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_close(beer_sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }

  // Wait for child processes to finish
  for (int i = 0; i < NUM_CHILD; i++) {
    if (wait(NULL) == -1) {
      perror("Wait failed!");
      exit(EXIT_FAILURE);
    }
  }

  // Close and unlink semaphores
  if (sem_unlink("/chips_sem") == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink("/beer_sem") == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }
  return 0;
}
