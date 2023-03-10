#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main() {
    int pid, sig_number;

    printf("Introduza o PID do processo: ");
    scanf("%d", &pid);
    printf("Introduza o número do sinal a enviar: ");
    scanf("%d", &sig_number);

    //send signal
    if (kill(pid, sig_number) == -1) {
        printf("Erro a enviar sinal\n");
        exit(1);
    } else {
        printf("Sinal enviado com sucesso\n");
    }

    return 0;
}