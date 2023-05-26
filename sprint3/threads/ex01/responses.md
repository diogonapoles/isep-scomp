Exercise 1
==========

**a)** 5 processes are created.
Firstly, the parent process will run the fork() instruction (line 2), which will result in a child process being created. Then, the child process will run the fork instruction from line 5 and create a child process, acounting to 2 child processes at the moment. Finally, in line 10, there is a fork() instruction which will be ran by both the parent and the two other processes previously created, resulting in a total of 5 processes created.

**b)** 2 threads are created, by both processes that run inside the if statement.
