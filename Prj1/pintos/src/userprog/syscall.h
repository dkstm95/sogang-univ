#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "lib/user/syscall.h"

void syscall_init (void);
/* Check address is in kernel memory */
void check_addr (const void *vaddr);
int s_pibo(int k);
int s_sum4(int val1, int val2, int val3, int val4);

#endif /* userprog/syscall.h */
