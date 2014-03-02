#ifndef GUIOPENFILE_H_
#define GUIOPENFILE_H_

#include "calc.h"

typedef struct tag_OFNHookOptions {
	BOOL bArchive;
	BOOL bFileSettings;
	int model;
} OFNHookOptions;

void GetOpenSendFileName(HWND, LPCALC);

#endif /*GUIOPENFILE_H_*/
