#include "stdafx.h"

#include "colorlcd.h"

#define PIXEL_OFFSET(x, y) (y * COLOR_LCD_WIDTH + x) * COLOR_LCD_DEPTH 
#define TRUCOLOR(color, bits) color * (0xFF / ((1 << bits) - 1))

#define DRIVER_CODE_REG 0x00
#define DRIVER_OUTPUT_CONTROL1_REG 0x01
#define ENTRY_MODE_REG 0x03
#define DISPLAY_CONTROL1_REG 0x07
#define CUR_Y_REG 0x20
#define CUR_X_REG 0x21
#define GRAM_REG 0x22
#define WINDOW_HORZ_START_REG 0x50
#define WINDOW_HORZ_END_REG 0x51
#define WINDOW_VERT_START_REG 0x52
#define WINDOW_VERT_END_REG 0x53
#define GATE_SCAN_CONTROL_REG 0x60
#define BASE_IMAGE_DISPLAY_CONTROL_REG 0x61
#define VERTICAL_SCROLL_CONTROL_REG 0x6A
#define PARTIAL_IMAGE1_DISPLAY_POSITION_REG 0x80
#define PARTIAL_IMAGE1_START_LINE_REG 0x81
#define PARTIAL_IMAGE1_END_LINE_REG 0x82
#define PARTIAL_IMAGE2_DISPLAY_POSITION_REG 0x83
#define PARTIAL_IMAGE2_START_LINE_REG 0x84
#define PARTIAL_IMAGE2_END_LINE_REG 0x85

// 1
#define INTERLACED_MASK BIT(10)

// 3
#define CUR_DIR_MASK BIT(3)
#define ROW_INC_MASK BIT(4)
#define COL_INC_MASK BIT(5)
#define ORG_MASK BIT(7)
#define BGR_MASK BIT(12)

// 7
#define DISPLAY_ON_MASK (BIT(0) | BIT(1))
#define SHOW_BASE_MASK BIT(8)
#define SHOW_PARTIAL1_MASK BIT(12)
#define SHOW_PARTIAL2_MASK BIT(13)
#define COLOR8_MASK BIT(3)

#define FLIP_COLS_MASK BIT(8)
#define EIGHTEEN_BIT_MASK BIT(14)
#define UNPACKED_MASK BIT(15)
#define TRI_MASK EIGHTEEN_BIT_MASK | UNPACKED_MASK
#define NDL_MASK BIT(2)

// 60
#define BASE_START_MASK (BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5))
#define BASE_NLINES_MASK (BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13))
#define GATE_SCAN_DIR_MASK BIT(15)

// 61
#define SCROLL_ENABLED_MASK BIT(1)
#define LEVEL_INVERT_MASK BIT(1)

// 6a
#define SCROLL_MASK 0x1FF

// 80
#define p2_POS_MASK 0x1FF

// 81
#define p2_START_MASK 0x1FF

// 82
#define p2_END_MASK 0x1FF

// 83
#define P2_POS_MASK 0x1FF

// 84
#define P2_START_MASK 0x1FF

// 85
#define P2_END_MASK 0x1FF

static int read_pixel(ColorLCD_t *lcd);
static void write_pixel18(ColorLCD_t *lcd);
static void write_pixel16(ColorLCD_t *lcd);
static void update_x(ColorLCD_t *lcd, BOOL should_update_row);
static void update_y(ColorLCD_t *lcd, BOOL should_update_col);

static void ColorLCD_reset(CPU_t *);
static void ColorLCD_LCDreset(ColorLCD_t *lcd);
static void ColorLCD_free(CPU_t *);
static void ColorLCD_command(CPU_t *, device_t *);
static void ColorLCD_data(CPU_t *, device_t *);
uint8_t *ColorLCD_Image(LCDBase_t *);

ColorLCD_t *ColorLCD_init(CPU_t *cpu, int model) {
	ColorLCD_t* lcd = (ColorLCD_t *)malloc(sizeof(ColorLCD_t));
	if (!lcd) {
		printf("Couldn't allocate memory for LCD\n");
		exit(1);
	}
	
	ColorLCD_LCDreset(lcd);
	return lcd;
}


