#ifndef DBVALUE_H_
#define DBVALUE_H_

#define VALUE_CLASS_NAME _T("WabbitValueClass")
#include "dbcommon.h"

HWND CreateValueField(
		HWND hwndParent, 
		TCHAR *name, 
		int label_width, 
		void *data, 
		size_t size, 
		int max_digits, 
		VALUE_FORMAT format,
		int max_value = MAXINT);


typedef struct tag_value_field_settings {
	void *data;
	size_t size;	// size in bytes of the value
	VALUE_FORMAT format;
	BOOL hot_lit;
	BOOL selected;
	BOOL editing;

	// private fields
	TCHAR szName[16];
	DWORD cxName;
	TCHAR szValue[32];
	TCHAR szTip[80];
	int max_digits;
	int max_value;
	RECT hot;
	HWND hwndTip;
	TOOLINFO toolInfo;
	HWND hwndVal;
} value_field_settings;

#define VF_DESELECT	130
#define VF_DESELECT_CHILDREN (VF_DESELECT+1)

#endif /*DBVALUE_H_*/
