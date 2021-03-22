#include "myHashMap.h"

int hash_function(char *key)
{
	int key_length, first_letter, hash_code, i;

	key_length = strlen(key);
	first_letter = key[0];

	if (first_letter % 2 == 0)
		hash_code = key_length * first_letter * 3;
	else
		hash_code =  key_length * first_letter * 8;

	for (i = 0; i < strlen(key); i++)
		hash_code += key[i] * i;

	return hash_code % TABLE_SIZE;
}

int create_element(struct element *elem, char *key, char *value)
{
	/* allocate memory */
	(*elem).key = malloc(strlen(key) + 1);
	/* check malloc */
	if ((*elem).key == NULL)
		return -ENOMEM;

	(*elem).value = malloc(strlen(value) + 1);
	/* check malloc */
	if ((*elem).value == NULL)
		return -ENOMEM;

	/* copy values */
	strcpy((*elem).key, key);
	strcpy((*elem).value, value);

	return 0;
}

int check_value(struct h_table *hashtable, char *value)
{
	int i_final, i_val;
	char *final_val, *token, *to_replace_with;
	char *tmp_value = malloc((strlen(value) + 1) * sizeof(char));
	/* check malloc */
	if (tmp_value == NULL)
		return -ENOMEM;

	/* it is not necessary to check anymore */
	if (strcmp(value, "") == 0) {
		free(tmp_value);
		return 0;
	}

	strcpy(tmp_value, value);

	/* used to store the modified value */
	final_val = calloc(300, sizeof(char));
	/* check calloc */
	if (final_val == NULL)
		return -ENOMEM;

	/* used to store the indexes */
	i_final = 0;
	i_val = 0;

	token = strtok(tmp_value, "\t\n []{}<>=+-*/%!&|^.,:;()\\");

	while (token) {
		if (strstr(token, "\"") != NULL) {
			token = strtok(NULL, "\t\n []{}<>=+-*/%!&|^.,:;()\\");

			/* we will stay in this loop until "text" */
			/* is finished */
			while (token) {
				/* copy strtok delimitators */
				for (; i_val < token - tmp_value; i_val++) {
					final_val[i_final] = value[i_val];
					i_final++;
				}

				/* if we find the second " */
				if (strstr(token, "\"") != NULL) {
					/* add to final */
					strcat(final_val, token);
					i_final += strlen(token);
					i_val += strlen(token);

					break;
				}

				/* add to final */
				strcat(final_val, token);
				i_final += strlen(token);
				i_val += strlen(token);

				token = strtok(NULL,
					"\t\n []{}<>=+-*/%!&|^.,:;()\\");
			}
		} else {
			/* copy spaces etc(strok delimitators) */
			for (; i_val < token - tmp_value; i_val++) {
				final_val[i_final] = value[i_val];
				i_final++;
			}

			to_replace_with = get(hashtable, token);

			/* replace word */
			if (to_replace_with != NULL) {
				strcat(final_val, to_replace_with);
				i_final += strlen(to_replace_with);
				i_val += strlen(token);
			} else { /* write the word as it is */
				strcat(final_val, token);
				i_final += strlen(token);
				i_val += strlen(token);
			}
		}

		token = strtok(NULL, "\t\n []{}<>=+-*/%!&|^.,:;()\\");
	}

	/* add what is after the last token */
	for (; i_val < strlen(value); i_val++) {
		final_val[i_final] = value[i_val];
		i_final++;
	}

	strcpy(value, final_val);

	free(tmp_value);
	free(final_val);

	return 0;
}

int create_h_table(struct h_table **hashtable)
{
	int i;

	/* allocate memory for the hashtable */
	(*hashtable) = malloc(TABLE_SIZE * sizeof(struct h_table));
	/* check malloc */
	if ((*hashtable) == NULL)
		return -ENOMEM;

	/* allocate memory for the vec of elements */
	for (i = 0; i < TABLE_SIZE; i++) {
		(*hashtable)[i].elem_vec = malloc(5 * sizeof(struct element));
		/* check malloc */
		if ((*hashtable)[i].elem_vec == NULL)
			return -ENOMEM;

		(*hashtable)[i].size = 5;
		(*hashtable)[i].nr_of_elements = 0;
	}

	return 0;
}

