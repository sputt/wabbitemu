#include <Windows.h>

#ifndef MAX_MISC
#define MAX_MISC 16
#endif

#if !defined(_HAS_MAPVIEWSETTINGS) && !defined(_IN_MAPVIEW_H)
#include "MapView.h"
#endif

#ifndef _HAS_MISC
#define _HAS_MISC
typedef struct tagMISCTYPE {
	TCHAR szMacro[32];
	TCHAR szName[32];
	TCHAR szDesc[48];
	int ArgCount;
	TCHAR szArgs[12][2][32];
} MISCTYPE, *LPMISCTYPE;

#define MISC_MAGIC_NUM 'M'
typedef struct tagMISC {
	int MagicNum;
	int x, w, y, h;
	double DragStartX, DragStartY;
	int DragStartW, DragStartH;
	int EdgeDrag;
	int MiscID;
	struct tagMAPVIEWSETTINGS *lpmvs;
	TCHAR szArgs[12][32];
} MISC, *LPMISC;

void DrawMisc(HDC hdc, LPMISC lpm);
int GetMiscEdge(LPMISC lpm, int x, int y);
BOOL LoadMiscTypes(LPCTSTR szFileName);
#endif

#define IDC_MISC_TYPE	999

#ifdef _HAS_MAPVIEWSETTINGS
void DrawMapMisc(HDC hdc, LPMAPVIEWSETTINGS lpmvs);
#endif
