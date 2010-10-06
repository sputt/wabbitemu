#ifndef SENDFILES_H
#define SENDFILES_H

#include "calc.h"
#include "link.h"

BOOL SendFileToCalc(const LPCALC lpCalc, LPCTSTR lpszFileName, BOOL fAsync);
int SizeofFileList(TCHAR *);
TCHAR* AppendName(TCHAR *, TCHAR *);
BOOL SendFile(HWND hwndParent, const LPCALC lpCalc, LPCTSTR lpszFileName, SEND_FLAG Destination);
void SendFiles(TCHAR *, int);
void ThreadSend(TCHAR *, int, LPCALC);
void NoThreadSend(const TCHAR*, int, LPCALC);

#endif
