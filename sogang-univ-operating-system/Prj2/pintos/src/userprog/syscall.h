#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "lib/user/syscall.h"
#include "filesys/off_t.h"
#include "threads/synch.h"

struct file {
	struct inode *inode;	/* File's inode */
	off_t pos;				/* Current position */
	bool deny_write;		/* Has file_deny_write() been called? */
};

struct lock file_lock;		/* Lock variable */

void syscall_init (void);
/* Check address is in kernel memory */
void check_addr (const void *vaddr);
int s_pibo(int k);
int s_sum4(int val1, int val2, int val3, int val4);

#endif /* userprog/syscall.h */
