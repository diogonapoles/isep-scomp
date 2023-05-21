#include <stddef.h>

#define SEM_NAME1 "/sem.8"
#define SEM_NAME2 "/sem.81"


#define MAX_PESSOAS 100
#define NAME 50
#define ADDR 100

#define SHM_NAME "/shm_ex8"
#define SEM_NAME "/sem_ex8"

typedef struct record_data record;
typedef struct shared_data shared;
struct record_data{
    int number;
    char name[NAME];
    char address[ADDR];
} ;
struct shared_data{
    int index;
    record entry[MAX_PESSOAS];
} ;