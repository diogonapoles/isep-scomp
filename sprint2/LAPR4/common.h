#pragma once

#include <stddef.h>

#define DATABASE     "/board-database"
#define DATABASE_SEM "/sem_database"

#define MAX_BOARDS   3
#define MAX_NAME_LEN 80

/* board dimensions */
enum {
    BOARD_ROWS = 5,
    BOARD_COLS = 5,
};

typedef struct boardlist boardlist;
struct boardlist {
    char boards[MAX_BOARDS][MAX_NAME_LEN];
    size_t len;
};

enum content_type {
    EMPTY,
    IMAGE,
    TEXT,
};
union post_it {
    char text[256];
    char image[1024];
};

typedef struct cell cell;
struct cell {
    enum content_type type;
    union post_it content;
    size_t num_readers;
};

typedef struct sharedboard sharedboard;
struct sharedboard {
    char name[MAX_NAME_LEN];
    cell cell[BOARD_ROWS][BOARD_COLS];
};

enum cell_sems { MUTEX, WRT, CELL_SEM_LAST };
