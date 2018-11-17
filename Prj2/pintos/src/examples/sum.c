#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syscall.h>
#include"userprog/syscall.h"
#include"lib/user/syscall.h"

int main(int argc, char *argv[]){
	int a,b,c,d;

	a = atoi(argv[1]);	
	b = atoi(argv[2]);	
	c = atoi(argv[3]);	
	d = atoi(argv[4]);	
	printf("%d %d\n",pibo(a),sum4(a,b,c,d));
	return EXIT_SUCCESS;

}

