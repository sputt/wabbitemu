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
#include "screenshothandle.h"
#include "link.h"
#include "keys.h"
#include "lcd.h"
#include "colorlcd.h"

#ifdef _WINDOWS
#include "CCalcAddress.h"
#include "CPage.h"
#include "exportvar.h"
#include "guiwizard.h"
#include "CWabbitemu.h"
#endif

#define FRAME_SUBDIVISIONS 1024
#define BOOTFREE_VER_MAJOR 11
#define BOOTFREE_VER_MINOR 258

static void calc_debug_callback(LPCALC lpCalc);
void exe_violation_callback(CPU_t *cpu);
void invalid_flash_callback(CPU_t *cpu);
void mem_read_callback(CPU_t *cpu);
void mem_write_callback(CPU_t *cpu);
void port_debug_callback(void *arg1, void *arg2);

/*
 * Determine the slot for a new calculator.  Return a pointer to the calc
 */
LPCALC calc_slot_new(void) {
#ifdef WITH_BACKUPS
	current_backup_index = 10;
#endif
	if (link_hub[MAX_CALCS] == NULL) {
		memset(link_hub, 0, sizeof(link_hub));
		link_t *hub_link = (link_t *) malloc(sizeof(link_t)); 
		if (!hub_link) {
			printf("Couldn't allocate memory for link hub\n");
		}
		hub_link->host		= 0;				//neither lines set
		hub_link->client	= &hub_link->host;	//nothing plugged in.
		link_hub[MAX_CALCS] = hub_link;
	}
	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active == FALSE) {
			memset(&calcs[i], 0, sizeof(calc_t));
			calcs[i].active = TRUE;
			calcs[i].gif_disp_state = GDS_IDLE;
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
	return 0;
}

static void setup_callbacks(LPCALC lpCalc) {
	lpCalc->cpu.exe_violation_callback = exe_violation_callback;
	lpCalc->cpu.invalid_flash_callback = invalid_flash_callback;
	lpCalc->cpu.mem_read_break_callback = mem_read_callback;
	lpCalc->cpu.mem_write_break_callback = mem_write_callback;
	lpCalc->cpu.pio.breakpoint_callback = port_debug_callback;
	lpCalc->cpu.mem_c->breakpoint_manager_callback = check_break_callback;
}

