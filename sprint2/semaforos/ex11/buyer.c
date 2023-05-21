#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/semaphore.h>
#include <time.h>
#include <unistd.h>

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

void close_sem(sem_t *sem) {
  if (sem_close(sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }
}

void unlink_sem(char *name) {
  if (sem_unlink(name) == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }
}

void print_text(char *text) {
  printf("[SELLER WITH PID: %d] %s", getpid(), text);
  fflush(stdout);
}

#define AVAILABLE_TICKETS 5

int main(void) {
  int fd, *shared_tickets;
  sem_t *seller_sem, *client_sem, *handler_sem;

  if ((fd = shm_open("/shm_tickets", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) ==
      -1) {
    perror("SHM_OPEN failed!");
    exit(EXIT_FAILURE);
  }

  shared_tickets =
      mmap(NULL, sizeof(int), PROT_READ, MAP_SHARED, fd, 0);
  if (shared_tickets == MAP_FAILED) {
    perror("MMAP failed!");
    exit(EXIT_FAILURE);
  }

  if ((seller_sem = sem_open("/SELLER", O_CREAT, 0644, 0)) ==
      SEM_FAILED) {
    perror("SEM_OPEN failed!");
    exit(EXIT_FAILURE);
  }

  if ((handler_sem = sem_open("/HANDLER", O_CREAT, 0644, 0)) ==
      SEM_FAILED) {
    perror("SEM_OPEN failed!");
    exit(EXIT_FAILURE);
  }

  if ((client_sem = sem_open("/CLIENT", O_CREAT, 0644, 1)) ==
      SEM_FAILED) {
    perror("SEM_OPEN failed!");
    exit(EXIT_FAILURE);
  }

  down(client_sem);  // blocks the client semaphore
  up(seller_sem);    // unblocks the seller semaphore
  down(handler_sem); // blocks the handler semaphore
  printf("Buyer bought ticket number %d\n", *shared_tickets);
  up(client_sem); // unblocks the client semaphore

  return EXIT_SUCCESS;
}
