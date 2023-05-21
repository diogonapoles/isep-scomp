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
#include <string.h>

#define SHM_DIFFER "ex6_differ"
#define SEM_MUTEX "ex6_mutex"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))
#define DIFFER 2

#define _printf(msg)      \
    {                     \
        fputs(msg, stdout); \
        fflush(stdout);     \
    }

typedef struct
{
    int contC;
    int contS;
} ctrl_data;

static int running = 1;

void handle_sigint(int signum)
{
    running = 0;
    shm_unlink(SHM_DIFFER);
    sem_unlink(SEM_MUTEX);
}

int main(void)
{
    sem_t *mutex;
    ctrl_data *data;

    int size = sizeof(ctrl_data);

    int fd = shm_open(SHM_DIFFER, O_CREAT |O_EXCL| O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, size) < 0)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    data->contC = 0;
    data->contS = 0;

    const char *parent_msg = "S";
    const char *child_msg = "C";

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGINT);
    act.sa_handler = handle_sigint;
    sigaction(SIGINT, &act, NULL);

    mutex = sem_open(SEM_MUTEX, O_CREAT, 0640, 1);
    if (mutex == SEM_FAILED)
    {
        perror("Semaphore: ");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork");
        return EXIT_FAILURE;
    case 0: /* child */
        do
        {
            sem_wait(mutex);

            signed int differ = (data->contS) - (data->contC);

            if (differ > -2)
            {
                printf(child_msg);
                fflush(stdout);
                data->contC++;
            }

            sem_post(mutex);
            sleep(1);

        } while (1);

        exit(EXIT_SUCCESS);
    }

    do
    {
        sem_wait(mutex);

        signed int differ = (data->contS) - (data->contC);

        if (differ < 2)
        {
            printf(parent_msg);
            fflush(stdout);
            data->contS++;
        }

        sem_post(mutex);
        sleep(1);

    } while (running);

    kill(pid, SIGKILL);

    fputs("\n", stdout);
    return EXIT_SUCCESS;
}
