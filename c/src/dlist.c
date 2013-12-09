#include <stdlib.h>
#include <stdio.h>
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
	//fun fact: if you don't malloc this, the statement that increases
	//size by 1 in dlist_append will change the value of the "cap"
	//variable in that function.
	*listp = (dlist*)malloc(sizeof(struct dlist));
	(*listp)->list = (char**)calloc(capacity, sizeof(char*));
	(*listp)->capacity = capacity;
	(*listp)->size = 0;
	return capacity;
}

void dlist_get(dlist **listp, int index, char **str) {
	char *value = ((*listp)->list)[index];
	int len = strlen(value);
	*str = strcpy(malloc(len), value);
}

int dlist_append(dlist **listp, char *str) {
	//for some reason dereferencing doesn't work.
	//can't seem to do an intermediate value to make this
	//code readable... oh well.

	printf("got a list\n");
	
	//do we reallocate the list to expand its size?
	int size = (*listp)->size;
	int cap = (*listp)->capacity;

	printf("cap only: %d\n", cap);
	/*
	if (size > cap) {
		(*listp)->capacity += 10;
		(*listp)->list = realloc((*listp)->list, (*listp)->capacity * sizeof(char**));
	}

	printf("finished realloc\n");
	*/

	//add new element to the end of the list
	int len = strlen(str);
	(*listp)->list[size] = calloc(len, sizeof(char));
	strcpy((*listp)->list[size], str);
	
	(*listp)->size = size + 1; //apparently this fucks up the cap variable? what.
	(*listp)->capacity = cap; //why the hell do we need to do this?
	printf("copied. size is now %d\n", (*listp)->size);
	
	return (*listp)->size;
}