static uint16_t get_register(ColorLCD_t *lcd, uint16_t reg) {
	switch (reg) {
	case DRIVER_CODE_REG:
		return 0x9335;
	default:
		return lcd->registers[reg];
	}
}

static uint16_t get_register_mask(uint8_t index) {
	switch (index) {
	case 0x01:
		return 0x0500;
	case 0x03:
		return 0xD0B8;
	case 0x04:
		return 0x0777;
	case 0x05:
		return 0x0003;
	case 0x06:
		return 0x0001;
	case 0x07:
		return 0x313B;
	case 0x08:
		return 0xFFFF;
	case 0x09:
		return 0x073F;
	case 0x0A:
		return 0x000F;
	case 0x0C:
		return 0x7133;
	case 0x0D:
		return 0x01FF;
	case 0x0F:
		return 0x001B;
	case 0x10:
		return 0x17F3;
	case 0x11:
		return 0x0777;
	case 0x12:
		return 0x008F;
	case 0x13:
		return 0x1F00;
	case 0x20: 
		return 0x00FF;
	case 0x21: 
		return 0x01FF;
	case 0x29: 
		return 0x003F;
	case 0x2B:
		return 0x000F;
	case 0x30: 
		return 0x0707;
	case 0x31: 
		return 0x0707;
	case 0x32: 
		return 0x0707;
	case 0x35:
		return 0x0707;
	case 0x36:
		return 0x1F0F;
	case 0x37:
		return 0x0707;
	case 0x38:
		return 0x0707;
	case 0x39:
		return 0x0707;
	case 0x3C:
		return 0x0707;
	case 0x3D:
		return 0x1F0F;
	case 0x50:
		return 0x00FF;
	case 0x51:
		return 0x00FF;
	case 0x52:
		return 0x01FF;
	case 0x53:
		return 0x01FF;
	case 0x60:
		return 0xBF3F;
	case 0x61:
		return 0x0007;
	case 0x6A:
		return 0x01FF;
	case 0x80:
		return 0x01FF;
	case 0x81:
		return 0x01FF;
	case 0x82:
		return 0x01FF;
	case 0x83:
		return 0x01FF;
	case 0x84:
		return 0x01FF;
	case 0x85:
		return 0x01FF;
	case 0x90:
		return 0x031F;
	case 0x92:
		return 0x0700;
	case 0x95:
		return 0x0300;
	case 0x97:
		return 0x0F00;
	case 0xA1:
		return 0x083F;
	case 0xA2:
		return 0x0001;
	case 0xA3:
		return 0x0003;
	case 0xA4:
		return 0xF7FF;
	case 0xA5:
		return 0xFFFF;
	case 0xE2:
		return 0x9FFF;
	case 0xE3:
		return 0xF0FF;
	case 0xE4:
		return 0x1F3C;
	case 0xE5:
		return 0x1FC7;
	case 0xE6:
		return 0x0001;
	case 0xE7:
		return 0x3FFF;
	case 0xE8:
		return 0x003F;
	case 0xE9:
		return 0x000F;
	case 0xEA:
		return 0xFFFF;
	case 0xEB:
		return 0xB37F;
	case 0xEC:
		return 0x7FFF;
	case 0xED:
		return 0x7FFF;
	case 0xEE:
		return 0x0F7F;
	case 0xEF:
		return 0xFB37;
	case 0xFE:
		return 0x0001;
	case 0xFF:
		return 0x000F;
	default:
		return 0xFFFF;
	}
}


static void reset_y(ColorLCD_t *lcd, uint16_t mode) {
	if (mode & ROW_INC_MASK) {
		lcd->base.y = lcd->registers[WINDOW_HORZ_START_REG];
	} else {
		lcd->base.y = lcd->registers[WINDOW_HORZ_END_REG];
	}
}

static void reset_x(ColorLCD_t *lcd, uint16_t mode) {
	if (mode & COL_INC_MASK) {
		lcd->base.x = lcd->registers[WINDOW_VERT_START_REG];
	} else {
		lcd->base.x = lcd->registers[WINDOW_VERT_END_REG];
	}
}

