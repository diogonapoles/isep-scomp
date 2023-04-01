/*
 * Copyright (c) 2023 Diogo Nápoles
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_pid_t.h>
#include <unistd.h>

#define NUM_MACHINES 4
#define NUM_PIECES 1000
#define M1_M2 5
#define M2_M3 5
#define M3_M4 10
#define M4_A1 100

int get_machine_values(int i) {
  switch (i) {
  case 0:
    return M1_M2;
  case 1:
    return M2_M3;
  case 2:
    return M3_M4;
  case 3:
    return M4_A1;
  default:
    perror("Invalid machine number!");
    exit(EXIT_FAILURE);
  }
}

char *get_machine_name(int i) {
  switch (i) {
  case 0:
    return "M2";
  case 1:
    return "M3";
  case 2:
    return "M4";
  case 3:
    return "A1";
  default:
    perror("Invalid machine number!");
    exit(EXIT_FAILURE);
  }
}

int main() {
  pid_t pid[NUM_MACHINES];
  int pieces = 0, total_pieces = 0, pieces_received = 0, fd[NUM_MACHINES][2],
      flag = 1;

  for (int i = 0; i < NUM_MACHINES; i++) {
    if (pipe(fd[i]) == -1) {
      perror("Pipe failed!");
      exit(EXIT_FAILURE);
    }
  }

  for (int machine_index = 0; machine_index < NUM_MACHINES; machine_index++) {
    pid[machine_index] = fork();

    if (pid[machine_index] == -1) {
      perror("Fork failed!");
      exit(EXIT_FAILURE);
    }

    if (pid[machine_index] == 0) {
      int required_pieces = get_machine_values(machine_index);
      char *machine_name = get_machine_name(machine_index);
      if (close(fd[machine_index][0]) == -1) {
        perror("Close failed!");
        exit(EXIT_FAILURE);
      }
      if (machine_index > 0) {
        if (close(fd[machine_index - 1][1]) == -1) {
          perror("Close failed!");
          exit(EXIT_FAILURE);
        }
      }

      while (total_pieces < NUM_PIECES) {
        switch (machine_index) {
        case 0:
          if (pieces == required_pieces) {
            flag = 1;
            total_pieces += pieces;

            write(fd[machine_index][1], &flag, sizeof(flag));
            write(fd[machine_index][1], &pieces, sizeof(pieces));

            char message[50];
            int message_length =
                sprintf(message, "# Shipment to %s | %d pieces [TOTAL - %d]\n",
                        machine_name, pieces, total_pieces);
            write(STDOUT_FILENO, message, message_length);

            flag = 0;
            pieces = 0;
          }
          pieces++;
          break;

        default:
          if (read(fd[machine_index - 1][0], &flag, sizeof(flag)) == -1) {
            perror("Read failed!");
            exit(EXIT_FAILURE);
          }
          if (flag == 0)
            break;

          if (read(fd[machine_index - 1][0], &pieces_received,
                   sizeof(pieces_received)) == -1) {
            perror("Read failed!");
            exit(EXIT_FAILURE);
          }
          pieces += pieces_received;
          if (pieces != required_pieces)
            break;

          flag = 1;
          total_pieces += pieces;

          write(fd[machine_index][1], &flag, sizeof(flag));
          write(fd[machine_index][1], &pieces, sizeof(pieces));

          char message[50];
          int message_length =
              sprintf(message, "# Shipment to %s | %d pieces [TOTAL - %d]\n",
                      machine_name, pieces, total_pieces);
          write(STDOUT_FILENO, message, message_length);

          flag = 0;
          pieces = 0;
          break;
        }
      }
      if (close(fd[machine_index][1]) == -1) {
        perror("Close error!");
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS);
    }
  }

  if (close(fd[3][1]) == -1) {
    perror("Close error!");
    exit(EXIT_FAILURE);
  }

  while (pieces_received < NUM_PIECES) {
    read(fd[3][0], &flag, sizeof(flag));
    if (flag == 1) {
      read(fd[3][0], &total_pieces, sizeof(total_pieces));
      pieces_received += total_pieces;
    }
  }

  char message[30];
  int len = sprintf(message, "Done!\n");
  write(STDOUT_FILENO, message, len);

  if (close(fd[3][0]) == -1) {
    perror("Error closing read end of pipe");
    exit(EXIT_FAILURE);
  }

  return 0;
}
