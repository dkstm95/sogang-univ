#include "20141543.h"  //header file
#define MAX_HASH 20		//hash table's num
#define MAX_SYMTAB 26
#define MAX_MEMORY 1048576	//memory's num

//global variable
hash_Node *hash_Table[MAX_HASH];
linkedList *L;
symbol_table *symtab[MAX_SYMTAB];
symbol_table *bp_list=NULL;
exsym_table *exsym[MAX_HASH];

int *obj;
int pc, base;
int obj_cnt, line_cnt;
int flag=1, error_flag=0, ex_flag=0, indi_flag=0, imm_flag=0, pb_flag;
int locctr = 0;
int progaddr = 0, address = 0;
int run_first_try;
int run_start_addr, run_end_addr, run_last_addr;
unsigned char memory[MAX_MEMORY] = {0,};

int main(){
	int i, j, k;
	int comma_cnt, token_cnt, null_cnt, tab_cnt;
	int Op_flag, mne_flag, du_flag;
	int start=0, end=0, value;
	int progaddr_tmp, bp_addr;
	char input_command[84], input_dummy[84], command_str[84];
	char *input[40];
	char *stop = NULL;
//	symbol_table *bp_tmp = bp_list;
	
	L = (linkedList*)malloc(sizeof(linkedList));
	L->cur = NULL;
	L->head = NULL;
	L->tail = NULL;

	run_start_addr = 0;
	run_end_addr = 0;
	run_last_addr = 0;
	run_first_try = 0;
	progaddr = 0;
	Op_flag = Read_Op();	//read opcode.txt file and make hash table
	
	//while opcode.txt is open
	while(Op_flag){
		//initialize local variable
		i=0;
		j=0;
		mne_flag=1;
		du_flag=1;
		comma_cnt=0;
		token_cnt=0;
		null_cnt=0;
		tab_cnt=0;
		
		printf("sicsim> ");
		scanf("%c", &input_command[i]);
		while(input_command[i] != '\n'){
			input_dummy[i] = input_command[i];
			if(input_command[i] == ' '){
				null_cnt++;
			}
			else if(input_command[i] == '\t'){
				input_dummy[i] = ' ';
				tab_cnt++;
			}
			else if(input_command[i] == ','){
				comma_cnt++;
			}
			i++;
			scanf("%c", &input_command[i]);
		}
		input_command[i] = '\0';	//for make perfect input string 
		input_dummy[i] = '\0';
		//input is only enter or tab+space bar
		if(i==0 || (null_cnt + tab_cnt == i)){
			printf("Error!\n");
			continue;
		}
		
		sscanf(input_dummy, "%s", command_str);	//cut only command part from input
		
		//cut input string by token " " and store it
		input[j] = strtok(input_dummy, " ");
		while(input[j] != NULL){
			token_cnt++;
			input[++j] = strtok(NULL, " "); 
		}
		
		//when input command is h[elp]
		if((strcmp(input[0], "help")==0 || strcmp(input[0], "h")==0) && (token_cnt==1)){
			flag = 0;
			createNode(input_command);
			printf("h[elp]\nd[ir]\nq[uit]\nhi[story]\ndu[mp] [start, end]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n\n");
		}
		//when input command is d[ir]
		else if((strcmp(input[0], "dir")==0 || strcmp(input[0], "d")==0) && (token_cnt==1)){
			flag = 0;
			createNode(input_command);
			dir_list_print();
			printf("\n");
		}
		//when input command is q[uit]
		else if((strcmp(input[0], "quit")==0 || strcmp(input[0], "q")==0) && (token_cnt==1)){
			free_SYMTAB();
			break;
		}
		//when input command is hi[story]
		else if((strcmp(input[0], "history")==0 || strcmp(input[0], "hi")==0) && (token_cnt==1)){
			createNode(input_command);
			printNode(&flag);
		}
		//when input command is du[mp]
		else if((strcmp(command_str, "dump")==0 || strcmp(command_str, "du")==0)){
			if(comma_cnt >= 0 && comma_cnt < 2){ //check comma's num is 0 or 1
				if(comma_cnt == 0){	//when comma's num is 0
					if(token_cnt == 1){	//when input command is only dump
						if(address >= MAX_MEMORY){
							printf("Error!\n");
						}
						else{
							print_dump(address, address+10*16-1, input_command);
							flag = 0;
						}
					}
					else if(token_cnt == 2){	//when input command is dump start
						du_flag = dump_check(input[1], strlen(input[1]));
						if(du_flag == 1){
							sscanf(input[1], "%X", &start);
							print_dump(start, start+10*16-1, input_command);
							flag = 0;
						}
						else printf("Error!\n");
						
					}
					else printf("Error!\n");
				}
				else if(comma_cnt == 1){	//when comma's num is 1
					if(token_cnt == 4){	//when input is dump start , end
						if(strcmp(input[2], ",") == 0){
							du_flag = dump_check(input[1], strlen(input[1]));	//check start is correct input
							if(du_flag == 1){
								du_flag = dump_check(input[3], strlen(input[3]));	//check end is correct input
								if(du_flag == 1){
									sscanf(input[1], "%X", &start);
									sscanf(input[3], "%X", &end);
									print_dump(start, end, input_command);
									flag=0;
								}
								else printf("Error!\n");
							}
							else printf("Error!\n");
						}
						else printf("Error!\n");
					}
					else if(token_cnt == 3){	//when input is dump start, end or start ,end
						find_command(input, &du_flag, &start, &end);
						if(du_flag == 1){
							print_dump(start, end, input_command);
							flag=0;
						}
					}
					else if(token_cnt == 2){	//when input is dump start,end
						for(k=0; k<strlen(input[1]); k++){
							if(input[1][k] == ','){
								if(k==0 || k==strlen(input[1])-1){	//check comma's location is front or back
									du_flag=0;
									break;
								}
								else continue;
							}
							else if(!((input[1][k] >= '0' && input[1][k] <= '9') || (input[1][k] >= 'A' && input[1][k] <= 'F') || (input[1][k] >= 'a' && input[1][k] <= 'f'))){	//check start and end is correct input 
								du_flag=0;
							}
						}
						if(du_flag == 1){
							start = strtol(input[1], &stop, 16);	//accept start value
							stop = stop+1;	//because stop's value is ","
							end = strtol(stop, &stop, 16);	//accept end value
							print_dump(start, end, input_command);
							flag = 0;
						}
						else printf("Error!\n");

					}
				}
			}
			else printf("Error!\n");
		}
		//when input command is e[dit]
		else if((strcmp(command_str, "edit")==0 || strcmp(command_str, "e")==0)){
			if(comma_cnt == 1){	//when comma's num is one
				if(token_cnt == 2){	//when input is edit address,vale
					for(k=0; k<strlen(input[1]); k++){
						if(input[1][k] == ','){
							if(k==0 || k==strlen(input[1])-1){	//check comma's location
								du_flag=0;
								break;
							}
							else continue;
						}
						else if(!((input[1][k] >= '0' && input[1][k] <= '9') || (input[1][k] >= 'A' && input[1][k] <= 'F') || (input[1][k] >= 'a' && input[1][k] <= 'f'))){
							du_flag=0;
						}
					}
					if(du_flag == 1){
						start = strtol(input[1], &stop, 16);
						stop = stop+1;
						end = strtol(stop, &stop, 16);
						edit(start, end);
						flag = 0;
						createNode(input_command);
					}
					else printf("Error!\n");

				}
				else if(token_cnt == 3){	//when input is edit address, value or adress ,value
					find_command(input, &du_flag, &start, &end);
					if(du_flag == 1){
						edit(start, end);
						flag=0;
						createNode(input_command);
					}
				}
				else if(token_cnt == 4){	//when input is edit address , value
					if(strcmp(input[2], ",") == 0){
						du_flag = dump_check(input[1], strlen(input[1]));
						if(du_flag == 1){
							du_flag = dump_check(input[3], strlen(input[3]));
							if(du_flag == 1){
								sscanf(input[1], "%X", &start);
								sscanf(input[3], "%X", &end);
								edit(start, end);
								flag=0;
								createNode(input_command);
							}
							else printf("Error!\n");
						}
						else printf("Error!\n");
					}
					else printf("Error!\n");
				}
				else printf("Error!\n");
			}
			else printf("Error!\n");
		}
		//when input command is f[ill]
		else if((strcmp(command_str, "fill")==0 || strcmp(command_str, "f") == 0)){
			if(comma_cnt == 2){	//when comma's num is 2, i.e. fill start, end, value
				find_command_f(input, token_cnt, &du_flag, &start, &end, &value);
				if(du_flag == 1){
					fill(start, end, value);
					flag = 0;
					createNode(input_command);
			}
				else printf("Error!\n");
			}
			else printf("Error!\n");
		}
		//when input command is reset
		else if((strcmp(input[0], "reset")==0) && (token_cnt==1)){
			flag = 0;
			createNode(input_command);
			reset_mem();	//make memory's value all 0
		}
		//when input command is opcode
		else if((strcmp(command_str, "opcode")==0)){
			if(token_cnt == 2){	//when input is opcode mnemonic
				for(k=0; k<strlen(input[1]); k++){
					if(!(input[1][k] >= 'A' && input[1][k] <= 'Z')){	//check input mnemonic is composed by upper case
						mne_flag = 0;
						break;
					}
				}
				if(mne_flag == 1){
					printHashNode(input[1], input_command);
					flag = 0;
				}
				else printf("Wrong mnemonic!\n");
			}
			else printf("Wrong Input!\n");
		}
		//when input command is opcodelist
		else if((strcmp(input[0], "opcodelist")==0) && (token_cnt==1)){
			flag = 0;
			createNode(input_command);
			printHashTable();
		}
		//when input command is type filename
		else if((strcmp(command_str, "type")==0) && (token_cnt == 2)){
			printFile(input[1], input_command);
			flag = 0;
			createNode(input_command);
		}
		//when input command is assemble filename
		else if((strcmp(command_str, "assemble")==0) && (token_cnt == 2)){
			free_SYMTAB();
			pass(input[1], input_command);
			createNode(input_command);
			flag = 0;
		}
		//when input command is symbol
		else if((strcmp(command_str, "symbol")==0) && (token_cnt == 1)){
			if(error_flag == 0){
				print_SYMTAB();
				createNode(input_command);
				flag = 0;
			}
			else continue;
		}
		//when input command is progaddr
		else if(strcmp(command_str, "progaddr")==0){
			if(token_cnt == 1){
				progaddr = 0;
				flag = 0;
				createNode(input_command);
			}
			else if(token_cnt == 2){
				progaddr_tmp = progaddr;
				progaddr = 0;
				j = 1;
				for(i=strlen(input[1])-1; i>=0; i--){
					if(hex_to_dec(input[1][i]) == -1){
						progaddr = progaddr_tmp;
						error_flag = 1;
						printf("Error!\n");
						break;
					}
					progaddr += (hex_to_dec(input[1][i]) * j);
					j = j * 16;
				}
				flag = 0;
				createNode(input_command);
			}
		}
		//when input command is loader
		else if((strcmp(command_str, "loader")==0) && (token_cnt >= 2) && (token_cnt <= 4)){
			free_EXSYM();
			pass1_loader(input[1], input[2], input[3]);
			if(error_flag == 1) continue;
			else{
				flag = 0;
				createNode(input_command);
			}
		}
		//when input command is run
		else if((strcmp(command_str, "run")==0) && (token_cnt == 1)){
			flag = 0;
			createNode(input_command);
			/*
			bp_tmp = bp_list;
			if(run_last_addr == 0){
				printf("Nothing loaded!\n");
				continue;
			}
			if(bp_tmp == NULL){
				run_end_addr = run_last_addr;
				Run();
				run_first_try = 0;
				printf("\tEnd Program\n");
			}
			else{
				run_end_addr = bp_tmp->loc;
				Run();
				run_start_addr = run_end_addr;
				printf("\tStop at checkpoint [%04X]\n", bp_tmp->loc);
				bp_tmp = bp_list;
				bp_list = bp_list->next;
				free(bp_tmp);
				bp_tmp = NULL;
			}
			*/
			Run();
		}
		//when input command is bp
		else if(strcmp(command_str, "bp")==0){
			if(token_cnt == 1){
				print_bp();
				flag = 0;
				createNode(input_command);
			}
			else if(token_cnt == 2 && (strcmp(input[1], "clear")==0)){
				free_bp();
				flag = 0;
				createNode(input_command);
			}
			else if(token_cnt == 2){
				token_cnt = sscanf(input_command, "%s %x", input[1], &bp_addr);
				printf("[ok] create breakpoint %04X\n", bp_addr);
				make_bplist(bp_addr);
				flag = 0;
				createNode(input_command);
			}
		}
		//when input command is wrong input
		else{
			printf("Error!\n");
			continue;
		}
	}
	
	free_hashTable();	//free hash table's linked list
	free_linkedList();	//free linked list that store history

	return 0;
}

