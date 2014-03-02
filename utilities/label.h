#ifndef LABEL_H
#define LABEL_H

#include "calc.h"

TCHAR* FindAddressLabel(LPCALC lpCalc, waddr_t waddr);
//void ImportBcalls(char* fn);
TCHAR* FindBcall(int address, int model);
void FindFlags(int flag, int bit, TCHAR **flagstring, TCHAR **bitstring);

void VoidLabels(LPCALC lpCalc);
label_struct *lookup_label(LPCALC lpCalc, TCHAR *label);
int labels_app_load(LPCALC lpCalc, LPCTSTR lpszFileName);

#endif