/* 81 */
int calc_init_81(LPCALC lpCalc, char *version) {
	int error = 0;
	/* INTIALIZE 81 */
	error |= memory_init_81(&lpCalc->mem_c);
	error |= tc_init(&lpCalc->timer_c, MHZ_2);
	error |= CPU_init(&lpCalc->cpu, &lpCalc->mem_c, &lpCalc->timer_c);
	ClearDevices(&lpCalc->cpu);
	//v2 is basically an 82
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
	lpCalc->flash_cond_break = (breakpoint_t **) calloc(lpCalc->mem_c.flash_pages, PAGE_SIZE);
	lpCalc->ram_cond_break = (breakpoint_t **) calloc(lpCalc->mem_c.ram_pages, PAGE_SIZE);
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

// TODO: move this
void check_bootfree_and_update(LPCALC lpCalc) {
	u_char *bootFreeString = lpCalc->mem_c.flash + (lpCalc->mem_c.flash_pages - 1) * PAGE_SIZE + 0x0F;
	if (*bootFreeString != '1') {
		//not using bootfree
		return;
	}
	if (bootFreeString[1] == '.') {
		//using normal bootpage
		return;
	}
#ifdef WINVER
	int majorVer, minorVer;
	scanf_s((char *) bootFreeString, "%d.%d", &majorVer, &minorVer);
	if (MAKELONG(BOOTFREE_VER_MINOR, BOOTFREE_VER_MAJOR) > MAKELONG(minorVer, majorVer)) {
		TCHAR hexFile[MAX_PATH];
		ExtractBootFree(lpCalc->model, hexFile);
		FILE *file;
		_tfopen_s(&file, hexFile, _T("rb"));
		writeboot(file, &lpCalc->mem_c, -1);
		fclose(file);
		_tfopen_s(&file, lpCalc->rom_path, _T("wb"));
		if (file) {
			fclose(file);
			MFILE *mfile = ExportRom(lpCalc->rom_path, lpCalc);
			mclose(mfile);
		}
	}
#endif
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
		switch (tifile->model) {
			case TI_82:
			case TI_83: {
				int size;
				char *rom = GetRomOnly(tifile->save, &size);
				char VerString[64];
				FindRomVersion(VerString, (unsigned char *) rom, size);
				error = calc_init_83(lpCalc, VerString);
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
				FreeTiFile(tifile);
				return FALSE;
		}
		if (error) {
			FreeTiFile(tifile);
			return FALSE;
		}

		LoadSlot(tifile->save, lpCalc);
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
		if (lpCalc->model >= TI_73) {
			check_bootfree_and_update(lpCalc);
		}
		if (tifile->save == NULL) {
			calc_reset(lpCalc);
		}
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
			link_hub[lpCalc->slot] = NULL;
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

#ifdef WITH_BACKUPS
		if (do_backups)
			free_backups(lpCalc);
#endif
	}
}

void calc_turn_on(LPCALC lpCalc)
{
	BOOL running = lpCalc->running;
	lpCalc->running = TRUE;
	calc_run_seconds(lpCalc, 1.0);
	keypad_press(&lpCalc->cpu, KEYGROUP_ON, KEYBIT_ON);
	calc_run_seconds(lpCalc, 1.0);
	keypad_release(&lpCalc->cpu, KEYGROUP_ON, KEYBIT_ON);
#ifdef QUICKLOOK
	calc_run_seconds(lpCalc, 0.5);
#endif
	lpCalc->running = running;
}

int calc_reset(LPCALC lpCalc) {
	CPU_reset(&lpCalc->cpu);
	lpCalc->cpu.pio.lcd->reset(&lpCalc->cpu);
	//calc_turn_on(lpCalc);
	return 0;
}

/*
 *	This is only attached if the program does not attach its own callback
 */
static void calc_debug_callback(LPCALC lpCalc)
{
#ifdef WINVER
	if (lpCalc->pWabbitemu != NULL) {
		waddr_t addr = addr_to_waddr(lpCalc->cpu.mem_c, lpCalc->cpu.pc);
		lpCalc->pWabbitemu->Fire_OnBreakpoint(&addr);
	}
#endif
}

int calc_run_frame(LPCALC lpCalc) {
	double cpu_sync = tc_elapsed((&lpCalc->timer_c));

	while(lpCalc->running) {
		CPU_step(&lpCalc->cpu);

		/* sync CPU */
		if (tc_elapsed((&lpCalc->timer_c)) - cpu_sync > (1.0f / FPS)) {
			if (lpCalc->speed == MAX_SPEED) return 0;
			if (tc_elapsed((&lpCalc->timer_c)) - cpu_sync > (lpCalc->speed / FPS)) return 0;
		}
	}

	return 0;
}

void handle_profiling(LPCALC lpCalc, uint64_t oldTStates, uint16_t oldPC)
{
	if (!lpCalc->profiler.running) {
		return;
	}

	uint64_t time = tc_tstates(&lpCalc->timer_c) - oldTStates;
	lpCalc->profiler.totalTime += time;
	bank_t bank = lpCalc->cpu.mem_c->banks[mc_bank(oldPC)];
	int block = oldPC / lpCalc->profiler.blockSize;
	if (bank.ram) {
		lpCalc->profiler.ram_data[bank.page][block] += time;
	} else {
		lpCalc->profiler.flash_data[bank.page][block] += time;
	}
}

int calc_run_tstates(LPCALC lpCalc, time_t tstates) {
	uint64_t time_end = tc_tstates(&lpCalc->timer_c) + tstates - lpCalc->time_error;

	while (lpCalc->running) {
		if (check_break(&lpCalc->mem_c, addr_to_waddr(&lpCalc->mem_c, lpCalc->cpu.pc))) {
			lpCalc->running = FALSE;
			lpCalc->breakpoint_callback(lpCalc);
			return 0;
		}
		uint64_t oldTStates = 0;
		uint16_t oldPC = 0;
		if (lpCalc->profiler.running) {
			oldTStates = tc_tstates(&lpCalc->timer_c);
			oldPC = lpCalc->cpu.pc % PAGE_SIZE;
		}

		if (link_hub_count > 1) {
			CPU_connected_step(&lpCalc->cpu);
			if (lpCalc->cpu.is_link_instruction) {
				lpCalc->time_error = (time_t)(tc_tstates((&lpCalc->timer_c)) - time_end);
				calc_waiting_link++;
				break;
			}
		} else {
			CPU_step(&lpCalc->cpu);
		}

		handle_profiling(lpCalc, oldTStates, oldPC);


		if (tc_tstates((&lpCalc->timer_c)) >= time_end) {
			lpCalc->time_error = (time_t)(tc_tstates((&lpCalc->timer_c)) - time_end);
			break;
		}
	}

	return 0;
}

BOOL calc_start_screenshot(LPCALC calc, const TCHAR *filename) {
	if (gif_write_state == GIF_IDLE) {
		gif_write_state = GIF_START;
		StringCbCopy(screenshot_file_name, sizeof(screenshot_file_name), filename);
		return TRUE;
	} else {
		return FALSE;
	}
}

void calc_stop_screenshot(LPCALC calc) {
	gif_write_state = GIF_END;
}

void calc_pause_linked() {
	for (int i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active && link_connected_hub(i)) {
			calcs[i].running = FALSE;
		}
	}
}

