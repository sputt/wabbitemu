#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <strsafe.h>
#include <mbstring.h>
#include <assert.h>

#include "wabbitemu.h"
#include "spasm.h"
#include "hash.h"
#include "ObjectGraphics.h"

#define MAX_DRAW_ENTRIES 32

#pragma pack(1)

typedef struct tagDRAWENTRY {
	BYTE x, w, y, h, z, d;
	WORD image;
	BYTE flags;
} DRAWENTRY, *LPDRAWENTRY;

#pragma pack()

// Reverse lookup of zelda labels
typedef struct {
	char *szValue;
	int Index;
} ZELDALABEL, *LPZELDALABEL;

hash_t *ZeldaImageHashTable = NULL;

extern OBJECTIMAGE ObjectImages[MAX_OBJECT_IMAGES];

void InitZeldaDrawQueue(void) {
	FILE *file = NULL;
	ZeldaImageHashTable = hash_init(5000, NULL);

	file = fopen("mapeditor.lab", "r");
	if (file != NULL) {
		char szBuffer[256];
		while (fgets(szBuffer, sizeof(szBuffer), file)) {
			if (strstr(szBuffer, "_GFX") == NULL && strstr(szBuffer, "_ANIM") == NULL && strstr(szBuffer, "_KEY") == NULL)
				continue;

			char szLine[256];
			WORD wLabel;
			unsigned char *psz = (unsigned char *) strtok(szBuffer, " =\r\n$");
			sprintf(szLine, ".dw %s", szBuffer);
			int Bytes = RunAssemblyWithArguments(szLine, (LPBYTE) &wLabel, sizeof(wLabel));
			if (wLabel != 0) {
				psz = (unsigned char *) strtok(NULL, " =\r\n$");
				if (strlen((char *) psz) == 5)
					psz++;

				if (hash_lookup(ZeldaImageHashTable, (char *) psz) == NULL) {
					LPZELDALABEL lpzl = (LPZELDALABEL) malloc(sizeof(ZELDALABEL));
					lpzl->szValue = (char *) _mbsdup(psz);
					lpzl->Index = wLabel;

					hash_insert(ZeldaImageHashTable, (void *) lpzl);
				}
			}
		}
		fclose(file);
	}
}


int CompareDrawEntries(const void *vpde1, const void *vpde2) {
	const LPDRAWENTRY pde1 = (LPDRAWENTRY) vpde1;
	const LPDRAWENTRY pde2 = (LPDRAWENTRY) vpde2;
	int key1 = pde1->h / 2 + pde1->y;
	int key2 = pde2->h / 2 + pde2->y;

	// Deal with the flats
	if (!(pde2->d != 0 || pde2->z != 0))
		return 1;
	if (!(pde1->d != 0 || pde1->z != 0))
		return -1;

	if (key1 == key2) {
		return 0;
	} else if (key1 > key2) {
		return 1;
	} else {
		return -1;
	}
}

static void DrawShadow(HDC hdc, LPDRAWENTRY lpde) {

	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	
	BITMAPINFOHEADER *bi = &bmi.bmiHeader;
	bi->biSize = sizeof(BITMAPINFOHEADER);
	bi->biWidth = lpde->w;
	bi->biHeight = lpde->h;
	bi->biPlanes = 1;
	bi->biBitCount = 32;
	bi->biCompression = BI_RGB;

	HDC hdcShad = CreateCompatibleDC(hdc);
	LPBYTE pBits;
	HBITMAP hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID *) &pBits, NULL, 0);

	HBITMAP hbmOld = (HBITMAP) SelectObject(hdcShad, hbm);

	RECT r = {0, 0, lpde->w, lpde->h};

	FillRect(hdcShad, &r, GetStockBrush(WHITE_BRUSH));


	double ShadRatio = ((double) lpde->z * (1.0/64.0));
	InsetRect(&r, lpde->w * ShadRatio, lpde->h * ShadRatio);
	
	SelectObject(hdcShad, GetStockObject(NULL_PEN));
	SelectObject(hdcShad, GetStockObject(BLACK_BRUSH));
	Ellipse(hdcShad, r.left, r.top + (r.bottom - r.top)/4, r.right, r.bottom);

	BYTE * pPixel = pBits;
	for (int y = 0; y < lpde->w; y++) {
		for (int x = 0; x < lpde->h; x++, pPixel+=4) {
			pPixel[3] = (pPixel[0] == 0xFF) ? 0x00 : 0x80;
		}
	}

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;	

	AlphaBlend(	hdc, lpde->x, lpde->y, lpde->w, lpde->h,
				hdcShad, 0, 0, lpde->w, lpde->h,
				bf);

	SelectObject(hdcShad, hbmOld);
	DeleteBitmap(hbm);
	DeleteDC(hdcShad);
}

