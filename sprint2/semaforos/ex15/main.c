#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/semaphore.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAXIMUM_CAPACITY 10
#define NUMBER_OF_CLIENTS 20

#define SHM "./shm15"

#define MUTEX_VIP "./vipmut15"
#define MUTEX_SPECIAL "./specialmut15"
#define MUTEX "./mutex15"

#define SEM_VIP "./vipsem15"
#define SEM_SPECIAL "./specialsem15"
#define SEM_GENERAL "./generalsem15"

typedef enum { NORMAL, SPECIAL, VIP } type;

void *init_mem(const char *restrict name, bool excl, size_t size) {
  int fd, trunc = true;
  struct stat sb;
  void *block;

  char *buf;
  size_t len = strlen("/dev/shm/") + strlen(name);

  if (!(buf = calloc(len + 1, 1))) {
    trunc = false;
  } else {
    if ((size_t)snprintf(buf, len + 1, "/dev/shm/%s", name) > len)
      return NULL;
    if (stat(buf, &sb) == 0)
      trunc = false;
    free(buf);
  }

  fd = shm_open(name,
                O_CREAT | O_RDWR | (excl ? O_EXCL : 0) | (trunc ? O_TRUNC : 0),
                0640);
  if (fd == -1) {
    perror("shm_open");
    return NULL;
  }

  if (trunc && ftruncate(fd, size) == -1) {
    perror("ftruncate");
    close(fd);
  }

  block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);

  if (block == MAP_FAILED) {
    perror("mmap");
    shm_unlink(name);
    return NULL;
  }

  return block;
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

typedef struct {
  int vip_waiting, special_waiting;
  int capacity;
} shared_data;

const char *getTypeString(type type) {
  switch (type) {
  case NORMAL:
    return "Normal";
  case SPECIAL:
    return "Special";
  case VIP:
    return "VIP";
  default:
    return "Unknown";
  }
}

void clubbing(int id, type priority) {
  int time = rand() % 5 + 1;

  printf("Client %2d |%9s| Having fun at the club! :D\n", id,
         getTypeString(priority));
  sleep(time);
  printf("Client %2d |%9s| Bye! Left the club after %d hours of fun!\n", id,
         getTypeString(priority), time);
}

void unlink_all(shared_data *data, int shared_data_size) {
  unlink_sem(SEM_GENERAL);
  unlink_sem(SEM_SPECIAL);
  unlink_sem(SEM_VIP);
  unlink_sem(MUTEX);
  unlink_sem(MUTEX_SPECIAL);
  unlink_sem(MUTEX_VIP);

  if (munmap(data, shared_data_size) == -1) {
    perror("Munmap failed!");
    exit(EXIT_FAILURE);
  }

  if (shm_unlink(SHM) == -1) {
    perror("Shm_unlink failed!");
    exit(EXIT_FAILURE);
  }
}

int main(void) {
  int shared_data_size = sizeof(shared_data);
  shared_data *data;
  sem_t *mutex_vip, *mutex_special, *mutex;
  sem_t *vip, *special, *general;

  if (!(data = init_mem(SHM, true, shared_data_size))) {
    perror("init_mem");
    exit(EXIT_FAILURE);
  }

  data->capacity = 0;
  data->special_waiting = 0;
  data->vip_waiting = 0;

  mutex_vip = sem_open(MUTEX_VIP, O_CREAT | O_EXCL, 0640, 1);
  if (mutex_vip == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  mutex_special = sem_open(MUTEX_SPECIAL, O_CREAT | O_EXCL, 0640, 1);
  if (mutex_special == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  mutex = sem_open(MUTEX, O_CREAT | O_EXCL, 0640, 1);
  if (mutex == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  vip = sem_open(SEM_VIP, O_CREAT | O_EXCL, 0640, 1);
  if (vip == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  special = sem_open(SEM_SPECIAL, O_CREAT | O_EXCL, 0640, 1);
  if (special == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  general = sem_open(SEM_GENERAL, O_CREAT | O_EXCL, 0640, MAXIMUM_CAPACITY);
  if (general == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUMBER_OF_CLIENTS; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      perror("Fork failed!");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      srand(time(NULL) ^ getpid());

      int time = rand() % 5 + 1;
      sleep(time);

      type priority;
      switch (rand() % 3) {
      case 0:
        priority = NORMAL;
        break;
      case 1:
        priority = SPECIAL;
        break;
      case 2:
        priority = VIP;
        break;
      }

      down(mutex);
      if (data->capacity <= MAXIMUM_CAPACITY) {
        // while club not full
        down(general);
        data->capacity++;
        up(mutex);
        printf("Client %2d |%9s| Club not full! Entered straight in. ^_^\n",
               i + 1, getTypeString(priority));
        clubbing(i + 1, priority);
        up(general);
        _exit(EXIT_SUCCESS);
      } else {
        // club full
        up(mutex);
        printf("Client %2d |%9s| Club full! Waiting in line.\n", i + 1,
               getTypeString(priority));

        switch (priority) {
        case NORMAL:
          down(special);
          down(vip);

          down(general);

          up(vip);
          up(special);

          printf("Client %2d |%9s| Someone left! Client entered.\n", i + 1,
                 getTypeString(priority));
          clubbing(i + 1, priority);

          up(general);
          _exit(EXIT_SUCCESS);
        case SPECIAL:
          down(mutex_special);
          data->special_waiting++;
          if (data->special_waiting == 1)
            down(special);
          up(mutex_special);

          down(vip);

          down(general);

          up(vip);

          down(mutex_special);
          data->special_waiting--;
          if (data->special_waiting == 1)
            up(special);
          up(mutex_special);

          printf("Client %2d |%9s| Someone left! Client entered.\n", i + 1,
                 getTypeString(priority));
          clubbing(i + 1, priority);

          up(general);
          _exit(EXIT_SUCCESS);
        case VIP:
          down(mutex_vip);
          data->vip_waiting++;
          if (data->vip_waiting == 1)
            down(vip);
          up(mutex_vip);

          down(general);

          down(mutex_vip);
          data->vip_waiting--;
          if (data->vip_waiting == 1)
            up(vip);
          up(mutex_vip);

          printf("Client %2d |%9s| Someone left! Client entered.\n", i + 1,
                 getTypeString(priority));
          clubbing(i + 1, priority);

          up(general);
          _exit(EXIT_SUCCESS);
        }
      }
    }
  }

  for (int i = 0; i < NUMBER_OF_CLIENTS; i++) {
    if (wait(NULL) == -1) {
      perror("Wait failed!");
      exit(EXIT_FAILURE);
    }
  }

  unlink_all(data, shared_data_size);
  sem_close(general);
  sem_close(special);
  sem_close(vip);
  sem_close(mutex);
  sem_close(mutex_special);
  sem_close(mutex_vip);
  return EXIT_SUCCESS;
}
