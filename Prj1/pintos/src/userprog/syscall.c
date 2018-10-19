#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int k;
	int val1, val2, val3, val4;
//	hex_dump(f->esp, f->esp, 100,1);
	switch(*(uint32_t *)(f->esp)){
		/* Project 1 */
		case SYS_HALT:
			halt();
			break;
		case SYS_EXIT:
			check_addr(f->esp+4);
			exit(*(uint32_t *)(f->esp+4));
			break;
		case SYS_EXEC:
			check_addr(f->esp+4);
			f->eax = exec((const char *)*(uint32_t *)(f->esp+4));
			break;
		case SYS_WAIT:
			check_addr(f->esp+4);
			f->eax = wait((pid_t)*(uint32_t *)(f->esp+4));
			break;
		case SYS_READ:
			check_addr(f->esp+4);
			check_addr(f->esp+8);
			check_addr(f->esp+12);
			f->eax = read((int)*(uint32_t *)(f->esp+4), (void *)*(uint32_t *)(f->esp+8), (unsigned)*((uint32_t *)(f->esp+12)));
			break;
		case SYS_WRITE:
			check_addr(f->esp+4);
			check_addr(f->esp+8);
			check_addr(f->esp+12);
			f->eax = write((int)*(uint32_t *)(f->esp+4), (void *)*(uint32_t *)(f->esp+8), (unsigned)*((uint32_t *)(f->esp+12))); 
			break;
		/* */
		case SYS_CREATE:
			break;
		case SYS_REMOVE:
			break;
		case SYS_OPEN:
			break;
		case SYS_FILESIZE:
			break;
		case SYS_SEEK:
			break;
		case SYS_TELL:
			break;
		case SYS_CLOSE:
			break;
		/* Prj 1's extra system call */
		case SYS_PIBO:
			k = *(int*)(f->esp +4);
			f->eax = s_pibo(k);
			break;
		case SYS_FSUM:
			val1 = *(int*)(f->esp + 4);
			val2 = *(int*)(f->esp + 8);
			val3 = *(int*)(f->esp + 12);
			val4 = *(int*)(f->esp + 16);
			f->eax = s_sum4(val1,val2,val3,val4);
			break;
	}
}

void halt (void){
	shutdown_power_off();
}

void exit (int status){
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_status = status;
	thread_exit ();
}

pid_t exec (const char *cmd_line){
	return process_execute(cmd_line);
}

int wait (pid_t pid){
	return process_wait(pid);
}

int read (int fd, void *buffer, unsigned size){
	int i;

	if(fd == 0){
		//input_getc();
		for(i=0; i<size; i++){
			if(((char *)buffer)[i] == '\0') break;
		}
	}

	return i;
}

int write (int fd, const void *buffer, unsigned size){
	if(fd == 1){
		putbuf(buffer, size);
		return size;
	}

	return -1;
}

void check_addr (const void *vaddr){
	if(!is_user_vaddr(vaddr)){
		exit(-1);
	}
}

int s_pibo(int k){
	int first=1,second=1;
	int temp;
	int i;

	if( k==1 || k==2)
		return 1;
	else{
		for(i=2;i<k;i++){
			temp= first + second;
			first = second;
			second = temp;
		}
		return second;
	}
}

int s_sum4(int val1, int val2, int val3, int val4){
	return val1+val2+val3+val4;
}
