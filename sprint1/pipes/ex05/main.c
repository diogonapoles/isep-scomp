#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 256

void upper(char *str) {
  for (int i = 0; str[i] != '\0'; ++i) {
    if (islower(str[i])) {
      str[i] = toupper(str[i]);
    }
  }
}

int main() {
  int fd_up[2], fd_down[2];
  pid_t pid;

  if (pipe(fd_up) == -1 || pipe(fd_down) == -1) {
    perror("Pipe error!");
    exit(EXIT_FAILURE);
  }

  pid = fork();
  if (pid == -1) {
    perror("Fork failed!");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // child (client)
    char msg[MAX_SIZE];
    printf("Enter a message: ");
    fgets(msg, MAX_SIZE, stdin);

    if (strlen(msg) >= MAX_SIZE) {
      perror("Message is too long!");
      exit(EXIT_FAILURE);
    }

    if (close(fd_up[0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    // write message to Up pipe
    if (write(fd_up[1], msg, strlen(msg)) < 0) {
      perror("Write error!");
      exit(EXIT_FAILURE);
    }

    if (close(fd_up[1]) == -1 || close(fd_down[1]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    // read message from Down pipe
    if (read(fd_down[0], msg, MAX_SIZE) < 0) {
      perror("Read error!");
      exit(EXIT_FAILURE);
    }

    // print converted message
    printf("Converted message: %s", msg);

    if (close(fd_down[0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
  } else {
    // parent (server)
    char msg[MAX_SIZE];
    ssize_t bytes_read;

    if (close(fd_up[1]) == -1 || close(fd_down[0]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    while (1) {
      // read message from Up pipe
      bytes_read = read(fd_up[0], msg, MAX_SIZE);
      if (bytes_read < 0) {
        perror("Read Error!");
        exit(EXIT_FAILURE);
      } else if (bytes_read == 0) { // end of file
        break;
      }

      // convert message
      upper(msg);

      // write converted message to Down pipe
      if (write(fd_down[1], msg, bytes_read) < 0) {
        perror("Write error!");
        exit(EXIT_FAILURE);
      }
    }

    if (close(fd_up[0]) == -1 || close(fd_down[1]) == -1) {
      perror("Close failed!");
      exit(EXIT_FAILURE);
    }

    // wait for child process to terminate
    if (waitpid(pid, NULL, 0) == -1) {
      perror("Waitpid error!");
      exit(EXIT_FAILURE);
    }
  }
  return 0;
}
