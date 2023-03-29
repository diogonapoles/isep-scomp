#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1
#define MSG_SIZE 100
#define FILE_NAME "file.txt"

int main(void)
{
    size_t size = 0;
    int fd[2];
    FILE *fp;
    char *line = NULL;
    int ocupied;

    if (pipe(fd) < -1)
    {
        perror("PIPE FAILED");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("FORK FAILED");
        return 1;
    }

    if (pid > 0)
    {
        close(fd[READ]);
        fp = fopen(FILE_NAME, "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);
        while ((ocupied = getline(&line, &size, fp)) != -1)
        {
            write(fd[WRITE], line, ocupied);
        }
        fclose(fp);
        free(line);
        close(fd[WRITE]);
        if (line != NULL)
            free(line);
        wait(NULL);
    }

    if (pid == 0)
    {
        close(fd[WRITE]);
        char buffer[MSG_SIZE];
        int n;
        printf("filho recebeu:\n");
        while ((n = read(fd[READ], buffer, MSG_SIZE - 1)))
        {
            buffer[n] = '\n';
            printf("%s", buffer);
        }
        close(fd[READ]);
    }

    return 0;
}
