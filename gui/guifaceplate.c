#include "stdafx.h"

#include "calc.h"


static POINT ptRgnEdge[] = {{75,675},
							{95,683},
							{262,682},
							{279,675},
							{316,535},
							{316,273},
							{37,273},
							{37,568}};

/*int DrawFaceplateRegion(Graphics *graphics) {
	int nPoints = (sizeof(ptRgnEdge) / sizeof(Point)) * 2;
	Point ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].X = 350 - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].Y = ptRgnEdge[nPoints - i - 1].y;
	}

	int colorVal = calcs[gslot].FaceplateColor;
	Color color(GetRValue(colorVal), GetGValue(colorVal), GetBValue(colorVal));
	SolidBrush solidBrush(color);
	Brush *brush = solidBrush.Clone();
	graphics->FillPolygon(brush, (Point *) ptRgn, nPoints, FillMode::FillModeWinding);
	return 0;
}*/
int DrawFaceplateRegion(HDC hdc) {
	unsigned int nPoints = (sizeof(ptRgnEdge) / sizeof(POINT)) * 2;
	POINT ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	u_int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].x = 350 - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].y = ptRgnEdge[nPoints - i - 1].y;
	}

	HRGN hrgn = CreatePolygonRgn(ptRgn, nPoints, WINDING);
	if (hrgn == NULL)
		return 1;
	int color = calcs[gslot].FaceplateColor;
	HBRUSH hFaceplateColor = CreateSolidBrush(color);
	FillRgn(hdc, hrgn, hFaceplateColor);
	DeleteObject(hFaceplateColor);
	return 0;
}

HRGN GetRegion()
{
	unsigned int nPoints = (sizeof(ptRgnEdge) / sizeof(POINT)) * 2;
	POINT ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	u_int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].x = 350 - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].y = ptRgnEdge[nPoints - i - 1].y;
	}

	HRGN hrgn = CreatePolygonRgn(ptRgn, nPoints, WINDING);
	return hrgn;

}

/*int AlphaBlendFaceplate(HDC hdc)
{
	/*HMENU hmenu = GetMenu(hwnd);
	int cyMenu;
	if (hmenu == NULL) {
		cyMenu = 0;
	} else {
		cyMenu = GetSystemMetrics(SM_CYMENU);
	}*
	unsigned int nPoints = (sizeof(ptRgnEdge) / sizeof(POINT)) * 2;
	POINT ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].x = 350 - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].y = ptRgnEdge[nPoints - i - 1].y;
	}
	/*for (i = 0; i < nPoints; i++) {
		ptRgn[i].x += GetSystemMetrics(SM_CXFIXEDFRAME);
		ptRgn[i].y += cyMenu + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME);
	}*

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	HRGN hrgn = CreatePolygonRgn(ptRgn, nPoints, WINDING);
	HDC hdcOverlay = CreateCompatibleDC(hdc);
	HBRUSH hFaceplateColor = CreateSolidBrush(/*calcs[gslot].faceplateColor* RGB(255,0,0));
	RECT rc;
	GetClientRect(calcs[gslot].hwndFrame, &rc);
	FillRect(hdcOverlay, &rc, hFaceplateColor);
	//FillRgn(hdcOverlay, hrgn, hFaceplateColor);
	if (hrgn == NULL)
		return 1;
	AlphaBlend(hdc, 0, 0, calcs[gslot].rectSkin.right, calcs[gslot].rectSkin.bottom, hdcOverlay,
				calcs[gslot].rectSkin.left, calcs[gslot].rectSkin.top, calcs[gslot].rectSkin.right, calcs[gslot].rectSkin.bottom, bf);
	return 0;
}*/
