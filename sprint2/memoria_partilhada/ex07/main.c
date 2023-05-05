#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define STR_SIZE 50
#define NR_DISC 10
#define CHILDS 2

typedef struct {
  int numero;
  char nome[STR_SIZE];
  int disciplinas[NR_DISC];
} aluno;

void read_student_data(aluno *data) {
  int i;

  printf("Student's number: ");
  scanf("%d", &(data->numero));
  while (getchar() != '\n') {
  } // Clear input buffer

  printf("Student's name: ");
  fgets(data->nome, STR_SIZE, stdin);
  data->nome[strcspn(data->nome, "\n")] = '\0'; // Remove newline character

  for (i = 0; i < NR_DISC; i++) {
    printf("Subject %d grade: ", i + 1);
    scanf("%d", &(data->disciplinas[i]));
    while (getchar() != '\n') {
    } // Clear input buffer
  }
  printf("\n");
}

int average(aluno *block) {
  int average = 0;
  for (size_t i = 0; i < NR_DISC; i++) {
    average += block->disciplinas[i];
  }
  return average / NR_DISC;
}

int lowest(aluno *block) {
  int lowest = INT_MAX;
  for (size_t i = 0; i < NR_DISC; i++) {
    if (block->disciplinas[i] < lowest) {
      lowest = block->disciplinas[i];
    }
  }
  return lowest;
}

int highest(aluno *block) {
  int highest = INT_MIN;
  for (size_t i = 0; i < NR_DISC; i++) {
    if (block->disciplinas[i] > highest) {
      highest = block->disciplinas[i];
    }
  }
  return highest;
}

int main(void) {
  int fd, shared_memory_size = sizeof(aluno);
  aluno *block;

  if ((fd = shm_open("/shmex07", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
    perror("Error creating shared memory");
    return EXIT_FAILURE;
  }

  if (ftruncate(fd, shared_memory_size) == -1) {
    perror("Error allocating memory");
    return EXIT_FAILURE;
  }

  if ((block = (aluno *)mmap(NULL, shared_memory_size, PROT_READ | PROT_WRITE,
                             MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("Error mapping memory");
    return EXIT_FAILURE;
  }

  pid_t child1 = fork();

  if (child1 == -1) {
    perror("Error creating child process");
    return EXIT_FAILURE;
  }

  if (child1 == 0) {
    while (!block->disciplinas[NR_DISC - 1]) {
      sleep(1);
    }

    int lowest_grade = lowest(block);
    int highest_grade = highest(block);
    printf("-> Lowest grade: %d\n", lowest_grade);
    printf("-> Highest grade: %d\n", highest_grade);

    return EXIT_SUCCESS;
  }

  pid_t child2 = fork();

  if (child2 == -1) {
    perror("Error creating child process");
    return EXIT_FAILURE;
  }

  if (child2 == 0) {
    while (!block->disciplinas[NR_DISC - 1]) {
      sleep(1);
    }

    average(block);
    printf("-> Average grade: %d\n", average(block));

    return EXIT_SUCCESS;
  }

  read_student_data(block);
  for (size_t i = 0; i < CHILDS; i++) {
    if (wait(NULL) == -1) {
      perror("Error waiting for child process");
      return EXIT_FAILURE;
    }
  }

  if (munmap(block, shared_memory_size) == -1) {
    perror("Error unmapping memory");
    return EXIT_FAILURE;
  }

  if (close(fd) == -1) {
    perror("Error closing file");
    return EXIT_FAILURE;
  }

  if (shm_unlink("/shmex07") == -1) {
    perror("Error removing shared memory");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
