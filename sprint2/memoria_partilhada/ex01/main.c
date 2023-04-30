#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_pid_t.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#define NAME_SIZE 60
#define ADDRESS_SIZE 100

typedef struct {
  char name[NAME_SIZE];
  char address[ADDRESS_SIZE];
  int number;
} Student;

int main() {
  int fd, data_size = sizeof(Student);
  Student *student;

  // create shared memory
  if ((fd = shm_open("/shmEX1", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR)) ==
      -1) {
    perror("Error creating shared memory!");
    exit(EXIT_FAILURE);
  }

  // adjust memory size
  if (ftruncate(fd, data_size) == -1) {
    perror("Truncate error! Couldn't adjust memory size.");
    exit(EXIT_FAILURE);
  }

  // map memory
  student = (Student *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                            fd, 0);
  if (student == MAP_FAILED) {
    perror("Error with mmap()");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("Fork failed!");
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    // get student's information
    printf("Insert student's name: ");
    scanf(" %59[^\n]", student->name);

    printf("Insert student's address: ");
    scanf(" %99[^\n]", student->address);

    printf("Insert student's number: ");
    scanf(" %d", &(student->number));

    // unmap memory
    if (munmap(student, data_size) == -1) {
      perror("Error with munmap()");
      exit(EXIT_FAILURE);
    }

    // close file descriptor
    if (close(fd) == -1) {
      perror("Error closing file descriptor!");
      exit(EXIT_FAILURE);
    }

    // delete shared memory space
    if (shm_unlink("/shmEX1") == -1) {
      perror("Error while deleting memory space");
    }
    exit(EXIT_FAILURE);
  } else {
    while (!student->number) {
      sleep(1);
    }

    printf("Read data:\n");
    printf("Name: %s\n", student->name);
    printf("Address: %s\n", student->address);
    printf("Number: %d\n", student->number);

    // unmap memory
    if (munmap(student, data_size) == -1) {
      perror("Error with munmap()");
      exit(EXIT_FAILURE);
    }

    // close file descriptor
    if (close(fd) == -1) {
      perror("Error closing file descriptor!");
      exit(EXIT_FAILURE);
    }

    if (shm_unlink("/shmEX1") == -1) {
      perror("Error while deleting memory space");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
  }

  return 0;
}
