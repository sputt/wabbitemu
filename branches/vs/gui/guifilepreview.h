#ifndef GUIFILEPREVIEW_H_
#define GUIFILEPREVIEW_H_

#include "guiopenfile.h"
HWND CreateFilePreviewPane(HWND, int, int, int, int, OFNHookOptions *);
LRESULT ShowFilePreview(HWND, LPSTR);

#endif /*GUIFILEPREVIEW_H_*/
