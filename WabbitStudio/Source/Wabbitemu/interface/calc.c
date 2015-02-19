#include "stdafx.h"

#define CALC_C
#include "calc.h"
#include "core.h"
#include "81hw.h"
#include "83hw.h"
#include "83phw.h"
#include "83psehw.h"
#include "86hw.h"
#include "device.h"
#include "var.h"
#include "gif.h"
#include "link.h"
#include "keys.h"
#include "lcd.h"
#include "colorlcd.h"
#include "savestate.h"

#pragma warning(push)
#pragma warning( disable : 4100 )

#define FRAME_SUBDIVISIONS 1024

const TCHAR *CalcModelTxt[] = {
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
	_T("TI-84+CSE"),
	_T("???")
};

static void calc_debug_callback(LPCALC lpCalc);
void exe_violation_callback(CPU_t *cpu);
void invalid_flash_callback(CPU_t *cpu);
void mem_read_callback(CPU_t *cpu);
void mem_write_callback(CPU_t *cpu);
void port_debug_callback(void *arg1, void *arg2);
void lcd_enqueue_callback(CPU_t *cpu);
void audio_frame_callback(CPU_t *cpu);

/*
 * Determine the slot for a new calculator.  Return a pointer to the calc
 */
LPCALC calc_slot_new(void) {
	if (link_hub == NULL) {
		memset(link_hub_list, 0, sizeof(link_hub_list));
		link_t *hub_link = (link_t *) malloc(sizeof(link_t)); 
		if (!hub_link) {
			printf("Couldn't allocate memory for link hub\n");
		}
		hub_link->host		= 0;				//neither lines set
		hub_link->client	= &hub_link->host;	//nothing plugged in.
		link_hub = hub_link;
	}
	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active == FALSE) {
			memset(&calcs[i], 0, sizeof(calc_t));
			calcs[i].active = TRUE;
			calcs[i].speed = 100;
			calcs[i].slot = i;
			calcs[i].breakpoint_callback = calc_debug_callback;
			return &calcs[i];
		}
	}

	return NULL;
}

u_int calc_count(void) {
	u_int count = 0;

	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active == TRUE)
			count++;
	}
	return count;
}

static int audio_init(LPCALC lpCalc) {
	lpCalc->audio			= &lpCalc->cpu.pio.link->audio;
	lpCalc->audio->enabled	= FALSE;
	lpCalc->audio->init		= FALSE;
	lpCalc->audio->timer_c	= &lpCalc->timer_c;
	lpCalc->audio->cpu		= &lpCalc->cpu;
	return 0;
}

static void setup_callbacks(LPCALC lpCalc) {
	lpCalc->cpu.exe_violation_callback = exe_violation_callback;
	lpCalc->cpu.invalid_flash_callback = invalid_flash_callback;
	lpCalc->cpu.mem_read_break_callback = mem_read_callback;
	lpCalc->cpu.mem_write_break_callback = mem_write_callback;
	lpCalc->cpu.lcd_enqueue_callback = lcd_enqueue_callback;
	lpCalc->cpu.pio.breakpoint_callback = port_debug_callback;
	lpCalc->cpu.mem_c->breakpoint_manager_callback = check_break_callback;
	if (lpCalc->audio != NULL) {
		lpCalc->audio->audio_frame_callback = audio_frame_callback;
	}
}

/* 81 */
int calc_init_81(LPCALC lpCalc, char *version) {
	/* INTIALIZE 81 */
	int error = memory_init_81(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_2);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	// v2 is basically an 82
	if (*version == '2') {
		BOOL isBad82 = TRUE;
		error |= device_init_83(&lpCalc->cpu, isBad82);
		error |= audio_init(lpCalc);
	} else {
		error |= device_init_81(&lpCalc->cpu);
	}
	/* END INTIALIZE 81 */

	setup_callbacks(lpCalc);
	lpCalc->flash_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.flash_size, sizeof(LPBREAKPOINT *));
	lpCalc->ram_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.ram_size, sizeof(LPBREAKPOINT *));
	return error;
}