static void set_register(ColorLCD_t *lcd, uint16_t reg, uint16_t value) {
	value = value & get_register_mask(reg);
	int mode = lcd->registers[ENTRY_MODE_REG];
	uint16_t old_value = lcd->registers[reg];
	lcd->registers[reg] = value;
	switch (reg) { 
	case ENTRY_MODE_REG:
		reset_x(lcd, value);
		reset_y(lcd, value);
		break;
	case DISPLAY_CONTROL1_REG:
		lcd->base.active = value & DISPLAY_ON_MASK ? TRUE : FALSE;
		break;
	case CUR_X_REG:
	case CUR_Y_REG: {
		if (mode & ORG_MASK) {
			if (reg == CUR_Y_REG) {
				reset_y(lcd, mode);
			} else {
				reset_x(lcd, mode);
			}
		} else {
			lcd->base.x = lcd->registers[CUR_X_REG];
			lcd->base.y = lcd->registers[CUR_Y_REG];
		}
		break;
	}
	case WINDOW_HORZ_START_REG: {
		if ((mode & ORG_MASK) && (mode & COL_INC_MASK)) {
			lcd->base.y = lcd->registers[WINDOW_HORZ_START_REG];
		}
		break;
	}
	case WINDOW_HORZ_END_REG: {
		if ((mode & ORG_MASK) && !(mode & COL_INC_MASK)) {
			lcd->base.y = lcd->registers[WINDOW_HORZ_END_REG];
		}
		break;
	}
	case WINDOW_VERT_START_REG: {
		if ((mode & ORG_MASK) && (mode & ROW_INC_MASK)) {
			lcd->base.x = lcd->registers[WINDOW_VERT_START_REG];
		}
		break;
	}
	case WINDOW_VERT_END_REG: {
		if ((mode & ORG_MASK) && !(mode & ROW_INC_MASK)) {
			lcd->base.x = lcd->registers[WINDOW_VERT_END_REG];
		}
		break;
	}
	}
}

/*
* Free space belonging to lcd
*/
static void ColorLCD_free(CPU_t *cpu) {
	free(cpu->pio.lcd);
}

void ColorLCD_command(CPU_t *cpu, device_t *device) {
	ColorLCD_t *lcd = (ColorLCD_t *) device->aux;

	if (cpu->output) {
		// BIG ENDIAN
		lcd->current_register = cpu->bus | (lcd->current_register << 8);
		if (lcd->read_step) {
			lcd->read_step = 0;
		}

		if (lcd->write_step) {
			lcd->write_step = 0;
		}

		cpu->output = FALSE;
	} else if (cpu->input) {
		cpu->bus = 0;
		cpu->input = FALSE;
	}
}

void ColorLCD_data(CPU_t *cpu, device_t *device) {
	ColorLCD_t *lcd = (ColorLCD_t *)device->aux;
	uint16_t reg_index = lcd->current_register & 0xFF;
	if (cpu->output) {
		lcd->write_buffer = lcd->write_buffer << 8 | cpu->bus;
		if (reg_index == GRAM_REG) {
			int mode = lcd->registers[ENTRY_MODE_REG] & TRI_MASK;
			if (mode & EIGHTEEN_BIT_MASK) {
				lcd->write_step++;
				if (lcd->write_step >= 3) {
					lcd->write_step = 0;
					write_pixel18(lcd);
				}
			} else {
				lcd->write_step = !lcd->write_step;
				if (!lcd->write_step) {
					write_pixel16(lcd);
				}
			}
		} else {
			lcd->write_step = !lcd->write_step;
			if (!lcd->write_step) {
				set_register(lcd, reg_index, lcd->write_buffer);
			}
		}

		cpu->output = FALSE;
	} else if (cpu->input) {
		if (reg_index == GRAM_REG) {
			// read from LCD mem
			int pixel = lcd->read_buffer;
			cpu->bus = pixel >> 16;
			lcd->read_step = !lcd->read_step;
			if (!lcd->read_step) {
				pixel = read_pixel(lcd);
				pixel = ((pixel & 0x3e0000) << 2
					| (pixel & 0x3f00) << 5
					| (pixel & 0x3e) << 7);
			} else {
				pixel <<= 8;
			}

			lcd->read_buffer = pixel;
		} else {
			int val = get_register(lcd, reg_index);
			lcd->read_step = !lcd->read_step;
			if (lcd->read_step) {
				cpu->bus = val >> 8;
			} else {
				cpu->bus = val & 0xFF;
			}
		}

		cpu->input = FALSE;
	}
}

