/*
 * guifaceplate.h
 *
 *  Created on: Apr 17, 2010
 *      Author: Chris
 */

#ifndef GUIFACEPLATE_H_
#define GUIFACEPLATE_H_

#ifdef USE_GDIPLUS2
int DrawFaceplateRegion(Graphics *graphics);
#else
int DrawFaceplateRegion(HDC, COLORREF);
#endif
HRGN GetRegion();

#endif /* GUIFACEPLATE_H_ */
