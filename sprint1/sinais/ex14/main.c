#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <wait.h>

void alarm_handler(int signum)
{
    char msg[80];
    sprintf(msg, "You were too slow and this program will end!\n");
    write(STDOUT_FILENO, msg, strlen(msg));
    exit(0);
}

int main(void)
{
    char input[256];
    int status;

    struct sigaction act;
    memset(&act , 0, sizeof(struct sigaction));

    act.sa_handler = alarm_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGALRM, &act, NULL);

    printf("U have 10 seconds to enter a string or the program will terminate: \n");

    alarm(10);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("FORK ERROR");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        fgets(input, sizeof(input), stdin); // Read a string from the keyboard
        exit(strlen(input)-1);
    }

    wait(&status);

    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
    {
        kill(getpid(), SIGALRM);
    }

    alarm(0); // Cancel the timer

    printf("The size of the string is %d\n", WEXITSTATUS(status));

    return 0;
}
