#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define CHILDS 10
#define MSG_SIZE 20

int main(void)
{

    struct s
    {
        size_t i;
        char win_value[MSG_SIZE];
    };

    struct s s;

    for (size_t i = 0; i < MSG_SIZE; i++)
    {
        s.win_value[i] = 0;
    }

    char won[4] = "Win";
    int fd[2];
    int pids[10];

    if (pipe(fd) < -1)
    {
        perror("pipe");
        return 1;
    }

    for (size_t i = 0; i < CHILDS; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("fork");
        }
        if (pids[i] == 0)
        {
            close(fd[WRITE]);
            read(fd[READ], &s, sizeof(s));
            close(fd[READ]);
            exit(s.i);
        }
    }
    close(fd[READ]);
    for (size_t i = 0; i < CHILDS; i++)
    {
        sprintf(s.win_value, "%s", won);
        s.i = i+1;
        sleep(2);

        write(fd[WRITE], &s, sizeof(s));
    }
    close(fd[WRITE]);


    int status;
    for (size_t i = 0; i < CHILDS; i++)
    {
        pid_t pid = wait(&status);
        printf("Child: %d, Won round: %d\n",pid,WEXITSTATUS(status));
    }

    return 0;
}