/*  82 83 */
static BOOL calc_init_83(LPCALC lpCalc, char *os) {
	/* INTIALIZE 83 */
	int error = memory_init_83(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_6);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	BOOL isBad82 = lpCalc->model == TI_82 && memcmp(os, "19.006", 6) != 0;
	error |= device_init_83(&lpCalc->cpu, isBad82);
	error |= audio_init(lpCalc);
	/* END INTIALIZE 83 */

	setup_callbacks(lpCalc);
	lpCalc->flash_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.flash_size, sizeof(LPBREAKPOINT *));
	lpCalc->ram_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.ram_size, sizeof(LPBREAKPOINT *));
	return error;
}

/* 85 86 */
static int calc_init_86(LPCALC lpCalc) {

	/* INTIALIZE 86 */
	int error = memory_init_86(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_4_8);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	error |= device_init_86(&lpCalc->cpu);
	error |= audio_init(lpCalc);
	/* END INTIALIZE 86 */

	setup_callbacks(lpCalc);
	lpCalc->flash_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.flash_size, sizeof(LPBREAKPOINT *));
	lpCalc->ram_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.ram_size, sizeof(LPBREAKPOINT *));
	return error;
}

/* 73 83+ */
int calc_init_83p(LPCALC lpCalc) {
	/* INTIALIZE 83+ */
	int error = memory_init_83p(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_6);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	error |= device_init_83p(&lpCalc->cpu);
	error |= audio_init(lpCalc);
	/* END INTIALIZE 83+ */

	setup_callbacks(lpCalc);
	lpCalc->flash_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.flash_size, sizeof(LPBREAKPOINT *));
	lpCalc->ram_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.ram_size, sizeof(LPBREAKPOINT *));
	return error;
}

/* 83+se 84+se */
int calc_init_83pse(LPCALC lpCalc) {
	/* INTIALIZE 83+se */
	int error = memory_init_83pse(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_6);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	error |= device_init_83pse(&lpCalc->cpu, lpCalc->model);
	error |= audio_init(lpCalc);
	/* END INTIALIZE 83+se */

	setup_callbacks(lpCalc);
	lpCalc->flash_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.flash_size, sizeof(LPBREAKPOINT *));
	lpCalc->ram_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.ram_size, sizeof(LPBREAKPOINT *));
	return error;
}

/* 84+ */
int calc_init_84p(LPCALC lpCalc) {
	/* INTIALIZE 84+ */
	int error = memory_init_84p(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_6);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	error |= device_init_83pse(&lpCalc->cpu, TI_84P);
	error |= audio_init(lpCalc);
	/* END INTIALIZE 84+ */

	setup_callbacks(lpCalc);
	lpCalc->flash_cond_break = (LPBREAKPOINT *)calloc(lpCalc->mem_c.flash_size, sizeof(LPBREAKPOINT *));
	lpCalc->ram_cond_break = (LPBREAKPOINT *)calloc(lpCalc->mem_c.ram_size, sizeof(LPBREAKPOINT *));
	return error;
}

/* 84+CSE */
int calc_init_84pcse(LPCALC lpCalc) {
	/* INTIALIZE 84+CSE */
	int error = memory_init_84pcse(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_6);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	error |= device_init_83pse(&lpCalc->cpu, TI_84PCSE);
	error |= audio_init(lpCalc);
	/* END INTIALIZE 84+CSE */

	setup_callbacks(lpCalc);
	lpCalc->flash_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.flash_size, sizeof(LPBREAKPOINT *));
	lpCalc->ram_cond_break = (LPBREAKPOINT *) calloc(lpCalc->mem_c.ram_size, sizeof(LPBREAKPOINT *));

	return error;
}

void calc_erase_certificate(unsigned char *mem, int size) {
	if (mem == NULL || size < 0x8000) {
		return;
	}

	memset(mem + size - 0x8000, 0xFF, PAGE_SIZE);

	mem[size - 0x8000]				= 0x00;
	mem[size - 0x8000 + 0x1FE0]		= 0x00;
	mem[size - 0x8000 + 0x1FE1]		= 0x00;
	return;
}

int calc_init_model(LPCALC lpCalc, int model, char *verString) {
	int error;
	switch (model) {
	case TI_81:
		error = calc_init_81(lpCalc, verString);
		break;
	case TI_82:
	case TI_83: {
		error = calc_init_83(lpCalc, verString);
		break;
	}
	case TI_73:
	case TI_83P:
		error = calc_init_83p(lpCalc);
		break;
	case TI_84PSE:
	case TI_83PSE:
		error = calc_init_83pse(lpCalc);
		break;
	case TI_84PCSE:
		error = calc_init_84pcse(lpCalc);
		break;
	case TI_84P:
		error = calc_init_84p(lpCalc);
		break;
	case TI_85:
	case TI_86:
		error = calc_init_86(lpCalc);
		break;
	default:
		return -1;
	}

	return error;
}

