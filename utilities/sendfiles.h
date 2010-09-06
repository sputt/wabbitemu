#ifndef SENDFILES_H
#define SENDFILES_H

int SizeofFileList(char *);
char* AppendName(char *, char*);
#ifdef _WINDLL
__declspec(dllexport) 
#endif
void SendFile(char *, int);
void SendFiles(char *, int);
void ThreadSend(char *, int, int);
void NoThreadSend(const char*, int);

#endif
