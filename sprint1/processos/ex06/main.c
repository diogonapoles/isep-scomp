#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(){
    int i;
    int status;
    pid_t pid[4];
    
    for (i = 0; i < 4; i++) {
        pid[i]=fork();
        if (pid[i] == 0) {
            sleep(1);
            printf("Son: %d.\n",i+1);
            exit(i+1);
        }
    }

    for (i = 0; i < 4; i++) {

        if (pid[i] % 2 == 0) {
            waitpid(pid[i], &status, 0);
            if (WIFEXITED(status))
            {
                int exitStatus = WEXITSTATUS(status);
                printf("Child with PID %d was exited with status of %d\n",pid[i],exitStatus);
            }
            
        }
    }
    printf("This is the end.\n");
} 

//a)
// é criado 15 processos
/* P
   |___C
   |    |
   |    |___C
   |    |   |___C
   |    |   |   |___C
   |    |   |   |
   |    |   |___C
   |    |   |   
   |    |___C
   |    |   |___C
   |    |   |___C
   |    |
   |___C
   |    |___C
   |    |   |___C
   |    |___C
   |
   |___C
   |    |___C
   |
   |___C
   |
*/

//b)
//subtituir o limite do for para 2
/*for (i = 0; i < 2; i++) {
        if (fork() == 0) {
            sleep(1);
        }
    }*/

//c)
/*for (i = 0; i < 2; i++) {
        pid[i]=fork();
        if (pid[i] == 0) {
            sleep(1);
            printf("Son: %d.\n",i+1);
        }
    }

    for (i = 0; i < 2; i++) {
        if (pid[i] % 2 == 0) {
            waitpid(pid[i], &status, 0);
        }
    }*/

//d)
//above