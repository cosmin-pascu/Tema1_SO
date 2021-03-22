#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef __HASHMAP__
#define __HASHMAP__

#define TABLE_SIZE 500

struct element {
	char *key;
	char *value;
} element;

struct h_table {
	struct element *elem_vec;
	int size;
	int nr_of_elements;
};

int hash_function(char *key);

int create_element(struct element *elem, char *key, char *value);

int create_h_table(struct h_table **hashtable);

/* used to check if any element from value is a key already and replace it */
int check_value(struct h_table *hashtable, char *value);

int add(struct h_table *hashtable, char *key, char *value);

char *get(struct h_table *hashtable, char *key);

int remove_element(struct h_table *h_table, char *key);

void free_hashtable(struct h_table *hashtable);

#endif
