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


#include "common.h"

int main(void)
{
    size_t len;
    const pid_t pid = getpid();

    sem_t *mutex;
    shared *shared_data;
    

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("Shared memory: ");
        exit(EXIT_FAILURE);
    }

    shared_data = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (shared_data == MAP_FAILED)
    {
        perror("Error mapping shared memory");
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }

    if ((mutex = sem_open(SEM_NAME, O_CREAT, 0640, 1)) == SEM_FAILED)
    {
        perror("sem_open");
        return EXIT_FAILURE;
    }



    srand(time(NULL) * pid);

    sem_wait(mutex);
    len = shared_data->len;
    for (size_t i = 0; i < len; i++)
    {
        printf("%zu: %s\n", i, shared_data->arr[i]);
        sem_post(mutex);
    }
    printf("Total lines: %zu\n", len);

    munmap(shared_data, sizeof(shared));
    sem_close(mutex);



    return EXIT_SUCCESS;
}