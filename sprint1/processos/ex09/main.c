#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_CHILDREN 10
#define NUM_NUMBERS 100

int main() {
    pid_t pid[NUM_CHILDREN];
    int i, j;

    for (i = 0; i < NUM_CHILDREN; i++) {
        pid[i] = fork();

        if (pid[i] == 0) { // child processes
            int start_num = (i * NUM_NUMBERS) + 1;
            int end_num = start_num + NUM_NUMBERS - 1;

            for (j = start_num; j <= end_num; j++) {
                printf("%d ", j);
            }

            printf("\n");

            exit(0);
        } else if (pid[i] < 0){
			perror("FORK FALHOU!"); 
			exit(1);
		}
    }

    // parent processes
    for (i = 0; i < NUM_CHILDREN; i++) {
        waitpid(pid[i], NULL, 0);
    }

    return 0;
}

/*
	-> O output não é necessariamente ordenado e não pode existir essa garantia.
	A ordem pela qual os processos filhos imprimem os seus números depende, unicamente,
	da ordem pela qual o escalonador os executa. Apesar de o processo pai criar os
	processos filhos, depois de criados, o processo pai não tem QUALQUER controlo sobre
	a sua ordem de execução. 
	
	Para além disso, o sistema operativo pode também executar os processos filhos
	de forma concorrente, o que provocaria um output intercalado dos processos filhos.
*/

