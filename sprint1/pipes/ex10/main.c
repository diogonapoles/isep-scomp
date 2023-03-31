#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define READ 0
#define WRITE 1
#define START_CREDIT 20
#define WIN_CREDIT 10
#define LOSE_CREDIT -5
#define BUFFER_SIZE 4

int main()
{

    int cont = 1;
    int term = 0;

    int bet;
    int fdBet[2];
    int fdState[2];

    if (pipe(fdBet) == -1)
    {
        perror("Pipe UP");
        exit(EXIT_FAILURE);
    }

    if (pipe(fdState) == -1)
    {
        perror("Pipe State");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        srand(time(NULL));

        close(fdBet[WRITE]);
        close(fdState[READ]);
        int childCredits = START_CREDIT;
        while (childCredits > 0)
        {
            int matchingNumber = (rand() % 5) + 1;
            write(fdState[WRITE], &cont, BUFFER_SIZE);
            read(fdBet[READ], &bet, BUFFER_SIZE);
            if (bet == matchingNumber)
            {
                printf("\n        WON       \n");
                printf("Matching Number: %d\n", matchingNumber);
                printf("Child's bet: %d\n", bet);
                childCredits = childCredits + WIN_CREDIT;
            }
            else
            {
                printf("\n        LOST       \n");
                printf("Matching Number: %d\n", matchingNumber);
                printf("Child's bet: %d\n", bet);
                childCredits = childCredits + LOSE_CREDIT;
            }
            printf("Available Credit: %d\n", childCredits);
        }
        write(fdState[WRITE], &term, BUFFER_SIZE);
        close(fdBet[READ]);
        close(fdState[WRITE]);
    }

    if (pid == 0)
    {
        srand(time(NULL) * getpid());

        close(fdBet[READ]);
        close(fdState[WRITE]);
        int shouldcont = 1;
        while (shouldcont)
        {
            read(fdState[READ], &shouldcont, BUFFER_SIZE);
            int matchingNumber = (rand() % 5) + 1; 
            write(fdBet[WRITE], &matchingNumber, BUFFER_SIZE);
        }
        close(fdBet[WRITE]);
        close(fdState[READ]);
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
