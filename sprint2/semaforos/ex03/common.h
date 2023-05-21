#include <stddef.h>

#define SHM_NAME "/_ex2"
#define SEM_NAME "/x2"

#define MAX_STRS 5
#define MAX_STR_LEN 80

typedef struct shared_data shared;
struct shared_data {
  char arr[MAX_STRS][MAX_STR_LEN];
  size_t len;
};
