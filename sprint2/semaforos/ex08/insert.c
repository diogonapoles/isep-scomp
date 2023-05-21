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
#include <string.h>

#include "common.h"

int main()
{
  int size = sizeof(shared);

  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd < 0)
  {
    perror("shm_open ");
    exit(EXIT_FAILURE);
  }
  if (ftruncate(fd, size) < 0)
  {
    perror("ftruncate ");
    exit(EXIT_FAILURE);
  }
  shared *shared_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared_data == MAP_FAILED)
  {
    perror("mmap ");
    exit(EXIT_FAILURE);
  }

  sem_t *sem = sem_open(SEM_NAME1, O_CREAT, 0640, 1);
  if (sem == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  sem_wait(sem);

  record *r = &(shared_data->entry[shared_data->index]);

  printf("Insert number:\n");

  scanf("%d", &(r->number));
  fflush(0);

  printf("Insert name:\n");

  char aux[50];
  scanf("%s", aux);
  strcpy(r->name, aux);
  fflush(0);

  printf("Insert address:\n");

  char aux2[100];
  scanf("%s", aux2);
  strcpy(r->address, aux2);
  fflush(0);

  shared_data->index++;  // Increment the index after both number and data have been inserted

  sem_post(sem);

  sem_unlink(SEM_NAME);

  munmap(shared_data, size);

  if (close(fd) < 0)
  {
    perror("close");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
