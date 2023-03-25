1 SIGHUP terminate process terminal line hangup
2 SIGINT terminate process interrupt program
3 SIGQUIT create core image quit program
4 SIGILL create core image illegal instruction
5 SIGTRAP create core image trace trap
6 SIGABRT create core image abort program (formerly SIGIOT)
7 SIGEMT create core image emulate instruction executed
8 SIGFPE create core image floating-point exception
9 SIGKILL terminate process kill program
10 SIGBUS create core image bus error
11 SIGSEGV create core image segmentation violation
12 SIGSYS create core image non-existent system call invoked
13 SIGPIPE terminate process write on a pipe with no reader
14 SIGALRM terminate process real-time timer expired

| Signal  | Number |                                                                                                       Action                                                                                                       |
| :-----: | :----: | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| SIGHUP  |   1    |                            This signal is sent to a process when its controlling terminal is closed or when the process is restarted. The default behavior is to terminate the process.                            |
| SIGINT  |   2    |                                    This signal is sent to a process when the user presses the interrupt key (usually Ctrl+C). The default behavior is to terminate the process.                                    |
| SIGQUIT |   3    |                          This signal is sent to a process when the user presses the quit key (usually Ctrl+). The default behavior is to terminate the process and generate a core dump.                           |
| SIGILL  |   4    |                                       This signal is sent to a process when it attempts to execute an illegal instruction. The default behavior is to terminate the process.                                       |
| SIGTRAP |   5    |                                 This signal is sent to a process when a trap or breakpoint is hit. The default behavior is to stop the process and allow a debugger to take over.                                  |
| SIGABRT |   6    |                                  This signal is sent to a process when it calls the abort() function. The default behavior is to terminate the process and generate a core dump.                                   |
| SIGEMT  |   7    | This signal is sent to a process when it encounters an instruction that is not supported on the current processor or when it attempts to execute an operation that is not supported in the current emulation mode. |
| SIGFPE  |   8    |             This signal is sent to a process when it attempts to execute a floating-point operation that is not defined (such as dividing by zero). The default behavior is to terminate the process.              |
| SIGKILL |   9    |                                     This signal is used to forcibly terminate a process. The default behavior is to terminate the process immediately and without any cleanup.                                     |
| SIGBUS  |   10   |  This signal is sent to a process when it attempts to access memory that is not correctly aligned or when it attempts to access an address that does not exist. The default behavior is to terminate the process.  |
| SIGSEGV |   11   |                          This signal is sent to a process when it attempts to access memory that it does not have permission to access. The default behavior is to terminate the process.                          |
| SIGSYS  |   12   |                                       This signal is sent to a process when it attempts to execute an invalid system call. The default behavior is to terminate the process.                                       |
| SIGPIPE |   13   |                           This signal is sent to a process when it attempts to write to a pipe that has been closed on the other end. The default behavior is to terminate the process.                            |
| SIGALRM |   14   |                                 This signal is sent to a process when an alarm that was set using the alarm() function goes off. The default behavior is to terminate the process.                                 |