static int read_pixel(ColorLCD_t *lcd) {
	int x = lcd->base.x % COLOR_LCD_WIDTH;
	int y = lcd->base.y % COLOR_LCD_HEIGHT;
	uint8_t *pixel_ptr = &lcd->display[PIXEL_OFFSET(x, y)];
	int pixel;
	if (lcd->registers[ENTRY_MODE_REG] & BGR_MASK) {
		pixel = (pixel_ptr[2] << 16) | (pixel_ptr[1] << 8) | pixel_ptr[0];
	} else {
		pixel = (pixel_ptr[0] << 16) | (pixel_ptr[1] << 8) | pixel_ptr[2];
	}

	return pixel;
}

static void write_pixel(ColorLCD_t *lcd, int red, int green, int blue) {
	int x = lcd->base.x;
	int y = lcd->base.y;
	int mode = lcd->registers[ENTRY_MODE_REG];

	if (lcd->registers[DRIVER_OUTPUT_CONTROL1_REG] & FLIP_COLS_MASK) {
		y = COLOR_LCD_HEIGHT - y - 1;
	}

	uint8_t *pixel_ptr = &lcd->display[PIXEL_OFFSET(x, y)];
	if (mode & BGR_MASK) {
		pixel_ptr[2] = red & 0x3F;
		pixel_ptr[1] = green & 0x3F;
		pixel_ptr[0] = blue & 0x3F;
	} else {
		pixel_ptr[0] = red & 0x3F;
		pixel_ptr[1] = green & 0x3F;
		pixel_ptr[2] = blue & 0x3F;
	}

	// TODO: fix
	//lcd->time = tc_elapsed(cpu->timer_c);
	if (mode & CUR_DIR_MASK) {
		update_x(lcd, TRUE);
	} else {
		update_y(lcd, TRUE);
	}
}

static void write_pixel18(ColorLCD_t *lcd) {
	int mode = lcd->registers[ENTRY_MODE_REG];
	int pixel_val;
	int red, green, blue;
	if (mode & UNPACKED_MASK) {
		pixel_val = lcd->write_buffer & 0xfcfcfc;
		red = (pixel_val >> 18);
		green = (pixel_val >> 10);
		blue = (pixel_val >> 2);
	} else {
		pixel_val = lcd->write_buffer & 0x3ffff;
		red = (pixel_val >> 12);
		green = (pixel_val >> 6);
		blue = pixel_val;
	}

	write_pixel(lcd, red, green, blue);
}

static void write_pixel16(ColorLCD_t *lcd) {
	int pixel_val = lcd->write_buffer;
	int red = ((pixel_val >> 10) & ~1) | ((pixel_val >> 15) & 1);
	int green = (pixel_val >> 5);
	int blue = (pixel_val << 1) | ((pixel_val >> 4) & 1);

	write_pixel(lcd, red, green, blue);
}

static void update_y(ColorLCD_t *lcd, BOOL should_update) {
	int mode = lcd->registers[ENTRY_MODE_REG];
	if (mode & ROW_INC_MASK) {
		if (lcd->base.y < lcd->registers[WINDOW_HORZ_END_REG]) {
			lcd->base.y++;
			return;
		}

		// back to top of the window
		lcd->base.y = lcd->registers[WINDOW_HORZ_START_REG];
	} else {
		if (lcd->base.y > lcd->registers[WINDOW_HORZ_START_REG]) {
			lcd->base.y--;
			return;
		}

		// to bottom of the window
		lcd->base.y = lcd->registers[WINDOW_HORZ_END_REG];
	}

	if (should_update) {
		update_x(lcd, FALSE);
	}
}

