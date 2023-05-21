In this exercise, we use the semaphores to make sure every child finishes the desired function,
th following pseudocode explains how it works:

```
using one semaphore named sem

Before everything:
    parse the user input:)
    setup timeval struct

    ...

    fork()

Child:
    sleep(execution time)
    up(sem)

Parent:
    wait for processes and down(sem) each one

    presents to the user the execution time
```
