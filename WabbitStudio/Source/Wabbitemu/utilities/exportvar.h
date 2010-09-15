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

MFILE *ExportVar(int, char *, symbol83P_t *);
MFILE *ExportApp(int, char *, apphdr_t *); 
MFILE *mopen(const char *, const char *);
int mclose(MFILE *);
int meof(MFILE *);
int mgetc(MFILE *);
int mputc(int, MFILE *);
int msize(MFILE *);

#endif
