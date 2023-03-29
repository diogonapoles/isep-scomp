#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1

int main(void)
{
    int vec1[1000];
    int vec2[1000];

    for (size_t i = 0; i < 1000; i++)
    {
        vec1[i] = 1;
        vec2[i] = 2;
    }

    int fd[2];
    pid_t pids[5];
    int sum = 0;

    if (pipe(fd) < -1)
    {
        perror("PIPE FAILED");
        return 1;
    }

    int tmp = 0;
    for (size_t i = 0; i < 5; i++)
    {
        pids[i] = fork();
        if (pids[i] < 0)
        {
            perror("FORK FAILED");
            return 1;
        }
        if (pids[i] == 0)
        {
            close(fd[READ]);
            //for (size_t j = i * 5; j < (i * 5) + 5; j++)
            for (size_t j = i * 200; j < (i * 200) + 200; j++)
            {
                tmp += vec1[j] + vec2[j];
            }
            write(fd[WRITE], &tmp, sizeof(tmp));
            close(fd[WRITE]);
            exit(EXIT_SUCCESS);
        }

        close(fd[WRITE]);
        read(fd[READ], &tmp, sizeof(tmp));
        close(fd[READ]);

        sum += tmp;
    }
    printf("%d\n", sum);

    return 0;
}
