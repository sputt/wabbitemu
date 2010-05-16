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

MFILE *ExportVar(int slot, char* fn, symbol83P_t* sym);
MFILE *mopen(const char * filename,const char * mode );
int mclose(MFILE* mf);
int meof(MFILE* mf);
int mgetc(MFILE* mf);
int mputc(int c,MFILE* mf);
int msize(MFILE* mf);

#endif
