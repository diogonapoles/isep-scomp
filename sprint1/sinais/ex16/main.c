#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <wait.h>
#include <string.h>

#define NUM_WORKERS 50
#define SUCCESS_RATE 1 

volatile sig_atomic_t workers_done = 0;
volatile sig_atomic_t workers_success = 0;

int simulate1()
{
    srand(time(NULL) + getpid());
    if ((rand() % SUCCESS_RATE))
    {
        return 1;
    }

    return 0;
}

void simulate2()
{   
    
    printf("simulate2\n");
    exit(EXIT_SUCCESS);
}

void worker_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        workers_success++;
        workers_done++;
    }
    else if (signum == SIGUSR2)
    {
        workers_done++;
    }
}
void worker_handler_child_USR1(int signum)
{
    simulate2();
}

int main(void)
{
    pid_t pids[NUM_WORKERS];
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = worker_handler;
    sigemptyset(&act.sa_mask);

    if (sigaction(SIGUSR1, &act, NULL) < 0)
    {
        perror("sigaction usr1");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR2, &act, NULL) < 0)
    {
        perror("sigaction usr2");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < NUM_WORKERS; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("FORK ERROR");
            exit(EXIT_FAILURE);
        }
        else if (pids[i] == 0)
        {

            struct sigaction act2;
            memset(&act2, 0, sizeof(struct sigaction));
            act2.sa_handler = worker_handler_child_USR1;
            sigemptyset(&act2.sa_mask);
            sigdelset(&act2.sa_mask, SIGUSR1);
            if (sigaction(SIGUSR1, &act2, NULL)<0)
            {
                perror("sig usr1 second error");
            }
            
            
            if (simulate1())
            {
                // sleep(5);
                kill(getppid(), SIGUSR1);
            }
            else
            {
                // sleep(5);
                kill(getppid(), SIGUSR2);
            }
            pause();
            exit(EXIT_FAILURE);
        }
    }

    while (workers_done < 25)
    {
        pause();
    };

    printf("done: %d\n", workers_done);
    printf("success: %d\n", workers_success);

    if (workers_success > 0)
    {
        printf("Search Successful\n");
        for (size_t i = 0; i < NUM_WORKERS; i++)
        {
            kill(pids[i], SIGUSR1);
        }
    }
    else
    {
        printf("Inefficient Algorithm\n");
        for (size_t i = 0; i < NUM_WORKERS; i++)
        {
            kill(pids[i], SIGKILL);
        }
    }   

    for (size_t i = 0; i < NUM_WORKERS; i++)
    {
        waitpid(-1, NULL, 0);
    }

    return EXIT_SUCCESS;
}
