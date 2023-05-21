#include <errno.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

#include "helpers.h"

#define __helpers_ONEMILLION    1000000

int libsem_timedwait(sem_t *sem, const struct timespec *restrict abstime);

int libsem_timedwait(sem_t *sem, const struct timespec *restrict abstime)
{
    unsigned int seconds = abstime->tv_nsec * __helpers_ONEMILLION;
    int ret, err;

    if (abstime->tv_nsec < 0 && abstime->tv_nsec > 1000 * __helpers_ONEMILLION) {
        errno = EINVAL;
        return -1;
    }

    alarm(seconds);

    ret = sem_wait(sem);
    err = errno;

    if (err == EINTR)
        errno = ETIMEDOUT;
    return ret;
}