#define D_NOSHADOW		1
#define D_INVERT		(1<<6)
#define D_HORIZONTAL	(1<<2)
#define D_SPARKLE		(1<<3)
#define D_PLAYER		(1<<4)

void DrawZeldaDrawEntries(HDC hdc) {
	DRAWENTRY SortedDrawEntries[MAX_DRAW_ENTRIES];
	LPDRAWENTRY lpDrawEntries = (LPDRAWENTRY) GetZeldaDrawQueue();
	int DrawEntryCount = GetZeldaDrawQueueCount();
	//assert(DrawEntryCount <= MAX_DRAW_ENTRIES);
	if (DrawEntryCount > MAX_DRAW_ENTRIES) {
		DrawEntryCount = MAX_DRAW_ENTRIES;
	}

	// Sort the draw entries
	memcpy(SortedDrawEntries, lpDrawEntries, sizeof(DRAWENTRY) * DrawEntryCount);
	qsort(SortedDrawEntries, DrawEntryCount, sizeof(DRAWENTRY), CompareDrawEntries);

	for (int i = 0; i < DrawEntryCount; i++) {
		LPDRAWENTRY lpde = &SortedDrawEntries[i];

		// Shadow stuff
		if (lpde->z != 0 && lpde->d != 0 && ((lpde->flags & D_NOSHADOW) == 0)) {
			DrawShadow(hdc, lpde);
		}
		

		// Draw it
		char szLabel[8];
		sprintf(szLabel, "%04X", lpde->image);
		LPZELDALABEL lpzl = (LPZELDALABEL) hash_lookup(ZeldaImageHashTable, szLabel);
		if (lpzl != NULL) {
			LPOBJECTIMAGE lpImage;
			
			if (_tcsicmp(_T("LINK_GFX"), ObjectImages[lpzl->Index].szName) == 0) {
				BYTE Flags;
				ReadVariable("game_flags", &Flags, 1);

				// pushing
				if (Flags & (1 << 0x02)) {
					lpImage = &ObjectImages[lpzl->Index + 8];
				} else {
					lpImage = &ObjectImages[lpzl->Index];
				}
			} else {
				lpImage = &ObjectImages[lpzl->Index];
			}
			
			BitBlt(hdc, lpde->x + (lpde->w - lpImage->cxImage) / 2, lpde->y + (lpde->h - lpImage->cyImage) - lpde->z, lpImage->cxImage, lpImage->cyImage,
				lpImage->hdcMask, lpImage->cxMargin, lpImage->cyMargin, SRCPAINT);
			
			DWORD rOp;
			if (lpde->flags & D_INVERT) {
				rOp = NOTSRCCOPY;
			} else {
				rOp = SRCAND;
			}
			BitBlt(hdc, lpde->x + (lpde->w - lpImage->cxImage) / 2, lpde->y + (lpde->h - lpImage->cyImage) - lpde->z, lpImage->cxImage, lpImage->cyImage,
				lpImage->hdc, lpImage->cxMargin, lpImage->cyMargin, rOp);
		}
	}
}