static void update_x(ColorLCD_t *lcd, BOOL should_update) {
	int mode = lcd->registers[ENTRY_MODE_REG];
	if (mode & COL_INC_MASK) {
		if (lcd->base.x < lcd->registers[WINDOW_VERT_END_REG]) {
			lcd->base.x++;
			return;
		}

		// back to the beginning of the window
		lcd->base.x = lcd->registers[WINDOW_VERT_START_REG];
	} else {
		if (lcd->base.x > lcd->registers[WINDOW_VERT_START_REG]) {
			lcd->base.x--;
			return;
		}

		// to the end of the window
		lcd->base.x = lcd->registers[WINDOW_VERT_END_REG];
	}

	if (should_update) {
		update_y(lcd, FALSE);
	}
}

void ColorLCD_LCDreset(ColorLCD_t *lcd) {
	ZeroMemory(lcd, sizeof(ColorLCD_t));

	lcd->base.free = &ColorLCD_free;
	lcd->base.reset = &ColorLCD_reset;
	lcd->base.command = (devp)&ColorLCD_command;
	lcd->base.data = (devp)&ColorLCD_data;
	lcd->base.image = &ColorLCD_Image;

	lcd->base.width = COLOR_LCD_WIDTH;
	lcd->base.display_width = COLOR_LCD_WIDTH;
	lcd->base.height = COLOR_LCD_HEIGHT;

	lcd->base.x = 319;
	lcd->base.y = 239;
	lcd->base.bytes_per_pixel = 3;
}

void ColorLCD_reset(CPU_t *cpu) {
	ColorLCD_t *lcd = (ColorLCD_t *) cpu->pio.lcd;
	ColorLCD_LCDreset(lcd);
}

static void draw_row_floating(uint8_t *dest, int size) {
	for (int i = 0; i < size; i++) {
		if (dest[i] < 0x3f) {
			dest[i]++;
		}
	}
}

static void draw_row_image(ColorLCD_t *lcd, uint8_t *dest, uint8_t *src, int size) {
	uint8_t *red, *green, *blue;
	BOOL level_invert = (lcd->registers[BASE_IMAGE_DISPLAY_CONTROL_REG] & LEVEL_INVERT_MASK);
	BOOL color8bit = (lcd->registers[DISPLAY_CONTROL1_REG] & COLOR8_MASK);

	if (level_invert) {
		red = src;
		green = src + 1;
		blue = src + 2;

		BOOL flip_rows = (lcd->registers[GATE_SCAN_CONTROL_REG] & GATE_SCAN_DIR_MASK);

		if (flip_rows) {
			red += size - 3;
			green += size - 3;
			blue += size - 3;
			for (int i = 0; i < size; i += 3) {
				dest[i] = red[-i] ^ 0x3f;
				dest[i+1] = green[-i] ^ 0x3f;
				dest[i+2] = blue[-i] ^ 0x3f;
			}
		} else {
			for (int i = 0; i < size; i += 3) {
				dest[i] = red[i] ^ 0x3f;
				dest[i+1] = green[i] ^ 0x3f;
				dest[i+2] = blue[i] ^ 0x3f;
			}
		}
	} else {
		for (int i = 0; i < size; i++) {
			dest[i] = TRUCOLOR(src[i], 6);
		}
	}

	if (color8bit) {
		for (int i = 0; i < size; i++) {
			dest[i] = (dest[i] >> 5) * 0x3f;
		}
	}
}

static void draw_partial_image(ColorLCD_t *lcd, uint8_t *dest, uint8_t *src,
	int offset, int size) {

	if (offset > COLOR_LCD_WIDTH * COLOR_LCD_DEPTH) {
		offset %= COLOR_LCD_WIDTH * COLOR_LCD_DEPTH;
	}

	if (offset + size > COLOR_LCD_WIDTH * COLOR_LCD_DEPTH) {
		int right_margin_size = (COLOR_LCD_WIDTH * COLOR_LCD_DEPTH) - offset;
		int left_margin_size = size - right_margin_size;
		BOOL flip_rows = (lcd->registers[GATE_SCAN_CONTROL_REG] & GATE_SCAN_DIR_MASK);

		if (flip_rows) {
			draw_row_image(lcd, dest + left_margin_size, src + offset, right_margin_size);
			draw_row_image(lcd, dest, src, left_margin_size);
		} else {
			draw_row_image(lcd, dest, src + offset, right_margin_size);
			draw_row_image(lcd, dest + right_margin_size, src, left_margin_size);
		}
	} else {
		draw_row_image(lcd, dest, src + offset, size);
	}
}

