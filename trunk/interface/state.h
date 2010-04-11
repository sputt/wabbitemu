#ifndef STATE_H_
#define STATE_H_

#include <stdio.h>
#include "core.h"
#include "var.h"		// TIFILE

typedef struct apphdr {
	char name[12];
	u_int page, page_count;
} apphdr_t;

typedef struct applist {
	u_int count;
	apphdr_t apps[96];
} applist_t;

typedef struct {
	uint8_t type_ID;
	uint8_t type_ID2;
	uint8_t version;
	uint16_t address;
	uint8_t page;
	uint8_t name_len;
	char name[9];
} symbol83P_t;

typedef struct symlist {
	symbol83P_t *programs;
	symbol83P_t *last;
	symbol83P_t symbols[512];
} symlist_t;

#define pTemp			0x982E
#define progPtr			0x9830
#define symTable		0xFE66

typedef struct upages {
	u_int start, end;
} upages_t;

#define circ10(z) ((((u_char) z) < 10) ? ((z) + 1) % 10 : (z))
#define tAns	0x72

void state_build_applist(CPU_t *, applist_t *);
void state_userpages(CPU_t *, upages_t *);
symlist_t* state_build_symlist_83P(CPU_t *, symlist_t *);
char *GetRealAns(CPU_t*);

#endif /*STATE_H_*/
