#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>

#include "list.h"
#include "hash.h"
#include "bitmap.h"

//for counting number of list, hash, bitmap
int l_cnt=0, h_cnt=0, b_cnt=0;

//for indexing number of list, hash, bitmap
int l_idx=0, h_idx=0, b_idx=0;

//for tokening input command
char *command = NULL, *inst1 = NULL, *inst2 = NULL, *inst3 = NULL, *inst4 = NULL, *inst5 = NULL;

/********************list structure*******************/
//list's link struct
typedef struct list_{
	char name[20];
	struct list *link;
}list_ll;
list_ll list_arr[10];

//list's data struct
struct list_data{
	int data;
	struct list_elem e;
};
/*****************************************************/

/********************hash structure*******************/
typedef struct hash_{
	char name[20];
	struct hash *link;
}hash_ll;
hash_ll hash_arr[10];

struct hash_data{
	int data;
	struct hash_elem e;
};
/****************************************************/

/******************bitmap's struct*******************/
typedef struct bitmap_{
	char name[20];
	struct bitmap *link;
}bitmap_ll;
bitmap_ll bit_arr[10];
/****************************************************/

//create list's linked list
struct list *list_create(){
	struct list *l;

	return l = (struct list*) malloc(sizeof(struct list));
}
//create list's data struct
struct list_data *list_data_create(){
	struct list_data *d;

	return d = (struct list_data*) malloc(sizeof(struct list_data));
}

//create hash's linked list
struct hash *hash_create(){
	struct hash *h;

	return h = (struct hash*) malloc(sizeof(struct hash));
}
//create hash's data struct
struct hash_data *hash_data_create(){
	struct hash_data *d;
	
	//d->data = 0;
	return d = (struct hash_data*) malloc(sizeof(struct hash_data));
}

//print list data and hash data
void dumpdata_list(char *);
void dumpdata_hash(char *);
void dumpdata_bitmap(char *);

//compare which one is large
bool less(const struct list_elem *tmp1, const struct list_elem *tmp2, void *aux);

//swap two elements
void list_swap(struct list_elem *a, struct list_elem *b);

//shuffle list
void list_shuffle(struct list *list);

//hash functions
unsigned hash_hash_function(const struct hash_elem *e, void *aux);
bool hash_less_function(const struct hash_elem *, const struct hash_elem *, void *);
void hash_destructor(struct hash_elem *e, void *aux);
void hash_square(struct hash_elem *e, void *aux);
void hash_triple(struct hash_elem *e, void *aux);

//find index according to parameter in list, hash, bitmap
int find_idx(char c, int cnt, char *inst);

/********list, hash, bitmap's additional function*********/
void list_swap(struct list_elem *a, struct list_elem *b);
void list_shuffle(struct list *list);
unsigned hash_int_2(int i);
struct bitmap *bitmap_expand(struct bitmap *bitmap, int size){
	int i;
	struct bitmap *bit_expand;
	size_t size_origin = bitmap_size(bitmap);

	bit_expand = bitmap_create(size_origin + size);
	if(bit_expand == NULL){
		printf("Bit_Expand size error!\n");
	}
	else{
		for(i=0; i<size_origin; i++){
			if(bitmap_test(bitmap, i) == true)
				bitmap_mark(bit_expand, i);
		}
	}

	return bit_expand;
}
/*********************************************************/

int find_idx(char c, int cnt, char *inst){
	int i;

	if(c == 'l'){
		for(i=0; i<cnt; i++){
			if(strcmp(inst, list_arr[i].name) == 0) break;
		}
	}
	else if(c == 'h'){
		for(i=0; i<cnt; i++){
			if(strcmp(inst, hash_arr[i].name) == 0) break;
		}
	}
	else if(c == 'b'){
		for(i=0; i<cnt; i++){
			if(strcmp(inst, bit_arr[i].name) == 0) break;
		}
	}

	return i;
}

