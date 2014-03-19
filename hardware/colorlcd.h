#ifndef COLORLCD_H
#define COLORLCD_H

#include "core.h"
#include "lcd.h"

#define COLOR_LCD_WIDTH 320
#define COLOR_LCD_HEIGHT 240
#define COLOR_LCD_DEPTH 3
#define COLOR_LCD_BUFFERS 3
#define COLOR_LCD_DISPLAY_SIZE COLOR_LCD_WIDTH * COLOR_LCD_HEIGHT * COLOR_LCD_DEPTH
#define MAX_BACKLIGHT_LEVEL 32
#define BACKLIGHT_OFF_DELAY 0.002

typedef struct ColorLCD {
	LCDBase_t base;

	int current_register;
	uint8_t display[COLOR_LCD_DISPLAY_SIZE];
	uint8_t queued_image[COLOR_LCD_DISPLAY_SIZE];
	uint16_t registers[0xFF];
	BOOL register_breakpoint[0xFF];

	int front;

	double last_draw;
	int draw_gate;
	double line_time;
	BOOL is_drawing;

	BOOL panic_mode;

	int read_buffer;
	int write_buffer;

	int read_step;
	int write_step;

	int frame_rate;
	int front_porch, back_porch;
	int display_lines;
	int clocks_per_line;
	int clock_divider;

	BOOL backlight_active;
	double backlight_off_elapsed;
} ColorLCD_t;

ColorLCD_t *ColorLCD_init(CPU_t *cpu, int model);
void ColorLCD_set_register(CPU_t *cpu, ColorLCD_t *lcd, uint16_t reg, uint16_t value);

#endif