void notify_event(LPCALC lpCalc, EVENT_TYPE event_type) {
	for (int i = 0; i < MAX_REGISTERED_EVENTS; i++) {
		if (lpCalc->registered_events[i].type == event_type) {
			lpCalc->registered_events[i].callback(lpCalc, lpCalc->registered_events[i].lParam);
		}
	}
}

BOOL rom_load(LPCALC lpCalc, LPCTSTR FileName) {
	if (lpCalc == NULL) {
		return FALSE;
	}
	TIFILE_t* tifile = importvar(FileName, FALSE);
	if (tifile == NULL) {
		return FALSE;
	}

	lpCalc->speed = 100;
	if (lpCalc->active) {
		calc_slot_free(lpCalc);
	}

	lpCalc->model = tifile->model;

	int error = 0;
	if (tifile->type == SAV_TYPE) {
		lpCalc->active 	= TRUE;
		char VerString[64] = { 0 };
		if (tifile->model == TI_82 || tifile->model == TI_83) {
			int size;
			char *rom = GetRomOnly(tifile->save, &size);
			FindRomVersion(VerString, (unsigned char *)rom, size);
		}

		calc_init_model(lpCalc, tifile->model, VerString);
		if (error) {
			FreeTiFile(tifile);
			return FALSE;
		}

		error = LoadSlot(tifile->save, lpCalc) == FALSE;
		if (error) {
			FreeTiFile(tifile);
			return FALSE;
		}

		StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), FileName);
		FindRomVersion(lpCalc->rom_version, lpCalc->mem_c.flash, lpCalc->mem_c.flash_size);
	} else if (tifile->type == ROM_TYPE) {

		switch (tifile->model) {
			case TI_81:
				calc_init_81(lpCalc, tifile->rom->version);
				memcpy(	lpCalc->cpu.mem_c->flash,
						tifile->rom->data,
						(lpCalc->cpu.mem_c->flash_size<=tifile->rom->size)?lpCalc->cpu.mem_c->flash_size:tifile->rom->size);
				break;
			case TI_82:
			case TI_83:
				calc_init_83(lpCalc, tifile->rom->version);
				memcpy(	lpCalc->cpu.mem_c->flash,
					tifile->rom->data,
					(lpCalc->cpu.mem_c->flash_size<=tifile->rom->size)?lpCalc->cpu.mem_c->flash_size:tifile->rom->size);
				break;
			case TI_85:
			case TI_86:
				calc_init_86(lpCalc);
				memcpy(	lpCalc->cpu.mem_c->flash,
						tifile->rom->data,
						(lpCalc->cpu.mem_c->flash_size<=tifile->rom->size)?lpCalc->cpu.mem_c->flash_size:tifile->rom->size);
				break;
			case TI_73:
			case TI_83P:
				calc_init_83p(lpCalc);
				memcpy(	lpCalc->cpu.mem_c->flash,
						tifile->rom->data,
						(lpCalc->cpu.mem_c->flash_size<=tifile->rom->size)?lpCalc->cpu.mem_c->flash_size:tifile->rom->size);
				calc_erase_certificate(lpCalc->cpu.mem_c->flash,lpCalc->cpu.mem_c->flash_size);
				break;
			case TI_84P:
				calc_init_84p(lpCalc);
				memcpy(	lpCalc->cpu.mem_c->flash,
						tifile->rom->data,
						(lpCalc->cpu.mem_c->flash_size<=tifile->rom->size)?lpCalc->cpu.mem_c->flash_size:tifile->rom->size);
				calc_erase_certificate(lpCalc->cpu.mem_c->flash,lpCalc->cpu.mem_c->flash_size);
				break;
			case TI_84PSE:
			case TI_83PSE:
				calc_init_83pse(lpCalc);
				memcpy(	lpCalc->cpu.mem_c->flash,
						tifile->rom->data,
						(lpCalc->cpu.mem_c->flash_size<=tifile->rom->size)?lpCalc->cpu.mem_c->flash_size:tifile->rom->size);
				calc_erase_certificate(lpCalc->cpu.mem_c->flash,lpCalc->cpu.mem_c->flash_size);
				break;
			case TI_84PCSE:
				calc_init_84pcse(lpCalc);
				memcpy(	lpCalc->cpu.mem_c->flash,
					tifile->rom->data,
					(lpCalc->cpu.mem_c->flash_size<=tifile->rom->size)?lpCalc->cpu.mem_c->flash_size:tifile->rom->size);
				calc_erase_certificate(lpCalc->cpu.mem_c->flash,lpCalc->cpu.mem_c->flash_size);
				break;
			default:
				FreeTiFile(tifile);
				return FALSE;
		}

		if (error) {
			FreeTiFile(tifile);
			return FALSE;
		}

		lpCalc->active = TRUE;
		memcpy(lpCalc->rom_version, tifile->rom->version, sizeof(lpCalc->rom_version));
		StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), FileName);
	} else {
		lpCalc = NULL;
		return FALSE;
	}

	if (lpCalc != NULL) {
		lpCalc->cpu.pio.model = lpCalc->model;

		if (tifile->save == NULL) {
			calc_reset(lpCalc);
		}

		if (auto_turn_on) {
			calc_turn_on(lpCalc);
		}

		notify_event(lpCalc, ROM_LOAD_EVENT);
	}

	FreeTiFile(tifile);
	return TRUE;
}

