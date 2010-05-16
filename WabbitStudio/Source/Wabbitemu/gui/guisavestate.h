#ifndef GUISAVESTATE_H_
#define GUISAVESTATE_H_

#include "savestate.h"
#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))
INT_PTR gui_savestate(HWND, SAVESTATE_t*, char*);

#endif /*GUISAVESTATE_H_*/
