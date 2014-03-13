#ifndef SCREENSHOTHANDLE_H
#define SCREENSHOTHANDLE_H

#include "calc.h"

extern int gif_base_delay_start;
BOOL get_screenshot_filename(TCHAR *);
void handle_screenshot(LPCALC lpCalc, LPVOID lParam);
uint8_t* generate_gif_image(LCDBase_t *lcd, int gif_size);

extern TCHAR pngext[5];
extern TCHAR gifext[5];

#endif