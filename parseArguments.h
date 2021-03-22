#include "myHashMap.h"

#define DIRECTORIES_SIZE 100

#ifndef __PARSER__
#define __PARSER__

int parse_define(struct h_table *hashtable, char *str);

int add_to_directories(char **directories, char *str, int *nr_elem_dir);

int pars_args(struct h_table *hashtable, int argc, char **argv,
	char inFile[], char outFile[], char **directories, int *nr_elem_dir);

#endif
