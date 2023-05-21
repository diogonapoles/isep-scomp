For this code i will use the sharing of limited resources pattern, restricting priorities.

```
Before everything:

    I will use 3 mutex semaphores:
        - mutex_vip = 1
        - mutex_special = 1
        - mutex = 1
    And 3 normal semaphores:
        - vip = 1
        - special = 1
        - general = MAXIMUM_CAPACITY // to control the club capacity
    In the shared memory i have:
        - vip_waiting = 0
        - special_waiting = 0
        - capacity = 0

Club not full:

    down(general)
    make sure club is not full
        down(mutex)
        shared_block->capacity++
        up(general)
        up(mutex)

Club full:
(Each client priority executes a different code)
    NORMAL:
        down(special);
        up(vip);

        down(general);

        up(vip);
        up(special);

        // have fun at the club

        up(general);
    SPECIAL:
        down(mutex_special);
        data->special_waiting++;
        if (data->special_waiting == 1)
            down(special);
        up(mutex_special);

        down(vip);

        down(general);

        up(vip);

        down(mutex_special);
        data->special_waiting--;
        if (data->special_waiting == 1)
            up(special);
        up(mutex_special);

        // have fun at the club

        up(general);
    VIP:
        down(mutex_vip);
        data->vip_waiting++;
        if (data->vip_waiting == 1)
            down(special);
        up(mutex_vip);

        down(general);

        down(mutex_vip);
        data->vip_waiting--;
        if (data->vip_waiting == 1)
            up(special);
        up(mutex_vip);

        // have fun at the club

        up(general);


```
