#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    int p = fork(); 
    if (p == 0) {
        printf("I’m..\n");
    } else {
        printf("I'll never join you!\n");
    }
    if (p == 0) {
        printf("the..\n");
    } else {
        printf("I'll never join you!\n");
    }
    if (p == 0) {
        printf(" father!\n");
    } else {
        printf("I'll never join you!\n");
    }
} 