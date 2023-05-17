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

#define NUM_CHILD 5

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
  sem_t *mutex;

  // Create and initialize semaphores
  chips_sem = sem_open("/chips1", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
  if (chips_sem == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }
  beer_sem = sem_open("/beer1", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
  if (beer_sem == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }
  mutex = sem_open("/mutex1", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (mutex == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }

  pid_t pid;

  for (int i = 0; i < NUM_CHILD; i++) {
    pid = fork();
    if (pid == -1) {
      perror("Fork failed!");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      // Child processes
      int choice = i % 2;
      if (choice == 0) {
        // Process buys chips
        sleep(rand() % 5); // Sleep for some random time
        down(mutex);       // Enter critical section
        buy_chips();
        up(mutex);     // Exit critical section
        up(chips_sem); // Signal that chips are acquired
      } else {
        // Process buys beer
        sleep(rand() % 5); // Sleep for some random time
        down(mutex);       // Enter critical section
        buy_beer();
        up(mutex);    // Exit critical section
        up(beer_sem); // Signal that beer is acquired
      }

      eat_and_drink();

      if (sem_close(chips_sem) == -1) {
        perror("Sem_close failed!");
        exit(EXIT_FAILURE);
      }
      if (sem_close(beer_sem) == -1) {
        perror("Sem_close failed!");
        exit(EXIT_FAILURE);
      }
      if (sem_close(mutex) == -1) {
        perror("Sem_close failed!");
        exit(EXIT_FAILURE);
      }

      exit(EXIT_SUCCESS);
    }
  }

  // Parent process
  sleep(rand() % 5); // Sleep for some random time

  int choice = (NUM_CHILD + 1) % 2;
  if (choice == 0) {
    // Parent buys chips
    down(mutex); // Enter critical section
    buy_chips();
    up(mutex);     // Exit critical section
    up(chips_sem); // Signal that chips are acquired
  } else {
    // Parent buys beer
    down(mutex); // Enter critical section
    buy_beer();
    up(mutex);    // Exit critical section
    up(beer_sem); // Signal that beer is acquired
  }

  eat_and_drink();

  if (sem_close(chips_sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_close(beer_sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_close(mutex) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }

  // Wait for all child processes to finish
  for (int i = 0; i < NUM_CHILD; i++) {
    if (wait(NULL) == -1) {
      perror("Wait failed!");
      exit(EXIT_FAILURE);
    }
  }

  // Close and unlink semaphores
  if (sem_unlink("/chips1") == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink("/beer1") == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink("/mutex1") == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }

  return 0;
}
