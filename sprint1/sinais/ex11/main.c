/*
 * Copyright (c) 2023 Diogo Nápoles
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void signal_handler(int signum) {
  char print[20];
  sigset_t set;
  sigprocmask(SIG_UNBLOCK, NULL, &set);

  write(STDOUT_FILENO, "\nBlocked signals:\n", strlen("\nBlocked signals:\n"));
  for (int i = 1; i < NSIG; i++) {
    if (sigismember(&set, i)) {
      sprintf(print, "\t%d: Blocked\n", i);
    } else {
      sprintf(print, "\t%d: Not blocked\n", i);
    }
    write(STDOUT_FILENO, print, strlen(print));
  }
}

int main() {
  pid_t pid = getpid();
  printf("Process ID: %d\n", pid);

  struct sigaction act;

  memset(&act, 0, sizeof(struct sigaction)); // clean the struct
  sigfillset(&act.sa_mask);
  act.sa_handler = signal_handler;
  act.sa_flags = 0;

  if (sigaction(SIGUSR1, &act, NULL) == -1) {
    perror("Sigaction failed!\n");
    exit(EXIT_FAILURE);
  }

  if (kill(pid, SIGUSR1) == -1) {
    perror("Kill failed!\n");
    exit(EXIT_FAILURE);
  }

  return 0;
}

// All signals are blocked except for signals 9 and 17.
// Signals 9 (SIGKILL) and 17 (SIGSTOP) cannot be blocked or unblocked by a
// process. Therefore, they remain blocked even after the sigprocmask function
// is called with the SIG_UNBLOCK flag.
