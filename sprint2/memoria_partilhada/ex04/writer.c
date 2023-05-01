#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

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

    int fd = shm_open(FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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

    shared *shared_data = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    srand(time(NULL));
    for (size_t i = 0; i < SEQ_SIZE; i++)
    {

        for (int j = 0; j < ARR_SIZE; j++)
        {
            shared_data->arr[i][j] = rand() % 20 + 1;
        }
        int avg_writer = 0;
        for (int j = 0; j < ARR_SIZE; j++)
        {
            avg_writer += shared_data->arr[i][j];
        }
        avg_writer = avg_writer / ARR_SIZE;
        printf("expected (writer) %d \n", avg_writer);
    }

    return 0;
}