#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main()
{
    pid_t p;
    if (fork() == 0) {
        printf("PID = %d\n", getpid()); exit(0);
    }
    if ((p=fork()) == 0) {
        printf("PID = %d\n", getpid()); exit(0);
    }
        printf("Parent PID = %d\n", getpid());
        printf("Waiting... (for PID=%d)\n",p);
        waitpid(p, NULL, 0);

        printf("Enter Loop...\n");
  //  while (1); /* Infinite loop */   
    getchar();
} 

/*a)
nao. No waitpid ele vai esperar pelo segundo filho (o da variavel p) e no momento em que ele chega ao loop infinito so
existem o processo pai e o primeiro filho pois o pai teve conhecimento que o segundo acabou.
*/

/*b)
o processo fica zombie, pois ele ja acabou mas o pai como nao estava a espera do sinal do filho estao ambos ainda na 
lista de processos*/

/*c)
*/


