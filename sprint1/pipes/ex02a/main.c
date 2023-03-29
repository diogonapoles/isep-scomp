#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1
#define MSG_SIZE 100

int main(void)
{

    int fd[2];
    int int_input;
    char str_input[MSG_SIZE];

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
        printf("Parent: Write an Integer: \n");
        scanf("%d", &int_input);
        printf("Parent: Write a String: \n");
        scanf("%s", str_input);
        write(fd[WRITE], &int_input, sizeof(int_input));
        write(fd[WRITE], &str_input, sizeof(str_input));
        close(fd[WRITE]);
    }

    if (pid == 0)
    {
        close(fd[WRITE]);
        read(fd[READ], &int_input, sizeof(int_input));
        read(fd[READ], &str_input, MSG_SIZE);
        close(fd[READ]);
        printf("Child: Received Integer Value: %d\n", int_input);
        printf("Child: Received String Value: %s\n", str_input);
        exit(EXIT_SUCCESS);
    }

    return 0;
} 

