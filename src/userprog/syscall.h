#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "lib/user/syscall.h"

void syscall_init(void);
void halt(void);
void exit(int status);
pid_t exec(const char *cmd_lime);
int wait(pid_t pid);
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);

#endif /* userprog/syscall.h */

/*

 B3: Describe your code for reading and writing user data from the
 kernel.

    For both write and read, we first check if the stack pointer is valid.
  If the stack is valid then we can dereference the pointer and find out
  which system call to run.
    Then we get the three arguments that will be use for the system call
  “write” or “read” depending on the dereferenced pointer. Next we will
  check if the buffer is valid. If it is then we can proceed with the
  system call.
  */
 
/*
B4: Suppose a system call causes a full page (4,096 bytes) of data
to be copied from user space into the kernel.  What is the least
and the greatest possible number of inspections of the page table
(e.g. calls to pagedir_get_page()) that might result?  What about
for a system call that only copies 2 bytes of data?  Is there room
for improvement in these numbers, and how much?
*/

/*
 B5: Briefly describe your implementation of the "wait" system call
 and how it interacts with process termination.
  The system call "wait" calls process_wait which will wait for child
  processes to terminate using a while loop with sentinel being the child
  process exit member variable.
*/
/*
 B6: Any access to user program memory at a user-specified address
 can fail due to a bad pointer value.  Such accesses must cause the
 process to be terminated.  System calls are fraught with such
 accesses, e.g. a "write" system call requires reading the system
 call number from the user stack, then each of the call's three
 arguments, then an arbitrary amount of user memory, and any of
 these can fail at any point.  This poses a design and
 error-handling problem: how do you best avoid obscuring the primary
 function of code in a morass of error-handling?  Furthermore, when
 an error is detected, how do you ensure that all temporarily
 allocated resources (locks, buffers, etc.) are freed?  In a few
 paragraphs, describe the strategy or strategies you adopted for
 managing these issues.  Give an example.
*/
