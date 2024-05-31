#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

void build_execution_stack(char *file_name, void **esp);
void parse_filename(char *src, char *dest);

#endif /* userprog/process.h */
/*
>> A3: Why does Pintos implement strtok_r() but not strtok()?

Pintos implement strtok_r() because in strtok() the point where the last
token was found is kept internally by the function to be use on the next
call to strtok(). The problem with this is that this will be prone to
race condition. Suppose two threads are calling strtok(), there is a possible
data race condition where one thread would use the last token held by another
thread. This would be incorrect and has potential to crash the kernel.

>> A4: In Pintos, the kernel separates commands into a executable name
>> and arguments.  In Unix-like systems, the shell does this
>> separation.  Identify at least two advantages of the Unix approach.

The kernel does not have to deal with parsing. Instead the shell will deal
with the parsing and error checking before passing the command line to the
kernel.
*/