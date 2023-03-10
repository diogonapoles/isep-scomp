#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int x = 1;
    pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/

    if (p == 0) {
        x = x+1;
        printf("1. x = %d\n", x);
    } else {
        x = x-1;
        printf("2. x = %d\n", x);
    }
    printf("3. %d; x = %d\n", p, x);
}

/* 
a)
    Tendo em conta que, no fork(), o PID do filho (valor > 0) é retornado para o pai e para o 
    processo filho é retornado o valor 0:

    -> Um possível output seria:

        2. x = 0        // p != 0   (processo pai)
        3. 1234; x = 0  // p = 1234 (processo pai)
        1. x = 2        // p = 0    (processo filho)
        3. 0; x = 2     // p = 0    (processo filho)

    -> No entanto, um output deste género também seria válido:

        2. x = 0        // p != 0   (processo pai)
        1. x = 2        // p = 0    (processo filho)
        3. 0; x = 2     // p = 0    (processo filho)
        3. 1234; x = 0  // p = 1234 (processo pai)

    -> Justificação:

        A ordem pela qual os processos correm é variável, sendo que é o escalonador
    que vai decidir quais processos executar, com base em métricas de performance
    e carga computacional. 

        Deste modo, não se pode garantir uma ordem. Podendo o processo pai executar 
    primeiro e apenas depois o processo filho (e vice-versa). 
    Seriam também possíveis outras situações, por exemplo, onde é primeiro 
    executada uma parte do processo pai, seguida da totalidade do processo filho e, 
    terminando com a parte restante do processo pai.

b) 
        Não é possível garantir a ordem pela qual os printf's são executados, visto que 
    a ordem de execução dos processos apenas está dependente do escalonador.
*/