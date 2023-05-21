Used MUTEX

```
create_or_delete_output_file()

create_semaphore()

open_numbers_file()

for i = 0 to 7:
    pid = fork()
    if pid == -1:
        exit_failure()
    else if pid == 0:
        wait_for_semaphore()

        open_numbers_file()

        open_output_file()

        for j = 0 to NUMBERS_COUNT - 1:
            lock(numbers_file)
            read_number(numbers_file)
            unlock(numbers_file)

            lock(output_file)
            write_number_to_output_file(output_file)
            unlock(output_file)

        close(numbers_file)
        close(output_file)

        release_semaphore()

        exit_success()

wait_for_children()

open_output_file()

while (ch = read_character(output_file)) != EOF:
    print_character(ch)

close(output_file)

close_semaphore()
```
