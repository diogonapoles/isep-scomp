#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define SEM_NAME "EX4"

#define PARENT_MSG "Father"
#define CHILD1_MSG "1st child"
#define CHILD2_MSG "2nd child"

int main()
{
  sem_t *mutex = sem_open(SEM_NAME, O_CREAT , 0640, 0);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  sem_t *mutex1 = sem_open(SEM_NAME, O_CREAT , 0640, 1);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  sem_t *mutex2 = sem_open(SEM_NAME, O_CREAT , 0640, 0);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid < 0)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid == 0)
  {
    sem_wait(mutex1);
    puts(CHILD1_MSG);
    sem_post(mutex);
    exit(EXIT_SUCCESS);
  }

  pid_t pid1 = fork();
  if (pid1 < 0)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid1 == 0)
  {
    sem_wait(mutex2);
    puts(CHILD2_MSG);
    exit(EXIT_SUCCESS);
  }

  sem_wait(mutex);
  puts(PARENT_MSG);
  sem_post(mutex2);

  wait(NULL);
  sem_unlink(SEM_NAME);

  return EXIT_SUCCESS;
}
