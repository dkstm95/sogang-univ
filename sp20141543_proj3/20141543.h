#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

//structure for linked list in history command
typedef struct _node{
	char data[84];
	struct _node *next;
}node;

typedef struct _list{
	struct _node *cur;
	struct _node *head;
	struct _node *tail;
}linkedList;

//structure for hash in opcode command
typedef struct _hash_node{
	int opcode;
	char mnemonic[10];
	char format[5];
	struct _hash_node *next;
}hash_Node;

//structure for symbol table
typedef struct _symbol{
	int loc;
	char sym[30];
	struct _symbol *next;
}symbol_table;

//structure for extenal symbol table
typedef struct _exsym{
	int type;
	int address;
	int len;
	int start_addr;
	char sym[30];
	struct _exsym *next;
}exsym_table;

//read opcode list
int Read_Op();

//make Hash Table
void createHashTable(int op, char *mne, char *form);

//print hash table list
void printHashTable();

//print hash table's data
void printHashNode(char *tdata, char *vdata);

//calculate hash key function
int hash_Func(char *mne);

//for directory print
int dir_list_print();

//for create node in linked list
void createNode(char *tdata);

//for print node in linked list
void printNode(int *flag);

//reset memory
void reset_mem();

//check input boundary
int dump_check(char *str, int len);

//find dump, edit's command
void find_command(char **command, int *tmp, int *a, int *b);

//find fill's command
void find_command_f(char **command, int cnt, int *tmp, int *a, int *b, int *v);

//print memory by command dump, edit, fill
void print_dump(int start, int end, char *tdata);

//edit function
void edit(int addr, int value);

//fill function
void fill(int a, int b, int v);

//print file's contents
void printFile(char *tdata, char *vdata);

//assemble function
void pass(char *tdata, char *vdata);

//calculate part of object code and find displacement
int cal_obj_disp(int len, char *data1, char *data2);

//calculate object code except BYTE
void cal_obj(int fo, int opco, int d);

//calculate object code when BYTE
int cal_BYTE_obj(char *line);

//calculate locctr by instruction's format
int Cal_form(char *data);

//calculate locctr when mnenonic is BYTE
int Cal_byte(char *line);

//calculate symbol table's key function
int sym_hash_Func(char *data);

//make symbol table
int make_SYMTAB(char *data, int loca);

//check parameter is in opcode table and if successful, return opcode
int is_OP(char *data);

//check parameter is in symbol table and if successful, return value(address)
int is_SYM(char *data);

//determine PC addressing or BASE addressing
int check_pb(int *d, int symnum_tmp);

//check if string has @ or #
void check_indi_imm(int fo, char *data);

//find register number
int find_reg_num(char *data);

//change hex to dec
int hex_to_dec(char c);

//change string to hex
int str_to_hex(char *c);

//print symbol table
void print_SYMTAB();

//pass1 in loader instruction
void pass1_loader(char objfile1[], char objfile2[], char objfile3[]);

//pass2 in loader instruction
void pass2_loader(char objfile[]);

//make external symbol table
void make_EXSYM(int check, char name_tmp[], int addr_tmp, int len_tmp);

//check symbol is already in external symbol table
int check_EXSYM(char *c);

//print load map
void print_Load_map();

//print bp list
void print_bp();

//make bp list
void make_bplist(int addr_tmp);

//run program
void Run();

//free hash table
void free_hashTable();

//free linked list
void free_linkedList();

//free symbol table
void free_SYMTAB();

//free external symbol table
void free_EXSYM();

//free bp list
void free_bp();
