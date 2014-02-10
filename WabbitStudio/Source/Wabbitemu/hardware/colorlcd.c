#include "stdafx.h"

#include "colorlcd.h"

#define PIXEL_OFFSET(x, y) ((y) * COLOR_LCD_WIDTH + (x)) * COLOR_LCD_DEPTH 
#define TRUCOLOR(color, bits) (color) * (0xFF / ((1 << (bits)) - 1))
#define LCD_REG(reg) (lcd->registers[reg])
#define LCD_REG_MASK(reg, mask) (LCD_REG(reg) & (mask))

typedef enum {
	DRIVER_CODE_REG = 0x00, 
	DRIVER_OUTPUT_CONTROL1_REG = 0x01,				DRIVER_OUTPUT_CONTROL1_MASK = 0x0500,
	ENTRY_MODE_REG = 0x03,							ENTRY_MODE_MASK = 0xD0B8,
	DATA_FORMAT_16BIT_REG = 0x05,					DATA_FORMAT_16BIT_MASK = 0x0003,
	DISPLAY_CONTROL1_REG = 0x07,					DISPLAY_CONTROL1_MASK= 0x313B,
	DISPLAY_CONTROL2_REG = 0x08,					DISPLAY_CONTROL2_MASK = 0xFFFF,
	DISPLAY_CONTROL3_REG = 0x09,					DISPLAY_CONTROL3_MASK = 0x073F,
	DISPLAY_CONTROL4_REG = 0x0A,					DISPLAY_CONTROL4_MASK = 0x000F,
	RGB_DISPLAY_INTERFACE_CONTROL1_REG = 0x0C,		RGB_DISPLAY_INTERFACE_CONTROL1_MASK = 0x7133,
	FRAME_MARKER_REG = 0x0D,						FRAME_MARKER_MASK = 0x01FF,
	RGB_DISPLAY_INTERFACE_CONTROL2_REG = 0x0F,		RGB_DISPLAY_INTERFACE_CONTROL2_MASK = 0x001B,
	POWER_CONTROL1_REG = 0x10,						POWER_CONTROL1_MASK = 0x17F3,
	POWER_CONTROL2_REG = 0x11,						POWER_CONTROL2_MASK = 0x0777,
	POWER_CONTROL3_REG = 0x12,						POWER_CONTROL3_MASK = 0x008F,
	POWER_CONTROL4_REG = 0x13,						POWER_CONTROL4_MASK = 0x1F00,
	CUR_Y_REG = 0x20,								CUR_Y_MASK = 0x00FF,
	CUR_X_REG = 0x21,								CUR_X_MASK = 0x01FF,
	GRAM_REG = 0x22,
	POWER_CONTROL7_REG = 0x29,						POWER_CONTROL7_MASK = 0x003F,
	FRAME_RATE_COLOR_CONTROL_REG = 0x2B,			FRAME_RATE_COLOR_CONTROL_MASK = 0x000F,
	GAMMA_CONTROL1_REG = 0x30,						GAMMA_CONTROL1_MASK = 0x0707,
	GAMMA_CONTROL2_REG = 0x31,						GAMMA_CONTROL2_MASK = 0x0707,
	GAMMA_CONTROL3_REG = 0x32,						GAMMA_CONTROL3_MASK = 0x0707,
	GAMMA_CONTROL4_REG = 0x35,						GAMMA_CONTROL4_MASK = 0x0707,
	GAMMA_CONTROL5_REG = 0x36,						GAMMA_CONTROL5_MASK = 0x1F0F,
	GAMMA_CONTROL6_REG = 0x37,						GAMMA_CONTROL6_MASK = 0x0707,
	GAMMA_CONTROL7_REG = 0x38,						GAMMA_CONTROL7_MASK = 0x0707,
	GAMMA_CONTROL8_REG = 0x39,						GAMMA_CONTROL8_MASK = 0x0707,
	GAMMA_CONTROL9_REG = 0x3C,						GAMMA_CONTROL9_MASK = 0x0707,
	GAMMA_CONTROL10_REG = 0x3D,						GAMMA_CONTROL10_MASK = 0x1F0F,
	WINDOW_HORZ_START_REG = 0x50,					WINDOW_HORZ_START_MASK = 0x00FF,
	WINDOW_HORZ_END_REG = 0x51,						WINDOW_HORZ_END_MASK = 0x00FF,
	WINDOW_VERT_START_REG = 0x52,					WINDOW_VERT_START_MASK = 0x01FF,
	WINDOW_VERT_END_REG = 0x53,						WINDOW_VERT_END_MASK = 0x01FF,
	GATE_SCAN_CONTROL_REG = 0x60,					GATE_SCAN_CONTROL_MASK = 0xBF3F,
	BASE_IMAGE_DISPLAY_CONTROL_REG = 0x61,			BASE_IMAGE_DISPLAY_CONTROL_MASK = 0x0007,
	VERTICAL_SCROLL_CONTROL_REG = 0x6A,				VERTICAL_SCROLL_CONTROL_MASK = 0x01FF,
	PARTIAL_IMAGE1_DISPLAY_POSITION_REG = 0x80,		PARTIAL_IMAGE1_DISPLAY_POSITION_MASK = 0x01FF,
	PARTIAL_IMAGE1_START_LINE_REG = 0x81,			PARTIAL_IMAGE1_START_LINE_MASK = 0x01FF,
	PARTIAL_IMAGE1_END_LINE_REG = 0x82,				PARTIAL_IMAGE1_END_LINE_MASK = 0x01FF,
	PARTIAL_IMAGE2_DISPLAY_POSITION_REG = 0x83,		PARTIAL_IMAGE2_DISPLAY_POSITION_MASK = 0x01FF,
	PARTIAL_IMAGE2_START_LINE_REG = 0x84,			PARTIAL_IMAGE2_START_LINE_MASK = 0x01FF,
	PARTIAL_IMAGE2_END_LINE_REG = 0x85,				PARTIAL_IMAGE2_END_LINE_MASK = 0x01FF,
	PANEL_INTERFACE_CONTROL1_REG = 0x90,			PANEL_INTERFACE_CONTROL1_MASK = 0x031F,
	PANEL_INTERFACE_CONTROL2_REG = 0x92,			PANEL_INTERFACE_CONTROL2_MASK = 0x0700,
	PANEL_INTERFACE_CONTROL4_REG = 0x95,			PANEL_INTERFACE_CONTROL4_MASK = 0x0300,
	PANEL_INTERFACE_CONTROL5_REG = 0x97,			PANEL_INTERFACE_CONTROL5_MASK = 0x0F00,
	OTP_VCM_PROGRAMMING_CONTROL_REG = 0xA1,			OTP_VCM_PROGRAMMING_CONTROL_MASK = 0x083F,
	OTP_VCM_STATUS_AND_ENABLE_REG = 0xA2,			OTP_VCM_STATUS_AND_ENABLE_MASK = 0xFF01,
	OTP_PROGRAMMING_ID_KEY_REG = 0xA5,				OTP_PROGRAMMING_ID_KEY_MASK = 0xFFFF,
	DEEP_STAND_BY_MODE_CONTROL_REG = 0xE6,			DEEP_STAND_BY_MODE_CONTROL_MASK = 0x0001
} COLOR_LCD_COMMAND;

