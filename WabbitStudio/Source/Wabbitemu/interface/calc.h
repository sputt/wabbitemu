#ifndef _CALC_H
#define _CALC_H

#include "stdafx.h"
#include "coretypes.h"

#include "core.h"
#include "lcd.h"
#include "keys.h"
#include "link.h"

#ifdef _WINDOWS
#include "Wabbitemu_h.h"
#include "sound.h"
#include "DropTarget.h"
#endif

#include "label.h"
#include "savestate.h"

typedef enum {
	GDS_IDLE,
	GDS_STARTING,
	GDS_RECORDING,
	GDS_ENDING
} gif_disp_states;

#define MIN_BLOCK_SIZE 16
typedef struct profiler {
	BOOL running;
	int blockSize;
	int lowAddress;
	int highAddress;
	long long totalTime;
	long data[0x10000 / MIN_BLOCK_SIZE];
} profiler_t;

typedef struct tagCALC {
	int slot;
	TCHAR rom_path[MAX_PATH];
	char rom_version[32];
	int model;

	time_t time_error;

	BOOL active;
	CPU_t cpu;
	memory_context_t mem_c;
	timer_context_t timer_c;
#ifdef WINVER
	AUDIO_t *audio; // FIXME: Bad!
#endif

#ifdef WINVER
	CDropTarget *pDropTarget;
	HWND hwndFrame;
	HWND hwndLCD;
	HWND hwndStatusBar;
	HWND hwndDebug;
	HWND hwndSmallClose;
	HWND hwndSmallMinimize;

	BOOL SkinEnabled;
	DWORD Scale;
	BOOL bCutout;
	HANDLE hdlThread;
	
	clock_t sb_refresh;

	BOOL do_drag;
	HDC hdcSkin;
	HDC hdcButtons;
	HDC hdcKeymap;
#else
	pthread_t hdlThread;
#endif

	BOOL running;
	//BOOL send;
	int speed;
	BYTE breakpoints[0x10000];
	//BOOL warp;
	label_struct labels[6000];
	profiler_t profiler;

	TCHAR labelfn[256];
	applist_t applist;
	apphdr_t *last_transferred_app;

	gif_disp_states gif_disp_state;

#ifdef WINVER
	RECT rectSkin;
	RECT rectLCD;
	COLORREF FaceplateColor;
	BOOL bCustomSkin;
	BOOL bAlwaysOnTop;
	BOOL bAlphaBlendLCD;
	WCHAR skin_path[256];
	WCHAR keymap_path[256];
	IWabbitemu *pWabbitemu;
	ICalcNotify *pCalcNotify;
#endif

} calc_t;

#ifdef WITH_BACKUPS
typedef struct DEBUG_STATE {
	SAVESTATE_t *save;
	struct DEBUG_STATE *next, *prev;
} debugger_backup;
#endif

#define MAX_CALCS	8
#define MAX_SPEED 100*50

typedef struct tagCALC CALC, *LPCALC;

void calc_turn_on(LPCALC);
LPCALC calc_slot_new(void);
u_int calc_count(void);
int calc_reset(LPCALC);
int CPU_reset(CPU_t *);
int calc_run_frame(LPCALC);
int calc_run_seconds(LPCALC, double);
int calc_run_timed(LPCALC, time_t);
int calc_run_all(void);
BOOL calc_start_screenshot(calc_t *calc, const char *filename);
void calc_stop_screenshot(calc_t *calc);

#ifdef WITH_BACKUPS
void do_backup(LPCALC);
void restore_backup(int index, LPCALC);
void init_backups();
void free_backups(LPCALC);
void free_backup(debugger_backup *);
#endif

BOOL rom_load(LPCALC lpCalc, LPCTSTR FileName);
void calc_slot_free(LPCALC);

void calc_unpause_linked();
void calc_pause_linked();

int calc_init_83p(LPCALC);
int calc_init_84p(LPCALC);
int calc_init_83pse(LPCALC);
LPCALC calc_from_cpu(CPU_t *);
void calc_erase_certificate(unsigned char *, int);
void port_debug_callback(void *, void *);
void mem_debug_callback(void *);

#ifdef CALC_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL calc_t calcs[MAX_CALCS];
GLOBAL LPCALC lpDebuggerCalc;

#ifdef WITH_BACKUPS
#define MAX_BACKUPS 10
GLOBAL debugger_backup * backups[MAX_CALCS];
GLOBAL int number_backup;
GLOBAL int current_backup_index;
GLOBAL int num_backup_per_sec;
#endif

GLOBAL u_int frame_counter;
GLOBAL int startX;
GLOBAL int startY;
GLOBAL BOOL exit_save_state;
GLOBAL BOOL load_files_first;
GLOBAL BOOL do_backups;
GLOBAL BOOL show_wizard;
GLOBAL BOOL break_on_exe_violation;
GLOBAL BOOL sync_cores;
GLOBAL link_t *link_hub[MAX_CALCS + 1];


GLOBAL const TCHAR *CalcModelTxt[]
#ifdef CALC_C
= {	//"???",
	_T("TI-81"),
	_T("TI-82"),
	_T("TI-83"),
	_T("TI-85"),
	_T("TI-86"),
	_T("TI-73"),
	_T("TI-83+"),
	_T("TI-83+SE"),
	_T("TI-84+"),
	_T("TI-84+SE"),
	_T("???")}
#endif
;

#define _HAS_CALC_H
#endif