void calc_slot_free(LPCALC lpCalc) {
	if (lpCalc == NULL)
		return;

	if (lpCalc->active) {
		lpCalc->active = FALSE;

		KillSound(lpCalc->audio);
		lpCalc->audio = NULL;

		free(lpCalc->flash_cond_break);
		lpCalc->flash_cond_break = NULL;
		free(lpCalc->ram_cond_break);
		lpCalc->ram_cond_break = NULL;

		free(lpCalc->mem_c.flash);
		lpCalc->mem_c.flash = NULL;
		free(lpCalc->mem_c.ram);
		lpCalc->mem_c.ram = NULL;
		free(lpCalc->mem_c.flash_break);
		lpCalc->mem_c.flash_break = NULL;
		free(lpCalc->mem_c.ram_break);
		lpCalc->mem_c.ram_break = NULL;

		if (link_connected_hub(lpCalc->slot)) {
			link_hub_list[lpCalc->slot] = NULL;
		}
		free(lpCalc->cpu.pio.link);
		lpCalc->cpu.pio.link = NULL;

		free(lpCalc->cpu.pio.keypad);
		lpCalc->cpu.pio.keypad = NULL;

		free(lpCalc->cpu.pio.stdint);
		lpCalc->cpu.pio.stdint = NULL;

		free(lpCalc->cpu.pio.se_aux);
		lpCalc->cpu.pio.se_aux = NULL;

		free(lpCalc->cpu.pio.lcd);
		lpCalc->cpu.pio.lcd = NULL;
	}
}

int calc_reset(LPCALC lpCalc) {
	CPU_reset(&lpCalc->cpu);
	lpCalc->cpu.pio.lcd->reset(&lpCalc->cpu);
	return 0;
}

static void calc_debug_callback(LPCALC lpCalc)
{
	notify_event(lpCalc, BREAKPOINT_EVENT);
}

int calc_run_frame(LPCALC lpCalc) {
	double cpu_sync = lpCalc->timer_c.elapsed;

	while(lpCalc->running) {
		CPU_step(&lpCalc->cpu);

		/* sync CPU */
		if (lpCalc->timer_c.elapsed - cpu_sync > (1.0f / FPS)) {
			if (lpCalc->speed == MAX_SPEED) return 0;
			if (lpCalc->timer_c.elapsed - cpu_sync > (lpCalc->speed / FPS)) return 0;
		}
	}

	return 0;
}

int calc_run_tstates(LPCALC lpCalc, time_t tstates) {
	uint64_t time_end = tc_tstates(&lpCalc->timer_c) + tstates - lpCalc->time_error;

	while (lpCalc->running) {
		if (check_break(&lpCalc->mem_c, addr16_to_waddr(&lpCalc->mem_c, lpCalc->cpu.pc))) {
			calc_set_running(lpCalc, FALSE);
			lpCalc->breakpoint_callback(lpCalc);
			return 0;
		}

		CPU_step(&lpCalc->cpu);

		if (lpCalc->cpu.pio.lcd != NULL && 
			(lpCalc->timer_c.elapsed - lpCalc->cpu.pio.lcd->lastaviframe) >= (1.0 / AVI_FPS))
		{
			notify_event(lpCalc, AVI_VIDEO_FRAME_EVENT);
			lpCalc->cpu.pio.lcd->lastaviframe += 1.0 / AVI_FPS;
		}

		if (tc_tstates((&lpCalc->timer_c)) >= time_end) {
			lpCalc->time_error = (time_t)(tc_tstates((&lpCalc->timer_c)) - time_end);
			break;
		}
	}

	return 0;
}

