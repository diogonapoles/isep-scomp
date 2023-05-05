
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
  if ((fd = shm_open("/sharedmem", O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
    perror("Error opening shared memory!");
    exit(EXIT_FAILURE);
  }

  // map memory
  student = (Student *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                            fd, 0);
  if (student == MAP_FAILED) {
    perror("Error with mmap()");
    exit(EXIT_FAILURE);
  }

  while (!student->number) {
    sleep(1);
  }

  printf("Read data\n");
  printf("=========\n");
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

  // delete shared memory space
  if (shm_unlink("/sharedmem") == -1) {
    perror("Error while deleting memory space");
    exit(EXIT_FAILURE);
  }

  return 0;
}