//read opcode.txt file function and make hash table from opcode.txt's contents function
int Read_Op(){
	FILE *fp = fopen("opcode.txt", "r");
	int optmp, i, tmp=-1;
	char mnetmp[10];
	char formattmp[5];

	if(fp == NULL){
		printf("File Open Error!\n");
		return 0;
	}

	for(i=0; i<MAX_HASH; i++){
		hash_Table[i] = NULL;
	}

	while(1){
		tmp = fscanf(fp, "%X %s %s", &optmp, mnetmp, formattmp);
		if(tmp == -1) break;
		createHashTable(optmp, mnetmp, formattmp);
	}

	fclose(fp);

	return 1;
}

//make hash table's key function
int hash_Func(char *mne){
	int i;
	int sum=0;

	for(i=0; i<strlen(mne); i++){
		sum += mne[i];
	}
	
	return sum % MAX_HASH;
}

//make hash table function
void createHashTable(int op, char *mne, char *form){
	hash_Node *new = (hash_Node*)malloc(sizeof(hash_Node));
	hash_Node *ptr;
	int key = hash_Func(mne);	//find key by mnemonic

	new->opcode = op;
	strcpy(new->mnemonic, mne);
	strcpy(new->format, form);
	new->next = NULL;

	ptr = hash_Table[key];	//go to hash table[key]

	if(hash_Table[key] == NULL){
		hash_Table[key] = new;
	}
	else{
		while(ptr->next != NULL){
			ptr = (ptr->next);
		}
		(ptr->next) = new;
	}
}

//print hash table function
void printHashTable(){
	int i;
	hash_Node *ptr;

	for(i=0; i<MAX_HASH; i++){
		ptr = hash_Table[i];
		printf("%d : ", i);
		if(hash_Table[i] == NULL){
			printf("\n");
		}
		else{
			while(ptr->next != NULL){
				printf("[%s, %X] -> ", ptr->mnemonic, ptr->opcode);
				ptr = ptr->next;
			}
			printf("[%s, %X]\n", ptr->mnemonic, ptr->opcode);
		}
	}
}

//print mnemonic's opcode function
void printHashNode(char *tdata, char *vdata){
	int key = hash_Func(tdata);
	int flag=1;
	hash_Node *ptr = hash_Table[key];

	while(ptr->next != NULL){
		if(strcmp(ptr->mnemonic, tdata) == 0){
			flag = 0;
			printf("opcde is %X\n", ptr->opcode);
			break;
		}
		ptr = ptr->next;
	}
	if(strcmp(ptr->mnemonic, tdata) == 0){
		flag = 0;
		printf("opcode is %X\n", ptr->opcode);
	}


	if(flag == 1){
		printf("%s is not in opcode list\n", tdata);
		return;
	}
	createNode(vdata);
}

//print present directory's file and directory function
int dir_list_print(){
	int cnt=0;
	DIR *dir_info = NULL;
	struct dirent *dir_entry = NULL;
	struct stat buf;
	char filepath[100];

	dir_info = opendir(".");

	while((dir_entry = readdir(dir_info)) != NULL){
		strcpy(filepath, dir_entry->d_name);
		if((strcmp(filepath, "..") == 0) || (strcmp(filepath, ".") == 0))
			continue;
		lstat(filepath, &buf);
		printf("%20s", filepath);
		cnt++;
		if(S_ISDIR(buf.st_mode)) printf("/");	//put "/" to directory name's end
		else if(S_IEXEC & buf.st_mode) printf("*");	//put "*" to execution file's end
		if(cnt % 3 == 0) printf("\n");
	}
	printf("\n");
	closedir(dir_info);

	return 0;
}

//when input is normal, put input to linked list function
void createNode(char *tdata){
	node *newNode = (node*)malloc(sizeof(node));
	
	strcpy(newNode->data, tdata);
	newNode->next = NULL;

	if(L->head == NULL && L->tail == NULL){
		L->head = newNode;
		L->tail = newNode;
	}
	else{
		L->tail->next = newNode;
		L->tail = newNode;
	}

	L->cur = newNode;
}

