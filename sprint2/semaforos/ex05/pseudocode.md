```
Open SEMAPHORES_NUMBER number of semaphores

for each (index i from 0 to SEMAPHORES_NUMBER - 1):
  create a semaphore with a unique name "/semaphore_i"
  initialize the semaphore to 1 for the first semaphore, 0 for others

for each (child process ID (id) from 0 to CHILDREN_NUMBER - 1):
  fork a child process
  if fork fails, print error and exit failure
  if it is a child process, break out of the loop

parent process:
  switch (id):
    - for id 0:
      - down sem[0]
      - print "Sistemas "
      - up sem[1]
      - down sem[0]
      - print "a "
      - up sem[1]
      - exit

    - for id 1:
      - down sem[1]
      - print "de "
      - up sem[2]
      - down sem[1]
      - print "melhor "
      - up sem[2]
      - exit

    - for id 2:
      - down sem[2]
      - print "Computadores - "
      - up sem[0]
      - down sem[2]
      - print "disciplina!\n"
      - exit

wait for child processes

close all semaphores

destroy all the semaphores

exit program
```