void calc_turn_on(LPCALC lpCalc) {
	if (lpCalc->cpu.pio.lcd->active) {
		return;
	}

	BOOL running = lpCalc->running;
	while (lpCalc->fake_running) {}
	lpCalc->fake_running = TRUE;
	lpCalc->running = TRUE;
	int time = lpCalc->cpu.timer_c->freq;
	if (lpCalc->model >= TI_84PCSE) {
		time *= 2;
	}

	calc_run_tstates(lpCalc, time);
	keypad_press(&lpCalc->cpu, KEYGROUP_ON, KEYBIT_ON);
	calc_run_tstates(lpCalc, time / 2);
	keypad_release(&lpCalc->cpu, KEYGROUP_ON, KEYBIT_ON);
	int tries = 0;
	do {
		tries++;
		calc_run_tstates(lpCalc, time / 2);
	} while (lpCalc->cpu.halt == FALSE && tries < 3);

	lpCalc->running = running;
	lpCalc->fake_running = FALSE;
}

void calc_register_event(LPCALC lpCalc, EVENT_TYPE event_type, event_callback callback, LPVOID lParam) {
	int i;
	for (i = 0; i < MAX_REGISTERED_EVENTS; i++) {
		if (lpCalc->registered_events[i].type == NO_EVENT) {
			break;
		}
	}

	if (i == MAX_REGISTERED_EVENTS) {
		return;
	}

	lpCalc->registered_events[i].type = event_type;
	lpCalc->registered_events[i].callback = callback;
	lpCalc->registered_events[i].lParam = lParam;
}

void calc_unregister_event(LPCALC lpCalc, EVENT_TYPE event_type, event_callback callback, LPVOID lParam) {
	int i;
	for (i = 0; i < MAX_REGISTERED_EVENTS; i++) {
		if (lpCalc->registered_events[i].type == event_type && 
			lpCalc->registered_events[i].callback == callback &&
			lpCalc->registered_events[i].lParam == lParam) {
			lpCalc->registered_events[i].type = NO_EVENT;
			lpCalc->registered_events[i].callback = NULL;
			lpCalc->registered_events[i].lParam = NULL;
			return;
		}
	}
}

BOOL calc_start_screenshot(const TCHAR *filename) {
	if (gif_write_state == GIF_IDLE) {
		gif_write_state = GIF_START;
		StringCbCopy(screenshot_file_name, sizeof(screenshot_file_name), filename);
		return TRUE;
	} else {
		return FALSE;
	}
}

void calc_stop_screenshot() {
	gif_write_state = GIF_END;
}

const TCHAR *calc_get_model_string(int model) {
	return CalcModelTxt[model];
}

void calc_pause_linked() {
	for (int i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active && calcs[i].running && link_connected_hub(i)) {
			calc_set_running(&calcs[i], FALSE);
		}
	}
}

void calc_unpause_linked() {
	for (int i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active && !calcs[i].running && link_connected_hub(i)) {
			calc_set_running(&calcs[i], TRUE);
		}
	}
}

void calc_set_running(LPCALC lpCalc, BOOL running) {
	lpCalc->running = running;
	notify_event(lpCalc, ROM_RUNNING_EVENT);

	if (link_connected_hub(lpCalc->slot)) {
		if (running) {
			calc_unpause_linked();
		} else {
			calc_pause_linked();
		}
	}
}