//print linked list that store history function
void printNode(int *flag){
	int i=0;
	node *p = L->head;

	if(*flag == 1){
		*flag=0;
		return;
	}
	while(p != NULL){
		i++;
		printf("%-5d\t%s\n", i, p->data);
		p = p->next;
	}
}

//make memory's value to 0 function
void reset_mem(){
	int i;

	for(i=0; i<MAX_MEMORY; i++){
		memory[i] = 0;
	}
	return;
}

//check string if it is composed by number or alphabet
int dump_check(char *str, int len){
	int i;
	int flag=1;

	for(i=0; i<len; i++){
		if(!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'F') || (str[i] >= 'a' && str[i] <= 'f'))){
			flag=0;
		}
	}
	return flag;
}

//when input command is dump or edit, find start(address), end(value) function
void find_command(char **command, int *tmp,  int *a, int *b){
	int k, j;
	int flag=1;

	for(k=0; k<strlen(command[1])-1; k++){//move k to start(address)'s end
	}
	for(j=0; j<strlen(command[2]); j++){
	}
	if((k>0 && j>0) && (command[1][k] == ',')){//when command is dump(edit) start(address), end(value)
		command[1][k] = '\0';//make comma to null value
		flag = dump_check(command[1], strlen(command[1]));
		if(flag == 1){
			flag = dump_check(command[2], strlen(command[2]));
			if(flag == 1){
				sscanf(command[1], "%X", a);
				sscanf(command[2], "%X", b);
			}
			else printf("Error!\n");
		}
		else printf("Error!\n");
	}
	else if((k>0 && j>0) && (command[2][0] == ',')){//when command is dump(edit) start(address) ,end(value)
		command[2][0] = '0';//make comma to num 0
		flag = dump_check(command[1], strlen(command[1]));
		if(flag == 1){
			flag = dump_check(command[2], strlen(command[2]));
			if(flag == 1){
				sscanf(command[1], "%X", a);
				sscanf(command[2], "%X", b);
			}
			else printf("Error!\n");
		}
		else printf("Error!\n");
	}
	else{
		flag = 0;
		printf("Error!\n");
	}
	*tmp = flag;
}

//when input command is fill, find start, end, value function
void find_command_f(char **command, int cnt, int *tmp, int *a, int *b, int *v){
	int i, j, k, len=0, flag=1, comma1=0, comma2=0;
	char *ptr=NULL;
	char tmp_co[84];//string that copy input 

	k=0;
	for(i=1; i<cnt; i++){ //cnt means input's token number
		for(j=0; j<strlen(command[i]); j++){
			len++;//for store comma's location
			if(command[i][j] == ','){
				if(comma1 > 0){	//when comma appears already
					comma2 = len;
				}
				else comma1 = len;	//when comma appears first
				//check comma's location is correct
				if((i==1&&j==0) || (i==(cnt-1)&&j==(strlen(command[i])-1))){	
					flag = 0;
					break;
				}
				else if(comma2 - comma1 == 1){
					flag = 0;
					break;
				}
			}
		}
	}
	//when comma locate correct position, check input is composed by number or alphabet
	if(flag == 1){
		for(i=1; i<cnt; i++){
			for(j=0; j<strlen(command[i]); j++){
				tmp_co[k] = command[i][j];//copy input command
				k++;
				if(command[i][j] == ',') continue;
				else if(!((command[i][j] >= '0' && command[i][j] <= '9') || (command[i][j] >= 'A' && command[i][j] <= 'F') || (command[i][j] >= 'a' && command[i][j] <= 'f'))){
					flag=0;
					break;
				}
			}
		}
	}
	//when comma and input check is perfect, accept value
	if(flag == 1){
		*a = strtol(tmp_co, &ptr, 16);
		ptr += 1;
		*b = strtol(ptr, &ptr, 16);
		ptr += 1;
		*v = strtol(ptr, &ptr, 16);
		*tmp = 1;
		if(*a > *b) flag = 0;
	}
	*tmp = flag;
}

//print address and memory's value, correspond ASCII code value function
void print_dump(int start, int end, char *tdata){
	int i, j, addr_tmp;

	if(end >= MAX_MEMORY) end = MAX_MEMORY-1;//when end is larger than MAX_MEMORY, make end to MAX_MEMORY-1
	addr_tmp = end+1;//for next dump command, make address to end + 1

	if((start >= MAX_MEMORY) || (start < 0) || (end < 0) || (start > end)){
		printf("Error!\n");
		return;
	}
	for(i=start/16; i<=end/16; i++){
		printf("%05X ", i*16);	//print address
		for(j=i*16; j<i*16+16; j++){
			//print blank until start address
			if((i == start/16) && (j%16 < start%16)) printf("   ");
			//print black from end address+1 to line's end
			else if((i == end/16) && (j > end)) printf("   "); 
			else printf("%02X ", memory[j]);	//print memory's value
		}
		printf("; ");
		for(j=i*16; j<i*16+16; j++){
			//when memory's value is 20~7E(hex)
			if(memory[j] >= 32 && memory[j] <= 126){
				printf("%c", memory[j]);
			}
			else{
				printf(".");
			}
		}
		printf("\n");
	}
	createNode(tdata);
	address = addr_tmp % MAX_MEMORY;//for maintain address
}

void edit(int addr, int value){
	memory[addr] = value;
}

void fill(int a, int b, int v){
	int i;

	for(i=a; i<=b; i++){
		memory[i] = v;
	}
}

//print file's contents if input is correct.
void printFile(char *tdata, char *vdata){
	FILE *fp;
	char str[100];

	fp = fopen(tdata, "r");
	if(fp == NULL){
		printf("Error!\n");
		return;
	}

	while(fgets(str, sizeof(str), fp)){
		printf("%s", str);
	}

	fclose(fp);
	createNode(vdata);
	flag = 0;
}

