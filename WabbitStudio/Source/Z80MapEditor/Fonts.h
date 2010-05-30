#pragma once

#include <windows.h>

void DeleteFonts();
void CreateFonts(HWND hwnd, int Size, TEXTMETRIC *tm);

typedef struct tagFONTSET {
	int BaseSize;
	HFONT hfontField;
	HFONT hfontFixed;
	HFONT hfontMessage;
	HFONT hfontMessageBold;
	HFONT hfontStatus;
	HFONT hfontLargeBold;
	HFONT hfontHuge;
} FONTSET;

#ifdef _FONTS_CPP
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN FONTSET g_Fonts;

#undef EXTERN

