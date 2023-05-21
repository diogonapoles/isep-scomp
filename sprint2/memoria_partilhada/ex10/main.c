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
#define NUM_VALUES 30
#define FILE "/ex10"

#define READ 0
#define WRITE 1

typedef struct shared
{
    int arr[ARR_SIZE];
    int num_entered;
    //int permission;
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

    shared *shared_data;
    if (shared_data = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)){
        perror("Error mapping shared memory");
        return EXIT_FAILURE;
    }

    shared_data->num_entered = 0;
    //shared_data->permission = 0;

    int fd1[2];
    int fd2[2];

    if (pipe(fd1) == -1)
    {
        perror("pipe error: ");
        exit(EXIT_FAILURE);
    }
    if (pipe(fd2) == -1)
    {
        perror("pipe error: ");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        close(fd2[WRITE]);
        close(fd1[READ]);

        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < ARR_SIZE; j++)
            {
                // while (!shared_data->permission);
                read(fd2[READ], &shared_data, sizeof(shared_data));

                if (shared_data->num_entered > 0)
                {
                    printf("Value: %d\n", shared_data->arr[j]);
                    shared_data->num_entered--;
                }
            }
            write(fd1[WRITE], &shared_data, sizeof(shared_data));
            // shared_data->permission = 0;
        }
        close(fd2[READ]);
        close(fd1[WRITE]);

        exit(EXIT_SUCCESS);
    }

    close(fd1[WRITE]);
    close(fd2[READ]);

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < ARR_SIZE; j++)
        {

            // while (shared_data->permission);
            write(fd2[WRITE], &shared_data, sizeof(shared_data));

            shared_data->arr[j] = (ARR_SIZE * i) + j;
            shared_data->num_entered++;
        }

        read(fd1[READ], &shared_data, sizeof(shared_data));
        // shared_data->permission = 1;
    }
    close(fd1[READ]);
    close(fd2[WRITE]);

    wait(NULL);

    if (shm_unlink(FILE) == -1)
    {
        perror("shm_unlink error: ");
        exit(EXIT_FAILURE);
    }

    return 0;
}