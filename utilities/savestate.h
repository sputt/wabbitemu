#ifndef SAVESTATE_H
#define SAVESTATE_H

#include "lcd.h"
#include "colorlcd.h"
#include "calc.h"

typedef struct {
	char tag[4];
	int pnt;
	int size;
	unsigned char *data;
} CHUNK_t;


#define MAX_CHUNKS 512

typedef struct {
	int version_major;
	int version_minor;
	int version_build;
	CalcModel model;
	int chunk_count;
	char author[32];
	char comment[64];	
	CHUNK_t* chunks[MAX_CHUNKS];
} SAVESTATE_t;


#define CUR_MAJOR 0
#define CUR_MINOR 1
#define CUR_BUILD 3

// old save state compatibility
#define MEM_C_CMD_BUILD				1
#define LCD_SCREEN_ADDR_BUILD		2
#define SEAUX_MODEL_BITS_BUILD		1
#define CPU_MODEL_BITS_BUILD		2
#define NEW_CONTRAST_MODEL_BUILD	3

#define DETECT_STR		"*WABBIT*"
#define DETECT_CMP_STR	"*WABCMP*"
#define FLASH_HEADER	"**TIFL**"

#define NO_CMP			0
#define ZLIB_CMP		1

#define SAVE_HEADERSIZE	116

#define INFO_tag				"INFO"
#define CPU_tag					"CPU "
#define MEM_tag					"MEMC"
#define ROM_tag					"ROM "
#define RAM_tag					"RAM "
#define TIMER_tag				"TIME"
#define LCD_tag					"LCD "
#define LINK_tag				"LINK"
#define KEYPAD_tag				"KEYS"
#define STDINT_tag				"STDI"
#define SE_AUX_tag				"SEAX"
#define USB_tag					"USB "
#define REMAP_tag				"RMAP"
#define RAM_LIMIT_tag			"RMLM"
#define RAM_BREAKS_tag			"RBRK"
#define FLASH_BREAKS_tag		"FBRK"
#define NUM_FLASH_BREAKS_tag	"NFBK"
#define NUM_RAM_BREAKS_tag		"NRBK"

#define MAX_SAVESTATE_AUTHOR_LENGTH 32
#define MAX_SAVESTATE_COMMENT_LENGTH 64

LPCALC DuplicateCalc(LPCALC lpCalc);
void WriteSave(const TCHAR *, SAVESTATE_t *, int);
BOOL LoadSlot(SAVESTATE_t* , LPCALC);
SAVESTATE_t* SaveSlot(LPCALC, const TCHAR *author, const TCHAR *comment);
SAVESTATE_t* CreateSave(const TCHAR *author, const TCHAR *comment, const CalcModel model);
SAVESTATE_t* ReadSave(FILE *ifile);
void FreeSave(SAVESTATE_t *);
char* GetRomOnly(SAVESTATE_t *save, int *);
BOOL LoadLCD(SAVESTATE_t *, LCD_t *);
BOOL LoadColorLCD(SAVESTATE_t *, ColorLCD_t *);

#endif

