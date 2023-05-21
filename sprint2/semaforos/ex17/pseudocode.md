I used the mutex pattern in the solving of this exercise

```
Before everything:

    I will use 1 pipe
    And 2 semaphores:
        - data = 1
        - alarm = 1
    In the shared memory i have:
        - measurements[SENSORS][READINGS]
        - total = 0
        - alarm = 0
```
```
//Sensor
alarm_activated = 0
previous_value

for loop between the 5 sensors
    value = generate random value

    down(data)

    sharedmem->measurements[i][j] = value
    sharedmem->total++

    write message to a buffer
    write to pipe

    up(data);

    if (!alarm_activated && value > 50) {
        alarm_activated = 1

        down(alarm)

        sharedmem->alarm_count++
        write message to a buffer
        write to pipe

        up(alarm)
    } else if (alarm_activated && value < 50 && previous_value < 50) {
        alarm_activated = 0

        down(alarm)

        sharedmem->alarm_count--
        write message to a buffer
        write to pipe

        up(alarm)
    }

    previous_value = value;
```
```
//Controller
for loop for 5*6 times:

    attempt to read from pipe
    read

    write pipe buffer to STDOUT_FILENO
```
