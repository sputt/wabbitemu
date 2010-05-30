#include <windows.h>
#include "ObjectGraphics.h"

#ifndef MAX_OBJECT
#define MAX_OBJECT 17
#endif

#ifndef MAX_ENEMY
#define MAX_ENEMY 10
#endif

#if !defined(_HAS_MAPVIEWSETTINGS) && !defined(_IN_MAPVIEW_H)
#include "MapView.h"
#endif

#ifndef _HAS_OBJECT
#define _HAS_OBJECT
typedef struct tagOBJECTTYPE {
	TCHAR szMacro[32];
	TCHAR szName[32];
	TCHAR szDesc[48];
	int ArgCount;
	TCHAR szArgs[16][2][32];
	//int cxObject, cyObject;
} OBJECTTYPE, *LPOBJECTTYPE;

#define OBJECT_MAGIC_NUM 'O'
typedef struct tagOBJECT {
	int MagicNum;
	LPOBJECTTYPE lpType;
	LPOBJECTIMAGE lpImage;
	double x, w, y, h, z, d;
	double DragStartX, DragStartY;
	struct tagMAPVIEWSETTINGS *lpmvs;
	TCHAR szArgs[12][32];
} OBJECT, *LPOBJECT;

#define ENEMY_MAGIC_NUM 'E'
#pragma pack(1)
typedef struct {
	BYTE ID, f, x, w, y, h, z, d, anim_ctr;
	WORD anim_ptr;
} ZOBJECT;
typedef struct {
	BYTE health, f, x, w, y, h, z, d;
	BYTE gen;
	BYTE com_ctr;
	WORD com_ptr;
	byte anim_ctr;
	WORD anim_ptr;
} ZENEMY;
#pragma pack()

HRESULT LoadObjectTypes(TCHAR *szFilename, LPOBJECTTYPE lpTypes, LPDWORD lpdwCount);
LPOBJECTTYPE LookupObjectType(TCHAR *szName);
HRESULT CreateObjectOutput(LPOBJECT lpo, char *szOutput, int cbOutput, BOOL fIncludeOptional);
void DrawObject(HDC hdc, LPOBJECT lpo, COLORREF cr);
HRESULT CreateObject(LPOBJECTTYPE lpot, int x, int y, LPOBJECT lpo);
HRESULT CreateEnemy(LPOBJECTTYPE lpot, int x, int y, LPOBJECT lpo);
HRESULT SetObjectFromReal(LPOBJECT lpo, const ZOBJECT *pzobj);
#endif

#ifdef _HAS_MAPVIEWSETTINGS
int AddObject(LPMAPVIEWSETTINGS lpmvs, LPOBJECTTYPE lpot, int x, int y);
int AddEnemy(LPMAPVIEWSETTINGS lpmvs, LPOBJECTTYPE lpot, int x, int y);
void DrawMapObjects(HDC hdc, LPMAPVIEWSETTINGS lpmvs);
void DrawMapEnemies(HDC hdc, LPMAPVIEWSETTINGS lpmvs);
#endif
