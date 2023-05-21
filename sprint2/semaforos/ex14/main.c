#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

#define SHM_NAME "ex14_shm"
#define SEM_MUTEX "ex14_mutex"
#define SEM_AX "ex14_ax"
#define SEM_BX "ex14_bx"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))

#define ARR_SIZE 10
#define AX_PROC 3
#define BX_PROC 2

#define _printf(msg)       \
    {                      \
        fputs(msg, stdout); \
        fflush(stdout);     \
    }

typedef struct
{
    int ax_n;
    int bx_n;
    int buf[ARR_SIZE];
} shared;

// Semaphore pointers
sem_t *mutex;
sem_t *ax_sem;
sem_t *bx_sem;

void ax_process(shared *shared_memory, int process_id)
{
    while (1)
    {
        // Waits for another AX process
        sem_wait(ax_sem);

        // Accesses data
        printf("Ax Process %d: Accessing shared memory\n", process_id);
        fflush(stdout);
        // Writing data to shared memory
        for (int i = 0; i < ARR_SIZE; i++)
        {
            shared_memory->buf[i] = process_id;
        }
        sleep(1);
        printf("Ax Process %d: Data written to shared memory\n", process_id);
        fflush(stdout);

        // Finished work
        shared_memory->ax_n--;

        // If there are no processes on set A available
        if (shared_memory->ax_n == 0)
        {
            // Permit BX process to do work
            sem_post(bx_sem);

            // When BX processes terminate, keep A set working
            sem_wait(mutex);
        }

        sem_post(ax_sem);
        // Sleeps for 5 seconds so that it does not stay in an infinite cycle
        sleep(5);

        // Process from A will start another writing
        shared_memory->ax_n++;
    }
}

void bx_process(shared *shared_memory, int process_id)
{
    while (1)
    {
        // Waits for another BX process
        sem_wait(bx_sem);

        // Accesses data
        printf("Bx Process %d: Accessing shared memory\n", process_id);
        fflush(stdout);
        // Writing data to shared memory
        for (int i = 0; i < ARR_SIZE; i++)
        {
            shared_memory->buf[i] = process_id;
        }
        sleep(1);
        printf("Bx Process %d: Data written to shared memory\n", process_id);
        fflush(stdout);

        // Finished work
        shared_memory->bx_n--;

        // If there are no processes on set B available
        if (shared_memory->bx_n == 0)
        {
            // Permit AX process to do work
            sem_post(mutex);

            // When AX processes terminate, keep B set working
            sem_wait(bx_sem);
        }

        sem_post(bx_sem);
        // Sleeps for 5 seconds so that it does not stay in an infinite cycle
        sleep(6);

        // Process from B will start another writing
        shared_memory->bx_n++;
    }
}

void handle_sigint(int signum)
{
  shared *shared_memory;
    // Close and unlink semaphores
    sem_close(mutex);
    sem_unlink(SEM_MUTEX);

    sem_close(ax_sem);
    sem_unlink(SEM_AX);

    sem_close(bx_sem);
    sem_unlink(SEM_BX);

    // Unmap shared memory
    munmap(shared_memory, sizeof(shared));

    // Unlink shared memory
    shm_unlink(SHM_NAME);

    fflush(stdout);
    exit(EXIT_SUCCESS);
}

int main(void)
{
    // Create and initialize shared memory
    shared *shared_memory;

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(shared)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shared_memory = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shared_memory->ax_n = AX_PROC;
    shared_memory->bx_n = BX_PROC;

    // Open semaphores
    mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0644, 0);
    if (mutex == SEM_FAILED)
    {
        perror("sem_open(mutex)");
        exit(EXIT_FAILURE);
    }

    ax_sem = sem_open(SEM_AX, O_CREAT | O_EXCL, 0644, 1);
    if (ax_sem == SEM_FAILED)
    {
        perror("sem_open(ax_sem)");
        exit(EXIT_FAILURE);
    }

    bx_sem = sem_open(SEM_BX, O_CREAT | O_EXCL, 0644, 0);
    if (bx_sem == SEM_FAILED)
    {
        perror("sem_open(bx_sem)");
        exit(EXIT_FAILURE);
    }

    // Create AX processes
    for (int i = 0; i < AX_PROC; i++)
    {
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
            ax_process(shared_memory, i);
            exit(EXIT_SUCCESS);
            break;
        case -1:
            perror("Error creating Ax process.\n");
            exit(EXIT_FAILURE);
            break;
        }
    }

    // Create BX processes
    for (int i = 0; i < BX_PROC; i++)
    {
        pid_t pid = fork();
        switch (pid)
        {
        case 0:
            bx_process(shared_memory, i);
            exit(EXIT_SUCCESS);
            break;
        case -1:
            perror("Error creating Bx process.\n");
            exit(EXIT_FAILURE);
            break;
        }
    }

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigfillset(&act.sa_mask); // Zeros the mask, and no signal is ignored (e.g., it's possible to stop the program with Ctrl+C)
    sigdelset(&act.sa_mask, SIGINT);
    act.sa_handler = handle_sigint;
    sigaction(SIGINT, &act, NULL);

    pause();

    fputs("\n", stdout);

    return EXIT_SUCCESS;
}
