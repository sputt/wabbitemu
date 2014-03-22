#ifndef _CALC_H
#define _CALC_H

#include "stdafx.h"
#include "coretypes.h"

#include "core.h"
#include "lcd.h"
#include "keys.h"
#include "link.h"
#include "sound.h"
#include "breakpoint.h"
#include "state.h"

typedef enum {
	GDS_IDLE,
	GDS_STARTING,
	GDS_RECORDING,
	GDS_ENDING
} gif_disp_state;

typedef enum {
	NO_EVENT = 0,
	ROM_LOAD_EVENT = 1,
	LCD_ENQUEUE_EVENT,
	ROM_RUNNING_EVENT,
	BREAKPOINT_EVENT,
	GIF_FRAME_EVENT,
	AVI_VIDEO_FRAME_EVENT,
	AVI_AUDIO_FRAME_EVENT,
} EVENT_TYPE;

#define MAX_REGISTERED_EVENTS 0xFF
#define KEY_STRING_SIZE 56
#define AVI_FPS 24

typedef struct {
	TCHAR *name;
	BOOL IsRAM;
	uint8_t page;
	uint16_t addr;
} label_struct;

struct tagCALC;

typedef void(*event_callback)(struct tagCALC *, LPVOID);

typedef struct registered_event {
	EVENT_TYPE type;
	event_callback callback;
	LPVOID lParam;
} registered_event_t;


typedef struct tagCALC {
#ifdef MACVER
	void *breakpoint_owner;
#endif
	void (*breakpoint_callback)(struct tagCALC *);
	int slot;
	TCHAR rom_path[MAX_PATH];
	char rom_version[32];
	CalcModel model;

	time_t time_error;

	BOOL active;
	BOOL running;
	volatile BOOL fake_running;
	CPU_t cpu;
	memory_context_t mem_c;
	timer_context_t timer_c;
	AUDIO_t *audio;

	union {
		struct {
			breakpoint_t **flash_cond_break;
			breakpoint_t **ram_cond_break;
		};
		breakpoint_t **cond_breakpoints[2];
	};

	int speed;
	BYTE breakpoints[0x10000];
	label_struct labels[10000];

	apphdr_t last_transferred_app;

	registered_event_t registered_events[MAX_REGISTERED_EVENTS];
} calc_t, CALC, *LPCALC;

#ifdef QUICKLOOK
#define MAX_CALCS	1
#else
#define MAX_CALCS	8
#endif
#define MAX_SPEED 100*50

void calc_turn_on(LPCALC);
void calc_set_running(LPCALC lpCalc, BOOL running);
LPCALC calc_slot_new(void);
u_int calc_count(void);
int calc_reset(LPCALC);
int CPU_reset(CPU_t *);
int calc_run_frame(LPCALC);
int calc_run_seconds(LPCALC, double);
int calc_run_timed(LPCALC, time_t);
int calc_run_tstates(LPCALC lpCalc, time_t tstates);
int calc_run_all(void);
BOOL calc_start_screenshot(const TCHAR *filename);
void calc_stop_screenshot();
const TCHAR *calc_get_model_string(int model);
void calc_register_event(LPCALC lpCalc, EVENT_TYPE event_type, event_callback callback, LPVOID lParam);
void calc_unregister_event(LPCALC lpCalc, EVENT_TYPE event_type, event_callback callback, LPVOID lParam);

BOOL rom_load(LPCALC lpCalc, LPCTSTR FileName);
void calc_slot_free(LPCALC);

void calc_unpause_linked();
void calc_pause_linked();

int calc_init_model(LPCALC lpCalc, int model, char *verString);

int link_connect(CPU_t *, CPU_t *);
int link_connect_hub(int slot, CPU_t *cpu);

LPCALC calc_from_cpu(CPU_t *);
LPCALC calc_from_memc(memc *);
void calc_erase_certificate(unsigned char *, int);

#ifdef CALC_C
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL calc_t calcs[MAX_CALCS];

GLOBAL BOOL exit_save_state;
GLOBAL BOOL check_updates;
GLOBAL BOOL show_whats_new;
GLOBAL BOOL new_calc_on_load_files;
GLOBAL BOOL do_backups;
GLOBAL BOOL break_on_exe_violation;
GLOBAL BOOL break_on_invalid_flash;
GLOBAL BOOL auto_turn_on;
GLOBAL BOOL sync_cores;
GLOBAL link_t *link_hub[MAX_CALCS + 1];
GLOBAL int link_hub_count;
GLOBAL int calc_waiting_link;
GLOBAL BOOL portable_mode;
GLOBAL TCHAR portSettingsPath[MAX_PATH];

#define _HAS_CALC_H
#endif
