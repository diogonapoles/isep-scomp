/*
 * Copyright (c) 2023 Diogo Nápoles
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  fork();
  fork();
  fork();
  printf("SCOMP!\n");
}

/*
a)
-> No total, serão criados 7 processos:

    Primeiramente, temos um processo, o processo pai. Após o primeiro fork(),
passam a existir 2 processos, o processo pai e UM processo filho.
    Após o segundo fork(), cada um dos dois processos cria outro processo filho,
resultando em um total de quatro processos.
    Por fim, após o terceiro fork(), cada um dos quatro processos cria outro
processo filho, resultando em um total de oito processos.
    Em suma, acabamos com um total de 8 processos e 7 processos criados,
resultando em oito execuções da instrução printf.

b)
|
|-----------------------|
|                       |
|-----------|           |-----------|
|           |           |           |
|-------|   |-------|   |-------|   |-------|
|       |   |       |   |       |   |       |
o       o   o       o   o       o   o       o

c)
"SCOMP!" será impresso 8 vezes, com base na explicação dada na alínea a).
*/
