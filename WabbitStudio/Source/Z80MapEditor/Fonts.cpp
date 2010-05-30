#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <strsafe.h>

#define _FONTS_CPP
#include "Fonts.h"

/* Delete all fonts created for this program */
void DeleteFonts() {
	DeleteFont(g_Fonts.hfontMessage);
	DeleteFont(g_Fonts.hfontMessageBold);
	DeleteFont(g_Fonts.hfontHuge);

	DeleteFont(g_Fonts.hfontField);
	DeleteFont(g_Fonts.hfontFixed);

	DeleteFont(g_Fonts.hfontStatus);
	ZeroMemory(&g_Fonts, sizeof(FONTSET));
}

/* Create all of the fonts used by this program */
void CreateFonts(HWND hwnd, int Size, TEXTMETRIC *ptm) {
	NONCLIENTMETRICS ncm = {0};

	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	HDC hdc = GetDC(hwnd);
	if (hdc != NULL) {
		LOGFONT lf;
		int LogPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);

		ZeroMemory(&lf, sizeof(lf));
		lf.lfHeight = -MulDiv(Size * 9 / 8, LogPixelsY, 72);
		lf.lfWeight = FW_NORMAL;
		StringCbCopy(lf.lfFaceName, sizeof(lf.lfFaceName), _T("Courier New"));
		g_Fonts.hfontFixed = CreateFontIndirect(&lf);

		memcpy(&lf, &ncm.lfMessageFont, sizeof(lf));
		lf.lfHeight = -MulDiv(Size, LogPixelsY, 72);
		g_Fonts.hfontField = CreateFontIndirect(&lf);
		g_Fonts.hfontMessage = CreateFontIndirect(&lf);

		lf.lfHeight =-MulDiv(Size * 5 / 4, LogPixelsY, 72); 
		lf.lfWeight = FW_BOLD;
		g_Fonts.hfontMessageBold = CreateFontIndirect(&lf);

		g_Fonts.hfontStatus = CreateFontIndirect(&ncm.lfStatusFont);

		memcpy(&lf, &ncm.lfMessageFont, sizeof(lf));
		lf.lfHeight = -MulDiv(Size * 27 / 8, LogPixelsY, 72);
		g_Fonts.hfontHuge = CreateFontIndirect(&lf);

		memcpy(&lf, &ncm.lfMessageFont, sizeof(lf));
		lf.lfWeight = FW_BOLD;
		lf.lfHeight = -MulDiv(Size * 23 / 8, LogPixelsY, 72);
		g_Fonts.hfontLargeBold = CreateFontIndirect(&lf);		

		SelectObject(hdc, g_Fonts.hfontFixed);
		if (ptm != NULL)
			GetTextMetrics(hdc, ptm);
		ReleaseDC(hwnd, hdc);
	}
}
