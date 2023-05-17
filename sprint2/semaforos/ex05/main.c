#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>

#define SEMAPHORES_NUMBER 3
#define CHILDREN_NUMBER 3

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
  printf("%s", text);
  fflush(stdout);
}

int main() {
  sem_t *sem[SEMAPHORES_NUMBER];

  // open semaphores
  for (int i = 0; i < SEMAPHORES_NUMBER; i++) {
    char semaphoreName[16];
    snprintf(semaphoreName, sizeof(semaphoreName), "/semaphore%d", i);
    sem[i] = sem_open(semaphoreName, O_CREAT | O_EXCL, 0644, i == 0 ? 1 : 0);
    if (sem[i] == SEM_FAILED) {
      perror("Semaphore creation failed!");
      exit(EXIT_FAILURE);
    }
  }

  int id;
  for (id = 0; id < CHILDREN_NUMBER; id++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("Fork failed!");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      break; // child
    }
  }

  switch (id) {
  case 0:
    down(sem[0]);
    print_text("Sistemas ");
    up(sem[1]);

    down(sem[0]);
    print_text("a ");
    up(sem[1]);
    exit(EXIT_SUCCESS);

  case 1:
    down(sem[1]);
    print_text("de ");
    up(sem[2]);

    down(sem[1]);
    print_text("melhor ");
    up(sem[2]);
    exit(EXIT_SUCCESS);

  case 2:
    down(sem[2]);
    print_text("Computadores - ");
    up(sem[0]);

    down(sem[2]);
    print_text("disciplina!\n");
    exit(EXIT_SUCCESS);

  default:
    break;
  }

  for (int i = 0; i < CHILDREN_NUMBER; i++) {
    if (wait(NULL) == -1) {
      perror("Wait failed!");
      exit(EXIT_FAILURE);
    };
  }

  // close semaphores
  for (int i = 0; i < SEMAPHORES_NUMBER; i++) {
    if (sem_close(sem[i]) == -1) {
      perror("Semaphore close failed!");
      exit(EXIT_FAILURE);
    }
  }

  // destroy semaphores
  for (int i = 0; i < SEMAPHORES_NUMBER; i++) {
    char semaphoreName[16];
    snprintf(semaphoreName, sizeof(semaphoreName), "/semaphore%d", i);
    if (sem_unlink(semaphoreName) == -1) {
      perror("Semaphore unlink failed!");
      exit(EXIT_FAILURE);
    }
  }

  return EXIT_SUCCESS;
}
