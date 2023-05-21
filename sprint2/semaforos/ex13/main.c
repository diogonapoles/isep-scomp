#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define CHILDS 7

#define MUTEX1 "/mut1_13"
#define MUTEX2 "/mut2_13"
#define MUTEX3 "/mut3_13"
#define W "/.w_13"
#define R "/.r_13"

#define SHM "/_SHM_13_"

typedef struct {
  int readcount;
  int writecount;
  char buf[100];
} shared_data;

void destroy_sem(sem_t *sem, char *name) {
  if (sem_close(sem) == -1) {
    perror("Sem_close failed!");
    exit(EXIT_FAILURE);
  }

  if (sem_unlink(name) == -1) {
    perror("Sem_unlink failed!");
    exit(EXIT_FAILURE);
  }
}

void destroy_shm(int fd, shared_data *block, int shared_data_size) {
  if (munmap(block, shared_data_size) == -1) {
    perror("munmap()");
    exit(EXIT_FAILURE);
  }

  if (close(fd) == -1) {
    perror("close()");
    exit(EXIT_FAILURE);
  }

  if (shm_unlink(SHM) == -1) {
    perror("shm_unlink()");
    exit(EXIT_FAILURE);
  }
}

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

int main(void) {
  int fd, shared_data_size = sizeof(shared_data);
  shared_data *block;
  sem_t *mutex1, *mutex2, *mutex3, *w, *r;

  fd = shm_open(SHM, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("shm_open()");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(fd, shared_data_size) == -1) {
    perror("ftruncate()");
    exit(EXIT_FAILURE);
  }

  if ((block =
           (shared_data *)mmap(NULL, shared_data_size, PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("mmap()");
    exit(EXIT_FAILURE);
  }

  block->readcount = 0;
  block->writecount = 0;

  mutex1 = sem_open(MUTEX1, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (mutex1 == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }

  mutex2 = sem_open(MUTEX2, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (mutex2 == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }

  mutex3 = sem_open(MUTEX3, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (mutex3 == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }

  w = sem_open(W, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (w == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }

  r = sem_open(R, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
  if (r == SEM_FAILED) {
    perror("Sem_open failed!");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < CHILDS; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork()");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      srand(getpid());
      int is_reader = rand() % 2;
      if (is_reader) {
        down(mutex3);
        down(r);
        down(mutex1);
        block->readcount++;
        if (block->readcount == 1) {
          down(w);
        }
        up(mutex1);
        up(r);
        up(mutex3);

        print_text("[READER] Reading...\n");
        sleep(1);
        printf("[READER] Read: %s | Number of readers: %d\n", block->buf,
               block->readcount);

        down(mutex1);
        block->readcount--;
        if (block->readcount == 0) {
          up(w);
        }
        up(mutex1);
      } else {
        down(mutex2);
        block->writecount++;
        if (block->writecount == 1) {
          down(r);
        }
        up(mutex2);

        down(w);
        print_text("[WRITER] Writing...\n");
        time_t t;
        time(&t);
        snprintf(block->buf, 100, "PID: %d | Time: %s", getpid(), ctime(&t));
        printf("[WRITER] Number of writers: %d | Number of readers: %d\n",
               block->writecount, block->readcount);
        up(w);

        down(mutex2);
        block->writecount--;
        if (block->writecount == 0) {
          up(r);
        }
        up(mutex2);
      }

      exit(EXIT_SUCCESS);
    }
  }

  for (size_t i = 0; i < CHILDS; i++) {
    wait(NULL);
  }

  destroy_sem(mutex1, MUTEX1);
  destroy_sem(mutex2, MUTEX2);
  destroy_sem(mutex3, MUTEX3);
  destroy_sem(w, W);
  destroy_sem(r, R);

  destroy_shm(fd, block, shared_data_size);

  return EXIT_SUCCESS;
}
