#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <semaphore.h>
#include <signal.h>

#define SEM_NAME1 "sem_1_16"
#define SEM_NAME2 "sem_2_16"
#define SEM_NAME3 "sem_3_16"
#define SEM_NAME5 "sem_5_16"
#define SHM_NAME "shm_16"

#define INTEGERS 100
#define PARTITIONS 20
#define FINAL_POS 10
#define P_MAX 5

typedef struct shared_data shared;
struct shared_data
{
  int max_value_final;
  int max_value;
  char final_arr[FINAL_POS];
};

void cleanup_resources()
{
  if (sem_unlink(SEM_NAME1) == -1)
  {
    perror("sem_unlink");
  }
  if (sem_unlink(SEM_NAME2) == -1)
  {
    perror("sem_unlink");
  }
  if (sem_unlink(SEM_NAME3) == -1)
  {
    perror("sem_unlink");
  }
  if (sem_unlink(SEM_NAME5) == -1)
  {
    perror("sem_unlink");
  }
}

void terminate_process(pid_t pid)
{
  if (kill(pid, SIGKILL) == -1)
  {
    perror("kill");
  }
}

int main()
{
  shared *shared_data;

  int size = sizeof(shared);

  int fd = shm_open(SHM_NAME, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd < 0)
  {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(fd, size) < 0)
  {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }
  shared_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared_data == MAP_FAILED)
  {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  sem_t *mutex = sem_open(SEM_NAME1, O_CREAT, 0640, 0);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  sem_t *mutex1 = sem_open(SEM_NAME2, O_CREAT, 0640, 1);
  if (mutex1 == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  sem_t *mutex4 = sem_open(SEM_NAME5, O_CREAT, 0640, 1);
  if (mutex4 == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  sem_t *mutex2 = sem_open(SEM_NAME3, O_CREAT, 0640, 1);
  if (mutex2 == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < FINAL_POS; i++)
  {
    shared_data->final_arr[i] = 0;
  }

  pid_t pid[6] = {1, 1, 1, 1, 1, 1};

  for (size_t i = 0; i < 6; i++)
  {
    pid[i] = fork();
    if (pid[i] < 0)
    {
      perror("fork");
      cleanup_resources();
      exit(EXIT_FAILURE);
    }
    if (pid[i] == 0)
    {
      break;
    }
  }

  int arr[INTEGERS];

  if (pid[P_MAX] == 0)
  {
    while (1)
    {
      sem_wait(mutex2);
      printf("WARNING: New larger value found in the final vector: %d\n", shared_data->max_value);
    }
  }

  for (size_t i = 0; i < 5; i++)
  {
    if (pid[i] == 0)
    {
      srand(time(NULL) * getpid());
      for (size_t j = i * PARTITIONS; j < (i + 1) * PARTITIONS; j++)
      {
        arr[j] = rand() % 100 + 1;

        sem_wait(mutex1);
        if (arr[j] > shared_data->max_value)
        {
          shared_data->max_value = arr[j];
          sem_post(mutex2);
        }
        sem_post(mutex1);

        if ((j + 1) % 10 == 0)
        {
          sem_wait(mutex4);
          shared_data->final_arr[(i * 2) + (j / 10)] = arr[j];
          sem_post(mutex4);
        }
      }
      exit(EXIT_SUCCESS);
    }
  }

  for (size_t i = 0; i < 5; i++)
  {
    wait(NULL);
  }

  int max_value_final = 0;
  for (size_t i = 0; i < FINAL_POS; i++)
  {
    if (shared_data->final_arr[i] > max_value_final)
    {
      max_value_final = shared_data->final_arr[i];
    }
  }
  shared_data->max_value_final = max_value_final;

  terminate_process(pid[P_MAX]);
  if (wait(NULL) < 0)
  {
    perror("wait");
  }

  for (size_t i = 0; i < FINAL_POS; i++)
  {
    printf("Final vector: %d\n", shared_data->final_arr[i]);
  }

  printf("Max value Parent: %d\n", shared_data->max_value_final);

  cleanup_resources();

  if (munmap(shared_data, sizeof(shared)) < 0)
  {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  if (shm_unlink(SHM_NAME) == -1)
  {
    perror("shm_unlink error");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
