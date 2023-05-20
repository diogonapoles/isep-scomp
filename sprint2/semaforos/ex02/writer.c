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

#define MAX_DELAY 5
#define MESSAGE "I'm the Father - with PID"
#define MAX_STRS 5
#define MAX_STR_LEN 80

#include "common.h"


int main(void)
{
    shared *shared_data;
    size_t len;
    const pid_t pid = getpid();

    sem_t *mutex;
    if ((mutex = sem_open(SEM_NAME, O_CREAT, 0640, 1)) == SEM_FAILED)
    {
        perror("sem_open");
        return EXIT_FAILURE;
    }

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("Shared memory: ");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(shared)) == -1)
    {
        perror("Ftrunccate: ");
        exit(EXIT_FAILURE);
    }

    shared_data = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data ==MAP_FAILED)
    {
        perror("Error mapping shared memory");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL) * pid);

    while (1)
    {
        sem_wait(mutex);
        len = shared_data->len;
        /*exit if no more msg were possible to write*/
        if (len == MAX_STRS)
        {
            sem_post(mutex);
            break;
        }
        //sprintf(shared_data->arr[len++], MESSAGE " %d", pid);
        snprintf(shared_data->arr[len++], MAX_STR_LEN, MESSAGE " %d", pid);
        shared_data->len = len;

        sleep((rand() % MAX_DELAY) + 1);
        sem_post(mutex);

    printf("Writer %d: Wrote message no. %zu\n", getpid(),len);
    }

    munmap(shared_data,sizeof(shared));
    sem_close(mutex);


    if (shm_unlink(SHM_NAME) == -1)
    {
        perror("shm_unlink error: ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}