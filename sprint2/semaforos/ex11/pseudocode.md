In order to solve this exercise, I opted to create my code based on the Producer-Consumer
problem!

```
seller:

    int shared_tickets -> shared memory
    AVAILABLE_TICKETS (defined)

    Semaphores: seller_sem = 0, handler_sem = 0

    Pseudo-code:
        *shared_tickets = 0

        while (*shared_tickets < AVAILABLE_TICKETS) {
            down(seller_sem)
            // sell ...
            shared_tickets++

            up(handler_sem)
        }
```
```
buyer:

    int shared_tickets -> shared memory
    AVAILABLE_TICKETS (defined)

    Semaphores: seller_sem = 0, client_sem = 1, handler_sem = 0

    Pseudo-code:
        *shared_tickets = 0

        while (*shared_tickets < AVAILABLE_TICKETS) {
            down(client_sem)
            up(seller_sem)
            down(handler_sem)
            
            // buy ...

            up(client_sem); 
        }
```