#ifndef LIST_H
#define LIST_H

struct dlist {
	char** list;
	int size;
	int capacity;
};

typedef struct dlist dlist;

/**
 * Create a list of the given initial capacity, with the dlist returned
 * as an output parameter.
 * @return The size of the newly allocated list, generally equal to capacity.
 */
int dlist_create(int capacity, dlist **listp);

/**
 * Get something from the specified list at the specified index. The returned
 * value is stored in str.
 */
void dlist_get(dlist **listp, int index, char **str);

/**
 * Append a string to the list.
 * @return the new size of the list.
 */
int dlist_append(dlist **listp, char *str);


#endif
