#ifndef LABEL_H
#define LABEL_H

#include "calc.h"

typedef struct {
	TCHAR name[32];
	int address;
} bcall_t;

TCHAR* FindAddressLabel(LPCALC lpCalc, waddr_t waddr);
bcall_t *get_bcalls(int model);
TCHAR* FindBcall(int address, int model);
void FindFlags(int flag, int bit, TCHAR **flagstring, TCHAR **bitstring);

void VoidLabels(LPCALC lpCalc);
label_struct *lookup_label(LPCALC lpCalc, TCHAR *label);
int labels_app_load(LPCALC lpCalc, LPCTSTR lpszFileName);

#endif

