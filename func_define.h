#include "myHashMap.h"

#ifndef __DEFINE_FUNC__
#define __DEFINE_FUNC__

#define BUFF_SIZE 300

int parse_add_define(struct h_table *hashtable, char *str, FILE *fin);

void define_subst(struct h_table *hashtable, char *str, FILE *fout);

int check_if_cond(struct h_table *hashtable, char *str);

int check_ifdef_cond(struct h_table *hashtable, char *str);

int do_include(struct h_table *hashtable, char *str, char **directories,
		int nr_elem_direct, FILE *fout, char inFile[]);

void lines_skipper(struct h_table *hashtable, char buff[], FILE *fin,
		int *is_if_taken);

int preproc_file(struct h_table *hashtable, FILE *fin, FILE *fout,
		char **directories, int nr_elem_direct, char inFile[]);

#endif
