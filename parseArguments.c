#include "parseArguments.h"

int parse_define(struct h_table *hashtable, char *str)
{
	int i, rc;

	char *key_buff = calloc(100, sizeof(char));
	/* check calloc */
	if (key_buff == NULL)
		return -ENOMEM;

	for (i = 0; i < strlen(str); i++) {
		if (str[i] == '=') {
			/* if there is nothing after "=" */
			if (i == (strlen(str) - 1)) {
				rc = add(hashtable, key_buff, "");
				if (rc == -ENOMEM)
					return -ENOMEM;

				free(key_buff);
				return 0;
			}

			str = str + i + 1;
			rc = add(hashtable, key_buff, str);
			if (rc == -ENOMEM)
				return -ENOMEM;

			free(key_buff);
			return 0;
		}
		key_buff[i] = str[i];
	}

	/* if there was no '=' */
	rc = add(hashtable, key_buff, "");
	if (rc == -ENOMEM)
		return -ENOMEM;

	free(key_buff);

	return 0;
}

int add_to_directories(char **directories, char *str, int *nr_elem_dir)
{
	/* check if contains \n */
	if (str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = '\0';

	directories[(*nr_elem_dir)] = malloc((strlen(str) + 1) * sizeof(char));
	/* check malloc */
	if (directories[(*nr_elem_dir)] == NULL)
		return -ENOMEM;

	strcpy(directories[(*nr_elem_dir)], str);

	(*nr_elem_dir)++;

	return 0;
}

int pars_args(struct h_table *hashtable, int argc, char **argv, char inFile[],
		char outFile[], char **directories, int *nr_elem_dir)
{
	int rc, i;
	int isInfile = 0;
	int isOutfile = 0;

	for (i = 1; i < argc; i++) {
		/* check -D <SYMBOL> case */
		if (strcmp(argv[i], "-D") == 0) {
			rc = parse_define(hashtable, argv[i + 1]);
			if (rc == -ENOMEM)
				return -ENOMEM;

			i++;
			continue;
		}

		/* check -D<SYMBOL> case */
		if (argv[i][0] == '-' && argv[i][1] == 'D') {
			rc = parse_define(hashtable, argv[i] + 2);
			if (rc == -ENOMEM)
				return -ENOMEM;

			continue;
		}

		/* chech -I <Dir> case */
		if (strcmp(argv[i], "-I") == 0) {
			rc = add_to_directories(directories, argv[i + 1],
						nr_elem_dir);
			if (rc == -ENOMEM)
				return -ENOMEM;

			i++;
			continue;
		}

		/* check -I<Dir> case */
		if (argv[i][0] == '-' && argv[i][1] == 'I') {
			rc = add_to_directories(directories, argv[i] + 2,
						nr_elem_dir);
			if (rc == -ENOMEM)
				return -ENOMEM;

			continue;
		}

		/* check if it is Infile */
		if (isInfile == 0 && argv[i][0] != '-') {
			strcpy(inFile, argv[i]);
			isInfile = 1;
			continue;
		}

		/* check if it Outfile */
		if (isOutfile == 0) {
			/* check if it is -o <OUTFILE> case */
			if (strcmp(argv[i], "-o") == 0 && i != (argc - 1)) {
				strcpy(outFile, argv[i + 1]);
				i++;
				isOutfile = 1;
				continue;
			}

			/* check if it is -o<OUTFILE> case */
			if (argv[i][0] == '-' && argv[i][1] == 'o') {
				strcpy(outFile, argv[i] + 2);
				isOutfile = 1;
				continue;
			}

			/* if it is only <OUTFILE> */
			strcpy(outFile, argv[i]);
			isOutfile = 1;
			continue;
		}

		/* check if it is an extra file */
		if (isInfile == 1 && isOutfile == 1)
			return -1;
	}

	/* if there were no INFILE or OUTFILE */
	if (isInfile == 0)
		strcpy(inFile, "");

	if (isOutfile == 0)
		strcpy(outFile, "");

	return 0;
}
