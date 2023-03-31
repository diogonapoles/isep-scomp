#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//b)
int spawn_childs(int n){
    pid_t pid;
    for (size_t i = 0; i < n; i++)
    {
        pid = fork();
        if (pid<0)
        {
            return -1;
        }else if (pid ==0)
        {
            return i+1;
        }
    }
    return 0;
}

int main()
{
    int n = 6;
    int i = spawn_childs(n);
    if (i!=0)
    {
        exit(i*2);
    }
    int status;
    for (size_t i = 0; i < 6; i++)
    {
        wait(&status);
        if (WIFEXITED(status))
        {
            printf("%d\n",WEXITSTATUS(status));
        }
    }
    return 0;
} 



/*a)
int spawn_childs(int n){
    pid_t pid;
    int i=0;
    for (size_t i = 0; i < n; i++)
    {
        pid = fork();
        if (pid<0)
        {
            return -1;
        }else if(pid==0){
            return i+1;
        }
    }
    return 0;
}*/