static void draw_nondisplay_area(uint8_t *dest, int size, int ndl_color) {
	memset(dest, ndl_color, size);
}

static void draw_row(ColorLCD_t *lcd, uint8_t *dest, uint8_t* src,
	int start_x, int display_width,
	int imgpos1, int imgoffs1, int imgsize1,
	int imgpos2, int imgoffs2, int imgsize2)
{
	uint8_t *optr;
	uint8_t interlace_buf[COLOR_LCD_WIDTH * COLOR_LCD_DEPTH];
	int i;

	int non_display_area_color = lcd->registers[BASE_IMAGE_DISPLAY_CONTROL_REG] & NDL_MASK ? 0x3F : 0x00;
	BOOL interlace_cols = (lcd->registers[DRIVER_OUTPUT_CONTROL1_REG] & INTERLACED_MASK);

	if (interlace_cols) {
		optr = interlace_buf;
	} else {
		optr = dest;
	}

	if (start_x) {
		if (interlace_cols) {
			draw_nondisplay_area(optr, start_x, 0);
		} else {
			draw_row_floating(optr, start_x);
		}
		optr += start_x;
	}

	int n = COLOR_LCD_WIDTH * COLOR_LCD_DEPTH - start_x - display_width;
	if (imgsize1 != n && imgsize2 != n) {
		draw_nondisplay_area(optr, n, non_display_area_color);
	}

	if (imgsize1) {
		draw_partial_image(lcd, optr + imgpos1, src, imgoffs1, imgsize1);
	}

	if (imgsize2) {
		draw_partial_image(lcd, optr + imgpos2, src, imgoffs2, imgsize2);
	}

	optr += n;

	if (display_width) {
		if (interlace_cols) {
			draw_nondisplay_area(optr, display_width, 0);
		} else {
			draw_row_floating(optr, display_width);
		}
		optr += display_width;
	}

	if (interlace_cols) {
		optr = interlace_buf;
		for (i = 0; i < COLOR_LCD_WIDTH / 2; i++, optr += 3) {
			*dest++ = optr[0];
			*dest++ = optr[1];
			*dest++ = optr[2];
			*dest++ = optr[(COLOR_LCD_DEPTH * COLOR_LCD_WIDTH) / 2];
			*dest++ = optr[(COLOR_LCD_DEPTH * COLOR_LCD_WIDTH) / 2 + 1];
			*dest++ = optr[(COLOR_LCD_DEPTH * COLOR_LCD_WIDTH) / 2 + 2];
		}
	}
}

