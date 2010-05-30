#define _OBJECT_SELECTION_CPP
#include <windows.h>
#include "MapObjects.h"
#include "ObjectSelection.h"
#include "list.h"

static list_t *SelectedObjects = NULL;

void ClearObjectSelection(void) {
	list_free(SelectedObjects, FALSE);
	SelectedObjects = NULL;
}

void AddObjectToSelection(LPVOID lpo) {
	if (IsObjectSelected(lpo) == FALSE) {
		SelectedObjects = list_append(SelectedObjects, lpo);
	}
}

void RemoveObjectFromSelection(LPVOID lpo) {
	list_t *list = SelectedObjects;
	list_t *prev = NULL;
	while (list != NULL) {
		if (list->data == lpo) {
			list_t *next = list->next;
			if (prev != NULL) {
				prev->next = next;
			} else {
				SelectedObjects = next;
			}
			list_free_node(list);
			return;
		}

		prev = list;
		list = list->next;
	}
}

DWORD GetSelectedObjectCount() {
	DWORD i = 0;
	list_t *list = SelectedObjects;

	while (list != NULL) {
		i++;
		list = list->next;
	}
	return i;
}

LPVOID GetSelectedObject(int Index) {
	int i = 0;
	list_t *list = SelectedObjects;
	while (list != NULL) {
		if (i == Index)
			return (LPOBJECT) list->data;
		i++;
		list = list->next;
	}
	return NULL;
}

/*
 * Pass NULL to see if any object is selected
 */
BOOL IsObjectSelected(const LPVOID lpo) {
	list_t *list = SelectedObjects;

	if (lpo == NULL) {
		return list == NULL ? FALSE : TRUE;
	}

	while (list != NULL) {
		if (list->data == lpo)
			return TRUE;
		list = list->next;
	}
	return FALSE;
}
