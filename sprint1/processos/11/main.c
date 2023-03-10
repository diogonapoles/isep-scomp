#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CHILDREN 5
#define ARRAY_SIZE 1000

int main() {
    int numbers[ARRAY_SIZE], result[ARRAY_SIZE], max_value[NUM_CHILDREN], max = 0;

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (int i = 0; i < ARRAY_SIZE; i++){
        numbers[i] = rand() % 256;    
    }

    // Find maximum value of 1/5 of the array
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == 0) { //child process
            int child_max = 0;
            int start_index = i * ARRAY_SIZE / NUM_CHILDREN;
            int end_index = (i + 1) * ARRAY_SIZE / NUM_CHILDREN;
            for (int j = start_index; j < end_index; j++) {
                if (numbers[j] > child_max) {
                    child_max = numbers[j];
                }
            }
            exit(child_max);
        } else if (pid < 0) {
            perror("Fork failed!");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < NUM_CHILDREN; i++) {
        int status;
        wait(&status);
        max_value[i] = WEXITSTATUS(status);
    }

    // Compute maximum value of the entire array
    for(int i = 0; i < NUM_CHILDREN; i++){
        printf("-> Maximum value of the array(%d/%d): %d\n", (i+1), NUM_CHILDREN, max_value[i]);
        if(max_value[i] > max) max = max_value[i];
    }
    printf("-> Maximum value of the array: %d\n", max);

    //=============================================

    // Compute results for first half of array using child process
    pid_t pid = fork();
    if (pid == 0) {
        // Child process computes result for first half of array
        for (int i = 0; i < ARRAY_SIZE / 2; i++) {
            result[i] = ((int) numbers[i] / max) * 100;
            printf("[CHILD] Result for index %d: %d\n", i, result[i]);
        }
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        perror("Fork failed!");
        exit(EXIT_FAILURE);
    }

    // Compute results for second half of array using parent process
    for (int i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
        result[i] = ((int) numbers[i] / max) * 100;
    }
    wait(NULL);

    for (int i = ARRAY_SIZE / 2; i < ARRAY_SIZE; i++) {
        printf("[PARENT] Result for index %d: %d\n", i, result[i]);
    }

    return 0;
}