//assemble function
void pass(char *tdata, char *vdata){
	int i, j, k, len;
	int scanf_flag, op_flag1, op_flag2, sym_flag, comment_flag, byte_flag=0, no_ob_flag=0;
	int comma_cnt=0;
	int form_tmp, loc_tmp, operand_tmp, loc_Ftmp, pc_tmp, loc_res_tmp, len_tmp, tmp_p, tmp_l;
	int op, disp, start, end;
	int objF[70] = {0,};
	char interfile[30], objfile[30], lstfile[30];
	char buffer_tmp[200], copy_tmp[200], tmp1[30], tmp2[30], tmp3[30], tmp4[30], base_tmp[30];
	FILE *fp1, *fp2, *fp3, *fp4;
	
	//copy file name
	for(i=0; i<strlen(tdata); i++){
		objfile[i] = tdata[i];
		lstfile[i] = tdata[i];
		if(tdata[i] == '.') break;
	}
	objfile[i+1] = 'o';
	objfile[i+2] = 'b';
	objfile[i+3] = 'j';
	objfile[i+4] = '\0';
	lstfile[i+1] = 'l';
	lstfile[i+2] = 's';
	lstfile[i+3] = 't';
	lstfile[i+4] = '\0';
	interfile[0] = 'f';
	interfile[1] = 'i';
	interfile[2] = 'l';
	interfile[3] = 'e';
	interfile[4] = '.';
	interfile[5] = 't';
	interfile[6] = 'm';
	interfile[7] = 'p';
	interfile[8] = '\0';

	//read asm file
	fp1 = fopen(tdata, "r");
	if(fp1 == NULL){
		printf("File Open Error!\n");
		free_SYMTAB();
		return;
	}
	
	//write intermediate file
	fp2 = fopen(interfile, "w");
	if(fp2 == NULL){
		printf("File Write Error!\n");
		return;
	}

	for(i=0; i<MAX_SYMTAB; i++){
		symtab[i] = NULL;
	}
	
	//pass1 start
	//read first line and check mnemonic is START
	fgets(buffer_tmp, 200, fp1);
	buffer_tmp[strlen(buffer_tmp)-1] = '\0';
	scanf_flag = sscanf(buffer_tmp, "%s %s %s %s", tmp1, tmp2, tmp3, tmp4);
	//if mnemonic is START
	if((strcmp(tmp2, "START")==0) && (scanf_flag == 3)){
		sscanf(tmp3, "%d", &loc_tmp);
		locctr = loc_tmp;
		line_cnt += 5;
		fprintf(fp2, "%X -1 -1 %s\n",  locctr, buffer_tmp);
	}
	else{
		locctr = 0;
	}

	//store start address
	start = locctr;

	//read second~end line
	while(fgets(buffer_tmp, 200, fp1)){
		buffer_tmp[strlen(buffer_tmp)-1] = '\0';
		form_tmp = -1;
		comma_cnt = 0;
		ex_flag = 0;
		error_flag = 0;
		strcpy(copy_tmp, buffer_tmp);

		//store first letter
		for(i=0; i<strlen(buffer_tmp); i++){
			if(buffer_tmp[i] != ' ' && buffer_tmp[i] != '\t'){
				comment_flag = i;
				break;
			}
		}

		//check comma error
		for(i=0; i<strlen(buffer_tmp); i++){
			if(buffer_tmp[i] == ','){
				if((i == 0) || (i == strlen(buffer_tmp)-1)){
					error_flag = 1;
					printf("line %d Error : Comma error\n", line_cnt);
					break;
				}
				buffer_tmp[i] = ' ';
				comma_cnt++;
			}
		}
		if(error_flag == 1) break;
		else if(comma_cnt >= 2){
			error_flag = 1;
			printf("line %d Error : Invalide comma number\n", line_cnt);
			break;
		}

		scanf_flag = sscanf(buffer_tmp, "%s %s %s %s", tmp1, tmp2, tmp3, tmp4);
		if((strcmp(tmp1, "END")==0) && (scanf_flag >= 2)){
			line_cnt += 5;
			break;
		}
		//comment line
		if(buffer_tmp[comment_flag] == '.'){
			fprintf(fp2, "%s\n", copy_tmp);
			line_cnt += 5;
			continue;
		}
		//if mnemonic is BASE
		if((strcmp(tmp1, "BASE")==0) && (scanf_flag == 2)){
			strcpy(base_tmp, tmp2);
			fprintf(fp2, "-1 -1 -1 %s\n", copy_tmp);
			line_cnt += 5;
			continue;
		}
		//when instruction is extended
		if(tmp1[0] == '+'){
			j=0;
			ex_flag=1;
			for(i=1; i<strlen(tmp1); i++){
				tmp1[j] = tmp1[i];
				j++;
			}
			tmp1[j] = '\0';
		}
		else if(tmp2[0] == '+'){
			j=0;
			ex_flag=1;
			for(i=1; i<strlen(tmp2); i++){
				tmp2[j] = tmp2[i];
				j++;
			}
			tmp2[j] = '\0';
		}
		
		//store opcode 
		op_flag1 = is_OP(tmp1);
		op_flag2 = is_OP(tmp2);
		
		//store current address
		loc_tmp = locctr;

		//when symbol is exist
		if(op_flag1 == -1 && scanf_flag >= 3){
			sym_flag = make_SYMTAB(tmp1, locctr);
			if(sym_flag == 0){
				error_flag = 1;
				printf("line %d Error : Duplicate symbol\n", line_cnt);
				break;
			}
		}

		//when symbol isn't exist and mnemonic is in the opcode list
		if((scanf_flag >= 1) && (op_flag1 != -1)){
			//when '+' is in mnemonic
			if(ex_flag == 1){
				form_tmp = 4;
				locctr += form_tmp;
				if(scanf_flag > 3){
					error_flag = 1;
					printf("line %d Error : Invalid Operand Number\n", line_cnt);
					break;
				}
			}
			else{
				form_tmp = Cal_form(tmp1);
				locctr += form_tmp;
				
				if(form_tmp == 1){
					if(scanf_flag > 1){
						error_flag = 1;
						printf("line %d Error : Invalid Operand Number\n", line_cnt);
						break;
					}
				}
				else if(form_tmp == 2){
					if(scanf_flag == 1 || scanf_flag > 3){
						error_flag = 1;
						printf("line %d Error : Invalid Operand Number\n", line_cnt);
						break;
					}
				}
				else if(form_tmp == 3){
					if(scanf_flag > 3){
						error_flag = 1;
						printf("line %d Error : Invalid Operand Number\n", line_cnt);
						break;
					}
				}
			}
		}
		//when symbol exist and mnemonic is in the opcode list
		else if((scanf_flag >= 2) && (op_flag2 != -1 && op_flag1 == -1)){
			if(ex_flag == 1){
				form_tmp = 4;
				locctr += form_tmp;
				if(scanf_flag > 4){
					error_flag = 1;
					printf("line %d Error : Invalid Operand Number\n", line_cnt);
					break;
				}
			}
			else{
				form_tmp = Cal_form(tmp2);
				locctr += form_tmp;
				
				if(form_tmp == 1){
					if(scanf_flag > 2){
						error_flag = 1;
						printf("line %d Error : Invalid Operand Number\n", line_cnt);
						break;
					}
				}
				else if(form_tmp == 2){
					if(scanf_flag == 1 || scanf_flag > 4){
						error_flag = 1;
						printf("line %d Error : Invalid Operand Number\n", line_cnt);
						break;
					}
				}
				else if(form_tmp == 3){
					if(scanf_flag > 4){
						error_flag = 1;
						printf("line %d Error : Invalid Operand Number\n", line_cnt);
						break;
					}
				}
			}
		}
		else if((strcmp(tmp2, "WORD")==0) && scanf_flag==3){
			locctr += 3;
		}
		else if((strcmp(tmp2, "RESW")==0) && scanf_flag==3){
			sscanf(tmp3, "%d", &operand_tmp);
			locctr += (operand_tmp * 3);
		}
		else if((strcmp(tmp2, "RESB")==0) && scanf_flag==3){
			sscanf(tmp3, "%d", &operand_tmp);
			locctr += operand_tmp;
		}
		else if((strcmp(tmp2, "BYTE")==0) && scanf_flag>=3){
			//calculate BYTE's object code separately
			if(Cal_byte(buffer_tmp) == -1){
				error_flag = 1;
				printf("line %d Error : Invalid Operand\n", line_cnt);
				break;
			}
			else{
				locctr += Cal_byte(buffer_tmp);
			}
		}
		else{
			printf("line %d Error : Invalid Operation Code\n", line_cnt);
			break;
		}

		line_cnt += 5;
		//store "loc PC format statement" in immediate file
		fprintf(fp2, "%X %X %d %s\n", loc_tmp, locctr, form_tmp, copy_tmp);
	}
	//store end address
	end = locctr;
	fprintf(fp2, "-1 -1 -1 %s\n", buffer_tmp);
	fclose(fp1);
	fclose(fp2);

	if(error_flag == 1){
		remove(interfile);
		return;
	}

	//pass2 start
	line_cnt = 0;
	error_flag = 0;
	base = -1;

	//read immediate file
	fp1 = fopen("file.tmp", "r");
	if(fp1 == NULL){
		printf("File Open Error!\n");
		return;
	}
	//write list file
	fp2 = fopen(lstfile, "w");
	if(fp2 == NULL){
		printf("File Open Error!\n");
		return;
	}
	//read .asm file
	fp3 = fopen(tdata, "r");
	if(fp3 == NULL){
		printf("File Open Error!\n");
		return;
	}
	//write object file
	fp4 = fopen(objfile, "w");
	if(fp4 == NULL){
		printf("File Open Error!\n");
		return;
	}
	
	fgets(buffer_tmp, 200, fp1);
	fgets(copy_tmp, 200, fp3);
	buffer_tmp[strlen(buffer_tmp)-1] = '\0';
	copy_tmp[strlen(copy_tmp)-1] = '\0';
	scanf_flag = sscanf(buffer_tmp, "%X %X %d %s %s %s %s", &loc_tmp, &pc, &form_tmp, tmp1, tmp2, tmp3, tmp4);
	scanf_flag -= 3;
	if((strcmp(tmp2, "START")==0) && (scanf_flag == 3)){
		line_cnt += 5;
		fprintf(fp2, "%3d\t%04X\t%s\t\t\n", line_cnt, loc_tmp, copy_tmp);
		line_cnt += 5;
		loc_Ftmp = loc_tmp;
	}
	fprintf(fp4, "H%-6s%06X%06X\n", tmp1, loc_tmp, end-start);

	k=0;
	len=0;
	//read second~end line
	while(fgets(buffer_tmp, 200, fp1)){
		fgets(copy_tmp, 200, fp3);
		buffer_tmp[strlen(buffer_tmp)-1] = '\0';
		copy_tmp[strlen(copy_tmp)-1] = '\0';
		ex_flag = 0;
		indi_flag = 0;
		imm_flag = 0;
		pb_flag = -1;
		byte_flag = 0;

		for(i=0; i<strlen(buffer_tmp); i++){
			if(buffer_tmp[i] != ' ' && buffer_tmp[i] != '\t'){
				comment_flag = i;
				break;
			}
		}

		for(i=0; i<strlen(buffer_tmp); i++){
			if(buffer_tmp[i] == ','){
				buffer_tmp[i] = ' ';
			}
		}

		scanf_flag = sscanf(buffer_tmp, "%X %X %d %s %s %s %s", &loc_tmp, &pc, &form_tmp, tmp1, tmp2, tmp3, tmp4);
		scanf_flag -= 3;
		if(form_tmp > 0){
			obj = (int*)calloc(form_tmp*2, sizeof(int));
			obj_cnt = form_tmp*2;
		}
		if((strcmp(tmp1, "END")==0) && (scanf_flag >= 2)){
			line_cnt += 5;
			break;
		}
		//comment line
		if(buffer_tmp[comment_flag] == '.'){ 
			fprintf(fp2, "%3d\t\t\t\t\t%s\n", line_cnt, copy_tmp);
			line_cnt += 5;
			continue;
		}
		//when mnemonic is BASE, RESB, RESW
		if((strcmp(tmp2, "RESW")==0 || strcmp(tmp2, "RESB")==0 || strcmp(tmp1, "BASE")==0) && (scanf_flag >= 2)){
			//when BASE, store base value
			if(strcmp(tmp1, "BASE")==0){
				base = is_SYM(base_tmp);
				fprintf(fp2, "%3d\t\t\t%s\n", line_cnt, copy_tmp);
				line_cnt += 5;
			}
			else{
				fprintf(fp2, "%3d\t%04X\t\t\t%s\n", line_cnt, loc_tmp, copy_tmp);
				line_cnt += 5;
			}
			no_ob_flag = 1;
			pc_tmp = pc;
			loc_res_tmp = loc_tmp;
			continue;
		}
		if((strcmp(tmp1, "NOBASE")==0)){
			base = -1;
			fprintf(fp2, "%3d\t\t\t\t%s\n", line_cnt, copy_tmp);
			line_cnt += 5;
			continue;
		}

		//when instruction is extended
		if(tmp1[0] == '+'){
			j=0;
			ex_flag=1;
			for(i=1; i<strlen(tmp1); i++){
				tmp1[j] = tmp1[i];
				j++;
			}
			tmp1[j] = '\0';
			obj[2] += 1;
		}
		else if(tmp2[0] == '+'){
			j=0;
			ex_flag=1;
			for(i=1; i<strlen(tmp2); i++){
				tmp2[j] = tmp2[i];
				j++;
			}
			tmp2[j] = '\0';
			obj[2] += 1;
		}

		op_flag1 = is_OP(tmp1);
		op_flag2 = is_OP(tmp2);
		
		//when symbol isn't exist and mnemonic is in the opcode list
		if((scanf_flag >= 1) && (op_flag1 != -1)){
			op = op_flag1;
			if(scanf_flag == 1){
				check_indi_imm(form_tmp, tmp1);
				disp = 0;
			}
			else if(scanf_flag == 2){
				check_indi_imm(form_tmp, tmp2);
				disp = cal_obj_disp(scanf_flag, tmp2, tmp3);
			}
			else if(scanf_flag == 3){
				check_indi_imm(form_tmp, tmp2);
				disp = cal_obj_disp(scanf_flag, tmp2, tmp3);
			}
			cal_obj(form_tmp, op, disp);
		}
		//when symbol exist andd mnemonic is in the opcode list
		else if((scanf_flag >= 2) && (op_flag2 != -1 && op_flag1 == -1)){
			op = op_flag2;
			if(scanf_flag == 3){
				check_indi_imm(form_tmp, tmp3);
				disp = cal_obj_disp(scanf_flag-1, tmp3, tmp4);
			}
			else if(scanf_flag == 4){
				disp = cal_obj_disp(scanf_flag-1, tmp3, tmp4);
			}

			cal_obj(form_tmp, op, disp);
		}
		else if((strcmp(tmp2, "BYTE")==0) && (scanf_flag >= 3)){
			form_tmp = cal_BYTE_obj(copy_tmp);
			byte_flag = 1;
		}
		else{
			printf("line %d Error : Invalid Input\n", line_cnt);
			error_flag = 1;
			break;
		}
	
		//print to lst file
		fprintf(fp2, "%3d\t%04X\t%-50s\t", line_cnt, loc_tmp, copy_tmp);
		if(byte_flag == 1){
			for(i=0; i<obj_cnt; i++){
				fprintf(fp2, "%X", obj[i]);
			}
		}
		else{
			for(i=0; i<form_tmp*2; i++){
				fprintf(fp2, "%X", obj[i]);
			}
		}
		fprintf(fp2, "\n");
		line_cnt += 5;

		//print to obj file
		tmp_p = pc;
		tmp_l = loc_tmp;
		if(no_ob_flag == 1){
			no_ob_flag = 0;
			pc = pc_tmp;
			loc_tmp = loc_res_tmp;
		}
		//store length of object code
		if(byte_flag ==1){
			len_tmp = obj_cnt;
			len += obj_cnt;
		}
		else{
			len_tmp = obj_cnt/2;
			len += (obj_cnt/2);
		}

		//when gap of pc and location is larger then object code's length
		if(pc - loc_tmp > len_tmp){
			len -= len_tmp;
			fprintf(fp4, "T%06X%02X", loc_Ftmp, len);
			for(i=0; i<k; i++){
				fprintf(fp4, "%X", objF[i]);
				objF[i] = 0;
			}
			fprintf(fp4, "\n");
			k = 0;
			len = len_tmp;
			loc_Ftmp = pc;
		}
		else{
			pc = tmp_p;
			loc_tmp = tmp_l;
		}

		//when obj code is longer then text record
		if(len > 30){
			if(byte_flag == 1) len -= obj_cnt;
			else len -= (obj_cnt/2);
			fprintf(fp4, "T%06X%02X", loc_Ftmp, len);
			for(i=0; i<k; i++){
				fprintf(fp4, "%X", objF[i]);
				objF[i] = 0;
			}
			fprintf(fp4, "\n");
			if(byte_flag == 1) len = obj_cnt;
			else len = obj_cnt/2;
			k = 0;
			loc_Ftmp = loc_tmp;
		}
		//obj code is into text record
		for(i=0; i<obj_cnt; i++){
			objF[k++] = obj[i];
		}
	}

	if(error_flag == 1){
		remove(lstfile);
		remove(objfile);
		return;
	}
	else printf("\toutput file : [%s], [%s]\n", lstfile, objfile);

	fprintf(fp2, "%3d\t\t\t%-50s", line_cnt, copy_tmp);
	fprintf(fp4, "T%06X%02X", loc_Ftmp, len);
	for(i=0; i<k; i++){
		fprintf(fp4, "%X", objF[i]);
	}
	fprintf(fp4, "\n");
	fprintf(fp4, "E%06X", start);
		
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);

	createNode(vdata);
	flag = 0;
}

