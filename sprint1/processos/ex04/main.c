#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    int a=0, b, c, d;
    b = (int) fork();
    c = (int) getpid(); 
    d = (int) getppid();
    a = a + 5;
    printf("\na=%d, b=%d, c=%d, d=%d\n",a,b,c,d);

    sleep(1);

    if (b==0)
    {
        printf("child");
        printf(
        "\nC - getpid -- 4321\n"
        "C - getPpid -- 1234\n");
    }else{
        printf("father");
        printf(
        "\nP - fork -- 4321\n"
        "P - getpid -- 1234\n");

    printf(
        "P"
        "|___C"
        "|"
        );

    }
    
    
} 

//as variaveis "a" de ambos os processos terao sempre o mesmo valor
//a variavel "b" do processo pai terá o valor de o pid do pai pós fork, e esta será igual ao ppid do filho que esta localizado na variavel "c".b(pai) = c(filho).
//a variavel "c" do pai será o seu pid que sera igual ao ppid do filho. c(pai) = d(filho). 

