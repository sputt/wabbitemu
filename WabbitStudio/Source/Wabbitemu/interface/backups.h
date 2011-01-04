#ifndef BACKUPS_H
#define BACKUPS_H

#include "savestate.h"

#ifdef WITH_BACKUPS
typedef struct DEBUG_STATE {
	SAVESTATE_t *save;
	struct DEBUG_STATE *next, *prev;
} debugger_backup;
void do_backup(LPCALC);
void restore_backup(int, LPCALC);
void init_backups();
void free_backups(LPCALC);
void free_backup(debugger_backup *);
#endif

#define MAX_BACKUPS 10
debugger_backup* backups[MAX_CALCS];
int number_backup;
int current_backup_index;

#endif			//BACKUPS_H