#include "func_define.h"

int parse_add_define(struct h_table *hashtable, char *str, FILE *fin)
{
	int i, j, rc, length;
	char *key_buff, *value_buff, *tmp, *tmp_buff;
	char buff[BUFF_SIZE];

	i = 0;
	key_buff = calloc(100, sizeof(char));
	/* check calloc */
	if (key_buff == NULL)
		return -ENOMEM;

	value_buff = calloc(100, sizeof(char));
	/* check calloc */
	if (value_buff == NULL)
		return -ENOMEM;

	/* parse SYMBOL */
	while (i < strlen(str) && str[i] != ' ' && str[i] != '\n') {
		key_buff[i] = str[i];
		i++;
	}

	/* case #define SYMBOL'\n' */
	if (str[i] == '\n') {
		strcpy(value_buff, "");
		rc = add(hashtable, key_buff, value_buff);
		if (rc == -ENOMEM)
			return -ENOMEM;

		free(key_buff);
		free(value_buff);

		return 0;
	}

	i++; /* to skip SPACE character */

	length = strlen(str);

	/* check for multiline define */
	if (str[length - 1] == '\n' && str[length - 2] == '\\') {
		/* we copy without the '\' character */
		strncpy(value_buff, str + i, strlen(str + i) - 2);

		/* we read lines until the multiline define is finished */
		while (fgets(buff, BUFF_SIZE, fin)) {

			/* remove the spaces */
			j = 0;
			while (buff[j] == ' ' || buff[j] == '\t')
				j++;

			/* create the buff without the extra spaces */
			tmp = malloc((sizeof(buff) + 1) * sizeof(char));
			/* check malloc */
			if (tmp == NULL)
				return -ENOMEM;

			strcpy(tmp, buff);
			tmp_buff = tmp + j;
			length = strlen(tmp_buff);

			/* if there are more lines in multiline */
			if (tmp_buff[length - 1] == '\n' &&
				tmp_buff[length - 2] == '\\') {
				strncat(value_buff, tmp_buff, length - 2);
			} else {
				/* the multiline is finished */
				strncat(value_buff, tmp_buff, length - 1);
				free(tmp);
				break;
			}
			free(tmp);
		}
	} else {
		/* it is not a multiline */

		/* check if there is no SYMBOL */
		if(strlen(str) == strlen(key_buff))
			strcpy(value_buff, "");
		else
			strncpy(value_buff, str + i, strlen(str + i) - 1);
	}

	rc = add(hashtable, key_buff, value_buff);
	if (rc == -ENOMEM)
		return -ENOMEM;

	free(key_buff);
	free(value_buff);

	return 0;
}

int define_replace(struct h_table *hashtable, char *str, FILE *fout)
{
	char *tmp_str, *final, *token, *to_replace_with;
	int i_final, i_str;

	tmp_str = malloc((strlen(str) + 1) * sizeof(char));
	/* check malloc */
	if (tmp_str == NULL)
		return -ENOMEM;

	strcpy(tmp_str, str);

	/* used to store the modified line */
	final = calloc(BUFF_SIZE, sizeof(char));
	/* check calloc */
	if (final == NULL)
		return -ENOMEM;

	/* used to store the indexes */
	i_final = 0;
	i_str = 0;

	token = strtok(tmp_str, "\t\n []{}<>=+-*/%!&|^.,:;()\\");

	while (token) {
		if (strstr(token, "\"") != NULL) {
			token = strtok(NULL, "\t\n []{}<>=+-*/%!&|^.,:;()\\");

			/* we will stay in this loop until "text" */
			/*  is finished */
			while (token) {
				/* copy strtok delimitators */
				for (; i_str < token - tmp_str; i_str++) {
					final[i_final] = str[i_str];
					i_final++;
				}

				/* if we find the second " */
				if (strstr(token, "\"") != NULL) {
					/* add to final */
					strcat(final, token);
					i_final += strlen(token);
					i_str += strlen(token);

					break;
				}

				/* add to final */
				strcat(final, token);
				i_final += strlen(token);
				i_str += strlen(token);

				token = strtok(NULL,
					"\t\n []{}<>=+-*/%!&|^.,:;()\\");
			}
		} else {
			/* copy spaces etc(strok delimitators) */
			for (; i_str < token - tmp_str; i_str++) {
				final[i_final] = str[i_str];
				i_final++;
			}

			to_replace_with = get(hashtable, token);

			/* replace word */
			if (to_replace_with != NULL) {
				strcat(final, to_replace_with);
				i_final += strlen(to_replace_with);
				i_str += strlen(token);
			} else { /* write the word as it is */
				strcat(final, token);
				i_final += strlen(token);
				i_str += strlen(token);
			}
		}

		token = strtok(NULL, "\t\n []{}<>=+-*/%!&|^.,:;()\\");
	}

	/* add what is after the last token */
	for (; i_str < strlen(str); i_str++) {
		final[i_final] = str[i_str];
		i_final++;
	}

	fprintf(fout, "%s", final);

	free(final);
	free(tmp_str);

	return 0;
}

