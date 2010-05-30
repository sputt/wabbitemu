#include <windows.h>

#ifndef MAX_ANIMATE
#define MAX_ANIMATE	8
#endif

#if !defined(_HAS_MAPVIEWSETTINGS) && !defined(_IN_MAPVIEW_H)
#include "MapView.h"
#endif

#ifndef _HAS_ANIMATE
#define _HAS_ANIMATE
#pragma pack(1)
typedef struct {
	BYTE alive, x, w, y, h, anim_ctr;
	WORD key_ptr, anim_ptr;
} ZANIMATE;
#pragma pack()

typedef struct tagANIMATEKEY {
	TCHAR szName[64];
	int FrameCount;
	struct {
		BYTE Time;
		WORD Offset;
	} Frames[16];
} ANIMATEKEY, *LPANIMATEKEY;

typedef struct tagANIMATETYPE {
	char szMacro[32];
	TCHAR szName[64];
	TCHAR szDesc[48];
	int TileIndex;
	LPANIMATEKEY lpKey;
} ANIMATETYPE, *LPANIMATETYPE;

typedef struct tagANIMATE {
	int x, w, y, h;
	LPANIMATETYPE lpType;
	int Frame;
	int TimeRemaining;
} ANIMATE, *LPANIMATE;

#define ANIMATE_COLOR RGB(90, 40, 120)

BOOL InitAnimateKeys(const LPTSTR lpszFilename);
BOOL LoadAnimateTypes(const LPTSTR lpszFilename);
HRESULT CreateAnimate(LPANIMATETYPE lpat, int x, int y, LPANIMATE lpa);
HRESULT SetAnimateFromReal(LPANIMATE lpa, const ZANIMATE *pzanim);
#endif

#ifdef _HAS_MAPVIEWSETTINGS
void DrawAnimate(HDC hdc, LPANIMATE lpa, LPMAPVIEWSETTINGS lpmvs, double Scale);
void DrawMapAnimate(HDC hdc, LPMAPVIEWSETTINGS lpmvs, double Scale, BOOL fTest);
int AddAnimate(LPMAPVIEWSETTINGS lpmvs, LPANIMATETYPE lpat, int x, int y);
#endif
