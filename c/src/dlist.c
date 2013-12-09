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
	//variable in that function. also causes weird issues with
	//attempting to use intermediate dereferenced ptr in functions.
	
	//originally i assigned this by creating a regular struct instance
	//in the function, then assigning it to the ptr. probably the same
	//principle as not returning a non-static array from a function.
	//otherwise you get function scope memory outside the function and
	//that is bad.
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
	dlist* list = *listp;
	
	//do we reallocate the list to expand its size?
	int size = list->size;
	int cap = list->capacity;
	
	if (size > cap) {
		list->capacity += 10;
		list->list = realloc(list->list, list->capacity * sizeof(char**));
	}

	//add new element to the end of the list
	int len = strlen(str);
	list->list[size] = calloc(len, sizeof(char));
	strcpy(list->list[size], str);
	
	list->size = size + 1;
	
	return list->size;
}
