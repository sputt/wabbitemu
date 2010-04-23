#include "lcd.h"
#include "gifhandle.h"
#ifdef WINVER
#include "registry.h"
#endif

#include <math.h>
#include "calc.h"

/* 
 * Differing interpretations of contrast require that
 * each model has its own base contrast level. 
 */
#define BASE_LEVEL_83PSE	48
#define BASE_LEVEL_83P		55
#define BASE_LEVEL_83		43

/* 
 * Column and Row Driver opcodes, opcode masks, and data masks
 */
typedef enum _CRD_COMMAND {
	// Display ON(1)/OFF(0)
	CRD_DPE 	= 0x02, CRD_DPE_MASK 	= 0xFE, CRD_DPE_DATA	= 0x01,
	// Word length: 8 bits(1)/6 bits(0)
	CRD_86E 	= 0x00, CRD_86E_MASK 	= 0xFE, CRD_86E_DATA	= 0x01,
	// Counter Select : bit1 Y(1)/X(0)  Mode Select bit0 UP(1)/DOWN(0)
	CRD_UDE 	= 0x04, CRD_UDE_MASK 	= 0xFC, CRD_UDE_DATA	= 0x03,
	// Test Mode Select
	CRD_CHE 	= 0x18, CRD_CHE_MASK 	= 0xF8, CRD_CHE_DATA	= 0x00,
	// Op-amp Power Control 1
	CRD_OPA1	= 0x10, CRD_OPA1_MASK	= 0xF8, CRD_OPA1_DATA	= 0x03,
	// Op-amp Power Control 2
	CRD_OPA2	= 0x08, CRD_OPA2_MASK	= 0xF8, CRD_OPA2_DATA	= 0x03,
	// Y-(Page) Address Set
	CRD_SYE		= 0x20, CRD_SYE_MASK	= 0xE0, CRD_SYE_DATA	= 0x1F,
	// Z-Address Set
	CRD_SZE		= 0x40, CRD_SZE_MASK	= 0xC0, CRD_SZE_DATA	= 0x3F,
	// X-Address Set
	CRD_SXE		= 0x80, CRD_SXE_MASK	= 0xC0, CRD_SXE_DATA	= 0x3F,
	// Contrast Set
	CRD_SCE		= 0xC0, CRD_SCE_MASK	= 0xC0, CRD_SCE_DATA	= 0x3F
} CRD_COMMAND;

/* 
 * This is a clever set of macros to simulate a switch-case
 * statement for Column and Row driver constants.
 */
