#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
	/* Initialize lock variable */
	lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int k;
	int val1, val2, val3, val4;

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
		/* Project 2 */
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
		case SYS_CREATE:
			check_addr(f->esp+4);
			check_addr(f->esp+8);
			f->eax = create((const char*)*(uint32_t*)(f->esp+4), (unsigned)*(uint32_t*)(f->esp+8));
			break;
		case SYS_REMOVE:
			check_addr(f->esp+4);
			f->eax = remove((const char*)*(uint32_t*)(f->esp+4));
			break;
		case SYS_OPEN:
			check_addr(f->esp+4);
			f->eax = open((const char*)*(uint32_t*)(f->esp+4));
			break;
		case SYS_FILESIZE:
			check_addr(f->esp+4);
			f->eax = filesize((int)*(uint32_t*)(f->esp+4));
			break;
		case SYS_SEEK:
			check_addr(f->esp+4);
			check_addr(f->esp+8);
			seek((int)*(uint32_t*)(f->esp+4), (unsigned)*(uint32_t*)(f->esp+8));
			break;
		case SYS_TELL:
			check_addr(f->esp+4);
			f->eax = tell((int)*(uint32_t*)(f->esp+4));
			break;
		case SYS_CLOSE:
			check_addr(f->esp+4);
			close((int)*(uint32_t*)(f->esp+4));
			break;
		/* Prj 1's additional system call */
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
	int cnt;
	
	cnt = 3;
	printf("%s: exit(%d)\n", thread_name(), status);
	thread_current()->exit_status = status;

	/* Close all file descriptor */
	while(cnt < 128){
		if(thread_current()->fd[cnt] != NULL){
			close(cnt);
		}
		cnt++;
	}

	thread_exit ();
}

pid_t exec (const char *cmd_line){
	return process_execute(cmd_line);
}

int wait (pid_t pid){
	return process_wait(pid);
}

int read (int fd, void *buffer, unsigned size){
	uint8_t tmp;
	int i;
	int result;

	result = -1;
	check_addr(buffer);

	/* Lock start */
	lock_acquire(&file_lock);
	if(fd == 0){	// When input is STDIN
		for(i=0; i<size; i++){
			tmp = input_getc();
			if(tmp == NULL) break;
		}
		result = i;
	}
	else if(fd > 2){
		/* When fd[fd] has no file */
		if(thread_current()->fd[fd] == NULL){
			lock_release(&file_lock);
			/* Lock finish */
			exit(-1);
		}
		result = file_read(thread_current()->fd[fd], buffer, size);
	}
	lock_release(&file_lock);
	/* Lock finish */

	return result;
}

int write (int fd, const void *buffer, unsigned size){
	int result;

	result = -1;
	check_addr(buffer);

	/* Lock start */
	lock_acquire(&file_lock);
	if(fd == 1){	// When output is STDOUT
		putbuf(buffer, size);
		result = size;
	}
	else if(fd > 2){
		/* When fd[fd] has no file */
		if(thread_current()->fd[fd] == NULL){
			lock_release(&file_lock);
			/* Lock finish */
			exit(-1);
		}
		if(thread_current()->fd[fd]->deny_write){
			file_deny_write(thread_current()->fd[fd]);
		}
		result = file_write(thread_current()->fd[fd], buffer, size);
	}
	lock_release(&file_lock);
	/* Lock finish */

	return result;
}

bool create (const char *file, unsigned initial_size){
	if(file == NULL){
		exit(-1);
	}
	check_addr(file);

	return filesys_create(file, initial_size);
}

bool remove (const char *file){
	if(file == NULL){
		exit(-1);
	}
	check_addr(file);

	return filesys_remove(file);
}

int open (const char *file){
	int result;
	int cnt;
	struct file *fp;

	if(file == NULL){
		exit(-1);
	}
	check_addr(file);

	result = -1;
	cnt = 3;

	/* Lock start */
	lock_acquire(&file_lock);
	fp = filesys_open(file);

	if(fp != NULL){
		while(cnt < 128){
			/* When find a empty fd */
			if(thread_current()->fd[cnt] == NULL){
				if(strcmp(thread_current()->name, file) == 0){
					file_deny_write(fp);
				}
				thread_current()->fd[cnt] = fp;
				result = cnt;
				break;
			}
			cnt ++;
		}
	}
	lock_release(&file_lock);
	/* Lock finish */

	return result;
}

int filesize(int fd) {
	if(thread_current()->fd[fd] == NULL){
		exit(-1);
	}

	return file_length(thread_current()->fd[fd]);
}

void seek(int fd, unsigned position) {
	if(thread_current()->fd[fd] == NULL){
		exit(-1);
	}

	file_seek(thread_current()->fd[fd], position);
}

unsigned tell(int fd) {
	if(thread_current()->fd[fd] == NULL){
		exit(-1);
	}

	return file_tell(thread_current()->fd[fd]);
}

void close(int fd) {
	struct file* tmp;

	if(thread_current()->fd[fd] == NULL){
		exit(-1);
	}
	
	tmp = thread_current()->fd[fd];
	thread_current()->fd[fd] = NULL;

	return file_close(tmp);
}

/* Check vaddr is not user address */ 
void check_addr (const void *vaddr){
	if(!is_user_vaddr(vaddr)){
		exit(-1);
	}
}

/* Prj 1's additional function */
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
