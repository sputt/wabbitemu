#ifndef DISPLAYOPTIONSTEST_H_
#define DISPLAYOPTIONSTEST_H_

#include "core.h"
#include "lcd.h"

u_char *displayoptionstest_draw_scroll(int,double,double);
u_char *displayoptionstest_draw_bounce(int,double,double);
u_char *displayoptionstest_draw_gradient(int,double,double);
void fastcopy(CPU_t*);

#endif /*DISPLAYOPTIONSTEST_H_*/
