#ifndef STDINT_H
#define STDINT_H

#include "coretypes.h"

typedef struct STDINT {
	unsigned char intactive;
#ifdef NO_TIMER_ELAPSED
	uint64_t lastchk1;
	uint64_t timermax1;
	uint64_t lastchk2;
	uint64_t timermax2;
	uint64_t freq[4];
#else
	double lastchk1;
	double timermax1;
	double lastchk2;
	double timermax2;
	double freq[4];
#endif
	int mem;
	int xy;	
	BOOL on_backup;
	BOOL on_latch;
} STDINT_t;




#endif
