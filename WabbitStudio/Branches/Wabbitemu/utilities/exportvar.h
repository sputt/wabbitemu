#ifndef EXPORTVAR_H
#define EXPORTVAR_H

#include "link.h"

typedef struct {
	FILE* stream;
	unsigned int pnt;
	unsigned int size;
	unsigned char *data;
	unsigned char *name;
	BOOL read;
	BOOL write;
	BOOL bin;
	int eof;
} MFILE;

MFILE *ExportVar(LPCALC, TCHAR *, symbol83P_t *);
MFILE *ExportApp(LPCALC, TCHAR *, apphdr_t *);
MFILE *ExportRom(TCHAR *lpszFile, LPCALC lpCalc);
MFILE * ExportOS(TCHAR *lpszFile, unsigned char *buffer, int size);
MFILE *mopen(const TCHAR *filename, const TCHAR *mode);
int mclose(MFILE *);
int meof(MFILE *);
int mgetc(MFILE *);
int mputc(int, MFILE *);
int msize(MFILE *);

#endif
