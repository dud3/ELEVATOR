#include <stdlib.h>
#include <string.h>
#include "dlist.h"

/*
 * dlist is a char** type. so a **list parameter is a char****.
 * Need more indirection.
 *
 * Easiest way to understand is that user declares a dlist* and passes
 * it in via &address.
 */

int dlist_create(int capacity, dlist **listp) {
	//this crazy line is allocating the list property of the dlist
	//struct with the size of one char*
	(*listp)->list = calloc(capacity, sizeof(*((*listp)->list)));
	return capacity;
}

void dlist_get(dlist **listp, int index, char* str) {
	//return the string at whatever index. just complex looking
	//because of the struct.
	str = *(((*listp)->list) + index);
}

int dlist_append(dlist **listp, char* str) {
	dlist *list = *listp;

	//do we reallocate the list to expand its size?
	if (list->size > list->capacity) {
		list->capacity += 10;
		list->list = realloc(list->list, list->capacity * sizeof (*(list->list)));
	}

	//add new element to the end of the list
	list->size++;
	size_t len = strlen(str);
	*((list->list) + list->size) = calloc(len, sizeof(char));

	strcpy(*((list->list) + list->size), str);
	
	return list->size;
}
