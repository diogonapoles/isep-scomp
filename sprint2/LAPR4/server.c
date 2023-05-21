#include <fcntl.h>
#include <stddef.h>
#include <sys/semaphore.h>

#if defined(__linux__)
# include <linux/limits.h>
#elif defined(__APPLE__)
# define PATH_MAX 1024
#else
# error "idk"
#endif

#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

/* TODO: remove this later */
#include <assert.h>

#include "common.h"
#include "helpers.h"

/* Default board names */
#define JOVAMI      "/board.jovami"
#define ORION       "/board.orion"
#define ANTI_YELLOW "/board.anti-yellow"

static void *init_mem(const char *restrict name, bool excl, size_t size);

void *
init_mem(const char *restrict name, bool excl, size_t size)
{
    int fd, trunc = true;
    struct stat sb;
    void *block;

    char *buf;
    size_t len = strlen("/dev/shm/") + strlen(name);

    if (!(buf = calloc(len + 1, 1))) {
        trunc = false;
    } else {
        if ((size_t) snprintf(buf, len + 1, "/dev/shm/%s", name) > len)
            return NULL;
        if (stat(buf, &sb) == 0)
            trunc = false;
        free(buf);
    }

    fd = shm_open(name,
                  O_CREAT | O_RDWR | (excl ? O_EXCL : 0) | (trunc ? O_TRUNC : 0),
                  0640);
    if (fd == -1) {
        perror("shm_open");
        return NULL;
    }

    if (trunc && ftruncate(fd, size) == -1) {
        perror("ftruncate");
        close(fd);
    }

    block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (block == MAP_FAILED) {
        perror("mmap");
        shm_unlink(name);
        return NULL;
    }

    return block;
}

static void init_sems(const char *board_name, const size_t rows, const size_t cols);
static void destroy_sems(const char *board_name, const size_t rows, const size_t cols);

void
init_sems(const char *board_name, const size_t rows, const size_t cols)
{
    char buf[PATH_MAX];

    size_t i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            int ret = snprintf(buf, sizeof(buf), "%s.%zu.%zu.mutex", board_name, i, j);
            assert((unsigned) ret < sizeof(buf));

            assert(sem_open(buf, O_CREAT | O_EXCL, 0640, 1));

            ret = snprintf(buf, sizeof(buf), "%s.%zu.%zu.wrt", board_name, i, j);
            assert((unsigned) ret < sizeof(buf));

            assert(sem_open(buf, O_CREAT | O_EXCL, 0640, 1));
        }
    }
}

void
destroy_sems(const char *board_name, const size_t rows, const size_t cols)
{
    char buf[PATH_MAX];

    size_t i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            int ret = snprintf(buf, sizeof(buf), "%s.%zu.%zu", board_name, i, j);
            assert((unsigned) ret < sizeof(buf));

            sem_unlink(buf);
        }
    }
}

int
main(void)
{
    boardlist *list = init_mem(DATABASE, true, sizeof(boardlist));
    if (!list){
        fputs("Failed to create database\n", stderr);
        return EXIT_FAILURE;
    }

    sem_t *database_sem = sem_open(DATABASE_SEM, O_CREAT | O_EXCL, 0640, 0);
    if (database_sem == SEM_FAILED){ 
        perror("database_sem: sem_open");
        shm_unlink(DATABASE);
        return EXIT_FAILURE;
    }

    puts("Initialized board database");

    sharedboard *b1 = init_mem(JOVAMI, true, sizeof(sharedboard));
    if (b1) {
        strncpy(list->boards[list->len++], JOVAMI, MAX_NAME_LEN);
        strncpy(b1->name, JOVAMI, MAX_NAME_LEN);
        init_sems(JOVAMI, BOARD_ROWS, BOARD_COLS);
    }

    sharedboard *b2 = init_mem(ORION, true, sizeof(sharedboard));
    if (b2) {
        strncpy(list->boards[list->len++], ORION, MAX_NAME_LEN);
        strncpy(b2->name, ORION, MAX_NAME_LEN);
        init_sems(ORION, BOARD_ROWS, BOARD_COLS);
    }

    sharedboard *b3 = init_mem(ANTI_YELLOW, true, sizeof(sharedboard));
    if (b3) {
        strncpy(list->boards[list->len++], ANTI_YELLOW, MAX_NAME_LEN);
        strncpy(b3->name, ANTI_YELLOW, MAX_NAME_LEN);
        init_sems(ANTI_YELLOW, BOARD_ROWS, BOARD_COLS);
    }

    puts("Initialized all boards");
    sem_post(database_sem);

    /* TODO: better signal handling */
    getchar();
    /* pause(); */

    shm_unlink(ANTI_YELLOW);
    destroy_sems(ANTI_YELLOW, BOARD_ROWS, BOARD_COLS);
    shm_unlink(ORION);
    destroy_sems(ORION, BOARD_ROWS, BOARD_COLS);
    shm_unlink(JOVAMI);
    destroy_sems(JOVAMI, BOARD_ROWS, BOARD_COLS);

    sem_unlink(DATABASE_SEM);
    shm_unlink(DATABASE);

    return 0;
}