// 0
#define DRIVER_CODE_VER 0x9335

// 1
#define FLIP_COLS_MASK BIT(8)
#define INTERLACED_MASK BIT(10)

// 3
#define CUR_DIR_MASK BIT(3)
#define ROW_INC_MASK BIT(4)
#define COL_INC_MASK BIT(5)
#define ORG_MASK BIT(7)
#define BGR_MASK BIT(12)
#define EIGHTEEN_BIT_MASK BIT(14)
#define UNPACKED_MASK BIT(15)
#define TRI_MASK EIGHTEEN_BIT_MASK | UNPACKED_MASK

// 7
#define DISPLAY_ON_MASK (BIT(0) | BIT(1))
#define COLOR8_MASK BIT(3)
#define BASEE_MASK BIT(8)
#define SHOW_PARTIAL1_MASK BIT(12)
#define SHOW_PARTIAL2_MASK BIT(13)

// 2B
#define FRAME_RATE_MASK (BIT(3) | BIT(2) | BIT(1) | BIT(0))

// 60
#define BASE_START_MASK (BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5))
#define BASE_NLINES_MASK (BIT(8) | BIT(9) | BIT(10) | BIT(11) | BIT(12) | BIT(13))
#define GATE_SCAN_DIR_MASK BIT(15)

