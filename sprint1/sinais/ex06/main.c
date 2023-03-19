#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    printf("\nthe difference between global and static is that a static variable is global to the file she is defined (locked by file),\n" 
    "otherwise an global can be used between files. One example could be that if we have two global variables with the same name\n"
    "is presented an error but if we say that those are static the error vanishes, and we can understand that those static\n"
    "variables are only global to that file that they are in.\n");
    return 0;
} 
