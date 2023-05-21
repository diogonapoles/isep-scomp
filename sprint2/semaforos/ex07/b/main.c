#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>
#include <sys/mman.h>
#include <sys/semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NUM_CHILD 6
#define CHIPS_SEM "/chips1"
#define BEER_SEM "/beer1"

#define SHM "/shm07"

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

typedef struct {
  int chips;
  int beer;
} food;

int main() {
  srand(time(NULL));

  int fd, data_size = sizeof(food);
  food *f;

  sem_t *chips_sem;
  sem_t *beer_sem;

  // Create and initialize shared memory
  if ((fd = shm_open(SHM, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) ==
      -1) {
    perror("Shm_open failed!");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(fd, data_size) == -1) {
    perror("Ftruncate failed!");
    exit(EXIT_FAILURE);
  }

  if ((f = (food *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                        0)) == MAP_FAILED) {
    perror("Mmap failed!");
    exit(EXIT_FAILURE);
  }

  f->chips = 0;
  f->beer = 0;

  // Create and initialize semaphores
  chips_sem = sem_open(CHIPS_SEM, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (chips_sem == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }
  beer_sem = sem_open(BEER_SEM, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (beer_sem == SEM_FAILED) {
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
        down(chips_sem);       // Enter critical section
        buy_chips();
        f->chips++;
        up(chips_sem); // Signal that chips are acquired
      } else {
        // Process buys beer
        sleep(rand() % 5); // Sleep for some random time
        down(beer_sem);       // Enter critical section
        buy_beer();
        f->beer++;
        up(beer_sem); // Signal that beer is acquired
      }

      while (f->beer + f->chips != NUM_CHILD + 1) {
        sleep(1); // Sleep for some random time
        if ((f->beer + f->chips) == NUM_CHILD + 1 && f->beer > 0 &&
            f->chips > 0)
          eat_and_drink();
      }

      exit(EXIT_SUCCESS);
    }
  }

  // Parent process
  sleep(rand() % 5); // Sleep for some random time
  int choice = (NUM_CHILD + 1) % 2;
  if (choice == 0) {
    // Process buys chips
    sleep(rand() % 5); // Sleep for some random time
    down(chips_sem);       // Enter critical section
    buy_chips();
    f->chips++;
    up(chips_sem); // Signal that chips are acquired
  } else {
    // Process buys beer
    sleep(rand() % 5); // Sleep for some random time
    down(beer_sem);       // Enter critical section
    buy_beer();
    f->beer++;
    up(beer_sem); // Signal that beer is acquired
  }

  while (f->beer + f->chips != NUM_CHILD + 1) {
    sleep(1);
    if ((f->beer + f->chips) == NUM_CHILD + 1 && f->beer > 0 && f->chips > 0)
      eat_and_drink();
  }

  if (f->beer == 0 || f->chips == 0)
    printf("Not enough beer or chips!\n");

  // Wait for all child processes to finish
  for (int i = 0; i < NUM_CHILD; i++) {
    if (wait(NULL) == -1) {
      perror("Wait failed!");
      exit(EXIT_FAILURE);
    }
  }

  // Close and unlink shared memory
  if (close(fd) == -1) {
    perror("Close failed!");
    exit(EXIT_FAILURE);
  }

  if (munmap(f, data_size) == -1) {
    perror("Munmap failed!");
    exit(EXIT_FAILURE);
  }

  if (shm_unlink(SHM) == -1) {
    perror("Shm_unlink failed!");
    exit(EXIT_FAILURE);
  }

  // Close and unlink semaphores
  if (sem_close(chips_sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_close(beer_sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }

  if (sem_unlink(CHIPS_SEM) == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink(BEER_SEM) == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }

  return 0;
}
