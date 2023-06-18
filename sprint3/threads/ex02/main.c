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
#include <pthread.h>
#include <string.h>


#define SHARED_BLOCKS 5
#define NAME_SIZE 100

typedef struct shared_data shared;
struct shared_data
{
  int number;
  int grade;
  char name[NAME_SIZE];
};

void* function(void *parameter){
  shared *shared_data = (shared*)parameter;
  printf("%d\n",shared_data->number);
  printf("%d\n",shared_data->grade);
  printf("%s\n",shared_data->name);

  pthread_exit((void*) NULL);
}

int main(void){

  shared arr[SHARED_BLOCKS];
  for (size_t i = 0; i < SHARED_BLOCKS; i++)
  {
    arr[i].grade=10;
    arr[i].number=12;
    strcpy(arr[i].name,"maria jaqueline");
  }
  
  pthread_t threads[SHARED_BLOCKS];

  for (size_t i = 0; i < SHARED_BLOCKS; i++)
  {
    pthread_create(&threads[i],NULL,function,(void*)&arr[i]);
  }
  printf("ALL threads created\n");
  for (size_t i = 0; i < SHARED_BLOCKS; i++)
  {
    pthread_join(threads[i],NULL);
  }
  printf("All threads terminated\n");


}