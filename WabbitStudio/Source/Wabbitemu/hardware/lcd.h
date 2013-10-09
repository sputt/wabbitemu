#ifndef LCD_H
#define LCD_H

#include "core.h"

#define STEADY_FREQ_MIN		30
#define STEADY_FREQ_MAX		120

/* 
 * LCD dimensions
 * Large enough to encompass all z80 models 
 */
#define LCD_HEIGHT			64
#define LCD_WIDTH			128
#define LCD_MEM_WIDTH		(LCD_WIDTH / 8)
// One bit per pixel
#define DISPLAY_SIZE		(LCD_MEM_WIDTH * LCD_HEIGHT)
// One byte per pixel
#define GRAY_DISPLAY_SIZE 	(DISPLAY_SIZE * 8)
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

/*
 * Cursor mode describes the 4
 * directions the cursor can be in.
 */
typedef enum _LCD_CURSOR_MODE {
	X_DOWN		= 0,
	X_UP		= 1,
	Y_DOWN		= 2,
	Y_UP		= 3,
	MODE_NONE	= 4,
} LCD_CURSOR_MODE;

/*
 * Modes describing how the LCD images are generated.
 * Perfect gray attempts to use full LCD images to generate
 * a grayscale image.
 * Steady mode captures at timed intervals and doesn't rely
 * on the LCD screen being updated to generate its images.
 */
typedef enum _LCD_MODE {
	MODE_PERFECT_GRAY = 0,		// Special grayscale mode
	MODE_STEADY,			// Steady frame capture mode
	MODE_GAME_GRAY,
} LCD_MODE;


/* Main structure describing all attributes specific to one LCD,
 * additionally calculation buffers (such as screen) are stored
 * here to prevent thread related issues with a static buffer
 */
typedef struct LCD {
	void (*free)(struct LCD*);		// Function to free this aux
	BOOL active;					// TRUE = on, FALSE = off
	u_int word_len;
	u_int x, y, z;					/* LCD cursors */
	int width;
	u_int lcd_delay;				//delay in tstate required to write
	
	LCD_CURSOR_MODE cursor_mode;	/* Y_UP, Y_DOWN, X_UP, X_DOWN */
	u_int last_read;				/* Buffer previous read */
	u_int contrast;					/* 0 to 63 */
	u_int base_level;				/* used in lcd level to handle contrast */
	uint8_t display[DISPLAY_SIZE];	/* LCD display memory */
	uint8_t screen[LCD_HEIGHT][LCD_WIDTH];
	int front;
	uint8_t queue[LCD_MAX_SHADES][DISPLAY_SIZE];/* holds previous buffers for grey */
	uint8_t gif[LCD_HEIGHT*2][LCD_WIDTH*2];		/*for rendering limited color gifs*/
	u_int shades;					/* number of shades of grey*/
	LCD_MODE mode;					/* Mode of LCD rendering */
	double steady_frame;			/* Length of a steady frame in seconds */
	double time;					/* Last lcd update in seconds*/
	double ufps, ufps_last;			/* User frames per second*/
	double lastgifframe;
	double write_avg, write_last;	/* Used to determine freq. of writes to the LCD */
	long long last_tstate;			// timer_c->tstate of the last write
	BOOL is_color;					//TRUE if model is 84PCSE
} LCD_t;

/* Device functions */
LCD_t *LCD_init(CPU_t *, int);
void LCD_timer_refresh(CPU_t *);
void LCD_command(CPU_t *, device_t *);
void LCD_data(CPU_t *, device_t *);

/* Interface functions */
uint8_t *LCD_image(LCD_t *);

#endif /* #ifndef LCD_H */
