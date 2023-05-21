This exercise is the Reader / Writer problem

```
shared memory:
        int readcount = 0 
        int writecount = 0

    semaphores:
        mutex1 = 1
        mutex2 = 1
        mutex3 = 1 
        w=1
        r=1
```
```
Reader:
    down(mutex2);
    writecount++;
    if (writecount == 1)
        down(r);
    up(mutex2);
    down(w);

    sleep(1);
    print string read from shared memory
    print number of readers at the time instant

    up(w);
    down(mutex2);
    writecount--;
    if (writecount == 0)
    up(r); up(mutex2);
```
```
Writer:
    down(mutex3);
    down(r); 
    down(mutex1);
    readcount++;
    if (readcount == 1)
        down(w);
    up(mutex1);
    up(r); 
    up(mutex3);

    write PID and current time to shared memory
    print number of writers and number of readers at the time instant

    down(mutex1);
    readcount--;
    if (readcount == 0)
        up(w);
    up(mutex1);
```