#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define READ 0
#define WRITE 1
#define N_BARCODE_READERS 5

struct product
{
    int id;
    float price;
    char name[20];
};
    typedef struct
    {
        size_t code;
        size_t index ;
    } s2;


int main(void)
{
    struct product prod[5];

    int fd[N_BARCODE_READERS][2];
    int fdStatus[2];

    if (pipe(fdStatus) == -1)
    {
        perror("pipe status");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < N_BARCODE_READERS; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            for (size_t j = 0; j < i; j++)
            {
                close(fd[j][WRITE]);
                close(fd[j][READ]);
            }

            struct product s1;
            close(fdStatus[READ]);
            srand(getpid());
            size_t value = rand() % 5;
            s2 types;
            types.code = value;
            types.index = i;

            write(fdStatus[WRITE], &types, sizeof(types));

            close(fd[i][WRITE]);

            if (read(fd[i][READ], &s1, sizeof(struct product)) > 0)
            {
                printf("Produt id %d, name: %s, price: %.2f \n", s1.id, s1.name, s1.price);
            }
            else
            {
                perror("out error");
            }

            close(fd[i][READ]);

            exit(EXIT_SUCCESS);
        }
    }

    // inicializa os campos de cada produto
    prod[0].id = 1;
    prod[0].price = 1.00;
    strcpy(prod[0].name, "Apple");
    prod[1].id = 2;
    prod[1].price = 2.00;
    strcpy(prod[1].name, "Banana");
    prod[2].id = 3;
    prod[2].price = 3.00;
    strcpy(prod[2].name, "Coconut");
    prod[3].id = 4;
    prod[3].price = 4.00;
    strcpy(prod[3].name, "Grapes");
    prod[4].id = 5;
    prod[4].price = 5.00;
    strcpy(prod[4].name, "Orange");

    close(fdStatus[WRITE]);


    for (size_t i = 0; i < N_BARCODE_READERS; i++)
    {
        s2 types;
        if (read(fdStatus[READ], &types, sizeof(types)) > 0)
        {
            close(fd[types.index][READ]);

            if (write(fd[types.index][WRITE], &prod[types.code], sizeof(struct product)) < 0)
            {

                perror("write");
            }

            close(fd[types.index][WRITE]);
        }
    }

    close(fdStatus[READ]);

    for (size_t i = 0; i < N_BARCODE_READERS; i++)
    {
        wait(NULL);
    }

    return 0;
}