void dumpdata_list(char *l){
	struct list_elem *e;
	struct list_data *d;
	
	l_idx = find_idx('l', l_cnt, l);

	for(e=list_begin(list_arr[l_idx].link);
			e!=list_end(list_arr[l_idx].link);
			e=list_next(e)){
		d = list_entry(e, struct list_data, e);
		printf("%d ", d->data);
	}
	printf("\n");
}

void dumpdata_hash(char *l){
	int i;
	struct list_elem *ptr;
	struct hash_data *d;
	struct hash_elem *he;
	
	h_idx = find_idx('h', h_cnt, l);

	//if hash_arr[h_idx] has any element
	if(hash_arr[h_idx].link->elem_cnt != 0){
		for(i=0; i<hash_arr[h_idx].link->bucket_cnt; i++){
			for(ptr = list_begin(&(hash_arr[h_idx].link->buckets[i]));
					ptr != list_end(&(hash_arr[h_idx].link->buckets[i]));
					ptr = list_next(ptr)){
				he = list_entry(ptr, struct hash_elem, list_elem);
				d = hash_entry(he, struct hash_data, e);
				printf("%d ", d->data);
			}
		}
		printf("\n");
	}
}

void dumpdata_bitmap(char *l){
	int i;
	
	b_idx = find_idx('b', b_cnt, l);

	for(i=0; i<bitmap_size(bit_arr[b_idx].link); i++){
		if(bitmap_test(bit_arr[b_idx].link, i) == 0){
			printf("0");
		}
		else printf("1");
	}
	printf("\n");
}

bool less(const struct list_elem *tmp1, const struct list_elem *tmp2, void *aux){
	struct list_data *x, *y;

	x = list_entry(tmp1, struct list_data, e);
	y = list_entry(tmp2, struct list_data, e);

	if(x->data < y->data) return true;
	else return false;
}

void list_swap(struct list_elem *a, struct list_elem *b){
	int tmp;
	struct list_data *x, *y;

	x = list_entry(a, struct list_data, e);
	y = list_entry(b, struct list_data, e);

	tmp = x->data;
	x->data = y->data;
	y->data = tmp;
}

void list_shuffle(struct list *list){
	int i, j, k;
	int len = (int)list_size(list);
	int tmp1, tmp2;
	struct list_elem *el1, *el2;

	srand((unsigned int)time(NULL));

	for(k=0; k<10; k++){
		el1 = list_begin(list);
		el2 = list_begin(list);

		tmp1 = rand()%len;
		tmp2 = rand()%len;
		for(i=0; i<tmp1; i++){
			el1 = el1->next;
		}
		for(j=0; j<tmp2; j++){
			el2 = el2->next;
		}
		list_swap(el1, el2);
	}
}

unsigned hash_hash_function(const struct hash_elem *e, void *aux){
	struct hash_data *tmp;

	tmp = hash_entry(e, struct hash_data, e);
	return hash_int(tmp->data);
}

bool hash_less_function(const struct hash_elem *x, const struct hash_elem *y, void *aux){
	struct hash_data *tmp1, *tmp2;

	tmp1 = hash_entry(x, struct hash_data, e);
	tmp2 = hash_entry(y, struct hash_data, e);

	if(tmp1->data < tmp2->data) return true;
	else return false;
}

unsigned hash_int_2(int i){
	i = (i+3)%10;
	return i;
}

void hash_destructor(struct hash_elem *e, void *aux){
	struct hash_elem *he;
	struct hash_data *d;

	he = list_entry(e->list_elem.prev, struct hash_elem, list_elem);
	e->list_elem.next = he->list_elem.next;
	he = list_entry(e->list_elem.next, struct hash_elem, list_elem);
	e->list_elem.prev = he->list_elem.prev;

	d = hash_entry(e, struct hash_data, e);
	free(d);
}

void hash_square(struct hash_elem *e, void *aux){
	struct hash_data *tmp; 

	tmp = hash_entry(e, struct hash_data, e);
	tmp->data = (tmp->data) * (tmp->data);
}

void hash_triple(struct hash_elem *e, void *aux){
	struct hash_data *tmp;

	tmp = hash_entry(e, struct hash_data, e);
	tmp->data = (tmp->data) * (tmp->data) * (tmp->data);
}
