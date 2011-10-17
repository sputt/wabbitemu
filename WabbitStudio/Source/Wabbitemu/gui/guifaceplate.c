#include "stdafx.h"

#include "gui.h"
#include "calc.h"


static POINT ptRgnEdge[] = {{75,675},
							{95,683},
							{262,682},
							{279,675},
							{316,535},
							{316,273},
							{37,273},
							{37,568}};

HRGN GetFaceplateRegion()
{
	unsigned int nPoints = (sizeof(ptRgnEdge) / sizeof(POINT)) * 2;
	POINT ptRgn[(sizeof(ptRgnEdge) / sizeof(POINT)) * 2];

	// Copy points and their reverses to the new array
	memcpy(ptRgn, ptRgnEdge, (nPoints / 2) * sizeof(POINT));

	u_int i;
	for (i = nPoints/2; i < nPoints; i++) {
		ptRgn[i].x = SKIN_WIDTH - ptRgnEdge[nPoints - i - 1].x;
		ptRgn[i].y = ptRgnEdge[nPoints - i - 1].y;
	}

	HRGN hrgn = CreatePolygonRgn(ptRgn, nPoints, WINDING);
	return hrgn;

}

int DrawFaceplateRegion(HDC hdc, COLORREF ref) {
	HRGN hrgn = GetFaceplateRegion();
	if (hrgn == NULL)
		return 1;
	HBRUSH hFaceplateColor = CreateSolidBrush(ref);
	FillRgn(hdc, hrgn, hFaceplateColor);
	DeleteObject(hFaceplateColor);
	DeleteObject(hrgn);
	return 0;
}