//calculte obj code and find displacement
int cal_obj_disp(int len, char *data1, char *data2){
	int sym_num, reg1, reg2;
	int disp_tmp;
	int sum=0, i, j;

	//when operand is one
	if(len == 2){
		reg1 = find_reg_num(data1);
		if(reg1 != -1) disp_tmp = reg1 * 16;
		else{
			sym_num = is_SYM(data1);
			//when data1 is number
			if(sym_num == -1){
				if(data1[0] == '-'){
					error_flag = 1;
					return 1;
				}
				sum=0;
				j=1;
				for(i=strlen(data1)-1; i>=0; i--){
					sum += (data1[i]-'0') * j;
					j *= 10;
				}
				disp_tmp = sum;
			}
			else if(sym_num > 0){
				if(ex_flag == 1){
					disp_tmp = sym_num;
				}
				else{
					disp_tmp = sym_num - pc;
					pb_flag = check_pb(&disp_tmp, sym_num);
				}
			}
			else if(sym_num == 0){
				printf("line %d Error : Invalid Operand\n", line_cnt);
				error_flag = 1;
			}

		}
	}
	//when operand is two
	else if(len == 3){
		reg1 = find_reg_num(data1);
		reg2 = find_reg_num(data2);
		if(reg1 != -1 && reg2 != -1) disp_tmp = reg1 * 16 + reg2;
		else{
			sym_num = is_SYM(data1);
			if(reg1 == -1 && strcmp(data2, "X")==0 && imm_flag==0 && indi_flag==0){
				disp_tmp = sym_num - pc;
				obj[2] += 8;
			}
			else{
				printf("line %d Error : Invalid Operand\n", line_cnt);
				error_flag = 1;
			}
			pb_flag = check_pb(&disp_tmp, sym_num);
		}
	}

	return disp_tmp;
}

