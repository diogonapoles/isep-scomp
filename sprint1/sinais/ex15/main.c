/*
 * Copyright (c) 2023 Diogo Nápoles
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
  char cmd[32];
  int time_cap;
} command_t;

void handle_SIGALRM(int signo) {
  write(STDOUT_FILENO, "The command did not complete in its allowed time!\n",
        49);
}

void handle_SIGCHLD(int signo) {
  write(STDOUT_FILENO, "Execution of the command ended!\n", 32);
}

int main(void) {
  command_t commands[] = {{"ls", 10}, {"cd", 3}};
  int num_commands = sizeof(commands) / sizeof(commands[0]);

  struct sigaction act1, act2;
  memset(&act1, 0, sizeof(struct sigaction));
  memset(&act2, 0, sizeof(struct sigaction));

  act1.sa_handler = handle_SIGALRM;
  act2.sa_handler = handle_SIGCHLD;

  sigemptyset(&act1.sa_mask);
  sigemptyset(&act2.sa_mask);

  if (sigaction(SIGALRM, &act1, NULL) < 0) {
    perror("Sigaction failed!");
    exit(EXIT_FAILURE);
  }
  if (sigaction(SIGCHLD, &act2, NULL) < 0) {
    perror("Sigaction failed!");
    exit(EXIT_FAILURE);
  }

  pid_t pid;
  int status;
  for (int i = 0; i < num_commands; i++) {
    pid = fork();
    if (pid == 0) { // child process
      if (execvp(commands[i].cmd, (char *[]){commands[i].cmd, NULL}) == -1) {
        perror("Failed to execute command");
        exit(EXIT_FAILURE);
      }
    } else if (pid > 0) { // parent process
      alarm(commands[i].time_cap);
      wait(&status);
    } else { // error
      perror("Failed to fork");
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}