int calc_run_all(void) {
	int i, j, active_calc = -1;

	for (i = 0; i < FRAME_SUBDIVISIONS; i++) {
		link_hub->host = 0;
		for (j = 0; j < MAX_CALCS; j++) {
			char hostVal = 0;
			for (int k = 0; k < MAX_CALCS; k++) {
				if (link_hub_list[k] != NULL && *link_hub_list[k]) {
					hostVal |= *link_hub_list[k];
				}
			}

			if (hostVal != link_hub->host) {
				link_hub->host = hostVal;
				/*calc_waiting = TRUE;
				for (int k = 0; k < MAX_CALCS; k++) {
					if (link_hub_list[k]) {
						link_hub_list[k]->hasChanged = TRUE;
						link_hub[k]->changedTime = calcs[k].timer_c.tstates;
					}
				}*/
			}

			if (calcs[j].active && !calcs[j].fake_running) {
				active_calc = j;
				int speed = calcs[j].speed;
				time_t time = ((time_t) speed * calcs[j].timer_c.freq / FPS / 100) / FRAME_SUBDIVISIONS;
				calc_run_tstates(&calcs[j], time);
			}
		}

		//this code handles screenshotting if were actually taking screenshots right now
		if (active_calc >= 0 && !calc_waiting_link && calcs[active_calc].cpu.timer_c != NULL && calcs[active_calc].cpu.pio.lcd != NULL) {
			if ((calcs[active_calc].cpu.timer_c->elapsed - calcs[active_calc].cpu.pio.lcd->lastgifframe) >= 0.01) {
				notify_event(&calcs[active_calc], GIF_FRAME_EVENT);
				calcs[active_calc].cpu.pio.lcd->lastgifframe += 0.01;
			}
		}
	}

	return 0;
}

void port_debug_callback(void *arg1, void *arg2) {
	CPU_t *cpu = (CPU_t *) arg1;
	LPCALC lpCalc = calc_from_cpu(cpu);
	lpCalc->breakpoint_callback(lpCalc);
}

void exe_violation_callback(CPU_t *cpu) {
	if (!break_on_exe_violation) {
		return;
	}

	LPCALC lpCalc = calc_from_cpu(cpu);
	lpCalc->breakpoint_callback(lpCalc);
}

void invalid_flash_callback(CPU_t *cpu) {
	if (!break_on_invalid_flash) {
		return;
	}

	LPCALC lpCalc = calc_from_cpu(cpu);
	if (lpCalc == NULL) {
		return;
	}

	lpCalc->breakpoint_callback(lpCalc);
}

void mem_read_callback(CPU_t *cpu) {
	LPCALC lpCalc = calc_from_cpu(cpu);
	if (lpCalc == NULL) {
		return;
	}

	lpCalc->breakpoint_callback(lpCalc);
}

void mem_write_callback(CPU_t *cpu) {
	LPCALC lpCalc = calc_from_cpu(cpu);
	if (lpCalc == NULL) {
		return;
	}

	lpCalc->breakpoint_callback(lpCalc);
}

void lcd_enqueue_callback(CPU_t *cpu) {
	LPCALC lpCalc = calc_from_cpu(cpu);
	if (lpCalc == NULL) {
		return;
	}

	notify_event(lpCalc, LCD_ENQUEUE_EVENT);
}

void audio_frame_callback(CPU_t *cpu) {
	LPCALC lpCalc = calc_from_cpu(cpu);
	if (lpCalc == NULL) {
		return;
	}

	notify_event(lpCalc, AVI_AUDIO_FRAME_EVENT);
}

int calc_run_seconds(LPCALC lpCalc, double seconds) {
	time_t time = (time_t ) (seconds * CLOCKS_PER_SEC);
	return calc_run_timed(lpCalc, time);
}

int link_connect_hub(int slot, CPU_t *cpu) {
	cpu->pio.link->client = &link_hub->host;
	link_hub_list[slot] = &cpu->pio.link->host;
	link_hub_count++;
	return 0;
}

BOOL link_connected_hub(int slot) {
	return link_hub_list[slot] != NULL;
}

// ticks
int calc_run_timed(LPCALC lpCalc, time_t time) {
	int frames = (int) time / TPF;

	int speed_backup = lpCalc->speed;

	lpCalc->speed = MAX_SPEED;
	while (frames--) 
		calc_run_frame(lpCalc);
	lpCalc->speed = speed_backup;
	return 0;
}

LPCALC calc_from_cpu(CPU_t *cpu) {
	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (&calcs[i].cpu == cpu)
			return &calcs[i];
	}
	return NULL;
}

LPCALC calc_from_memc(memc *memc) {
	for (int i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].cpu.mem_c == memc) {
			return &calcs[i];
		}
	}
	return NULL;
}

#pragma warning(pop)