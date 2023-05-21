#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

#define SHM_NAME     "ex10_shm"
#define SEM_CAPACITY "ex10_sem_capacity"
#define SEM_DOOR1    "ex10_sem_door1"
#define SEM_DOOR2    "ex10_sem_door2"
#define SEM_DOOR3    "ex10_sem_door3"

#define N_PASSENGER    400
#define SIMUL_TIME     5  /* in seconds */
#define TIME_TO_EMBARK 50 /* nanoseconds */

#define MAX_CAPACITY 200
#define MILLION      1000000
#define ON_TRAIN_INI 150

/* 25% chance of wanting to leave */
#define wantstoleave() (rand() % 4 == 0)

enum { D1, D2, D3, D_LAST }; /* doors */
enum state_t { IN_TRAIN, IN_STATION };

typedef struct shm_block_t shm_block_t;
struct shm_block_t {
    int capacity;
    int serial_no;
};

struct door_t {
    int number;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

static inline int gotodoor(struct door_t *door);
static void simulate(size_t id,
                     struct door_t *door,
                     shm_block_t *train,
                     enum state_t *state,
                     pthread_mutex_t *capacity_mutex);

inline int gotodoor(struct door_t *door)
{
    const struct timespec spec = {.tv_sec = 0, .tv_nsec = TIME_TO_EMBARK};
    return pthread_mutex_timedlock(&door->mutex, &spec);
}

void simulate(size_t id,
              struct door_t *door,
              shm_block_t *train,
              enum state_t *state,
              pthread_mutex_t *capacity_mutex)
{
    bool entered;

    switch (*state) {
    case IN_STATION:
        if (gotodoor(door) == 0) {
            pthread_mutex_lock(capacity_mutex);
            entered = train->capacity < MAX_CAPACITY;
            if (entered)
                train->capacity++;
            pthread_mutex_unlock(capacity_mutex);
            pthread_mutex_unlock(&door->mutex);

            if (entered) {
                printf("Passenger %zu entered train through door no. %hu!\n", id, door->number);
            } else {
                printf("Passenger %zu couldn't enter the train. Train was full D:\n", id);
            }
        } else {
            pthread_mutex_unlock(&door->mutex);
            printf("Passenger %zu couldn't enter the train. Too many people at door no. %hu.\n", id, door->number);
        }
        break;
    case IN_TRAIN:
        if (!wantstoleave())
            return;

        if (gotodoor(door) == 0) {
            pthread_mutex_lock(capacity_mutex);
            train->capacity--;
            pthread_mutex_unlock(capacity_mutex);
            pthread_mutex_unlock(&door->mutex);

            printf("Passenger %zu left through door no. %hu\n", id, door->number);
        } else {
            pthread_mutex_unlock(&door->mutex);
            printf("Passenger %zu couldn't leave the train on time. Too many people D:\n", id);
        }
        break;
    }

    *state ^= 1;
}

int main(void)
{
    size_t i;

    shm_block_t *train;
    pthread_mutex_t capacity_mutex;
    pthread_mutexattr_t mutexattr;
    struct door_t doors[D_LAST];

    time_t now;
    enum state_t state;

    if (pthread_mutexattr_init(&mutexattr) != 0) {
        perror("pthread_mutexattr_init");
        return EXIT_FAILURE;
    }
    if (pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED) != 0) {
        perror("pthread_mutexattr_setpshared");
        return EXIT_FAILURE;
    }

    if (pthread_mutex_init(&capacity_mutex, &mutexattr) != 0) {
        perror("pthread_mutex_init");
        return EXIT_FAILURE;
    }

    for (i = 0; i < D_LAST; i++) {
        doors[i].number = i;
        if (pthread_mutex_init(&doors[i].mutex, &mutexattr) != 0) {
            perror("pthread_mutex_init");
            return EXIT_FAILURE;
        }
        if (pthread_cond_init(&doors[i].cond, NULL) != 0) {
            perror("pthread_cond_init");
            return EXIT_FAILURE;
        }
    }

    int size = sizeof(shm_block_t);

    int fd = shm_open(SHM_NAME, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, size) < 0) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    train = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (train == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    train->capacity = ON_TRAIN_INI;

    now = time(NULL);

    for (i = 0; i < N_PASSENGER; i++) {
        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        case 0:
            state = i < ON_TRAIN_INI ? IN_TRAIN : IN_STATION;
            srand(getpid() * now);

            while (time(NULL) - now < SIMUL_TIME)
                simulate(i, &doors[rand() % D_LAST], train, &state, &capacity_mutex);
            exit(EXIT_SUCCESS);
        }
    }

    while (waitpid(-1, NULL, 0) > 0)
        ;

    for (i = 0; i < D_LAST; i++) {
        pthread_mutex_destroy(&doors[i].mutex);
        pthread_cond_destroy(&doors[i].cond);
    }
    pthread_mutex_destroy(&capacity_mutex);

    munmap(train, sizeof(shm_block_t));
    unlink(SHM_NAME);

    return EXIT_SUCCESS;
}