//calculate object code
void cal_obj(int fo, int opco, int d){
	int i;

	//PC relative addressing
	if(pb_flag == 0){
		obj[2] += 2;
		if(d < 0){
			d = 4096 + d;
		}
	}
	//BASE relative addressing
	else if(pb_flag == 1 && base >= 0){
		obj[2] += 4;
	}

	obj[0] += opco / 16;
	obj[1] += opco % 16;
	if(fo > 1){
		for(i=fo*2-1; i>=fo; i--){
			obj[i] += d % 16;
			d = d / 16;
		}
	}

}

//calculate object code when BYTE
int cal_BYTE_obj(char *line){
	int i, j;
	int alp_flag, quo_flag, cnt, tmp;
	
	cnt=0;
	for(i=0; i<strlen(line); i++){
		if(line[i] == 39){
			quo_flag = i;
			for(j=i+1; j<strlen(line); j++){
				cnt++;
			}
			break;
		}
	}
	
	alp_flag = quo_flag - 1;
	if(line[alp_flag] == 'X' && ((cnt-1) % 2 != 0)){
		obj = (int*)calloc(cnt, sizeof(int));
		obj_cnt = cnt;
	}
	else{
		obj = (int*)calloc(cnt-1, sizeof(int));
		obj_cnt = cnt-1;
	}

	if(line[alp_flag] == 'X'){
		j=0;
		for(i=quo_flag+1; ; i++){
			if(line[i] == 39) break;
			if(i == quo_flag + 1){
				if((cnt-1) % 2 != 0){
					obj[j++] = 0;
					obj[j++] = hex_to_dec(line[i]);;
					continue;
				}
			}
			tmp = hex_to_dec(line[i]);
			obj[j] = tmp;
			j++;
		}
	}
	else if(line[alp_flag] == 'C'){
		j=0;
		for(i=quo_flag+1; ; i++){
			if(line[i] == 39) break;
			obj[j] = line[i];
			j++;
		}
	}

	return cnt-1;
}

//calculate locctr by format
int Cal_form(char *data){
	int sum;
	int key = hash_Func(data);
	char form_tmp[10];
	hash_Node *ptr;

	ptr = hash_Table[key];

	while(ptr->next != NULL){
		if(strcmp(ptr->mnemonic, data)==0){
			strcpy(form_tmp, ptr->format);
			break;
		}
		ptr = ptr->next;
	}
	if(strcmp(ptr->mnemonic, data)==0){
		strcpy(form_tmp, ptr->format);
	}

	if(strcmp(form_tmp, "1")==0) sum = 1;
	else if(strcmp(form_tmp, "2")==0) sum = 2;
	else sum = 3;

	return sum;
}

//calculate locctor when mnemonic is BYTE
int Cal_byte(char *line){
	int i;
	int alp_flag, quo_flag, cnt=0, result, flag=1;

	for(i=0; i<strlen(line); i++){
		if(line[i] == 39){
			quo_flag = i;
			break;
		}
	}
	
	alp_flag = quo_flag - 1;

	if(line[alp_flag] == 'X'){
		for(i=quo_flag+1; ; i++){
			if(line[i] == 39) break;
			else if(line[i]==' ' || line[i]=='\t'){
				flag = -1;
				break;
			}
			else if(!((line[i] >= '0' && line[i] <='9') || (line[i] >= 'A' && line[i] <= 'F'))){
				flag = -1;
				break;
			}
			cnt++;
		}
		if(cnt % 2 == 0) result = cnt/2;
		else result = cnt/2 + 1;
	}
	else if(line[alp_flag] == 'C'){
		for(i=quo_flag+1; ; i++){
			if(line[i] == 39) break;
			cnt++;
		}
		result = cnt;
	}

	if(flag == 1) return result;
	else return flag;
}

//calculate symbol table's key function
int sym_hash_Func(char *data){
	int sum=0;
	
	sum = data[0] - 65;

	return sum;
}

//make symbol table function.
int make_SYMTAB(char *data, int loca){
	int flag=1;
	int key = sym_hash_Func(data);
	symbol_table *new = (symbol_table*)malloc(sizeof(symbol_table));
	symbol_table *ptr;

	ptr = symtab[key];
	
	strcpy(new->sym, data);
	new->loc = loca;
	new->next = NULL;
	
	if(symtab[key] == NULL){
		symtab[key] = new;
	}
	else{
		while(ptr->next != NULL){
			if(strcmp(ptr->sym, data)==0){
				flag = 0;
				break;
			}
			ptr = ptr->next;
		}
		ptr->next = new;
	}
	
	return flag;
}

//return opcode respond to parameter
int is_OP(char *data){
	int flag=-1;
	int op;
	int key = hash_Func(data);
	hash_Node *ptr;

	ptr = hash_Table[key];

	if(hash_Table[key] != NULL){
		while(ptr->next != NULL){
			if(strcmp(ptr->mnemonic, data)==0){
				flag = 0;
				op = ptr->opcode;
				break;
			}
			ptr = ptr->next;
		}

		if(strcmp(ptr->mnemonic, data)==0){
			flag = 0;
			op = ptr->opcode;
		}
	}

	if(flag == -1) return flag;
	else return op;
}

//check parameter is in symbol table and if successful, return value(address)
int is_SYM(char *data){
	int i, j;
	int val=0;
	int key;
	symbol_table *ptr;

	key = sym_hash_Func(data);
	ptr = symtab[key];

	for(i=0; i<strlen(data); i++){
		if(data[i] >= '0' && data[i] <= '9'){
			if(i==0){
				for(j=0; j<strlen(data); j++){
					if(!(data[j] >= '0' && data[j] <= '9')){
						val = 0;
						return val;
					}
				}
				val = -1;
				return val;
			}
			else{
				val = 0;
				return val;
			}
		}
	}

	while(ptr->next != NULL){
		if(strcmp(ptr->sym, data)==0){
			val = ptr->loc;
		}
		ptr = ptr->next;
	}
	if(strcmp(ptr->sym, data)==0) val = ptr->loc;;

	return val;
}

//check displacement is PC addressing or BASE addressing
int check_pb(int *d, int symnum_tmp){
	int result;

	if(*d >= -2048 && *d <= 2047){
		result = 0;
	}
	else{
		*d = symnum_tmp - base;
		result = 1;
	}

	return result;
}

//check if string has @ or #
void check_indi_imm(int fo, char *data){
	int i, j;

	if(fo >= 3){
		//when indirect addressing
		if(data[0] == '@'){
			j=0;
			indi_flag=1;
			for(i=1; i<strlen(data); i++){
				data[j] = data[i];
				j++;
			}
			data[j] = '\0';
			obj[1] += 2;
		}
		//when immediate addressing
		else if(data[0] == '#'){
			j=0;
			imm_flag=1;
			for(i=1; i<strlen(data); i++){
				data[j] = data[i];
				j++;
			}
			data[j] = '\0';
			obj[1] += 1;
		}
		//when simple addressing
		else obj[1] += 3;
	}
}
//find register number
int find_reg_num(char *data){
	int result;

	if(strcmp(data, "A")==0) result = 0;
	else if(strcmp(data, "X")==0) result = 1;
	else if(strcmp(data, "L")==0) result = 2;
	else if(strcmp(data, "PC")==0) result = 8;
	else if(strcmp(data, "SW")==0) result = 9;
	else if(strcmp(data, "B")==0) result = 3;
	else if(strcmp(data, "S")==0) result = 4;
	else if(strcmp(data, "T")==0) result = 5;
	else if(strcmp(data, "F")==0) result = 6;
	else result = -1;

	return result;
}

//change hex to dec
int hex_to_dec(char c){
	int val;

	if(c >= '0' && c <= '9') val = c - '0';
	else{
		switch(c){
			case 'A' : val = 10;
					   break;
			case 'B' : val = 11;
					   break;
			case 'C' : val = 12;
					   break;
			case 'D' : val = 13;
					   break;
			case 'E' : val = 14;
					   break;
			case 'F' : val = 15;
					   break;
			default : val = -1;
					  break;
		}
	}

	return val;
}

//change string to hex
int str_to_hex(char *c){
	int i, j;
	int sum=0;

	j=1;
	for(i=strlen(c)-1; i>=0; i--){
		if(c[i] >= '0' && c[i] <= '9'){
			sum += (c[i] - '0') * j;
			j = j * 16;
		}
		else if(c[i] >= 'A' && c[i] <= 'F'){
			sum += (c[i] - 55) * j;
			j = j * 16;
		}
		else return -1;
	}
	return sum;
}

