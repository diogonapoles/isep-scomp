#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

#define ARR_SIZE 10
#define SEQ_SIZE 5
#define FILE "/ex4"

typedef struct shared
{
    int arr[SEQ_SIZE][ARR_SIZE];
    int allow;
} shared;

int main(void)
{

    int fd = shm_open(FILE, O_CREAT | O_RDONLY, S_IRUSR);
    if (fd < 0)
    {
        perror("Shared memory: ");
        exit(EXIT_FAILURE);
    }

    shared *shared_data = mmap(NULL, sizeof(shared), PROT_READ, MAP_SHARED, fd, 0);

    for (size_t i = 0; i < SEQ_SIZE; i++)
    {
    int average = 0;
        for (int j = 0; j < ARR_SIZE; j++)
        {
            average += shared_data->arr[i][j];
        }
        average = average / ARR_SIZE;

        printf("Average (Reader) %d \n", average);
    }

    shm_unlink(FILE);

    return 0;
}