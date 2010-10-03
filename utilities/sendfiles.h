#ifndef SENDFILES_H
#define SENDFILES_H

int SizeofFileList(TCHAR *);
TCHAR* AppendName(TCHAR *, TCHAR *);
#ifdef _WINDLL
__declspec(dllexport) 
#endif
void SendFile(TCHAR *, int);
void SendFiles(TCHAR *, int);
void ThreadSend(TCHAR *, int, LPCALC);
void NoThreadSend(const TCHAR*, int, LPCALC);

#endif
