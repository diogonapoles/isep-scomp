#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <wait.h>
#include <string.h>

#define NUM_WORKERS 50
#define SUCCESS_RATE 25 // 1 in 25 chance of success

volatile sig_atomic_t workers_done = 0;
volatile sig_atomic_t workers_success = 0;

int simulate1()
{
    srand(time(NULL) + getpid());
    if ((rand() % 100) < 1)
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
    exit(EXIT_SUCCESS);
}

int main(void)
{
    pid_t pids[NUM_WORKERS];
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = worker_handler;
    sigemptyset(&act.sa_mask);
    // act.sa_flags = 0;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

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
            sigfillset(&act2.sa_mask);
            // act2.sa_flags = 0;
            sigaction(SIGUSR1, &act2, NULL);
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

    

    while (workers_done < 25 )
    {
        printf("%d\n", workers_done);
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
    }

    for (size_t i = 0; i < NUM_WORKERS; i++)
    {
        kill(pids[i], SIGKILL);
        waitpid(pids[i], NULL, 0);
    }

    /*
    int success_found = 0;

    for (size_t i = 0; i < NUM_WORKERS; i++)
    {
          int wstatus;
          waitpid(pids[i], &wstatus, 0);

          if (WEXITSTATUS(wstatus) == 1)
          {
              printf("Worker %ld found\n", i);
              workers_success++;
          }
          workers_done++;

        int wstatus;
        waitpid(pids[i], &wstatus, 0);
        printf("%d\n", ret);
        if (ret == 1)
        {
            printf("Worker %ld found\n", i);
            workers_success++;
        }
        workers_done++;

        if (workers_done >= 25 && workers_success == 0)
        {
            printf("Inefficient algorithm!\n");
            for (size_t i = 0; i < NUM_WORKERS; i++)
            {
                kill(pids[i], SIGKILL);
            }
            break;
        }
        else if (workers_success > 0 && !success_found)
        {
            success_found = 1;
            for (size_t i = 0; i < NUM_WORKERS; i++)
            {
                kill(pids[i], SIGUSR1);
            }
        }
    }
    printf("Workers done: %d, success: %d\n", workers_done, workers_success);

    for (size_t i = 0; i < NUM_WORKERS; i++)
    {
        wait(&status);
    }

    for (size_t i = 0; i < NUM_WORKERS; i++)
    {
        wait(&status);
        if (WIFSIGNALED(status))
        {
            if (WTERMSIG(status) == SIGUSR1)
            {
                simulate2();
            }
            else if (WTERMSIG(status) == SIGUSR2)
            {
                printf("Worker %ld did not find relevant data.\n", i);
            }
        }
    }

    */

    return EXIT_SUCCESS;
}
