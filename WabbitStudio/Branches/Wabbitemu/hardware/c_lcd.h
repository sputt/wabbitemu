#ifndef CLCD_H
#define CLCD_H

#include "core.h"

#define STEADY_FREQ_MIN		30
#define STEADY_FREQ_MAX		120

/* 
 * LCD dimensions
 * Large enough to encompass all z80 models 
 */
#define CLCD_HEIGHT			240
#define CLCD_WIDTH			320
// One bit per pixel
#define CDISPLAY_SIZE		(CLCD_WIDTH * CLCD_HEIGHT)
// Frames to spread gray generation over
#define LCD_GRAY_SPREAD		6.0

/* 
 * Maximum shades the LCD will be able to
 * render.  Actual shades rendered is stored
 * in LCD.shades.  The default value is also
 * specified (generates a good image for
 * perfect gray mode)
 */
#define LCD_MAX_SHADES		12
#define LCD_DEFAULT_SHADES	6

/* Main structure describing all attributes specific to one LCD,
 * additionally calculation buffers (such as screen) are stored
 * here to prevent thread related issues with a static buffer
 */
typedef struct C_LCD {
	//Common elements
	void (*free)(struct C_LCD*);		// Function to free this aux
	void (*timer_refresh)(struct CPU*);		// Function to timer refresh
	u_char* (*image)(struct C_LCD*);	// Function to get an lcd_image
	BOOL active;					// TRUE = on, FALSE = off
	int width;
	u_int x, y, z;					/* LCD cursors */
	u_int contrast;					/* 0 to 63 */
	double ufps, ufps_last;			/* User frames per second*/
	double time;					/* Last lcd update in seconds*/
	double lastgifframe;
	double write_avg, write_last;	/* Used to determine freq. of writes to the LCD */
	long long last_tstate;			// timer_c->tstate of the last write
	//Uncommon elements
	//u_int word_len;
	//u_int lcd_delay;				//delay in tstate required to write
	//
	//LCD_CURSOR_MODE cursor_mode;	/* Y_UP, Y_DOWN, X_UP, X_DOWN */
	//u_int last_read;				/* Buffer previous read */
	//u_int base_level;				/* used in lcd level to handle contrast */
	//uint8_t display[CDISPLAY_SIZE];	/* LCD display memory */
	//uint8_t screen[CLCD_HEIGHT][CLCD_WIDTH];
	//int front;
	//uint8_t queue[LCD_MAX_SHADES][CDISPLAY_SIZE];/* holds previous buffers for grey */
	//uint8_t gif[CLCD_HEIGHT*2][CLCD_WIDTH*2];		/*for rendering limited color gifs*/
	//u_int shades;					/* number of shades of grey*/
	//LCD_MODE mode;					/* Mode of LCD rendering */
	//double steady_frame;			/* Length of a steady frame in seconds */
} C_LCD_t;

/* Device functions */
C_LCD_t *CLCD_init(CPU_t *);
void CLCD_timer_refresh(CPU_t *);
void CLCD_command(CPU_t *, device_t *);
void CLCD_data(CPU_t *, device_t *);

/* Interface functions */
uint8_t *CLCD_image(C_LCD_t *);

#endif /* #ifndef CLCD_H */
