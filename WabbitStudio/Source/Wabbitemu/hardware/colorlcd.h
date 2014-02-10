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

	int current_register;
	uint8_t display[COLOR_LCD_DISPLAY_SIZE];
	uint8_t queued_image[COLOR_LCD_DISPLAY_SIZE];
	uint16_t registers[0xFF];

	int front;

	int read_buffer;
	int write_buffer;

	int read_step;
	int write_step;

	int frame_rate;
} ColorLCD_t;

ColorLCD_t *ColorLCD_init(CPU_t *cpu, int model);

#endif