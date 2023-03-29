#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define MSG_SIZE 100

int main(void)
{
    struct s{
        int s_int_input;
        char s_str[MSG_SIZE];
    };

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
        struct s s_parent;
        s_parent.s_int_input=int_input;
        strcpy(s_parent.s_str, str_input);
        write(fd[WRITE], &s_parent, sizeof(s_parent));
        close(fd[WRITE]);
    }

    if (pid == 0)
    {
        close(fd[WRITE]);
        struct s s_child;
        read(fd[READ], &s_child, MSG_SIZE);
        close(fd[READ]);
        printf("Child: Received Integer Value: %d\n", s_child.s_int_input);
        printf("Child: Received String Value: %s\n", s_child.s_str);
        exit(EXIT_SUCCESS);
    }

    return 0;
} 

