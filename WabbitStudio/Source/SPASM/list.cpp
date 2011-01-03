#include "stdafx.h"

#include "spasm.h"

#include "list.h"
#include "utils.h"

/*
 * Adds a node to the end of the
 * list, returns the new start
 * of the list
 */

EXPORT list_t *list_append (list_t *first_node, void *data) {
	list_t *curr_node, *new_node;

	new_node = (list_t *)malloc_chk (sizeof (list_t));
	new_node->data = data;
	new_node->next = NULL;

	if (first_node == NULL)
		return new_node;

	curr_node = first_node;
	while (curr_node->next)
		curr_node = curr_node->next;

	curr_node->next = new_node;
	return first_node;
}


/*
 * Adds a node to the beginning
 * of the list, returns the new
 * start of the list
 */

EXPORT list_t *list_prepend (list_t *first_node, void *data) {
	list_t *new_node;

	new_node = (list_t *)malloc_chk (sizeof (list_t));
	new_node->data = data;
	new_node->next = first_node;
	return new_node;
}


/*
 * Inserts a node after prev
 */
EXPORT list_t *list_insert(list_t *prev, void *data) {
	list_t *new_next;
	
	new_next = (list_t *) malloc_chk(sizeof(list_t));
	new_next->data = data;
	new_next->next = prev->next;
	
	prev->next = new_next;
	return new_next;
}

/*
 * Removes a node from the list,
 * returns the new start of the
 * list
 */

EXPORT list_t *list_remove (list_t *first_node, list_t *remove_node) {
	list_t *curr_node;

	// if the node to remove is the first one, then it's easy
	if (first_node == remove_node)
		return first_node->next;

	// otherwise, find the node before the node to remove
	curr_node = first_node;
	while (curr_node->next != remove_node && curr_node != NULL)
		curr_node = curr_node->next;

		// then move around pointers
	if (curr_node != NULL)	
		curr_node->next = remove_node->next;
	return first_node;
}


/*
 * Frees a list node
 */

EXPORT void list_free_node (list_t *first_node) {
	if (first_node)
		free (first_node);
}


/*
 * Frees an entire list
 */

EXPORT void list_free (list_t *curr_node, bool free_data) {
	
	while (curr_node) {
		list_t *next = curr_node->next;
		if (free_data && curr_node->data)
			free(curr_node->data);
		
		list_free_node(curr_node);
		curr_node = next;
	}
}
