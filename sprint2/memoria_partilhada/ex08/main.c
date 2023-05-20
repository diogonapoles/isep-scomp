#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#define PATH_SIZE 100
#define WORD_SIZE 30
#define NUM_CHILDS 10
#define FILE_NAME "/ex8"

typedef struct shared
{
    char file_path[1000];
    char word_search[NUM_CHILDS][WORD_SIZE];
    int ocurrences[NUM_CHILDS];
} shared;

void add_child_words(shared *shared_data)
{
    
    strcpy(shared_data->word_search[0], "maria");
    strcpy(shared_data->word_search[1], "joana");
    strcpy(shared_data->word_search[2], "toni");
    strcpy(shared_data->word_search[3], "rui");
    strcpy(shared_data->word_search[4], "marco");
    strcpy(shared_data->word_search[5], "tiago");
    strcpy(shared_data->word_search[6], "nando");
    strcpy(shared_data->word_search[7], "tomas");
    strcpy(shared_data->word_search[8], "tobia");
    strcpy(shared_data->word_search[9], "marics");
}

int main(void)
{
    int fd = shm_open(FILE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("Shared memory: ");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, sizeof(shared)) == -1)
    {
        perror("Ftrunccate: ");
        exit(EXIT_FAILURE);
    }

    shared *shared_data = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    add_child_words(shared_data);
    strcpy(shared_data->file_path,"text.txt");


    for (size_t i = 0; i < NUM_CHILDS; i++)
    {
        pid_t pid = fork();
        FILE *file;
        char text[1000];
        int occurrences = 0;

        if (pid < 0)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            file = fopen(shared_data->file_path, "r");
            if (file == NULL)
            {
                perror("fopen error: ");
                exit(EXIT_FAILURE);
            }
            while (fscanf(file, "%s", text) == 1)
            {
                if (strstr(text, shared_data->word_search[i]) != 0)
                {
                    occurrences++;
                }
            }
            if (fclose(file) == EOF)
            {
                perror("fclose error: ");
                exit(EXIT_FAILURE);
            }
            shared_data->ocurrences[i] = occurrences;
            exit(EXIT_SUCCESS);
        }
    }


    for (size_t i = 0; i < NUM_CHILDS; i++)
    {
        wait(NULL);
    }
    for (size_t i = 0; i < NUM_CHILDS; i++)
    {
        printf("CHILD %ld: ocurrences %d\n", i, shared_data->ocurrences[i]);
    }

    if (shm_unlink(FILE_NAME) == -1)
    {
        perror("shm_unlink error: ");
        exit(EXIT_FAILURE);
    }
    return 0;
}