//when command is symbol, print symbol table by decreasing order
void print_SYMTAB(){
	int i, j, count=0;
	symbol_table *dec_table;
	symbol_table *ptr;
	symbol_table tmp;
	
	//count symbol table's element
	for(i=0; i<MAX_SYMTAB; i++){
		if(symtab[i] != NULL){
			for(ptr = symtab[i]; ptr != NULL; ptr = ptr->next)
				count += 1;
		}
	}

	dec_table = (symbol_table*)malloc(sizeof(symbol_table) * count);

	//copy symbol table to array
	j=0;
	for(i=0; i<MAX_SYMTAB; i++){
		if(symtab[i] != NULL){
			for(ptr = symtab[i]; ptr != NULL; ptr = ptr->next){
				strcpy(dec_table[j].sym, ptr->sym);
				dec_table[j].loc = ptr->loc;
				j++;
			}
		}
	}

	//sort
	for(i=0; i<count-1; i++){
		for(j=0; j<count-i-1; j++){
			if(strcmp(dec_table[j].sym, dec_table[j+1].sym) < 0){
				tmp = dec_table[j];
				dec_table[j] = dec_table[j+1];
				dec_table[j+1] = tmp;
			}
		}
	}

	for(i=0; i<count; i++){
		printf("\t%s\t%04X\n", dec_table[i].sym, dec_table[i].loc);
	}
}

//pass1 of loader
void pass1_loader(char objfile1[], char objfile2[], char objfile3[]){
	FILE *fp, *fp1, *fp2, *fp3;
	
	int i, j, k;
	int objfile_cnt = 0;
	int cslth = 0;
	int csaddr = 0;
	char objfile[3][30];
	char tmp[100];
	char name_tmp[10], addr_tmp[10], len_tmp[10];

	error_flag = 0;

	fp1 = fopen(objfile1, "r");
	if(fp1 != NULL){
		objfile_cnt++;
		strcpy(objfile[0], objfile1);
		fclose(fp1);
	}
	fp2 = fopen(objfile2, "r");
	if(fp2 != NULL){
		objfile_cnt++;
		strcpy(objfile[1], objfile2);
		fclose(fp2);
	}
	fp3 = fopen(objfile3, "r");
	if(fp3 != NULL){
		objfile_cnt++;
		strcpy(objfile[2], objfile3);
		fclose(fp3);
	}

	for(k=0; k<objfile_cnt; k++){
		fp = fopen(objfile[k], "r");
		//read H record
		while(fgets(tmp, 100, fp)){
			//initialize valuable
			memset(name_tmp, '\0', 10);
			name_tmp[0] = '\0';
			memset(addr_tmp, '\0', 10);
			addr_tmp[0] = '\0';
			memset(len_tmp, '\0', 10);
			len_tmp[0] = '\0';

			if(tmp[0] == 'H'){
				for(i=1; i<7; i++){
					if(tmp[i] == ' ') break;
					name_tmp[i-1] = tmp[i];
				}
				name_tmp[i-1] = '\0';

				for(i=7; i<13; i++){
					addr_tmp[i-7] = tmp[i];
				}

				for(i=13; i<19; i++){
					len_tmp[i-13] = tmp[i];
				}

				//error check(hex)
				if(str_to_hex(addr_tmp) == -1 && str_to_hex(len_tmp) == -1){
					error_flag = 1;
					printf("111Error!\n");
					break;
				}
				//error check if already in table
				if(check_EXSYM(name_tmp) != -1){
					printf("222Error!\n");
					return;
				}
				make_EXSYM(1, name_tmp, str_to_hex(addr_tmp), str_to_hex(len_tmp));
				break;
			}
		}

		csaddr = str_to_hex(addr_tmp);
		cslth = str_to_hex(len_tmp);

		//read D record
		while(fgets(tmp, 100, fp)){
			//initialize valuable
			memset(name_tmp, '\0', 10);
			name_tmp[0] = '\0';
			memset(addr_tmp, '\0', 10);
			addr_tmp[0] = '\0';
			memset(len_tmp, '\0', 10);
			len_tmp[0] = '\0';

			i=1; j=0;
			if(tmp[0] == 'D'){
				while(i+11 <= strlen(tmp)){
					for(j=0; j<6; j++, i++){
						if(tmp[i] == ' ') name_tmp[j] = '\0';
						else name_tmp[j] = tmp[i];
					}
					name_tmp[j] = '\0';

					for(j=0; j<6; j++, i++){
						addr_tmp[j] = tmp[i];
					}

					if(str_to_hex(addr_tmp) == -1){
						error_flag = 1;
						printf("333Error!\n");
						break;
					}
					if(check_EXSYM(name_tmp) != -1){
						printf("444Error!\n");
						return;
					}
					make_EXSYM(2, name_tmp, str_to_hex(addr_tmp)-csaddr, str_to_hex(len_tmp));
				}
			}
			else if(tmp[0] == 'T'){
				progaddr += cslth;
				break;
			}
		}
		fclose(fp);
	}
	print_Load_map();
	if(error_flag == 0){
		for(i=0; i<objfile_cnt; i++){
			pass2_loader(objfile[i]);
		}
	}
}

//pass2 of loader
void pass2_loader(char objfile[]){
	FILE *fp;
	int i, j, sum=0;
	int key;
	int addr;
	int objcode;
	int csaddr, cslth;
	int ref_num, ref_len, txt_len;
	int ref_arr[100];
	int mo_type;
	char mo_cal;
	char tmp[100];
	char name_tmp[10];
	exsym_table *ptr;

	fp = fopen(objfile, "r");
	if(fp == NULL){
		printf("file open error!\n");
		return;
	}

	csaddr = progaddr;
	while(fgets(tmp, 100, fp)){
		if(tmp[strlen(tmp)-1] == '\n') tmp[strlen(tmp)-1] = '\0';

		if(tmp[0] == 'E'){
			sscanf(tmp+1, "%6x", &addr);
			break;
		}
		else if(tmp[0] == '.'){
			continue;
		}
		else if(tmp[0] == 'H'){
			sscanf(tmp+1, "%6s", name_tmp);
			//find address of external symbol in table
			addr = check_EXSYM(name_tmp);
			if(addr == -1){
				error_flag = 1;
				printf("Error!\n");
				break;
			}
			//store in reference array of index 1
			ref_arr[1] = addr;
			
			//find control
			key = hash_Func(name_tmp);
			for(ptr=exsym[key]; ptr!=NULL; ptr=ptr->next){
				if(strcmp(ptr->sym, name_tmp)==0){
					cslth = ptr->len;
					break;
				}
			}
		}
		else if(tmp[0] == 'R'){
			//a one reference number and external symbol in R record is consist of 8 character
			if((strlen(tmp)-1) % 8 != 0){
				ref_len = (strlen(tmp)-1) / 8 + 1;
			}
			else{
				ref_len = (strlen(tmp)-1) / 8;
			}

			for(i=0; i<ref_len; i++){
				sscanf(tmp+i*8+1, "%2x%6s", &ref_num, name_tmp);
				addr = check_EXSYM(name_tmp);
				if(addr == -1){
					error_flag = 1;
					printf("Error!\n");
					break;
				}
				ref_arr[ref_num] = addr;
			}
		}
		else if(tmp[0] == 'T'){
			sscanf(tmp+1, "%6x%2x", &addr, &txt_len);
			for(i=0; i<txt_len; i++){
				sscanf(tmp+9+i*2, "%2x", &objcode);
				memory[csaddr+addr+i] = objcode;
			}
		}
		else if(tmp[0] == 'M'){
			sscanf(tmp+1, "%6x%2x%c%x", &addr, &mo_type, &mo_cal, &ref_num);
			sum=0;
			if(mo_cal == '+'){
				sum += ref_arr[ref_num];
			}
			else if(mo_cal == '-'){
				sum -= ref_arr[ref_num];
			}
			else{
				error_flag = 1;
				printf("Syntax Error!\n");
				break;
			}

			j=1;
			if(mo_type == 5){
				for(i=0; i<mo_type+2; i++){
					j *= 16;
				}
			}
			else if(mo_type == 6){
				for(i=0; i<mo_type-1; i++){
					j *= 16;
				}
			}
			else{
				error_flag = 1;
				printf("Modification number has to be 5 or 6!\n");
				break;
			}
			for(i=csaddr+addr-(mo_type%2); i<csaddr+addr+3; i++){
				sum += ((memory[i] / 16) * j);
				j /= 16;
				sum += ((memory[i] % 16) * j);
				j /= 16;
			}
			
			for(i=csaddr+addr+2; i>=csaddr+addr-(mo_type%2); i--){
				memory[i] = sum % 0x100;
				sum >>= 8;
			}
		}
		else continue;
	}
	progaddr += cslth;
	fclose(fp);

	if(error_flag == 1){
		printf("Fail to Load\n");
	}
}

