#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t child1, child2;
  int status1, status2;

  child1 = fork();
  if (child1 == 0) {
    printf("[A EXECUTAR] Filho 1 com PID %d\n", getpid());
    sleep(1);
    printf("\n[FIM DE EXECUÇÃO] Filho 1 com PID %d\n", getpid());
    exit(1);
  } else if (child1 == -1) {
    perror("Fork falhou");
    exit(1);
  }

  child2 = fork();
  if (child2 == 0) {
    printf("[A EXECUTAR] Filho 2 com PID %d\n", getpid());
    sleep(2);
    printf("\n[FIM DE EXECUÇÃO] Filho 2 com PID %d\n", getpid());
    exit(2);
  } else if (child2 == -1) {
    perror("Fork falhou");
    exit(1);
  }

  // wait for both processes to finish
  waitpid(child1, &status1, 0);
  printf("O processo filho com PID %d retornou: %d\n", child1,
         WEXITSTATUS(status1));

  waitpid(child2, &status2, 0);
  printf("O processo filho com PID %d retornou: %d\n", child2,
         WEXITSTATUS(status2));
  return 0;
}
