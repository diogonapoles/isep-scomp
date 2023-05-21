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
  sem_t *seller_sem, *handler_sem;

  if ((fd = shm_open("/shm_tickets", O_CREAT | O_TRUNC | O_RDWR,
                     S_IRUSR | S_IWUSR)) == -1) {
    perror("SHM_OPEN failed!");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(fd, sizeof(int)) == -1) {
    perror("FTRUNCATE failed!");
    exit(EXIT_FAILURE);
  }

  shared_tickets =
      mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared_tickets == MAP_FAILED) {
    perror("MMAP failed!");
    exit(EXIT_FAILURE);
  }

  if ((seller_sem = sem_open("/SELLER", O_CREAT | O_EXCL, 0644, 0)) ==
      SEM_FAILED) {
    perror("SEM_OPEN failed!");
    exit(EXIT_FAILURE);
  }

  if ((handler_sem = sem_open("/HANDLER", O_CREAT | O_EXCL, 0644, 0)) ==
      SEM_FAILED) {
    perror("SEM_OPEN failed!");
    exit(EXIT_FAILURE);
  }

  *shared_tickets = 0;
  srand(time(NULL));
  while (*shared_tickets < AVAILABLE_TICKETS) {
    down(seller_sem);

    print_text("Selling ticket...\n");
    sleep((rand() % 10) + 1);
    (*shared_tickets)++;
    print_text("Ticket sold!\n");
    printf("Tickets left: %d\n", AVAILABLE_TICKETS - *shared_tickets);

    up(handler_sem); // makes client wait for seller
  } 
  
  close_sem(seller_sem);
  unlink_sem("/SELLER");

  close_sem(handler_sem);
  unlink_sem("/HANDLER");

  unlink_sem("/CLIENT");

  if (munmap(shared_tickets, sizeof(int)) == -1) {
    perror("MUNMAP failed!");
    exit(EXIT_FAILURE);
  }

  if (close(fd) == -1) {
    perror("CLOSE failed!");
    exit(EXIT_FAILURE);
  }

  if (shm_unlink("/shm_tickets") == -1) {
    perror("SHM_UNLINK failed!");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
