#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// signal handler function
void handle_SIGINT(int signo) {
  write(STDOUT_FILENO, "I won't let the process end with CTRL-C!\n", 42);
}

void handle_SIGQUIT(int signo) {
  write(STDOUT_FILENO, "I won't let the process end by pressing CTRL-\\!\n", 48);
}

int main() {
  // SIGINT
  struct sigaction act_sigint;

  memset(&act_sigint, 0, sizeof(struct sigaction));
  sigemptyset(&act_sigint.sa_mask); // clear signal mask
  act_sigint.sa_flags = 0; // set flags to 0, not necessary (but good practice)
  act_sigint.sa_handler = handle_SIGINT;

  sigaction(SIGINT, &act_sigint, NULL);

  // SIGQUIT
  struct sigaction act_sigquit;

  memset(&act_sigquit, 0, sizeof(struct sigaction));
  sigemptyset(&act_sigquit.sa_mask); // clear signal mask
  act_sigquit.sa_flags = 0; // set flags to 0, not necessary (but good practice)
  act_sigquit.sa_handler = handle_SIGQUIT;

  sigaction(SIGQUIT, &act_sigquit, NULL);

  for ( ; ; ) {
    printf("I like signals\n");
    sleep(1);
  }
}