uint8_t *ColorLCD_Image(LCDBase_t *lcdBase) {
	ColorLCD_t *lcd = (ColorLCD_t *) lcdBase;
	uint8_t *buffer = (uint8_t *) malloc(COLOR_LCD_DISPLAY_SIZE);
	ZeroMemory(buffer, COLOR_LCD_DISPLAY_SIZE);

	int p1pos, p1start, p1end, p1width, p2pos, p2start, p2end, p2width;
	int imgpos1, imgoffs1, imgsize1;
	int imgpos2, imgoffs2, imgsize2;

	if (!lcdBase->active) {
		return buffer;
	}

	int start_x = (lcd->registers[GATE_SCAN_CONTROL_REG] & BASE_START_MASK) << 3;
	int pixel_width = ((lcd->registers[GATE_SCAN_CONTROL_REG] & BASE_NLINES_MASK) >> 5) + 8;
	if (start_x > COLOR_LCD_WIDTH) {
		start_x = COLOR_LCD_WIDTH;
	}

	if (pixel_width > COLOR_LCD_WIDTH - start_x) {
		pixel_width = COLOR_LCD_WIDTH - start_x;
	}

	start_x = start_x * COLOR_LCD_DEPTH;
	int display_width = (COLOR_LCD_WIDTH - (start_x + pixel_width)) * 3;

	if (lcd->registers[DISPLAY_CONTROL1_REG] & SHOW_BASE_MASK) {
		p2pos = 0;
		p2width = pixel_width;
		if (lcd->registers[BASE_IMAGE_DISPLAY_CONTROL_REG] & SCROLL_ENABLED_MASK) {
			p2start = (lcd->registers[VERTICAL_SCROLL_CONTROL_REG] & SCROLL_MASK) * COLOR_LCD_DEPTH;
		} else {
			p2start = 0;
		}

		p1pos = p1start = p1width = 0;
	} else {
		if (lcd->registers[DISPLAY_CONTROL1_REG] & SHOW_PARTIAL1_MASK) {
			p1pos = (lcd->registers[PARTIAL_IMAGE1_DISPLAY_POSITION_REG] & p2_POS_MASK) % COLOR_LCD_WIDTH;
			p1start = (lcd->registers[PARTIAL_IMAGE1_START_LINE_REG] & p2_START_MASK) % COLOR_LCD_WIDTH;
			p1end = (lcd->registers[PARTIAL_IMAGE1_END_LINE_REG] & p2_END_MASK) % COLOR_LCD_WIDTH;

			p1width = p1end + 1 - p1start;
			if (p1width < 0)
				p1width += COLOR_LCD_WIDTH;

			if (p1pos > pixel_width) {
				p1pos = pixel_width;
			}

			if (p1pos + p1width > pixel_width) {
				p1width = pixel_width - p1pos;
			}
		} else {
			p1pos = p1start = p1width = 0;
		}

		if (lcd->registers[DISPLAY_CONTROL1_REG] & SHOW_PARTIAL2_MASK) {
			p2pos = (lcd->registers[PARTIAL_IMAGE2_DISPLAY_POSITION_REG] & P2_POS_MASK) % COLOR_LCD_WIDTH;
			p2start = (lcd->registers[PARTIAL_IMAGE2_START_LINE_REG] & P2_START_MASK) % COLOR_LCD_WIDTH;
			p2end = (lcd->registers[PARTIAL_IMAGE2_END_LINE_REG] & P2_END_MASK) % COLOR_LCD_WIDTH;

			p2width = p2end + 1 - p2start;
			if (p2width < 0) {
				p2width += COLOR_LCD_WIDTH;
			}

			if (p2pos > pixel_width) {
				p2pos = pixel_width;
			}

			if (p2pos + p2width > pixel_width) {
				p2width = pixel_width - p2pos;
			}
		} else {
			p2pos = p2start = p2width = 0;
		}
	}

	BOOL flip_rows = (lcd->registers[GATE_SCAN_CONTROL_REG] & GATE_SCAN_DIR_MASK);
	if (flip_rows) {
		int tmp = display_width;
		display_width = start_x;
		start_x = tmp;

		p1pos = COLOR_LCD_WIDTH - (p1pos + p1width);
		p2pos = COLOR_LCD_WIDTH - (p2pos + p2width);
	}

	uint8_t *src = lcd->display;
	uint8_t *dest = buffer;

	imgpos1 = p2pos * COLOR_LCD_DEPTH;
	imgoffs1 = p2start * COLOR_LCD_DEPTH;
	imgsize1 = p2width * COLOR_LCD_DEPTH;
	imgpos2 = p1pos * COLOR_LCD_DEPTH;
	imgoffs2 = p1start * COLOR_LCD_DEPTH;
	imgsize2 = p1width * COLOR_LCD_DEPTH;

	for (int i = 0; i < COLOR_LCD_HEIGHT; i++) {
		draw_row(lcd, dest, src,
			start_x, display_width,
			imgpos1, imgoffs1, imgsize1,
			imgpos2, imgoffs2, imgsize2);

		dest += COLOR_LCD_WIDTH * COLOR_LCD_DEPTH;
		src += COLOR_LCD_WIDTH * COLOR_LCD_DEPTH;
	}

	return buffer;
}