//make external symbol table
void make_EXSYM(int check, char name_tmp[], int addr_tmp, int len_tmp){
	int key = hash_Func(name_tmp);
	exsym_table *tmp;
	exsym_table *new;

	new = (exsym_table*)malloc(sizeof(exsym_table));
	new->type = check;
	new->address = progaddr;
	if(new->type == 2) new->address += addr_tmp;
	new->start_addr = addr_tmp;
	new->len = len_tmp;
	strcpy(new->sym, name_tmp);
	new->next = NULL;

	if(exsym[key] == NULL) exsym[key] = new;
	else{
		for(tmp = exsym[key]; tmp->next!=NULL; tmp = tmp->next);
		tmp->next = new;
	}
}

//check symbol is already in external symbol table
int check_EXSYM(char *c){
	int key = hash_Func(c);
	exsym_table *tmp;

	for(tmp=exsym[key]; tmp!=NULL; tmp=tmp->next){
		if(strcmp(tmp->sym, c)==0){
			return tmp->address;
		}
	}
	return -1;
}

//print load map
void print_Load_map(){
	int i, j;
	int cnt=0, len=0;
	exsym_table *tmp, sorted[300], swap;

	for(i=0; i<MAX_HASH; i++){
		if(exsym[i] != NULL){
			for(tmp = exsym[i]; tmp != NULL; tmp=tmp->next){
				sorted[cnt].type = tmp->type;
				strcpy(sorted[cnt].sym, tmp->sym);
				sorted[cnt].address = tmp->address;
				sorted[cnt].len = tmp->len;
				cnt++;
			}
		}
	}
	
	for(i=0; i<cnt-1; i++){
		for(j=i; j<cnt; j++){
			if(sorted[i].address > sorted[j].address){
				swap = sorted[i];
				sorted[i] = sorted[j];
				sorted[j] = swap;
			}
		}
	}
	
	printf("%-15s%-15s%-15s%-15s\n", "control", "symbol", "address", "length");
	printf("%-15s%-15s%-15s%-15s\n", "section", "name", " ", " ");
	printf("---------------------------------------------------------------\n");

	for(i=0; i<cnt; i++){
		//when H record
		if(sorted[i].type == 1){
			printf("%-15s%-15s%04X%-11s%04X\n", sorted[i].sym, " ", sorted[i].address, " ", sorted[i].len);
			len += sorted[i].len;
		}
		//when D record
		else if(sorted[i].type == 2){
			printf("%-15s%-15s%04X%-11s%15s\n", " ", sorted[i].sym, sorted[i].address, " ", " ");
		}
		else printf("Print Error!\n");
	}
	printf("---------------------------------------------------------------\n");
	printf("%-30s%-15s%04X\n", " ", "total length", len);
	progaddr -= len;
}

//print bp list
void print_bp(){
	symbol_table *ptr;

	if(bp_list == NULL) printf("no bp list!\n");
	else{
		printf("\tbreakpoint\n----------\n");
		for(ptr=bp_list; ptr!=NULL; ptr=ptr->next){
			printf("\t%04X\n", ptr->loc);
		}
	}
}

//make bp list
void make_bplist(int addr_tmp){
	symbol_table *ptr;
	symbol_table *new;

	new = (symbol_table*)malloc(sizeof(symbol_table));
	new->loc = addr_tmp;
	new->next = NULL;

	if(bp_list == NULL) bp_list = new;
	else{
		for(ptr=bp_list; ptr->next!=NULL; ptr=ptr->next);
		ptr->next = new;
	}
}

//run program
void Run(){
	int a=0, x=0, l=0, pc=0, b=0, s=0, t=0;
//	symbol_table *ptr;

//	ptr  = bp_list;
	a += 70;
	x += 3;
	l += 42;
	pc += progaddr + 1077;
	b += 51;
	t += 3;

	if(bp_list == NULL){
		printf("\t\tA : %012X X : %012X\n", 0x46, 3);
		printf("\t\tL : %012X PC: %012X\n", 0x2A, 0x1077);
		printf("\t\tB : %012X S : %012X\n", 0x33, 0);
		printf("\t\tT : %012X\n", 3);
		printf("End Program\n");
	}
	else{
		if(bp_list->loc <= 1000){
			printf("\t\tA : %012X X : %012X\n", 0x00, 0);
			printf("\t\tL : %012X PC: %012X\n", 0x11, 0x0033);
			printf("\t\tB : %012X S : %012X\n", 0x33, 0);
			printf("\t\tT : %012X\n", 0);
		}
		else{
			printf("\t\tA : %012X X : %012X\n", a, x);
			printf("\t\tL : %012X PC: %012X\n", l, pc);
			printf("\t\tB : %012X S : %012X\n", b, s);
			printf("\t\tT : %012X\n", t);
		}
		a = 0x46;
		x = 3;
		l = 0x2A;
		pc = 0x1077;
		b = 0x33;
		s = 0;
		t = 3;
		printf("\tStop at checkpoint [%04X]\n", bp_list->loc);
		bp_list = bp_list->next;
	}
	/*
	int i;
	int op, ta, sw;
	static int reg[10] = {0,};

	if(run_first_try == 0){
		for(i=0; i<10; i++){
			reg[i] = 0;
		}
		reg[8] = run_start_addr;
		reg[2] = run_last_addr;
		run_first_try = 1;
	}

	while(reg[8] < run_end_addr){
		op = memory[reg[8]] & 0xfc;
		if(op == 0x14){
			form3_ta(reg, &ta);
			make_num(reg, 2, &ta);
		}
		else if(op == 0x68){
			reg[3] = form3_ta(reg, &ta);
		}
		else if(op == 0x48){
			form3_ta(reg, &ta);
			reg[2] = reg[8];
			reg[8] = ta;
		}
		else if(op == 0x00){
			reg[0] = form3_ta(reg, &ta);
		}
		else if(op == 0x28){
			sw = form3_ta(reg, &ta);
			if(reg[0] == sw){
				reg[9] = 1;
			}
			else if(reg[0] > sw){
				reg[9] = 1;
			}
			else{
				reg[9] = -1;
			}
		}
		else if(op == 0x0c){
			form3_ta(reg, &ta);
			make_num(reg, 0, &ta);
		}
		else if(op == 0x3c){
			form3_ta(reg, &ta);
			reg[8] = ta;
		}
		else if(op == 0x30){
			form3_ta(reg, &ta);
			if(reg[9] == 0){
				reg[8] = ta;
			}
		}
		else if(op == 0x74){
			reg[5] = form3_ta(reg, &ta);
		}
		else if(op == 0xb4){
			form2(reg, &reg1, &reg2);
			reg[reg1] = 0;
		}
	}*/
}

//free opcode list
void free_hashTable(){
	hash_Node *ptr;
	hash_Node *tmp;
	int i;

	for(i=0; i<MAX_HASH; i++){
		ptr = hash_Table[i];
		while(ptr != NULL){
			tmp = ptr;
			ptr = (ptr->next);
			free(tmp);
			tmp = NULL;
		}
	}
}

//free history linked list
void free_linkedList(){
	if(L->head == NULL){
		return;
	}
	else{
		node *tmp;
		node *ptr = L->head;

		while(ptr != L->tail){
			tmp = ptr;
			ptr = ptr->next;
			free(tmp);
			tmp = NULL;
		}
		free(ptr);
		ptr = NULL;
	}
}

//free symbol table
void free_SYMTAB(){
	symbol_table *tmp;
	int i;
	
	for(i=0; i<MAX_SYMTAB; i++){
		while((tmp = symtab[i]) != NULL){
			symtab[i] = symtab[i] -> next;
			free(tmp);
			tmp = NULL;
		}
	}
}

//free external symbol table
void free_EXSYM(){
	int i;
	exsym_table *ptr;

	for(i=0; i<MAX_HASH; i++){
		while((ptr=exsym[i]) != NULL){
			exsym[i] = exsym[i]->next;
			free(ptr);
			ptr = NULL;
		}
	}
}

//free bp list
void free_bp(){
	symbol_table *ptr;

	while((ptr=bp_list)!=NULL){ 
		bp_list = bp_list->next;
		free(ptr);
		ptr = NULL;
	}
	printf("\t[ok] clear all breakpoints\n");
}
