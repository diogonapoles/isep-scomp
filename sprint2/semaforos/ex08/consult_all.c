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

#include "common.h"

int main()
{
  shared *shared_data;
  int size = sizeof(shared_data);

  int fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  if (fd < 0)
  {
    perror("shm_open ");
    exit(EXIT_FAILURE);
  }

  shared_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared_data == MAP_FAILED)
  {
    perror("mmap ");
    exit(EXIT_FAILURE);
  }

  sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
  if (sem == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  sem_wait(sem);

  if (shared_data->index == 0)
  {
    printf("No records available.\n");
  }
  else
  {
    int i;
    for (i = 0; i < shared_data->index; i++)
    {
      record *r = &(shared_data->entry[i]);
      printf("Record: %d\nNumber: %d  Name: %s  Address: %s\n", i + 1, r->number, r->name, r->address);
      printf("\n");
    }
  }

  sem_post(sem);

  munmap(shared_data, size);

  if (close(fd) < 0)
  {
    perror("close");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
