#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define ARR_SIZE 1000
#define NUM_CHILDS 10
#define FILE "/ex6"

typedef struct shared
{
    int arr[ARR_SIZE];
    int arr_check[NUM_CHILDS];
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

    for (int i = 0; i < ARR_SIZE; i++)
    {
        shared_data->arr[i] = ( rand() % 1000) + 1;
    }
   

    for (size_t i = 0; i < NUM_CHILDS; i++)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            int max_num = shared_data->arr[100 * i];

            for (size_t j = 100 * i; j < 100 * (i + 1); j++)
            {
                if (shared_data->arr[j] > max_num)
                {
                    max_num = shared_data->arr[j];
                }
            }
            shared_data->arr_check[i] = max_num;

            exit(EXIT_SUCCESS);
        }
    }
    for (size_t i = 0; i < NUM_CHILDS; i++)
    {
        wait(NULL);
    }

    int biggest = 0;
    for (size_t i = 0; i < NUM_CHILDS; i++)
    {
        if (shared_data->arr_check[i] > biggest)
        {
            biggest = shared_data->arr_check[i];
        }
    }

    shm_unlink(FILE);
    printf("Maximum: %d\n", biggest);

    return 0;
}