#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    printf("\na)\n"
        "The signal function provides a simple interface for establishing an action for a particular signal and is not very consistent.\n"
        "the only portable use of signal() is to set a signal's disposition to SIG_DFL or SIG_IGN.\n"
        "Using sigaction() with just the sa_handler is the same as using signal().\n"
        "According to the manuals the most suitable function is signaction() for the reasons showned above.\n");
    
    printf("\nb)\n"
        "sa_handler specifies the action to be associated with signum and could be SIG_DFL the default action or SIG_IGN to ignore the signal\n"
        "sa_mask specifies a mask of signals which should be blocked\n"
        "sa_flags specifies a set of flags which modify the behavior of the signal. \n"
        "sa_restorer field is not intended for application use.\n"
        "when using the sa_siginf the the sa_flags field, sa_sigaction should be used instead of sa_handler\n");
    return 0;
} 
/*
a)


b)

*/