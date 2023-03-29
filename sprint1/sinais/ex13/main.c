#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

sig_atomic_t flag = 0;

void handler_usr1(int signo) { flag = 1; }

int main() {
  pid_t pid = fork();
  srand((unsigned)time(NULL));

  struct sigaction act;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = handler_usr1;
  if (sigaction(SIGUSR1, &act, NULL) < 0) {
    perror("Sigaction failed!");
    exit(1);
  }

  if (pid == -1) {
    perror("Fork failed!");
    exit(1);
  } else if (pid == 0) { // child
    int task_b_time = 1 + rand() % 5;

    sleep(task_b_time);
    write(STDOUT_FILENO, "Task B: done!\n", 14);

    while (flag != 1) {
      pause();
    }

    sleep(1);
    write(STDOUT_FILENO, "Task C: done!\n", 14);
    exit(0);
  } else { // parent
    sleep(3);
    write(STDOUT_FILENO, "Task A: done!\n", 14);
    kill(pid, SIGUSR1);

    if (wait(NULL) == -1) {
      perror("Wait failed!");
      exit(1);
    }
    write(STDOUT_FILENO, "Job is complete!\n", 18);
  }

  return 0;
}
