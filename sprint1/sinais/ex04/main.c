#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    return 0;
} 
/*
a)
The signal function provides a simple interface for establishing an action for a particular signal and is not very consistent. 
the only portable use of signal() is to set a signal's disposition to SIG_DFL or SIG_IGN.
Using sigaction() with just the sa_handler is the same as using signal().
According to the manuals the most suitable function is signaction() for the reasons showned above.

b)
sa_handler specifies the action to be associated with signum and could be SIG_DFL the default action or SIG_IGN to ignore the signal
sa_mask specifies a mask of signals which should be blocked
sa_flags specifies a set of flags which modify the behavior of the signal. 
sa_restorer field is not intended for application use.
when using the sa_siginf the the sa_flags field, sa_sigaction should be used instead of sa_handler
*/