void calc_unpause_linked() {
	for (int i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active && link_connected_hub(i)) {
			calcs[i].running = TRUE;
		}
	}
}

int calc_run_all(void) {
	int i, j, active_calc = -1;
	BOOL calc_waiting = FALSE;

	for (i = 0; i < FRAME_SUBDIVISIONS; i++) {
		link_hub[MAX_CALCS]->host = 0;
		for (j = 0; j < MAX_CALCS; j++) {
			char hostVal = 0;
			for (int k = 0; k < MAX_CALCS; k++) {
				if (link_hub[k] != NULL && link_hub[k]->host) {
					hostVal |= link_hub[k]->host;
					calc_waiting |= link_hub[k]->hasChanged;
				}
			}
			if (hostVal != link_hub[MAX_CALCS]->host) {
				link_hub[MAX_CALCS]->host = hostVal;
				calc_waiting = TRUE;
				for (int k = 0; k < MAX_CALCS; k++) {
					if (link_hub[k]) {
						link_hub[k]->hasChanged = TRUE;
						link_hub[k]->changedTime = calcs[k].timer_c.tstates;
					}
				}
			}
			if (calcs[j].active) {
				/*if (link_hub[j] != NULL && (!link_hub[MAX_CALCS]->host || link_hub[MAX_CALCS]->host != link_hub[j]->host)
					&& calcs[j].cpu.is_link_instruction || ((int) (calcs[j].cpu.linking_time - calcs[j].cpu.timer_c->tstates) >= 100000)) {
					calcs[j].cpu.is_link_instruction = FALSE;
					calcs[j].cpu.linking_time = 0;
					CPU_step(&calcs[j].cpu);
				}*/
				if (calcs[j].cpu.is_link_instruction && calcs[j].cpu.pio.link->changedTime - calcs[j].timer_c.tstates >= 100000) {
					calcs[j].cpu.is_link_instruction = FALSE;
					calcs[j].cpu.pio.link->changedTime = 0;
					calcs[j].cpu.pio.link->hasChanged = FALSE;
					CPU_step(&calcs[j].cpu);
				}
				active_calc = j;
				// TODO: figure out how to do this
				int speed = calcs[j].speed == MAX_SPEED ? 2400 : calcs[j].speed;
				int time = (int)((int64_t) speed * calcs[j].timer_c.freq / FPS / 100) / FRAME_SUBDIVISIONS;
				if (!calcs[j].cpu.is_link_instruction || !calc_waiting || calcs[j].cpu.pio.link->hasChanged == TRUE) {
					calc_run_tstates(&calcs[j], time);
				} /*else {
					calcs[j].cpu.linking_time += time;
				}*/
			}
		}

		if (link_hub_count > 1 && calc_waiting_link >= link_hub_count) {
			for (int k = 0; k < MAX_CALCS; k++) {
				if (calcs[k].cpu.is_link_instruction) {
					calcs[k].cpu.is_link_instruction = FALSE;
					CPU_step(&calcs[k].cpu);
				}
			}
			calc_waiting_link = 0;
		}

		//this code handles screenshotting if were actually taking screenshots right now
		if (active_calc >= 0 && !calc_waiting_link && calcs[active_calc].cpu.timer_c != NULL && calcs[active_calc].cpu.pio.lcd != NULL &&
				((tc_elapsed(calcs[active_calc].cpu.timer_c) - calcs[active_calc].cpu.pio.lcd->lastgifframe) >= 0.01)) {
			handle_screenshot();
			calcs[active_calc].cpu.pio.lcd->lastgifframe += 0.01;
		}
	}

	return 0;
}

