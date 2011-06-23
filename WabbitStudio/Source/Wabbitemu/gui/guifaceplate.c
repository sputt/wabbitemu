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

/*int DrawFaceplateRegion(Graphics *graphics, int colorVal) {
	int nPoints = (sizeof(ptRgnEdge) / sizeof(Point)) * 2;
	Point ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].X = 350 - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].Y = ptRgnEdge[nPoints - i - 1].y;
	}

	Color color(GetRValue(colorVal), GetGValue(colorVal), GetBValue(colorVal));
	SolidBrush solidBrush(color);
	Brush *brush = solidBrush.Clone();
	graphics->FillPolygon(brush, (Point *) ptRgn, nPoints, FillMode::FillModeWinding);
	return 0;
}*/

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

int DrawFaceplateRegion(HDC hdc, COLORREF ref) {
	HRGN hrgn = GetRegion();
	if (hrgn == NULL)
		return 1;
	HBRUSH hFaceplateColor = CreateSolidBrush(ref);
	FillRgn(hdc, hrgn, hFaceplateColor);
	DeleteObject(hFaceplateColor);
	DeleteObject(hrgn);
	return 0;
}