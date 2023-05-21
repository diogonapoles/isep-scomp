#pragma once

#include <semaphore.h>

#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
#if defined(__APPLE__)
/* macOS does not define sem_timedwait(), so we use our own impl */
int libsem_timedwait(sem_t *sem, const struct timespec *restrict abstime);
#define sem_timedwait(sem, abstime) libsem_timedwait(sem, abstime)
#endif /* __APPLE__ */
#endif /* _POSIX_C_SOURCE >= 200112L */
