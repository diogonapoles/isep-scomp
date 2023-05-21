#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SEM_NAME1 "EX4.1"
#define SEM_NAME2 "EX4.2"
#define SEM_NAME3 "EX4.3"

#define PARENT_MSG "Father\n"
#define CHILD1_MSG "1st child\n"
#define CHILD2_MSG "2nd child\n"
#define TIMES 15

int main()
{
  
  sem_t *mutex = sem_open(SEM_NAME1, O_CREAT , 0640, 0);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  sem_t *mutex1 = sem_open(SEM_NAME2, O_CREAT, 0640, 1);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }
  sem_t *mutex2 = sem_open(SEM_NAME3, O_CREAT, 0640, 0);
  if (mutex == SEM_FAILED)
  {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  pid_t pid[2]= {1,1};
  

  pid[0] = fork();
  if (pid[0] < 0)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid[0] > 0)
  {
    pid[1] = fork();
    if (pid[1] < 0)
    {
      perror("fork");
      exit(EXIT_FAILURE);
    }
  }

  for (size_t i = 0; i < TIMES; i++)
  {
    if (pid[0] == 0)
    {
      sem_wait(mutex1);
      printf(CHILD1_MSG);
      fflush(stdout);
      sem_post(mutex);
    }

    if (pid[1] == 0)
    {
      sem_wait(mutex2);
      printf(CHILD2_MSG);
      fflush(stdout);
      sem_post(mutex1);
    }
    if (pid[1] > 0 && pid[0] > 0)
    {
      sem_wait(mutex);
      printf(PARENT_MSG);
      fflush(stdout);
      sem_post(mutex2);
    }
  }
  if (pid[0]==0)
  {
    exit(EXIT_SUCCESS);
  }
  if(pid[1]==0)
  {
    exit(EXIT_SUCCESS);
  }
  

  wait(NULL);
  
  if (sem_unlink(SEM_NAME1))
  {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }
if (sem_unlink(SEM_NAME2))
  {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }
  if (sem_unlink(SEM_NAME3))
  {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
