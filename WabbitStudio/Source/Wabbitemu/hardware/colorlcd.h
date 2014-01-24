#ifndef COLORLCD_H
#define COLORLCD_H

#include "core.h"

typedef struct ColorLCD {

} ColorLCD_t;

ColorLCD_t *ColorLCD_init(CPU_t *cpu, int model);
void ColorLCD_command(CPU_t *, device_t *);
void ColorLCD_data(CPU_t *, device_t *);

#endif