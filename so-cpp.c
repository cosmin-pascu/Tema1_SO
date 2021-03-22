#include "myHashMap.h"
#include "parseArguments.h"
#include "func_define.h"

int init_IO_files(char inFile[], char outFile[], FILE **fin, FILE **fout)
{
	/* init files */
	if (strlen(inFile) == 0) {
		*fin = stdin;
	} else {
		*fin = fopen(inFile, "r");
		if (*fin == NULL)
			return 1;
	}

	if (strlen(outFile) == 0)
		*fout = stdout;
	else
		*fout = fopen(outFile, "w");

	return 0;
}

int main(int argc, char **argv)
{
	char inFile[100], outFile[100];
	FILE *fin, *fout;
	struct h_table *hashtable;
	char **directories = malloc(DIRECTORIES_SIZE * sizeof(char *));
	int nr_elem_direct = 0, rc, i;

	/* create hashtable */
	rc = create_h_table(&hashtable);
	if (rc == -ENOMEM)
		exit(ENOMEM);

	rc = pars_args(hashtable, argc, argv, inFile, outFile, directories,
			&nr_elem_direct);

	if (rc == -ENOMEM)
		exit(ENOMEM);

	if (rc == -1)
		return -1;

	rc = init_IO_files(inFile, outFile, &fin, &fout);

	if (rc == 1)
		return -1;

	rc = preproc_file(hashtable, fin, fout, directories, nr_elem_direct,
			inFile);

	if (rc == -ENOMEM)
		exit(ENOMEM);

	/* print_hashtable(hashtable); */

	free_hashtable(hashtable);

	/* free directories */
	for (i = 0 ; i < nr_elem_direct; i++)
		free(directories[i]);

	free(directories);

	fclose(fin);
	fclose(fout);

	return 0;
}