#define TOKCAT(x,y) x##y
#define TOKCAT2(x,y) TOKCAT(x,y)
#define LINECAT(x) TOKCAT2(x, __LINE__ )	
#define CRD_SWITCH(zz) int CRD_TEST = (zz); if (0)
#define CRD_DATA(zz) (CRD_TEST & CRD_##zz##_DATA)
#define CRD_CASE(zz) goto LINECAT(A);}\
while ((CRD_TEST & (CRD_##zz##_MASK))==CRD_##zz) {LINECAT(A)

/*
 * Macro to calculate the offset into LCD memory given x, y, and z
 */
#define LCD_OFFSET(x, y, z) (((((y)+(z))%LCD_HEIGHT)*LCD_MEM_WIDTH)+((x)%LCD_MEM_WIDTH))

/*
 * Prototypes
 */
static void LCD_advance_cursor(LCD_t *);
static void LCD_reset(LCD_t *);
static void LCD_enqueue(LCD_t *);
static void LCD_free(LCD_t *);
u_char *LCD_update_image(LCD_t *lcd);


//static FILE *log;

/* 
 * Initialize LCD for a given CPU
 */
LCD_t* LCD_init(CPU_t* cpu, int model) {
	
	//log = fopen("screen_delay.txt","w");
	
	LCD_t* lcd = (LCD_t *) malloc(sizeof(LCD_t));
	if (!lcd) {
		printf("Couldn't allocate memory for LCD\n");
		exit(1);
	}

	lcd->free = &LCD_free;
	LCD_reset(lcd);
	
	switch (model) {
		case TI_82:
		case TI_83:
			lcd->base_level = BASE_LEVEL_83;
			break;
		case TI_86:
			lcd->base_level = BASE_LEVEL_83P;
			lcd->contrast = lcd->base_level;
			break;
		case TI_73:
		case TI_83P:
			lcd->base_level = BASE_LEVEL_83P;
			break;
		case TI_83PSE:
			lcd->base_level = BASE_LEVEL_83PSE;
			break;
		default:
			lcd->base_level = BASE_LEVEL_83P;
			break;
	}

	if (model == TI_86 || model == TI_85) {
		lcd->width = 128;
	} else {
		lcd->width = 96;
	}
	
	// Set all values to the defaults
#ifdef WINVER
	lcd->shades = QueryWabbitKey("shades");
	lcd->mode = (LCD_MODE) QueryWabbitKey("lcd_mode");
	lcd->steady_frame = 1.0 / QueryWabbitKey("lcd_freq");
#else
	lcd->shades = 6;
	lcd->mode = MODE_PERFECT_GRAY;
	lcd->steady_frame = 1.0 / FPS;
#endif
	if (lcd->shades > LCD_MAX_SHADES)
		lcd->shades = LCD_MAX_SHADES;
	
	lcd->time = tc_elapsed(cpu->timer_c);
	lcd->ufps_last = tc_elapsed(cpu->timer_c);
	lcd->ufps = 0.0f;
	lcd->lastgifframe = tc_elapsed(cpu->timer_c);
	lcd->write_avg = 0.0f;
	lcd->write_last = tc_elapsed(cpu->timer_c);
	return lcd;
}

/* 
 * Free space belonging to lcd
 */
static void LCD_free(LCD_t *lcd) {
	free(lcd);
}

/* 
 * Device code for LCD commands 
 */
void LCD_command(CPU_t *cpu, device_t *dev) {
	LCD_t *lcd = (LCD_t *) dev->aux;
	
	if (cpu->output) {
		// Test the bus to determine which command to run
		CRD_SWITCH(cpu->bus) {
			CRD_CASE(DPE):
				lcd->active = CRD_DATA(DPE);
				break;
			CRD_CASE(86E):
				lcd->word_len = CRD_DATA(86E);
				break;
			CRD_CASE(UDE):
				lcd->cursor_mode = (LCD_CURSOR_MODE) CRD_DATA(UDE);
				break;
			CRD_CASE(CHE):
			CRD_CASE(OPA1):
			CRD_CASE(OPA2):
				break;
			CRD_CASE(SYE):
				lcd->y = CRD_DATA(SYE);
				break;
			CRD_CASE(SZE):
				lcd->z = CRD_DATA(SZE);
				break;
			CRD_CASE(SXE):
				lcd->x = CRD_DATA(SXE);
				break;
			CRD_CASE(SCE):
				lcd->contrast = CRD_DATA(SCE);
				break;
		}
		cpu->output = FALSE;
	} else if (cpu->input) {
		cpu->bus = (lcd->word_len << 6) | (lcd->active << 5) | lcd->cursor_mode;
		cpu->input = FALSE;
	}
}

/* 
 * Output to the LCD data port.
 * Also manage user FPS and grayscale 
 */
void LCD_data(CPU_t *cpu, device_t *dev) {
	LCD_t *lcd = (LCD_t *) dev->aux;

	// Get a pointer to the byte referenced by the CRD cursor
	u_int shift = 0;
	u_char *cursor;
	if (lcd->word_len) {
		cursor = &lcd->display[ LCD_OFFSET(lcd->y, lcd->x, lcd->z) ];
	} else {
		u_int new_y = lcd->y * 6;
		shift = 10 - (new_y % 8);
		
		cursor = &lcd->display[ LCD_OFFSET(new_y / 8, lcd->x, lcd->z) ];
	}
	
	if (cpu->output) {
		// Run some sanity checks on the write vars
		if (lcd->write_last > tc_elapsed(cpu->timer_c))
			lcd->write_last = tc_elapsed(cpu->timer_c);
		
		double write_delay = tc_elapsed(cpu->timer_c) - lcd->write_last;
		if (lcd->write_avg == 0.0) lcd->write_avg = write_delay;
		lcd->write_last = tc_elapsed(cpu->timer_c);
	
		// If there is a delay that is significantly longer than the
		// average write delay, we can assume a frame has just terminated
		// and you can push this complete frame towards generating the
		// final image.
		
		// If you are in steady mode, then this simply serves as a
		// FPS calculator
		if (write_delay < lcd->write_avg * 100.0) {
			lcd->write_avg = (lcd->write_avg * 0.90) + (write_delay * 0.10);
		} else {
			double ufps_length = tc_elapsed(cpu->timer_c) - lcd->ufps_last;
			lcd->ufps = 1.0 / ufps_length;
			lcd->ufps_last = tc_elapsed(cpu->timer_c);
			
			if (lcd->mode == MODE_PERFECT_GRAY) {
				LCD_enqueue(lcd);
				lcd->time = tc_elapsed(cpu->timer_c);
			}
		}
		
		if (lcd->mode == MODE_GAME_GRAY) {
			if (lcd->x == 0 && lcd->y == 0) {
				int i;
				for (i = 0; i < lcd->shades; i++) {
					if (memcmp(lcd->display, lcd->queue[i], DISPLAY_SIZE) == 0) {
						LCD_update_image(lcd);
						break;
					}
				}
				
				LCD_enqueue(lcd);
				lcd->time = tc_elapsed(cpu->timer_c);
			}
		}

		// Set the cursor based on the word mode
		if (lcd->word_len) {
			cursor[0] = cpu->bus;
		} else {
			u_short	data = cpu->bus << shift,
					mask = ~(0x003F << shift);

			cursor[0] = (cursor[0] & (mask >> 8)) | (data >> 8);
			cursor[1] = (cursor[1] & (mask & 0xFF)) | (data & 0xFF);	
		}
		
		LCD_advance_cursor(lcd);
		cpu->output = FALSE;
	} else if (cpu->input) {
		cpu->bus = lcd->last_read;
		
		if (lcd->word_len) {
			lcd->last_read = cursor[0];
		} else {
			lcd->last_read = (cursor[0] << 8) | cursor[1];
			lcd->last_read >>= shift;
			lcd->last_read &= 0x3F;
		}
		
		LCD_advance_cursor(lcd);
		cpu->input = FALSE;
	}
	
	// Make sure timers are valid
	if (lcd->time > tc_elapsed(cpu->timer_c))
		lcd->time = tc_elapsed(cpu->timer_c);
	
	else if (tc_elapsed(cpu->timer_c) - lcd->time > (2.0/STEADY_FREQ_MIN))
		lcd->time = tc_elapsed(cpu->timer_c) - (2.0/STEADY_FREQ_MIN);
	
	// Perfect gray mode should time out too in case the screen update rate is too slow for
	// proper grayscale (essentially a fallback on steady freq)
	if (lcd->mode == MODE_PERFECT_GRAY || lcd->mode == MODE_GAME_GRAY) {	
		if ((tc_elapsed(cpu->timer_c) - lcd->time) >= (1.0/STEADY_FREQ_MIN)) {
			LCD_enqueue(lcd);
			lcd->time += (1.0/STEADY_FREQ_MIN);
		}
	} else if (lcd->mode == MODE_STEADY) {
		if ((tc_elapsed(cpu->timer_c) - lcd->time) >= lcd->steady_frame) {
			LCD_enqueue(lcd);
			lcd->time += lcd->steady_frame;
		}
	}


	if ((tc_elapsed(cpu->timer_c) - lcd->lastgifframe) >= 0.01){
		handle_screenshot();
		lcd->lastgifframe += 0.01;
	}

}

/* 
 * Moves the CRD cursor to the next position, according
 * to the increment/decrement mode set by lcd->cursor_mode
 */
static void LCD_advance_cursor(LCD_t *lcd) {
	switch (lcd->cursor_mode) {
		case X_UP:
			lcd->x = (lcd->x + 1) % 64;
			break;
		case X_DOWN:
			lcd->x = (lcd->x - 1) % 64;
			break;
		case Y_UP:
			{
				lcd->y++;
				u_int bound = lcd->word_len ? 15 : 19;
				if (lcd->y >= bound) lcd->y = 0;
				break;
			}
		case Y_DOWN:
			if (lcd->y <= 0) {
				lcd->y = lcd->word_len ? 14 : 18;
			} else
			lcd->y--;
			break;
		case MODE_NONE:
			break;
	}
}


/* 
 * Simulates the state of the LCD after a power reset
 */
static void LCD_reset(LCD_t *lcd) {
	lcd->active = FALSE;
	lcd->word_len = 8;
	lcd->cursor_mode = Y_UP;
	lcd->x = 0;
	lcd->y = 0;
	lcd->z = 0;
	lcd->contrast = 32;
	lcd->last_read = 0;
	
	memset(lcd->display, 0, DISPLAY_SIZE);
	
	lcd->front = 0;
	memset(lcd->queue, 0, sizeof(lcd->queue[0]) * LCD_MAX_SHADES);
}


/* 
 * Add a black and white LCD image to the LCD grayscale queue
 */
static void LCD_enqueue(LCD_t *lcd) {

	if (lcd->front == 0) lcd->front = lcd->shades;
	lcd->front--;
	
	memcpy(lcd->queue[lcd->front], lcd->display, DISPLAY_SIZE);
}


/*
 * Clear the LCD's display, including all grayscale buffers
 */
void LCD_clear(LCD_t *lcd) {
	int i;
	for (i = 0; i < LCD_MAX_SHADES; i++) 
		memset(lcd->queue[i], 0x00, DISPLAY_SIZE);
}


u_char *LCD_update_image(LCD_t *lcd) {
	int level = abs((int) lcd->contrast - (int) lcd->base_level);
	int base = (lcd->contrast - 54) * 24;
	if (base < 0) base = 0;

	if (level > 12) level = 0;
	else level = (12 - level) * (255 - base)/lcd->shades / 12;

	u_int row, col;
	for (row = 0; row < LCD_HEIGHT; row++) {
		for (col = 0; col < LCD_MEM_WIDTH; col++) {
			u_int p0=0,p1=0,p2=0,p3=0,p4=0,p5=0,p6=0,p7=0;
			int i;
			
			for (i = 0; i < lcd->shades; i++) {
				u_int u = lcd->queue[i][row * 16 + col];
				p7 += u & 1; u >>= 1;
				p6 += u & 1; u >>= 1;
				p5 += u & 1; u >>= 1;
				p4 += u & 1; u >>= 1;
				p3 += u & 1; u >>= 1;
				p2 += u & 1; u >>= 1;
				p1 += u & 1; u >>= 1;
				p0 += u;
			}
			
			u_char *scol = &lcd->screen[row][col * 8];
			scol[0] = p0 * level + base;
			scol[1] = p1 * level + base;
			scol[2] = p2 * level + base;
			scol[3] = p3 * level + base;
			scol[4] = p4 * level + base;
			scol[5] = p5 * level + base;
			scol[6] = p6 * level + base;
			scol[7] = p7 * level + base;
		}
	}

	return (uint8_t*) lcd->screen;	
}

/* 
 * Generate a grayscale image from the black and white images
 * pushed to the queue.  If there are no images in the queue,
 * the generated image will be blank
 */
u_char* LCD_image(LCD_t *lcd) {
	if (lcd->mode == MODE_GAME_GRAY)
		return (uint8_t*) lcd->screen;
	else
		return LCD_update_image(lcd);
}












