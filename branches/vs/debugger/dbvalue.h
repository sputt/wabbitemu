#ifndef DBVALUE_H_
#define DBVALUE_H_

#include <stdio.h>
#include <windows.h>

#define VALUE_CLASS_NAME "WabbitValueClass"
#include "dbcommon.h"

HWND CreateValueField(
		HWND hwndParent, 
		char *name, 
		int label_width, 
		void *data, 
		size_t size, 
		int max_digits, 
		VALUE_FORMAT format);

#define VF_DESELECT	130
#define VF_DESELECT_CHILDREN (VF_DESELECT+1)

#endif /*DBVALUE_H_*/