// 61
#define LEVEL_INVERT_MASK BIT(0)
#define SCROLL_ENABLED_MASK BIT(1)
#define NDL_MASK BIT(2)

// 6a
#define SCROLL_MASK 0x1FF

// 80
#define P1_POS_MASK 0x1FF

// 81
#define P1_START_MASK 0x1FF

// 82
#define P1_END_MASK 0x1FF

// 83
#define P2_POS_MASK 0x1FF

// 84
#define P2_START_MASK 0x1FF

// 85
#define P2_END_MASK 0x1FF

static int read_pixel(ColorLCD_t *lcd);
static void write_pixel18(ColorLCD_t *lcd, timerc *timerc);
static void write_pixel16(ColorLCD_t *lcd, timerc *timerc);
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
	int mode = LCD_REG(ENTRY_MODE_REG);
	switch (reg) {
	case DRIVER_OUTPUT_CONTROL1_REG:
		lcd->registers[DRIVER_OUTPUT_CONTROL1_REG] = value & DRIVER_OUTPUT_CONTROL1_MASK;
		break;
	case ENTRY_MODE_REG: {
		lcd->registers[ENTRY_MODE_REG] = value & ENTRY_MODE_MASK;
		LCD_CURSOR_MODE row_mode = LCD_REG_MASK(ENTRY_MODE_REG, ROW_INC_MASK) ? Y_UP : Y_DOWN;
		LCD_CURSOR_MODE col_mode = LCD_REG_MASK(ENTRY_MODE_REG, COL_INC_MASK) ? X_UP : X_DOWN;
		lcd->base.cursor_mode = (LCD_CURSOR_MODE) (row_mode | col_mode);
		reset_x(lcd, value);
		reset_y(lcd, value);
		break;
	}
	case DATA_FORMAT_16BIT_REG:
		lcd->registers[DATA_FORMAT_16BIT_REG] = value & DATA_FORMAT_16BIT_MASK;
		break;
	case DISPLAY_CONTROL1_REG:
		lcd->registers[DISPLAY_CONTROL1_REG] = value & DISPLAY_CONTROL1_MASK;
		lcd->base.active = value & DISPLAY_ON_MASK ? TRUE : FALSE;
		break;
	case DISPLAY_CONTROL2_REG:
		lcd->registers[DISPLAY_CONTROL2_REG] = value & DISPLAY_CONTROL2_MASK;
		break;
	case DISPLAY_CONTROL3_REG:
		lcd->registers[DISPLAY_CONTROL3_REG] = value & DISPLAY_CONTROL3_MASK;
		break;
	case DISPLAY_CONTROL4_REG:
		lcd->registers[DISPLAY_CONTROL4_REG] = value & DISPLAY_CONTROL4_MASK;
		break;
	case RGB_DISPLAY_INTERFACE_CONTROL1_REG:
		lcd->registers[RGB_DISPLAY_INTERFACE_CONTROL1_REG] = value & RGB_DISPLAY_INTERFACE_CONTROL1_MASK;
		break;
	case FRAME_MARKER_REG:
		lcd->registers[FRAME_MARKER_REG] = value & FRAME_MARKER_MASK;
		break;
	case RGB_DISPLAY_INTERFACE_CONTROL2_REG:
		lcd->registers[RGB_DISPLAY_INTERFACE_CONTROL2_REG] = value & RGB_DISPLAY_INTERFACE_CONTROL2_MASK;
		break;
	case POWER_CONTROL1_REG:
		lcd->registers[POWER_CONTROL1_REG] = value & POWER_CONTROL1_MASK;
		break;
	case POWER_CONTROL2_REG:
		lcd->registers[POWER_CONTROL2_REG] = value & POWER_CONTROL2_MASK;
		break;
	case POWER_CONTROL3_REG:
		lcd->registers[POWER_CONTROL3_REG] = value & POWER_CONTROL3_MASK;
		break;
	case POWER_CONTROL4_REG:
		lcd->registers[POWER_CONTROL4_REG] = value & POWER_CONTROL4_MASK;
		break;
	case CUR_X_REG:
	case CUR_Y_REG: {
		lcd->registers[reg] = value & (reg == CUR_X_REG ? CUR_X_MASK : CUR_Y_MASK);

		if (mode & ORG_MASK) {
			if (reg == CUR_Y_REG) {
				reset_y(lcd, mode);
			} else {
				reset_x(lcd, mode);
			}
		} else {
			lcd->base.x = LCD_REG(CUR_X_REG);
			lcd->base.y = LCD_REG(CUR_Y_REG);
		}
		break;
	}
	case POWER_CONTROL7_REG:
		lcd->registers[POWER_CONTROL7_REG] = value & POWER_CONTROL7_MASK;
		break;
	case FRAME_RATE_COLOR_CONTROL_REG:
		lcd->registers[FRAME_RATE_COLOR_CONTROL_REG] = value & FRAME_RATE_COLOR_CONTROL_MASK;
		switch (value & FRAME_RATE_MASK) {
		default:
		case 0:
			lcd->frame_rate = 31;
			break;
		case 1:
			lcd->frame_rate = 32;
			break;
		case 2:
			lcd->frame_rate = 34;
			break;
		case 3:
			lcd->frame_rate = 36;
			break;
		case 4:
			lcd->frame_rate = 39;
			break;
		case 5:
			lcd->frame_rate = 41;
			break;
		case 6:
			lcd->frame_rate = 34;
			break;
		case 7:
			lcd->frame_rate = 48;
			break;
		case 8:
			lcd->frame_rate = 52;
			break;
		case 9:
			lcd->frame_rate = 57;
			break;
		case 10:
			lcd->frame_rate = 62;
			break;
		case 11:
			lcd->frame_rate = 69;
			break;
		case 12:
			lcd->frame_rate = 78;
			break;
		case 13:
			lcd->frame_rate = 89;
			break;
		}
		break;
	case GAMMA_CONTROL1_REG:
	case GAMMA_CONTROL2_REG:
	case GAMMA_CONTROL3_REG:
	case GAMMA_CONTROL4_REG:
	case GAMMA_CONTROL6_REG:
	case GAMMA_CONTROL7_REG:
	case GAMMA_CONTROL8_REG:
	case GAMMA_CONTROL9_REG:
		lcd->registers[reg] = value & GAMMA_CONTROL1_MASK;
		break;
	case GAMMA_CONTROL5_REG:
	case GAMMA_CONTROL10_REG:
		lcd->registers[reg] = value & GAMMA_CONTROL5_MASK;
		break;
	case WINDOW_HORZ_START_REG: {
		lcd->registers[WINDOW_HORZ_START_REG] = value & WINDOW_HORZ_START_MASK;
		if ((mode & ORG_MASK) && (mode & COL_INC_MASK)) {
			lcd->base.y = LCD_REG(WINDOW_HORZ_START_REG);
		}
		break;
	}
	case WINDOW_HORZ_END_REG: {
		lcd->registers[WINDOW_HORZ_END_REG] = value & WINDOW_HORZ_END_MASK;
		if ((mode & ORG_MASK) && !(mode & COL_INC_MASK)) {
			lcd->base.y = lcd->registers[WINDOW_HORZ_END_REG];
		}
		break;
	}
	case WINDOW_VERT_START_REG: {
		lcd->registers[WINDOW_VERT_START_REG] = value & WINDOW_VERT_START_MASK;
		if ((mode & ORG_MASK) && (mode & ROW_INC_MASK)) {
			lcd->base.x = LCD_REG(WINDOW_VERT_START_REG);
		}
		break;
	}
	case WINDOW_VERT_END_REG: {
		lcd->registers[WINDOW_VERT_END_REG] = value & WINDOW_VERT_END_MASK;
		if ((mode & ORG_MASK) && !(mode & ROW_INC_MASK)) {
			lcd->base.x = LCD_REG(WINDOW_VERT_END_REG);
		}
		break;
	}
	case GATE_SCAN_CONTROL_REG:
		lcd->registers[GATE_SCAN_CONTROL_REG] = value & GATE_SCAN_CONTROL_MASK;
		break;
	case BASE_IMAGE_DISPLAY_CONTROL_REG:
		lcd->registers[BASE_IMAGE_DISPLAY_CONTROL_REG] = value & BASE_IMAGE_DISPLAY_CONTROL_MASK;
		break;
	case VERTICAL_SCROLL_CONTROL_REG:
		lcd->base.z = 
			lcd->registers[VERTICAL_SCROLL_CONTROL_REG] = value & VERTICAL_SCROLL_CONTROL_MASK;
		break;
	case PARTIAL_IMAGE1_DISPLAY_POSITION_REG:
		lcd->registers[PARTIAL_IMAGE1_DISPLAY_POSITION_REG] = value & PARTIAL_IMAGE1_DISPLAY_POSITION_MASK;
		break;
	case PARTIAL_IMAGE1_START_LINE_REG:
		lcd->registers[PARTIAL_IMAGE1_START_LINE_REG] = value & PARTIAL_IMAGE1_START_LINE_MASK;
		break;
	case PARTIAL_IMAGE1_END_LINE_REG:
		lcd->registers[PARTIAL_IMAGE1_END_LINE_REG] = value & PARTIAL_IMAGE1_END_LINE_MASK;
		break;
	case PARTIAL_IMAGE2_DISPLAY_POSITION_REG:
		lcd->registers[PARTIAL_IMAGE2_DISPLAY_POSITION_REG] = value & PARTIAL_IMAGE2_DISPLAY_POSITION_MASK;
		break;
	case PARTIAL_IMAGE2_START_LINE_REG:
		lcd->registers[PARTIAL_IMAGE2_START_LINE_REG] = value & PARTIAL_IMAGE2_START_LINE_MASK;
		break;
	case PARTIAL_IMAGE2_END_LINE_REG:
		lcd->registers[PARTIAL_IMAGE2_END_LINE_REG] = value & PARTIAL_IMAGE2_END_LINE_MASK;
		break;
	case PANEL_INTERFACE_CONTROL1_REG:
		lcd->registers[PANEL_INTERFACE_CONTROL1_REG] = value & PANEL_INTERFACE_CONTROL1_MASK;
		break;
	case PANEL_INTERFACE_CONTROL2_REG:
		lcd->registers[PANEL_INTERFACE_CONTROL2_REG] = value & PANEL_INTERFACE_CONTROL2_MASK;
		break;
	case PANEL_INTERFACE_CONTROL4_REG:
		lcd->registers[PANEL_INTERFACE_CONTROL4_REG] = value & PANEL_INTERFACE_CONTROL4_MASK;
		break;
	case PANEL_INTERFACE_CONTROL5_REG:
		lcd->registers[PANEL_INTERFACE_CONTROL5_REG] = value & PANEL_INTERFACE_CONTROL5_MASK;
		break;
	case OTP_VCM_PROGRAMMING_CONTROL_REG:
		lcd->registers[OTP_VCM_PROGRAMMING_CONTROL_REG] = value & OTP_VCM_PROGRAMMING_CONTROL_MASK;
		break;
	case OTP_VCM_STATUS_AND_ENABLE_REG:
		lcd->registers[OTP_VCM_STATUS_AND_ENABLE_REG] = value & OTP_VCM_STATUS_AND_ENABLE_MASK;
		break;
	case OTP_PROGRAMMING_ID_KEY_REG:
		lcd->registers[OTP_PROGRAMMING_ID_KEY_REG] = value & OTP_PROGRAMMING_ID_KEY_MASK;
		break;
	case DEEP_STAND_BY_MODE_CONTROL_REG:
		lcd->registers[DEEP_STAND_BY_MODE_CONTROL_REG] = value & DEEP_STAND_BY_MODE_CONTROL_MASK;
		break;
	default:
		lcd->registers[reg] = value & 0xFFFF;
		break;
	}
}

