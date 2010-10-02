#ifndef SENDFILES_H
#define SENDFILES_H

#include "calc.h"

int SizeofFileList(char *);
char* AppendName(char *, char*);
#ifdef _WINDLL
__declspec(dllexport) 
#endif
BOOL SendFile(HWND hwndParent, const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination);
void SendFiles(char *, int);
void ThreadSend(char *, int, int);
void NoThreadSend(const char*, int);

#endif
