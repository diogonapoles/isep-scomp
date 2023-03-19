#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


void sig_handler(int signum){
    char msg[80];
    sprintf(msg,"I captured a SIGUSR1 sent by the process with PID %d\n", getpid());
    write(STDOUT_FILENO, msg, strlen(msg));
}



int main(void) {
    struct sigaction act = { .sa_handler = &sig_handler};
    
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);

    printf("My PID is %d\n", getpid());
    
    raise(SIGUSR1);

    return 0;
}


