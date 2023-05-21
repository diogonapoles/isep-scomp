#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/mman.h>
#include <sys/signal.h>
#include <time.h>
#include <unistd.h>

#define SENSORS 5
#define READINGS 6

#define SHM_NAME "/saffaass"
#define SEM_DATA "/s_faaa7"
#define SEM_ALARM "/araafa17"

void down(sem_t *sem) {
  if (sem_wait(sem) == -1) {
    perror("sem_wait");
    exit(EXIT_FAILURE);
  }
}

void up(sem_t *sem) {
  if (sem_post(sem) == -1) {
    perror("sem_post");
    exit(EXIT_FAILURE);
  }
}

typedef struct {
  int measurements[SENSORS][READINGS];
  int total;
  int alarm;
} block;

block *b;

int main(void) {
  int fd, size = sizeof(block);
  int fd_pipe[2];
  sem_t *data, *alarm;

  if (pipe(fd_pipe) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  if ((fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0660)) == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(fd, size) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  if ((b = (block *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                         0)) == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  // intialize shared memory to 0
  for (int i = 0; i < SENSORS; i++) {
    for (int j = 0; j < READINGS; j++) {
      b->measurements[i][j] = 0;
    }
  }
  b->total = 0;
  b->alarm = 0;

  if ((data = sem_open(SEM_DATA, O_CREAT | O_EXCL, 0660, 1)) == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  if ((alarm = sem_open(SEM_ALARM, O_CREAT | O_EXCL, 0660, 1)) == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  // create sensor processes
  for (int i = 0; i < SENSORS; i++) {
    pid_t pid = fork();

    if (pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      int alarm_activated = 0;
      int previous_value;

      if (close(fd_pipe[0]) == -1) {
        perror("close7");
        exit(EXIT_FAILURE);
      }

      for (int j = 0; j < READINGS; j++) {
        srand(time(NULL) ^ getpid());
        int value = rand() % 101;

        down(data);
        b->measurements[i][j] = value;
        b->total++;

        char buf[100];
        int buf_size = snprintf(buf, sizeof(buf), "\n[MEASUREMENT] Sensor %d: %d\n", i, value);

        if (write(fd_pipe[1], buf, buf_size) == -1) {
          perror("write");
          exit(EXIT_FAILURE);
        }
        up(data);

        if (!alarm_activated && value > 50) {
          alarm_activated = 1;
          down(alarm);
          b->alarm++;

          buf_size = snprintf(buf, sizeof(buf),
                   "\n[ALARM] Sensor %d activated alarm | "
                   "Number of active alarms: %d\n",
                   i, b->alarm);

          if (write(fd_pipe[1], buf, buf_size) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
          }
          up(alarm);
        }

        if (alarm_activated && value < 50 && previous_value < 50) {
          alarm_activated = 0;
          down(alarm);
          b->alarm--;

          buf_size = snprintf(buf, sizeof(buf),
                   "\n[ALARM] Sensor %d deactivated alarm | "
                   "Number of active alarms: %d\n",
                   i, b->alarm);

          if (write(fd_pipe[1], buf, buf_size) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
          }
          up(alarm);
        }

        previous_value = value;
        sleep(1);
      }

      if (close(fd_pipe[1]) == -1) {
        perror("close6");
        exit(EXIT_FAILURE);
      }

      _exit(EXIT_SUCCESS);
    }
  }

  if (close(fd_pipe[1]) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  char buf[100];
  int total = 0;

  while (total < READINGS * SENSORS) {
    ssize_t bytes_read;
    if ((bytes_read = read(fd_pipe[0], buf, sizeof(buf))) == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(STDOUT_FILENO, buf, bytes_read);
    if (bytes_written == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }

    down(data);
    total = b->total;
    up(data);
  }

  if (close(fd_pipe[0]) == -1) {
    perror("close1");
    exit(EXIT_FAILURE);
  }

  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  if (munmap(b, size) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  if (shm_unlink(SHM_NAME) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }

  if (sem_close(data) == -1) {
    perror("sem_close");
    exit(EXIT_FAILURE);
  }

  if (sem_close(alarm) == -1) {
    perror("sem_close");
    exit(EXIT_FAILURE);
  }

  if (sem_unlink(SEM_DATA) == -1) {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }

  if (sem_unlink(SEM_ALARM) == -1) {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
