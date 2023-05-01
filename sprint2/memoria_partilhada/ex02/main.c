#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM_EXEC 100
#define FILE "/ex2"

typedef struct shared
{
    int v1;
    int v2;
    short allow_child;
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
        for (int i = 0; i < NUM_EXEC; i++)
        {
            while (!shared_data->allow_child);
            shared_data->v2--;
            shared_data->v1++;
            shared_data->allow_child = 0;
        }


    shm_unlink(FILE);

    return 0;
}