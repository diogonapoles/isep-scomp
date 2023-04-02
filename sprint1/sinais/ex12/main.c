#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

volatile sig_atomic_t child_num = 5;

void children_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        child_num--;
    }
}


int main(void)
{
    pid_t pid;
    int status;
    struct sigaction act, alarm_act;

    memset(&act , 0, sizeof(struct sigaction));
    memset(&alarm_act , 0, sizeof(struct sigaction));



    act.sa_handler = children_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDWAIT | SA_NOCLDSTOP;
    // wait previne que o pai receba o SIGCHLD ao terminar
    // stop previne que o pai receba o SIGCHLD quando o filho e parado ou continuado

    sigaction(SIGUSR1, &act, NULL);

    

    for (size_t i = 0; i < 5; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("FORK ERROR");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            for (size_t j = i + 10; j < i + 15; j++)
            //for (size_t j = i*200; j < (i+5)*200; j++)
            {
                printf("Child nº %ld, prints value: %ld\n", i, j);
            }

            kill(getppid(), SIGUSR1);
            exit(EXIT_SUCCESS);
        }
    }


    while (child_num > 0)
    {
        pause();
    }

    for (size_t i = 0; i < 5; i++)
    {
        if (wait(&status) == -1)
        {
            perror("WAIT ERROR");
        }
    }

    return 0;
}
