#ifndef __LIST_H
#define __LIST_H

#include <stdbool.h>

typedef struct _list {
	void *data;
	struct _list *next;
} list_t;

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT  
#endif

EXPORT list_t *list_insert(list_t *, void *);
EXPORT list_t *list_append (list_t *first_node, void *data);
EXPORT list_t *list_prepend (list_t *first_node, void *data);
EXPORT list_t *list_remove (list_t *first_node, list_t *remove_node);
EXPORT void list_free_node (list_t *first_node);
EXPORT void list_free(list_t *, bool);

#endif
