#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

/* For busy-waiting, store current thread's status and exit status */
int mem_lock;
int mem_exit_status;

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
/* Extract file name from command function */
void find_filename(char *src, char *dst);
/* Build stack function */
void build_stack(char *command, void **esp);

#endif /* userprog/process.h */