/*
* Free space belonging to lcd
*/
static void ColorLCD_free(CPU_t *cpu) {
	free(cpu->pio.lcd);
}

static void ColorLCD_enqueue(ColorLCD_t *lcd) {
	memcpy(lcd->queued_image, lcd->display, COLOR_LCD_DISPLAY_SIZE);
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

		if (lcd->current_register == 0) {
			// sync
			//ColorLCD_enqueue(lcd);
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
			int mode = LCD_REG_MASK(ENTRY_MODE_REG, TRI_MASK);
			if (mode & EIGHTEEN_BIT_MASK) {
				lcd->write_step++;
				if (lcd->write_step >= 3) {
					lcd->write_step = 0;
					write_pixel18(lcd, cpu->timer_c);
				}
			} else {
				lcd->write_step = !lcd->write_step;
				if (!lcd->write_step) {
					write_pixel16(lcd, cpu->timer_c);
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
			int val = LCD_REG(reg_index);
			lcd->read_step = !lcd->read_step;
			if (lcd->read_step) {
				cpu->bus = val >> 8;
			} else {
				cpu->bus = val & 0xFF;
			}
		}

		cpu->input = FALSE;
	}

	// Make sure timers are valid
	if (lcd->base.time > tc_elapsed(cpu->timer_c))
		lcd->base.time = tc_elapsed(cpu->timer_c);

	if ((tc_elapsed(cpu->timer_c) - lcd->base.time) >= (1.0 / lcd->frame_rate)) {
		ColorLCD_enqueue(lcd);
		lcd->base.time += (1.0 / lcd->frame_rate);
	}
}

static int read_pixel(ColorLCD_t *lcd) {
	int x = lcd->base.x % COLOR_LCD_WIDTH;
	int y = lcd->base.y % COLOR_LCD_HEIGHT;
	uint8_t *pixel_ptr = &lcd->display[PIXEL_OFFSET(x, y)];
	int pixel;
	if (LCD_REG_MASK(ENTRY_MODE_REG, BGR_MASK)) {
		pixel = (pixel_ptr[2] << 16) | (pixel_ptr[1] << 8) | pixel_ptr[0];
	} else {
		pixel = (pixel_ptr[0] << 16) | (pixel_ptr[1] << 8) | pixel_ptr[2];
	}

	return pixel;
}

static void write_pixel(ColorLCD_t *lcd, timerc *timer_c, int red, int green, int blue) {
	int x = lcd->base.x;
	int y = lcd->base.y;

	if (LCD_REG_MASK(DRIVER_OUTPUT_CONTROL1_REG, FLIP_COLS_MASK)) {
		y = COLOR_LCD_HEIGHT - y - 1;
	}

	uint8_t *pixel_ptr = &lcd->display[PIXEL_OFFSET(x, y)];
	if (LCD_REG_MASK(ENTRY_MODE_REG, BGR_MASK)) {
		pixel_ptr[2] = red;
		pixel_ptr[1] = green;
		pixel_ptr[0] = blue;
	} else {
		pixel_ptr[0] = red;
		pixel_ptr[1] = green;
		pixel_ptr[2] = blue;
	}

	if (LCD_REG_MASK(ENTRY_MODE_REG, CUR_DIR_MASK)) {
		update_x(lcd, TRUE);
	} else {
		update_y(lcd, TRUE);
	}
}

static void write_pixel18(ColorLCD_t *lcd, timerc *timerc) {
	int pixel_val;
	int red, green, blue;
	if (LCD_REG_MASK(ENTRY_MODE_REG, UNPACKED_MASK)) {
		pixel_val = lcd->write_buffer & 0xfcfcfc;
		red = (pixel_val >> 18) & 0x3F;
		green = (pixel_val >> 10) & 0x3F;
		blue = (pixel_val >> 2) & 0x3F;
	} else {
		pixel_val = lcd->write_buffer & 0x3ffff;
		red = (pixel_val >> 12) & 0x3F;
		green = (pixel_val >> 6) & 0x3F;
		blue = pixel_val & 0x3F;
	}

	write_pixel(lcd, timerc, red, green, blue);
}

static void write_pixel16(ColorLCD_t *lcd, timerc *timerc) {
	int pixel_val = lcd->write_buffer;
	int red_significant_bit = pixel_val & BIT(15) ? 1 : 0;
	int blue_significant_bit = pixel_val & BIT(4) ? 1 : 0;

	int red = (pixel_val >> 10) | red_significant_bit;
	int green = (pixel_val >> 5) & 0x3F;
	int blue = ((pixel_val << 1) | blue_significant_bit) & 0x3F;

	write_pixel(lcd, timerc, red, green, blue);
}

static void update_y(ColorLCD_t *lcd, BOOL should_update) {
	if (LCD_REG_MASK(ENTRY_MODE_REG, ROW_INC_MASK)) {
		if (lcd->base.y < LCD_REG(WINDOW_HORZ_END_REG)) {
			lcd->base.y++;
			return;
		}

		// back to top of the window
		lcd->base.y = LCD_REG(WINDOW_HORZ_START_REG);
	} else {
		if (lcd->base.y > LCD_REG(WINDOW_HORZ_START_REG)) {
			lcd->base.y--;
			return;
		}

		// to bottom of the window
		lcd->base.y = LCD_REG(WINDOW_HORZ_END_REG);
	}

	if (should_update) {
		update_x(lcd, FALSE);
	}
}

static void update_x(ColorLCD_t *lcd, BOOL should_update) {
	if (LCD_REG_MASK(ENTRY_MODE_REG, COL_INC_MASK)) {
		if (lcd->base.x < LCD_REG(WINDOW_VERT_END_REG)) {
			lcd->base.x++;
			return;
		}

		// back to the beginning of the window
		lcd->base.x = LCD_REG(WINDOW_VERT_START_REG);
	} else {
		if (lcd->base.x > LCD_REG(WINDOW_VERT_START_REG)) {
			lcd->base.x--;
			return;
		}

		// to the end of the window
		lcd->base.x = LCD_REG(WINDOW_VERT_END_REG);
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

	lcd->current_register = 0xFFFFFFFF;

	lcd->registers[DRIVER_CODE_REG] = DRIVER_CODE_VER;
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
	BOOL level_invert = !LCD_REG_MASK(BASE_IMAGE_DISPLAY_CONTROL_REG, LEVEL_INVERT_MASK);
	BOOL color8bit = LCD_REG_MASK(DISPLAY_CONTROL1_REG, COLOR8_MASK);

	if (level_invert) {
		red = src;
		green = src + 1;
		blue = src + 2;

		BOOL flip_rows = LCD_REG_MASK(GATE_SCAN_CONTROL_REG, GATE_SCAN_DIR_MASK);

		if (flip_rows) {
			red += size - 3;
			green += size - 3;
			blue += size - 3;
			for (int i = 0; i < size; i += 3) {
				dest[i] = TRUCOLOR(red[-i] ^ 0x3f, 6);
				dest[i + 1] = TRUCOLOR(green[-i] ^ 0x3f, 6);
				dest[i + 2] = TRUCOLOR(blue[-i] ^ 0x3f, 6);
			}
		} else {
			for (int i = 0; i < size; i += 3) {
				dest[i] = TRUCOLOR(red[i] ^ 0x3f, 6);
				dest[i + 1] = TRUCOLOR(green[i] ^ 0x3f, 6);
				dest[i + 2] = TRUCOLOR(blue[i] ^ 0x3f, 6);
			}
		}
	} else {
		for (int i = 0; i < size; i++) {
			dest[i] = TRUCOLOR(src[i], 6);;
		}
	}

	if (color8bit) {
		for (int i = 0; i < size; i++) {
			dest[i] = TRUCOLOR(dest[i] >> 5, 3);
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
		BOOL flip_rows = LCD_REG_MASK(GATE_SCAN_CONTROL_REG, GATE_SCAN_DIR_MASK);

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
	uint8_t interlace_buf[COLOR_LCD_WIDTH * COLOR_LCD_DEPTH];

	int non_display_area_color = LCD_REG_MASK(BASE_IMAGE_DISPLAY_CONTROL_REG, NDL_MASK) ? 0x3F : 0x00;
	BOOL interlace_cols = LCD_REG_MASK(DRIVER_OUTPUT_CONTROL1_REG, INTERLACED_MASK);

	uint8_t *optr = interlace_cols ? interlace_buf : dest;

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
		for (int i = 0; i < COLOR_LCD_WIDTH / 2; i++, optr += 3) {
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
	ColorLCD_t *lcd = (ColorLCD_t *)lcdBase;
	uint8_t *buffer = (uint8_t *)malloc(COLOR_LCD_DISPLAY_SIZE);
	ZeroMemory(buffer, COLOR_LCD_DISPLAY_SIZE);

	int p1pos, p1start, p1end, p1width, p2pos, p2start, p2end, p2width;
	int imgpos1, imgoffs1, imgsize1;
	int imgpos2, imgoffs2, imgsize2;

	if (!lcdBase->active) {
		return buffer;
	}

	int start_x = LCD_REG_MASK(GATE_SCAN_CONTROL_REG, BASE_START_MASK) << 3;
	int pixel_width = (LCD_REG_MASK(GATE_SCAN_CONTROL_REG, BASE_NLINES_MASK) >> 5) + 8;
	if (start_x > COLOR_LCD_WIDTH) {
		start_x = COLOR_LCD_WIDTH;
	}

	if (pixel_width > COLOR_LCD_WIDTH - start_x) {
		pixel_width = COLOR_LCD_WIDTH - start_x;
	}

	start_x = start_x * COLOR_LCD_DEPTH;
	int display_width = (COLOR_LCD_WIDTH - (start_x + pixel_width)) * COLOR_LCD_DEPTH;

	if (LCD_REG_MASK(DISPLAY_CONTROL1_REG, BASEE_MASK)) {
		p2pos = 0;
		p2width = pixel_width;
		if (LCD_REG_MASK(BASE_IMAGE_DISPLAY_CONTROL_REG, SCROLL_ENABLED_MASK)) {
			p2start = lcd->base.z;
		} else {
			p2start = 0;
		}

		p1pos = p1start = p1width = 0;
	} else {
		if (LCD_REG_MASK(DISPLAY_CONTROL1_REG, SHOW_PARTIAL1_MASK)) {
			p1pos = LCD_REG_MASK(PARTIAL_IMAGE1_DISPLAY_POSITION_REG, P1_POS_MASK) % COLOR_LCD_WIDTH;
			p1start = LCD_REG_MASK(PARTIAL_IMAGE1_START_LINE_REG, P1_START_MASK) % COLOR_LCD_WIDTH;
			p1end = LCD_REG_MASK(PARTIAL_IMAGE1_END_LINE_REG, P1_END_MASK) % COLOR_LCD_WIDTH;

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

		if (LCD_REG_MASK(DISPLAY_CONTROL1_REG, SHOW_PARTIAL2_MASK)) {
			p2pos = LCD_REG_MASK(PARTIAL_IMAGE2_DISPLAY_POSITION_REG, P2_POS_MASK) % COLOR_LCD_WIDTH;
			p2start = LCD_REG_MASK(PARTIAL_IMAGE2_START_LINE_REG, P2_START_MASK) % COLOR_LCD_WIDTH;
			p2end = LCD_REG_MASK(PARTIAL_IMAGE2_END_LINE_REG, P2_END_MASK) % COLOR_LCD_WIDTH;

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

	BOOL flip_rows = LCD_REG_MASK(GATE_SCAN_CONTROL_REG, GATE_SCAN_DIR_MASK);
	if (flip_rows) {
		int tmp = display_width;
		display_width = start_x;
		start_x = tmp;

		p1pos = COLOR_LCD_WIDTH - (p1pos + p1width);
		p2pos = COLOR_LCD_WIDTH - (p2pos + p2width);
	}

	uint8_t *dest = buffer;
	uint8_t *src = lcd->queued_image;

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