int check_if_cond(struct h_table *hashtable, char *str)
{
	char *value;

	/* remove the '\n' if it exists */
	if (str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = '\0';

	value = get(hashtable, str);

	/* if it was a define SYMBOL */
	if (value != NULL)
		return atoi(value);

	return atoi(str);
}

int check_ifdef_cond(struct h_table *hashtable, char *str)
{
	char *value;

	/* remove the '\n' if it exists */
	if (str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = '\0';

	value = get(hashtable, str);

	/* if it was a define SYMBOL */
	if (value != NULL)
		return 1;
	else
		return 0;
}

int do_include(struct h_table *hashtable, char *str, char **directories,
		int nr_elem_direct, FILE *fout, char inFile[])
{
	char *path;
	int i, last_slash, file_found, rc;
	char *filename;
	FILE *fin;

	last_slash = 0;
	file_found = 0;

	path = calloc(100, sizeof(char));
	/* check calloc */
	if (path == NULL)
		return -ENOMEM;

	filename = calloc((strlen(inFile) + 1), sizeof(char));
	/* check calloc */
	if (filename == NULL)
		return -ENOMEM;

	/* get the filename without "" */
	strncpy(filename, str, strlen(str) - 2);

	for (i = 0; i < strlen(inFile); i++)
		if (inFile[i] == '/')
			last_slash = i;

	/* get the path from input file */
	if (last_slash != 0)
		/* add the path only if it is one */
		strncpy(path, inFile, last_slash + 1);

	strcat(path, filename);

	fin = fopen(path, "r");

	if (fin != NULL) {
		rc = preproc_file(hashtable, fin, fout, directories,
				nr_elem_direct, inFile);

		if (rc == -ENOMEM)
			return -ENOMEM;
	} else {
		for (i = 0; i < nr_elem_direct; i++) {
			strcpy(path, directories[i]);
			strcat(path, "/");
			strcat(path, filename);

			fin = fopen(path, "r");

			/* if we found a file */
			if (fin != NULL) {
				/* preprocess file and write it */
				rc = preproc_file(hashtable, fin, fout,
					directories, nr_elem_direct, inFile);

				if (rc == -ENOMEM)
					return -ENOMEM;

				file_found = 1;
				break;
			}
		}
		/* if no file was found */
		if (file_found == 0) {
			free(path);
			free(filename);
			exit(-2);
		}
	}

	free(path);
	free(filename);
	fclose(fin);

	return 0;
}

void lines_skipper(struct h_table *hashtable, char buff[], FILE *fin,
		int *is_if_taken)
{
	char *ret;

	while (fgets(buff, BUFF_SIZE, fin)) {
		/* skip until #endif */
		if (strncmp(buff, "#endif", strlen("#endif")) == 0)
			break;

		/* skip until #else */
		if (strncmp(buff, "#else", strlen("#else")) == 0)
			break;

		if (strncmp(buff, "#elif", strlen("#elif")) == 0) {
			ret = strstr(buff, "#elif ");
			ret += 6; /* to jump the #elif */

			/* if the elif is taken */
			if (check_if_cond(hashtable, ret) == 1) {
				(*is_if_taken)++;
				break;
			}
			/* if the elif is not taken, continue to loop until */
			/* one condition is met (and don't print anything) */
		}
	}
}

int preproc_file(struct h_table *hashtable, FILE *fin, FILE *fout,
	char **directories, int nr_elem_direct, char inFile[])
{
	char buff[BUFF_SIZE];
	int is_if_taken, rc;
	char *ret;

	is_if_taken = 0;

	while (fgets(buff, BUFF_SIZE, fin)) {
		/* ----------------- begin the #define case ------------- */
		if (strncmp(buff, "#define ", strlen("#define ")) == 0) {
			ret = strstr(buff, "#define ");
			ret += 8; // to jump the "#define "

			rc = parse_add_define(hashtable, ret, fin);
			if (rc == -ENOMEM)
				return -ENOMEM;

			continue;
		}

		/* ----------------- begin the #if case ----------------- */
		if (strncmp(buff, "#if ", strlen("#if ")) == 0) {
			ret = strstr(buff, "#if ");
			ret += 4; /* to jump the #if */

			/* if the if branch isn't taken */
			if (check_if_cond(hashtable, ret) == 0) {
				/* begin to skip */
				lines_skipper(hashtable, buff, fin,
					&is_if_taken);
			} else {
				is_if_taken++;
			}
			continue;
		}

		/* ----------------- begin the #else case ----------------- */
		if (strncmp(buff, "#else", strlen("#else")) == 0)
			if (is_if_taken != 0)
				/* skip lines */
				while (fgets(buff, BUFF_SIZE, fin))
					/* skip until #endif */
					/* strlen("#endif") */
					if (strncmp(buff, "#endif", 6) == 0) {
						is_if_taken--;
						break;
					}

		/* ----------------- begin the #endif case ---------------- */
		if (strncmp(buff, "#endif", strlen("#endif")) == 0) {
			/* don't write the line */
			is_if_taken--;
			continue;
		}

		/* ----------------- begin the #undef case ---------------- */
		if (strncmp(buff, "#undef ", strlen("#undef ")) == 0) {
			ret = strstr(buff, "#undef ");
			ret += 7; /* to jump the "#undef " */

			/* the skip extra spaces until the key */
			while (ret[0] == ' ')
				ret++;

			/* remove '\n' */
			ret[strlen(ret) - 1] = '\0';

			rc = remove_element(hashtable, ret);
			if (rc == -ENOMEM)
				return -ENOMEM;

			continue;
		}

		/* ----------------- begin the #ifdef case ----------------- */
		if (strncmp(buff, "#ifdef", strlen("#ifdef")) == 0) {
			ret = strstr(buff, "#ifdef ");
			ret += 7; /* to jump #ifdef */

			/* skip the lines */
			if (check_ifdef_cond(hashtable, ret) == 0) {
				/* begin to skip */
				while (fgets(buff, BUFF_SIZE, fin)) {
					/* skip until #endif */
					/* strlen("#endif") = 6 */
					if (strncmp(buff, "#endif", 6) == 0)
						break;

					/* skip until #else */
					/* strlen("#else") = 5 */
					if (strncmp(buff, "#else", 5) == 0)
						break;
				}
			} else {
				is_if_taken++;
			}
			continue;
		}

		/* ----------------- begin the #ifndef case --------------- */
		if (strncmp(buff, "#ifndef", strlen("#ifndef")) == 0) {
			ret = strstr(buff, "#ifndef ");
			ret += 8; /* to jump #ifdef */

			/* skip the lines */
			/* (is the previous condition reversed) */
			if (check_ifdef_cond(hashtable, ret) == 1) {
				/* begin to skip */
				while (fgets(buff, BUFF_SIZE, fin)) {
					/* skip until #endif */
					/* strlen("#endif") = 6 */
					if (strncmp(buff, "#endif", 6) == 0)
						break;

					/* skip until #else */
					/* strlen("#else") = 5 */
					if (strncmp(buff, "#else", 5) == 0)
						break;
				}
			} else {
				is_if_taken++;
			}
			continue;
		}

		/* ----------------- begin the #include case -------------- */
		if (strncmp(buff, "#include", strlen("#include")) == 0) {
			ret = strstr(buff, "#include ");
			/* tu jump #include " */
			ret += strlen("#include ") + 1;

			rc = do_include(hashtable, ret, directories,
				nr_elem_direct, fout, inFile);

			if (rc == -ENOMEM)
				return -ENOMEM;

			continue;
		}


		/* if there is no other case, replace defined words */
		rc = define_replace(hashtable, buff, fout);
		if (rc == -ENOMEM)
			return -ENOMEM;
	}

	return 0;
}
