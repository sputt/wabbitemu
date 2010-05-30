#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <stdlib.h>

#include "ObjectGraphics.h"
#include "spasm.h"

OBJECTIMAGE ObjectImages[MAX_OBJECT_IMAGES];
// 2 so there is no 0 or 1
static int ImageCount = 2;

extern HINSTANCE g_hInstance;

typedef enum {
	ROG_SEARCH_LABEL,
	ROG_READ_INCLUDE,
} ROG_STATE;

#define MISC_COLOR RGB(100, 200, 100)
#define MASK_COLOR RGB(168, 230, 29)

HRESULT CreateObjectImage(const TCHAR *szImageFilename, LPOBJECTIMAGE lpoi) {
	
    HBITMAP hbmObjectMask = (HBITMAP) LoadImage(g_hInstance, szImageFilename, IMAGE_BITMAP,
            0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (hbmObjectMask == NULL)
		return E_FAIL;

	HDC hdcObjectMask = CreateCompatibleDC(NULL);
	SelectObject(hdcObjectMask, hbmObjectMask);
	BITMAP bm;

	GetObject(hbmObjectMask, sizeof(BITMAP), &bm);
	for (int i = 0; i < bm.bmWidth; i++) {
		for (int j = 0; j < bm.bmHeight; j++) {
			COLORREF cr = GetPixel(hdcObjectMask, i, j);
			if (cr == MASK_COLOR) {
				SetPixel(hdcObjectMask, i, j, RGB(0, 0, 0));
			} else {
				SetPixel(hdcObjectMask, i, j, RGB(255, 255, 255));
			}
		}
	}

	HDC hdcObject = CreateCompatibleDC(NULL);
	HBITMAP hbmObject = (HBITMAP) LoadImage(g_hInstance,  szImageFilename, IMAGE_BITMAP,
            0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_MONOCHROME);
	SelectObject(hdcObject, hbmObject);

	for (int i = 0; i < bm.bmWidth; i++) {
		for (int j = 0; j < bm.bmHeight; j++) {
			COLORREF cr = GetPixel(hdcObject, i, j);
			if (cr == MASK_COLOR) {
				SetPixel(hdcObject, i, j, RGB(255, 255, 255));
			} else if (cr == RGB(0, 0, 0)) {
				SetPixel(hdcObject, i, j, RGB(0, 0, 0));
			}
		}
	}

	lpoi->cxImage = bm.bmWidth;
	lpoi->cyImage = bm.bmHeight;
	lpoi->hdc = hdcObject;
	lpoi->hdcMask = hdcObjectMask;

	return S_OK;
}


LPOBJECTIMAGE GetObjectImage(int Index) {
	if (Index >= ImageCount)
		return NULL;

	return &ObjectImages[Index];
}


HRESULT ReadObjectGraphics(const TCHAR *szFilename) {
	FILE *file = _tfopen(szFilename, _T("r"));
	char szBuffer[256];
	char szLabel[256];
	ROG_STATE state = ROG_SEARCH_LABEL;
	int padding = 0;
	int xcount = 0, ycount = 0;

	if (file == NULL)
		return E_FAIL;

	while (fgets(szBuffer, sizeof(szBuffer), file)) {
		switch (state)
		{
		case ROG_SEARCH_LABEL:
			{
				if (strlen(szBuffer) < 2)
					break;

				if (szBuffer[0] == '.')
					break;

				if (szBuffer[0] == '#')
					break;

				if (isspace(szBuffer[0]))
					break;

				if (szBuffer[0] == ';')
					break;

				char *p = strtok(szBuffer, " \r\n");
				if (p == NULL)
					break;

				padding = 0;
				xcount = 1;
				ycount = 1;
				strcpy(szLabel, p);
				p = strtok(NULL, "\r\n");
				if (p != NULL) {
					padding = 1;
					sscanf(p, "with bm_map = %dx%d", &xcount, &ycount);
				}
				state = ROG_READ_INCLUDE;
				break;
			}
		case ROG_READ_INCLUDE:
			{
				// If it's not an include that directory follows a label, we are not interested
				if (szBuffer[0] != '#') {
					state = ROG_SEARCH_LABEL;
					break;
				}

				char *p = strtok(szBuffer, "\"");
				if (p == NULL) {
					state = ROG_SEARCH_LABEL;
					break;
				}

				p = strtok(NULL, "\"");

				char szLine[256];
				sprintf(szLine, "#define %s %d", szLabel, ImageCount);
				RunAssemblyWithArguments(szLine, NULL, 0);

				char szAnim[64];
				StringCbCopyA(szAnim, sizeof(szAnim), szLabel);
				char *psz = strstr(szAnim, "_gfx");
				if (psz != NULL) {
					strcpy(psz, "_anim");
					sprintf(szLine, "#define %s %d", szAnim, ImageCount);
					RunAssemblyWithArguments(szLine, NULL, 0);
				}

				TCHAR szFilename[MAX_PATH];
				MultiByteToWideChar(CP_ACP, 0, p, -1, szFilename, MAX_PATH);
				CreateObjectImage(szFilename, &ObjectImages[ImageCount]);

				MultiByteToWideChar(CP_ACP, 0, szLabel, -1, ObjectImages[ImageCount].szName, 64);
				StringCbPrintf(ObjectImages[ImageCount].szID, sizeof(ObjectImages[ImageCount].szID), _T("%d"), ImageCount);
				ObjectImages[ImageCount].cxMargin = padding;
				ObjectImages[ImageCount].cyMargin = padding;
				ObjectImages[ImageCount].cxImage -= padding * xcount * 2;
				ObjectImages[ImageCount].cyImage -= padding * ycount * 2;
				ObjectImages[ImageCount].cxImage /= xcount;
				ObjectImages[ImageCount].cyImage /= ycount;

				sprintf(szLine, "#define %s_width %d", szLabel, ObjectImages[ImageCount].cxImage);
				RunAssemblyWithArguments(szLine, NULL, 0);
				sprintf(szLine, "#define %s_height %d", szLabel, ObjectImages[ImageCount].cyImage);
				RunAssemblyWithArguments(szLine, NULL, 0);

				int BaseImage = ImageCount;

				for (int x_loop = 0; x_loop < xcount; x_loop++) {
					for (int y_loop = 0; y_loop < ycount; y_loop++) {
						sprintf(szLine, "#define %s%d %d", szLabel, x_loop * ycount + y_loop + 1, ImageCount);
						RunAssemblyWithArguments(szLine, NULL, 0);

						ObjectImages[ImageCount] = ObjectImages[BaseImage];
						ObjectImages[ImageCount].cxMargin = padding + (ObjectImages[ImageCount].cxImage + padding * 2) * x_loop;
						ObjectImages[ImageCount].cyMargin = padding + (ObjectImages[ImageCount].cyImage + padding * 2) * y_loop;
						ImageCount++;
					}
				}

				state = ROG_SEARCH_LABEL;
				break;
			}
		}
	}

	fclose(file);
	return S_OK;
}


void PopulateComboBoxWithGraphics(HWND hwndCombo) {
	ComboBox_ResetContent(hwndCombo);

	for (int i = 0; i < ImageCount; i++) {
		TCHAR szName[64];
		StringCbCopy(szName, sizeof(szName), ObjectImages[i].szName);
		//TCHAR *psz = _tcsstr(szName, _T("_gfx"));
		//if (psz != NULL)
		//	*psz = _T('\0');
		ComboBox_AddString(hwndCombo, szName);
		ComboBox_SetItemData(hwndCombo, i, (DWORD) ObjectImages[i].szID);
	}
}