int add(struct h_table *hashtable, char *key, char *value)
{
	int hash_code, size, i;
	struct element tmp_elem;

	hash_code = hash_function(key);

	/* if that position is empty */
	if (hashtable[hash_code].nr_of_elements == 0) {
		/* check if any element from value is already */
		/* a key and replace it */
		if (check_value(hashtable, value) == -ENOMEM)
			return -ENOMEM;

		/* struct element tmp_elem; */
		if (create_element(&tmp_elem, key, value) == -ENOMEM)
			return -ENOMEM;

		hashtable[hash_code].elem_vec[0] = tmp_elem;
		hashtable[hash_code].nr_of_elements++;

		return 0;
	}

	/* check if there is enough allocated space */
	/* if not, reallocate */
	if (hashtable[hash_code].nr_of_elements >= hashtable[hash_code].size) {
		hashtable[hash_code].size += 5;
		size = hashtable[hash_code].size;
		hashtable[hash_code].elem_vec =
			realloc(hashtable[hash_code].elem_vec,
				size * sizeof(element));

		/* check realloc */
		if (hashtable[hash_code].elem_vec == NULL)
			return -ENOMEM;
	}

	/* check if the key already exists in order to replace value */
	for (i = 0; i < hashtable[hash_code].nr_of_elements; i++) {
		if (strcmp(hashtable[hash_code].elem_vec[i].key, key) == 0) {
			/* check if any element from value is already */
			/* a key and replace it */
			if (check_value(hashtable, value) == -ENOMEM)
				return -ENOMEM;

			strcpy(hashtable[hash_code].elem_vec[i].value, value);

			return 0;
		}
	}

	/* add the new element */
	i = hashtable[hash_code].nr_of_elements;

	/* check if any element from value is already a key and replace it */
	if (check_value(hashtable, value) == -ENOMEM)
		return -ENOMEM;

	/* struct element tmp_elem; */
	if (create_element(&tmp_elem, key, value) == -ENOMEM)
		return -ENOMEM;

	hashtable[hash_code].elem_vec[i] = tmp_elem;
	hashtable[hash_code].nr_of_elements++;

	return 0;
}

char *get(struct h_table *hashtable, char *key)
{
	int hash_code, i;

	hash_code = hash_function(key);

	/* search for the key */
	for (i = 0; i < hashtable[hash_code].nr_of_elements; i++)
		if (strcmp(hashtable[hash_code].elem_vec[i].key, key) == 0)
			return hashtable[hash_code].elem_vec[i].value;

	/* if the key was not found return null */
	return NULL;
}

int remove_element(struct h_table *h_table, char *key)
{
	int h_code, i, j, size;

	h_code = hash_function(key);

	/* search for the key */
	for (i = 0; i < h_table[h_code].nr_of_elements; i++) {
		if (strcmp(h_table[h_code].elem_vec[i].key, key) == 0) {
			h_table[h_code].nr_of_elements--; /* dec nr of elem */

			/* free the element that will be overwritten */
			free(h_table[h_code].elem_vec[i].key);
			free(h_table[h_code].elem_vec[i].value);

			for (j = i; j < h_table[h_code].nr_of_elements; j++)
				h_table[h_code].elem_vec[j] =
					h_table[h_code].elem_vec[j + 1];

			/* remove the duplicate of the last element */
			size = h_table[h_code].nr_of_elements;
			h_table[h_code].size = h_table[h_code].nr_of_elements;
			h_table[h_code].elem_vec =
				realloc(h_table[h_code].elem_vec,
					size * sizeof(element));

			/* check realloc */
			if (h_table[h_code].elem_vec == NULL && size != 0)
				return -ENOMEM;

			/* if it was a single element we have */
			/* to alloc memory */
			if (size == 0) {
				h_table[h_code].size = 5;
				h_table[h_code].elem_vec =
					malloc(5 * sizeof(struct element));
				/* check malloc */
				if (h_table[h_code].elem_vec == NULL)
					return -ENOMEM;
			}

			return 0;
		}
	}

	return 0;
}

void free_hashtable(struct h_table *hashtable)
{
	int i, j;

	for (i = 0; i < TABLE_SIZE; i++) {
		for (j = 0; j < hashtable[i].nr_of_elements; j++) {
			free(hashtable[i].elem_vec[j].key);
			free(hashtable[i].elem_vec[j].value);
		}

		free(hashtable[i].elem_vec);
	}

	free(hashtable);
}
