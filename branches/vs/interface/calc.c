#define CALC_C
#include "calc.h"
#include "core.h"
#include "83hw.h"
#include "83phw.h"
#include "83psehw.h"
#include "86hw.h"
#include "device.h"
#ifdef WINVER
#include "disassemble.h"
#endif

/* Determine the slot for a new calculator.  Return the slot if found,
 * otherwise, return -1.
 */
int calc_slot_new(void) {
	int i;
	for (i = 0; i < MAX_CALCS; i++) {
		if (calcs[i].active == FALSE) {
			memset(&calcs[i], 0, sizeof(calc_t));
			calcs[i].gif_disp_state = GDS_IDLE;
			return i;
		}
	}
	return -1;
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

/*  82 83 */
static BOOL calc_init_83(int slot, char* os) {
	/* INTIALIZE 83 */ 
	memory_init_83(&calcs[slot].mem_c); 
	tc_init(&calcs[slot].timer_c, MHZ_6); 
	CPU_init(&calcs[slot].cpu, &calcs[slot].mem_c, &calcs[slot].timer_c);
	ClearDevices(&calcs[slot].cpu);
	if (calcs[slot].model == TI_82) {
		if (memcmp(os,"19.006",6)==0) {
			device_init_83(&calcs[slot].cpu,0);
		} else {
			device_init_83(&calcs[slot].cpu,1);
		}
	} else {
		device_init_83(&calcs[slot].cpu,0);
	}
	/* END INTIALIZE 83 */

	calcs[slot].send			= FALSE;
	#ifdef WINVER // FIXME: dirty cheater!
	calcs[slot].audio			= &calcs[slot].cpu.pio.link->audio;
	calcs[slot].audio->enabled	= FALSE;
	calcs[slot].audio->init		= FALSE;
	#endif
	return TRUE;
}

/* 85 86 */
static int calc_init_86(int slot) {
#ifndef _WINDLL
	/* INTIALIZE 83+ */ 
	printf("initializing 86!\n");
	memory_init_86(&calcs[slot].mem_c); 
	tc_init(&calcs[slot].timer_c, MHZ_4_8); 
	CPU_init(&calcs[slot].cpu, &calcs[slot].mem_c, &calcs[slot].timer_c);
	ClearDevices(&calcs[slot].cpu);
	device_init_86(&calcs[slot].cpu);
	/* END INTIALIZE 83+ */

	calcs[slot].send			= FALSE;
	#ifdef WINVER // FIXME: dirty cheater!
	calcs[slot].audio			= &calcs[slot].cpu.pio.link->audio;
	calcs[slot].audio->enabled	= FALSE;
	calcs[slot].audio->init		= FALSE;
	#endif
#endif
	return 0;
}

/* 73 83+ */
static int calc_init_83p(int slot) {
	/* INTIALIZE 83+ */ 
	memory_init_83p(&calcs[slot].mem_c); 
	tc_init(&calcs[slot].timer_c, MHZ_6); 
	CPU_init(&calcs[slot].cpu, &calcs[slot].mem_c, &calcs[slot].timer_c);
	ClearDevices(&calcs[slot].cpu);
	device_init_83p(&calcs[slot].cpu);
	/* END INTIALIZE 83+ */

	calcs[slot].send			= FALSE;
	#ifdef WINVER // FIXME: dirty cheater!
	calcs[slot].audio			= &calcs[slot].cpu.pio.link->audio;
	calcs[slot].audio->enabled	= FALSE;
	calcs[slot].audio->init		= FALSE;
	#endif
	return 0;
}

/* 83+se */
static int calc_init_83pse(int slot) {
#ifndef _WINDLL
	/* INTIALIZE 83+se */ 
	memory_init_83pse(&calcs[slot].mem_c); 
	
	tc_init(&calcs[slot].timer_c, MHZ_6); 
	CPU_init(&calcs[slot].cpu, &calcs[slot].mem_c, &calcs[slot].timer_c);
	ClearDevices(&calcs[slot].cpu);
	device_init_83pse(&calcs[slot].cpu);
	/* END INTIALIZE 83+se */


	calcs[slot].send			= FALSE;
	#ifdef WINVER // FIXME: dirty cheater!
	calcs[slot].audio			= &calcs[slot].cpu.pio.link->audio;
	calcs[slot].audio->enabled	= FALSE;
	calcs[slot].audio->init		= FALSE;
	#endif
#endif
	return 0;
}


void calc_erase_certificate( u_char* mem, int size) {
	if (mem == NULL || size < 32768) return;

	memset(mem + size - 32768, 0xFF, 16384);
	
	mem[(size-32768)]			= 0x00;
	mem[(size-32768)+0x1FE0]	= 0x00;
	mem[(size-32768)+0x1FE1]	= 0x00;
	return;
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
int rom_load(int slot, char * FileName) {
	if (slot == -1)
		return -1;

	TIFILE_t* tifile = importvar(FileName);
	if (tifile == NULL)
		return -1;
	
	if (calcs[slot].active) 
		calc_free(slot);
	calcs[slot].model = tifile->model;
	
	if (tifile->type == SAV_TYPE) {
		calcs[slot].active 	= TRUE;
		switch (tifile->model) {
			case TI_82:
			case TI_83:
			{
				int size;
				char* rom = GetRomOnly(tifile->save,&size);
				char VerString[64];
				FindRomVersion(	tifile->model,
								VerString,
								(unsigned char*) rom,
								size);
				
				calc_init_83(slot,VerString);
				break;
			} 
			case TI_73:
			case TI_83P:
				calc_init_83p(slot);
				break;
			case TI_84PSE:
			case TI_83PSE:
				calc_init_83pse(slot);
				break;
			default:
				puts("Unknown model");
				FreeTiFile(tifile);
				return -1;
		}
	
		LoadSlot(tifile->save,slot);
		strcpy(calcs[slot].rom_path, FileName);
		FindRomVersion(	tifile->model,
						calcs[slot].rom_version,
						calcs[slot].mem_c.flash,
						calcs[slot].mem_c.flash_size);
	} else if (tifile->type == ROM_TYPE) {

		switch (tifile->model) {
			case TI_82:
			case TI_83:
				calc_init_83(slot,tifile->rom->version);
				memcpy(	calcs[slot].cpu.mem_c->flash, 
						tifile->rom->data, 
						(calcs[slot].cpu.mem_c->flash_size<=tifile->rom->size)?calcs[slot].cpu.mem_c->flash_size:tifile->rom->size);
				break;
			case TI_86:
				calc_init_86(slot);
				memcpy(	calcs[slot].cpu.mem_c->flash, 
						tifile->rom->data, 
						(calcs[slot].cpu.mem_c->flash_size<=tifile->rom->size)?calcs[slot].cpu.mem_c->flash_size:tifile->rom->size);
				break;
			case TI_73:
			case TI_83P:
				calc_init_83p(slot);
				memcpy(	calcs[slot].cpu.mem_c->flash, 
						tifile->rom->data, 
						(calcs[slot].cpu.mem_c->flash_size<=tifile->rom->size)?calcs[slot].cpu.mem_c->flash_size:tifile->rom->size);
				calc_erase_certificate(calcs[slot].cpu.mem_c->flash,calcs[slot].cpu.mem_c->flash_size);
				break;
			case TI_84P:
				calc_init_83pse(slot);
				printf("Setting custom memory settings for the 84+\n");
				calcs[slot].mem_c.banks[1].addr = calcs[slot].mem_c.flash+0x3F*PAGE_SIZE;
				calcs[slot].mem_c.banks[1].page = 0x3F;
				calcs[slot].mem_c.banks[2].addr = calcs[slot].mem_c.flash+0x3F*PAGE_SIZE;
				calcs[slot].mem_c.banks[2].page = 0x3F;
				calcs[slot].mem_c.flash_pages = 0x40;
#ifndef _WINDLL
				void port21_84p(CPU_t *cpu, device_t *dev);
				calcs[slot].cpu.pio.devices[0x21].code = (devp) &port21_84p;
#endif
				
				memcpy(	calcs[slot].cpu.mem_c->flash,
						tifile->rom->data, 
						(calcs[slot].cpu.mem_c->flash_size<=tifile->rom->size)?calcs[slot].cpu.mem_c->flash_size:tifile->rom->size);
				calc_erase_certificate(calcs[slot].cpu.mem_c->flash,calcs[slot].cpu.mem_c->flash_size);
				break;
			case TI_84PSE:
			case TI_83PSE:
				calc_init_83pse(slot);
				memcpy(	calcs[slot].cpu.mem_c->flash,
						tifile->rom->data, 
						(calcs[slot].cpu.mem_c->flash_size<=tifile->rom->size)?calcs[slot].cpu.mem_c->flash_size:tifile->rom->size);
				calc_erase_certificate(calcs[slot].cpu.mem_c->flash,calcs[slot].cpu.mem_c->flash_size);
				break;
			default:
				FreeTiFile(tifile);
				return -1;
		}
		
		calcs[slot].active = TRUE;
		memcpy(calcs[slot].rom_version, tifile->rom->version, sizeof(calcs[slot].rom_version));
		strcpy(calcs[slot].rom_path, FileName);
		calc_reset(slot);
		
	} else slot = -1;
	if (slot != -1) {
		calcs[slot].cpu.pio.model =calcs[slot].model;
	}
	
#ifndef _WINDLL
	gui_frame_update(slot);
#endif
	
	FreeTiFile(tifile);
	return slot;
}

void calc_free(int slot) {
	if (slot == -1)
		return;
	
	if (calcs[slot].active) {
		calcs[slot].active = FALSE;
		#ifdef WINVER
		/* don't forget to change this when audio for non-Windows
		 * builds is implemented, or bad things happen! */
		KillSound(calcs[slot].audio);
		#endif
		printf("Freeing memory\n");
		free(calcs[slot].mem_c.flash);
		free(calcs[slot].mem_c.ram);
		printf("Freeing hardware\n");
		free(calcs[slot].cpu.pio.link);
		printf("freeing keypad\n");
		free(calcs[slot].cpu.pio.keypad);
		printf("freeing stdint\n");
		free(calcs[slot].cpu.pio.stdint);
		printf("freeing se aux %p\n", calcs[slot].cpu.pio.se_aux);
		free(calcs[slot].cpu.pio.se_aux);
		printf("freeing lcd\n");
		free(calcs[slot].cpu.pio.lcd);
		printf("Done freeing\n");
	}

}
	
/* Clear RAM and start calculator at $0000 */
int calc_reset(int slot) {
	//memset(calcs[slot].mem_c.ram, 0, calcs[slot].mem_c.ram_size);
	calcs[slot].cpu.pc			= 0;
	calcs[slot].cpu.sp			= 0;
	calcs[slot].cpu.interrupt	= FALSE;
	calcs[slot].cpu.imode		= 1;
	calcs[slot].cpu.ei_block	= FALSE;
	calcs[slot].cpu.iff1		= FALSE;
	calcs[slot].cpu.iff2		= FALSE;
	calcs[slot].cpu.halt		= FALSE;
	calcs[slot].cpu.read		= FALSE;
	calcs[slot].cpu.write		= FALSE;
	calcs[slot].cpu.output		= FALSE;
	calcs[slot].cpu.input		= FALSE;
	calcs[slot].cpu.prefix		= 0;
	calcs[slot].cpu.mem_c->port28_remap_count = 0;
	return 0;
}

int calc_run_frame(int slot) {
	double cpu_sync = tc_elapsed((&calcs[slot].timer_c));
	
	while(calcs[slot].running) {
		CPU_step(&calcs[slot].cpu);

		/* sync CPU */
		if (tc_elapsed((&calcs[slot].timer_c)) - cpu_sync > (1.0f / FPS)) {
			if (!calcs[slot].warp) return 0;
			if (tc_elapsed((&calcs[slot].timer_c)) - cpu_sync > (8.0f / FPS)) return 0;
		}
	}
	return 0;
}

int calc_run_tstates(int slot, time_t tstates) {
	long long time_end = tc_tstates((&calcs[slot].timer_c)) + tstates - calcs[slot].time_error;
	
	while(calcs[slot].running) {
		
		if (check_break(&calcs[slot].mem_c, calcs[slot].cpu.pc) & 1) {
			calcs[slot].running = FALSE;
			bank_t *bank = &calcs[slot].mem_c.banks[mc_bank(calcs[slot].cpu.pc)];
			
			Z80_info_t z[2];
			disassemble(&calcs[slot].mem_c, calcs[slot].cpu.pc, 1, z);
			
			if (calcs[slot].ole_callback != NULL) {
				PostMessage(calcs[slot].ole_callback, WM_USER, bank->ram<<16 | bank->page, z[0].size<<16 | calcs[slot].cpu.pc);
				printf("postmessage called!\n");
			} else {
				gui_debug(slot);
			}
			return 0;
		}

		CPU_step(&calcs[slot].cpu);
	
		if (tc_tstates((&calcs[slot].timer_c)) >= time_end) {
			calcs[slot].time_error = tc_tstates((&calcs[slot].timer_c)) - time_end;
			break;	
		}
	}
	
	return 0;
}


#define FRAME_SUBDIVISIONS	1024
#ifdef _WINDLL
__declspec(dllexport)
#endif
int calc_run_all(void) {
	int i,j;
	int mult = 1;
	for (j = 0; j < MAX_CALCS; j++) {
		if (calcs[j].active && calcs[j].warp) {
			mult = 8; // if any are warped, all are warped
		}
			
	}
		
	for (i = 0; i < FRAME_SUBDIVISIONS; i++) {
		for (j = 0; j < MAX_CALCS; j++) {
			if (calcs[j].active) 
				calc_run_tstates(j, (mult*calcs[j].timer_c.freq/FPS)/FRAME_SUBDIVISIONS);
		}
	}
	
	return 0;
}


int calc_run_seconds(int slot, double seconds) {
	time_t time = (time_t ) (seconds * CLOCKS_PER_SEC);
	return calc_run_timed(slot, time);
}


// ticks
int calc_run_timed(int slot, time_t time) {
	int frames = (int) time / TPF;
	
	BOOL warp_backup = calcs[slot].warp;

	calcs[slot].warp = TRUE;
	while (frames--) calc_run_frame(slot);
	calcs[slot].warp = warp_backup;
	return 0;
}

int calc_from_hwnd(HWND hwnd) {
	if (hwnd == NULL)
		return -1;

	int slot;
	for (slot = 0; slot < MAX_CALCS; slot++) {
		if (calcs[slot].active) {
			if ((hwnd == calcs[slot].hwndFrame) || 
				(hwnd == calcs[slot].hwndLCD) || 
				(hwnd == calcs[slot].hwndStatusBar)) {
				return slot;
			}
		}
	}
	return -1;
}
