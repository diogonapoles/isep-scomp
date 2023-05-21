#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>

#define SHM_BLOCK_NAME "/ex12_shm"
#define SEM_ADD        "/ex12_add"
#define SEM_TAKE       "/ex12_take"
#define SEM_MTX        "/ex12_mtx"

#define TIMEOUT 4 /* nanosleep */
#define N_PRODS 2

#define DATA_SZ    10
#define TOTAL_NUMS 30
#define MAX_ITEM   100

typedef int item_t;

typedef struct shm_block_t shm_block_t;
struct shm_block_t {
    item_t items[DATA_SZ];
    size_t rd_len, wr_len;
};

static void produce(size_t id, shm_block_t *data, sem_t *mutex1, sem_t *mutex2, sem_t *mutex);
static void consume(shm_block_t *data, sem_t *mutex1, sem_t *mutex2, sem_t *mutex);

void produce(size_t id, shm_block_t *data, sem_t *mutex1, sem_t *mutex2, sem_t *mutex)
{
    size_t i;
    int item;

    item_t *items = data->items;
    const struct timespec spec = {.tv_sec = 0, .tv_nsec = TIMEOUT};

    srand(time(NULL) * getpid());

    for (i = 0; i < TOTAL_NUMS; i++) {
        item = rand() % MAX_ITEM;

        printf("Producer %zu: generated %d\n", id, item);
        sem_wait(mutex1);

        sem_wait(mutex);
        /* exclusive access */
        items[data->wr_len++] = item;
        data->wr_len %= DATA_SZ;
        sem_post(mutex);

        sem_post(mutex2);

        nanosleep(&spec, NULL);
    }
}

void consume(shm_block_t *data, sem_t *mutex1, sem_t *mutex2, sem_t *mutex)
{
    size_t i;
    item_t item;
    const item_t *items = data->items;

    for (i = 0; i < TOTAL_NUMS; i++) {
        sem_wait(mutex2);

        sem_wait(mutex);
        item = items[data->rd_len++];
        data->rd_len %= DATA_SZ;
        sem_post(mutex);

        sem_post(mutex1);

        printf("Consumer: got value %d\n", item);
    }
}

int main(void)
{
    size_t i;

    shm_block_t *block;
    sem_t *mutex, *mutex1, *mutex2;

    int shm_fd = shm_open(SHM_BLOCK_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd < 0) {
        perror("shm_open");
        return EXIT_FAILURE;
    }

    if (ftruncate(shm_fd, sizeof(shm_block_t)) < 0) {
        perror("ftruncate");
        return EXIT_FAILURE;
    }

    block = mmap(NULL, sizeof(shm_block_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (block == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    mutex = sem_open(SEM_MTX, O_CREAT, S_IRUSR | S_IWUSR, 1);
    mutex1 = sem_open(SEM_ADD, O_CREAT, S_IRUSR | S_IWUSR, DATA_SZ);
    mutex2 = sem_open(SEM_TAKE, O_CREAT, S_IRUSR | S_IWUSR, 0);

    if (mutex == SEM_FAILED || mutex1 == SEM_FAILED || mutex2 == SEM_FAILED) {
        perror("sem_open");
        return EXIT_FAILURE;
    }

    for (i = 0; i < N_PRODS; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { /* child */
            consume(block, mutex1, mutex2, mutex);
            exit(EXIT_SUCCESS);
        } else { /* parent */
            produce(i, block, mutex1, mutex2, mutex);

            while (waitpid(-1, NULL, 0) > 0)
                ;
        }
    }

    if (sem_unlink(SEM_TAKE) < 0 || sem_unlink(SEM_ADD) < 0 || sem_unlink(SEM_MTX) < 0) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

    if (sem_close(mutex2) < 0 || sem_close(mutex1) < 0 || sem_close(mutex) < 0) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }

    if (munmap(block, sizeof(shm_block_t)) < 0) {
        perror("munmap");
        exit(EXIT_FAILURE);
    
    }

    if (close(shm_fd) < 0) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(SHM_BLOCK_NAME) < 0) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
