#ifndef DBTRACK_H_
#define DBTRACK_H_

#include "core.h"

int AddDebugTrack(int slot, TCHAR *szName, waddr_t waddr, DWORD dwSize, TCHAR *szFmt);
void UpdateDebugTrack(void);

#endif /*DBTRACK_H_*/
