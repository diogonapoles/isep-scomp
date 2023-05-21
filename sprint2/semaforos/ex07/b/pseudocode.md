For this exercise, I inspired myself in the barrier pattern and created a different approach using shared memory

```
Before Everthing:
    Shared Memory:
        int chips = 0
        int beer = 0

    Semaphores:
        chips_sem = 0
        beer_sem = 0

Buy chips:
    down(chips_sem);
    buy_chips();
    f->chips++;
    up(chips_sem);

Buy beer:
    down(beer_sem);
    buy_beer();
    f->beer++;
    up(beer_sem);

After buying:
Instead of using the barrier approach, i opted to compare the actual data i wanted to have with the data i had in the shared memory, giving me more flexibility:

    while (f->beer + f->chips != NUM_CHILD + 1) {
        sleep(1);
        if ((f->beer + f->chips) == NUM_CHILD + 1 && f->beer > 0 && f->chips > 0)
            eat_and_drink();
    }

    if (f->beer == 0 || f->chips == 0)
        printf("Not enough beer or chips!\n");
```
