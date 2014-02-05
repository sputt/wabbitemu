#ifndef COLORLCD_H
#define COLORLCD_H

#include "core.h"
#include "lcd.h"

#define COLOR_LCD_WIDTH 320
#define COLOR_LCD_HEIGHT 240
#define COLOR_LCD_DEPTH 3
#define COLOR_LCD_DISPLAY_SIZE COLOR_LCD_WIDTH * COLOR_LCD_HEIGHT * COLOR_LCD_DEPTH

typedef struct ColorLCD {
	LCDBase_t base;

	uint16_t current_register;
	unsigned char display[COLOR_LCD_DISPLAY_SIZE];
	uint16_t registers[0xFF];

	int read_buffer;
	int write_buffer;

	int read_step;
	int write_step;
} ColorLCD_t;

ColorLCD_t *ColorLCD_init(CPU_t *cpu, int model);

#endif