void link_step(CPU_t *cpu) {

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
	lpCalc->breakpoint_callback(lpCalc);
}

void mem_read_callback(CPU_t *cpu) {
	LPCALC lpCalc = calc_from_cpu(cpu);
	lpCalc->breakpoint_callback(lpCalc);
}

void mem_write_callback(CPU_t *cpu) {
	LPCALC lpCalc = calc_from_cpu(cpu);
	lpCalc->breakpoint_callback(lpCalc);
}

#ifdef WITH_BACKUPS
void do_backup(LPCALC lpCalc) {
	if (!lpCalc->running)
		return;
	int slot = lpCalc->slot;
	if (number_backup > MAX_BACKUPS) {
		debugger_backup* oldestBackup = backups[slot];
		while(oldestBackup->prev != NULL)
			oldestBackup = oldestBackup->prev;
		oldestBackup->next->prev = NULL;
		free_backup(oldestBackup);
	}
	debugger_backup *backup = (debugger_backup *) malloc(sizeof(debugger_backup));
	backup->save = SaveSlot(lpCalc, _T("Backup save"), _T("Wabbitemu"));
	backup->next = NULL;
	backup->prev = backups[slot];
	if (backups[slot] != NULL)
		backups[slot]->next = backup;
	backups[slot] = backup;
	number_backup++;
}
void restore_backup(int index, LPCALC lpCalc) {
	int slot = lpCalc->slot;
	debugger_backup* backup = backups[slot];
	while (index > 0) {
		if (backup->prev == NULL)
			break;
		backup = backup->prev;
		free_backup(backup->next);
		index--;
	}
	//shouldn't happen
	if (backup != NULL)
		LoadSlot(backup->save, lpCalc);
	backups[slot] = backup;
}

void init_backups() {
	int i;
	number_backup = 0;
	for(i = 0; i < MAX_CALCS; i++)
		backups[i] = NULL;
}

void free_backup(debugger_backup* backup) {
	if (backup == NULL)
		return;
	FreeSave(backup->save);
	free(backup);
	number_backup--;
}

/*
 * Frees all backups from memory
 */
void free_backups(LPCALC lpCalc) {
	int slot = lpCalc->slot;
	debugger_backup *backup_prev, *backup = backups[slot];
	if (backup == NULL)
		return;
	do {
		backup_prev = backup->prev;
		free_backup(backup);
		backup = backup_prev;
	} while(backup != NULL);
	backups[slot] = NULL;
	number_backup = 0;
}
#endif

int calc_run_seconds(LPCALC lpCalc, double seconds) {
	time_t time = (time_t ) (seconds * CLOCKS_PER_SEC);
	return calc_run_timed(lpCalc, time);
}

int link_connect_hub(int slot, CPU_t *cpu) {
	link_hub[slot] = cpu->pio.link;
	cpu->pio.link->client = &link_hub[MAX_CALCS]->host;
	link_hub_count++;
	return 0;
}

BOOL link_connected_hub(int slot) {
	return link_hub[slot] != NULL;
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
