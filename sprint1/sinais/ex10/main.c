#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

#define MILI_SEC_10 10000000


volatile sig_atomic_t USR1_counter;

void handle_USR1(int signum)
{
    char msg[80];

    USR1_counter++;
    sprintf(msg, "SIGUSR1 signal captured: USR1 counter = %d\n", USR1_counter);
    write(STDOUT_FILENO, msg, strlen(msg));
    sleep(3);
    
}

int main(void)
{
    struct sigaction act;

    memset(&act , 0, sizeof(struct sigaction));

    //sigfillset(&act.sa_mask); //ignore all signals during handle_USR1 function
    sigemptyset(&act.sa_mask);  

    act.sa_handler = handle_USR1;

    
    //act.sa_flags=0;

    sigaction(SIGUSR1, &act, NULL);

    pid_t pid = fork();

    if(pid== 0){
        struct timespec time;
        time.tv_nsec = MILI_SEC_10;

        pid_t parent = getppid();

        for (size_t i = 0; i < 12; i++)
        {
            sleep(4);
            kill(parent,SIGUSR1);

            //nanosleep(&time,NULL);
        }
        kill(parent, SIGINT);
        exit(1);
        
    }
    

    do
    {
        printf("I'm working!\n");
        sleep(1);
    } while (1);

    return 0;
}
