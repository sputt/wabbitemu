#include "stdafx.h"

#include "lcd.h"
#include "keys.h"
#include "83phw.h"
#include "link.h"
#include "device.h"
#include "calc.h"
#include "83psehw.h"
#ifdef WINVER
#include "dbbreakpoints.h"
#endif

#define BIT(bit) (1 << bit)

static double timer_freq83p[4] = { 1.0f / 560.0f, 1.0f / 248.0f, 1.0f / 170.0f, 1.0f / 118.0f };

//------------------------
// bit 0 - Tip
// Bit 1 - Ring
// bit 2 - Link assist active
// bit 3 - Link assist completed a byte
// bit 4 - Last written tip (Not implemented...yet)
// bit 5 - Last written ring (Not implemented)
// bit 6 - Link assist receiving
// bit 7 - Not used
// set bit 2 to enable link assist
static void port0(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t *assist = (LINKASSIST_t *) dev->aux;
	link_t *link = cpu->pio.link;
	if (cpu->input) {
		link->hasChanged = FALSE;
		link->changedTime = 0;

		cpu->bus = (((link->host & 0x03) | (link->client[0] & 0x03)) ^ 0x03) | (assist->link_enable & 0x04);
		if (assist->read)
			cpu->bus += 8;
		if (assist->receiving)
			cpu->bus += 64;
		cpu->input = FALSE;
	} else if (cpu->output) {
#ifdef WINVER // lazy me
		if ((link->host & 0x01) != (cpu->bus & 0x01)) {
			FlippedLeft(cpu, cpu->bus & 0x01);				//sound .. #$%# you
		}
		if ((link->host & 0x02) != (cpu->bus & 0x02)) {
			FlippedRight(cpu, (cpu->bus & 0x02) >> 1);	//sound, not portable
		}
#endif		
		assist->link_enable = cpu->bus & 0x04;
		link->host = cpu->bus & 0x03;
		cpu->output = FALSE;
	}
#ifdef WINVER // :P
	if (link->audio.init && link->audio.enabled) nextsample(cpu);
#endif
}

static void port2(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (cpu->pio.model == TI_73 ? 0x39 : 0x3B) | (cpu->mem_c->flash_locked ? 0 : 4);
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

static void port3(CPU_t *cpu, device_t *dev) {
	STDINT_t * stdint = (STDINT_t *) dev->aux;
	
	if (cpu->input) {
		cpu->bus = stdint->intactive;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ( (!(stdint->intactive & 0x08)) && (cpu->bus & 0x08) ) {
			cpu->pio.lcd->active = TRUE;		//I'm worried about this
			/*
			Normally when the calc is set to a low power state and hits a halt
			the LCD ram disconnects from the LCD.  It does not physically power
			off the LCD.  However for the sake of ease, its powering off here.
			should normally be fine, since no one should rely on a powered on
			disconnected LCD.
			*/
		}
		if ((cpu->bus & 0x01) == 0)
			stdint->on_latch = FALSE;
		
		stdint->intactive = cpu->bus;
		cpu->output = FALSE;
	}
	
	/*Read note above*/
	if (!(stdint->intactive & 0x08)  && (cpu->halt==TRUE)) {
		cpu->pio.lcd->active = FALSE;
	}

/*
	Standard interrupt
	occurs on a base frequency
	when mask timer continues to tick but 
	does not generate an interrupt. */
	if (stdint->intactive & 0x02) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1)
			cpu->interrupt = TRUE;
	} else if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
		while ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1)
			stdint->lastchk1 += stdint->timermax1;
	}

/*
	Double speed standard interrupt
	occurs (1/(4*frequency)) second after standard timer.
	when mask timer continues to tick but 
	does not generate an interrupt. */
	if (stdint->intactive & 0x04) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2)
			cpu->interrupt = TRUE;
	} else if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) {
		while ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2)
			stdint->lastchk2 += stdint->timermax2;
	}

	if ((stdint->intactive & 0x01) && (cpu->pio.keypad->on_pressed & KEY_VALUE_MASK) && ((stdint->on_backup & KEY_VALUE_MASK) == 0))  {
		stdint->on_latch = TRUE;
	}
	stdint->on_backup = cpu->pio.keypad->on_pressed;
	if (stdint->on_latch)
		cpu->interrupt = TRUE;
}

static void port4(CPU_t *cpu, device_t *dev) {
	STDINT_t * stdint = (STDINT_t *) dev->aux;
	if (cpu->input) {
		unsigned char result = 0;
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) result += 2;
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) result += 4;
		
		if (stdint->on_latch) result += 1;
		if (!cpu->pio.keypad->on_pressed) result += 8;
		
		//printf("interrupt checked: %02x\n", result);
		cpu->bus = result;
		cpu->input = FALSE;
	} else if (cpu->output) {
		/* I'm not sure if this is how the interrupts work. */
		/* but for practicality its close enough for now. */
		int freq = ((cpu->bus & 6) >> 1);
		stdint->timermax1 = stdint->freq[freq];
		stdint->timermax2 = stdint->freq[freq] / 2.0f;
		stdint->lastchk2  = stdint->lastchk1 + (stdint->freq[freq] / 4.0f);

		if (cpu->bus & 1) {
			cpu->mem_c->boot_mapped = TRUE;
			cpu->mem_c->banks = cpu->mem_c->bootmap_banks;
			update_bootmap_pages(cpu->mem_c);
		} else {
			cpu->mem_c->boot_mapped = FALSE;
			cpu->mem_c->banks = cpu->mem_c->normal_banks;
		}
		cpu->output = FALSE;
	}	
}

//link assist port, returns whatever is read from link assisting
static void port5(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t *assist = (LINKASSIST_t *) dev->aux;
	link_t *link = cpu->pio.link;
	if (!cpu->input && !cpu->output) {
		if (assist->link_enable & 0x04) {
			if (LinkRead != 3)
				assist->ready = FALSE;
			switch (LinkRead & 0x03) {
				case 01:
					if (assist->bit < 8) {
						if (!assist->receiving)
							assist->bit = 0;
						assist->receiving = TRUE;
						if (!link->host) {
							assist->working  = (assist->working >> 1) + 128;
							assist->bit++;
							link->host = 1;
						} else {
							link->host = 0;
						}
					} else if (assist->bit == 8) {
						if (link->host)
							link->host = 0;
						if (!assist->read) {
							assist->receiving = FALSE;
							assist->read = TRUE;
							assist->in = assist->working;
							assist->bit = 0;
						}
					}
					break;
				case 02:
					if (assist->bit < 8) {
						if (!assist->receiving)
							assist->bit = 0;
						assist->receiving = TRUE;
						if (!link->host) {
							assist->working  = (assist->working >> 1);
							assist->bit++;
							link->host = 2;
						} else {
							link->host = 0;
						}
					} else if (assist->bit == 8) {
						if (link->host)
							link->host = 0;
						if (!assist->read) {
							assist->receiving = FALSE;
							assist->read = TRUE;
							assist->in = assist->working;
							assist->bit = 0;
						}
					}
					break;
				case 03:
					if (assist->bit >=8 && !assist->read) {
						assist->receiving = FALSE;
						assist->read = TRUE;
						assist->in = assist->working;
						assist->bit = 0;
					} else if (assist->bit ==0) {
						assist->ready = TRUE;
					}
					break;
			} 
		}
	}
	if (cpu->input) {
		assist->read = FALSE;
		cpu->bus = assist->in;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->protected_page_set = cpu->bus  & 0x07;
		cpu->output = FALSE;
	}
}

static void port6(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (cpu->mem_c->banks[1].ram << 6) + cpu->mem_c->banks[1].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		BOOL ram = (cpu->bus >> 6) & 1;
		if (ram)
			change_page(cpu, 1, (cpu->bus & 0x1f) % cpu->mem_c->ram_pages, ram);
		else
			change_page(cpu, 1, (cpu->bus & 0x1f) % cpu->mem_c->flash_pages, ram);
		cpu->output = FALSE;
	}
}

static void port7(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = ((cpu->mem_c->banks[2].ram) << 6) + cpu->mem_c->banks[2].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		BOOL ram = (cpu->bus >> 6) & 1;
		if (ram)
			change_page(cpu, 2, (cpu->bus & 0x1f) % cpu->mem_c->ram_pages, ram);
		else
			change_page(cpu, 2, (cpu->bus & 0x1f) % cpu->mem_c->flash_pages, ram);
		cpu->output = FALSE;
	}
}


static void port14(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->input = FALSE;
	} else if (cpu->output) {
		int bank = cpu->pc >> 14;
		if (is_priveleged_page(cpu)) {
			cpu->mem_c->flash_locked = !(cpu->bus & BIT(0));
		}
		cpu->output = FALSE;
	}
}

static void port16(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		port14(cpu, dev);
		cpu->input = FALSE;
	} else if (cpu->output) {
		int offset = cpu->mem_c->protected_page_set;
		if (offset == 7)
			offset = 3;
		cpu->mem_c->protected_page[offset] = cpu->bus;
		cpu->output = FALSE;
	}
}

/*----------------------------------------------*/
/*												*/
/*				Initialize						*/
/*												*/
/*----------------------------------------------*/

static STDINT_t* INT83P_init(CPU_t* cpu) {
	STDINT_t * stdint = (STDINT_t *) malloc(sizeof(STDINT_t));
	if (!stdint) {
		printf("Couldn't allocate memory for standard interrupt\n");
		return NULL;
	}
	
	memcpy(stdint->freq, timer_freq83p, 4 * sizeof(stdint->freq[0]));
	
	stdint->intactive = 0;
	stdint->timermax1 = stdint->freq[3];
	stdint->lastchk1 = tc_elapsed(cpu->timer_c);
	stdint->timermax2 = stdint->freq[3] / 2.0f;
	stdint->lastchk2 = tc_elapsed(cpu->timer_c) + stdint->freq[3] / 4.0f;
	stdint->on_backup = 0;
	stdint->on_latch = FALSE;
	return stdint;
}

static link_t* link83p_init(CPU_t* cpu) {
	link_t * link = (link_t *) malloc(sizeof(link_t));
	if (!link) {
		printf("Couldn't allocate memory for link\n");
		exit(1);
	}
	link->host		= 0;			//neither lines set
	link->client	= &link->host;	//nothing plugged in.

	return link;
}

int device_init_83p(CPU_t *cpu) {
	ClearDevices(cpu);
	
	LINKASSIST_t *assist = (LINKASSIST_t *) malloc(sizeof(LINKASSIST_t));
	assist->link_enable = 0;
	link_t *link = link83p_init(cpu);
	cpu->pio.devices[0x00].active = TRUE;
	cpu->pio.devices[0x00].aux = assist;
	cpu->pio.devices[0x00].code = (devp) port0;

	keypad_t *keyp = keypad_init(cpu);
	cpu->pio.devices[0x01].active = TRUE;
	cpu->pio.devices[0x01].aux = keyp;
	cpu->pio.devices[0x01].code = (devp) keypad;

	cpu->pio.devices[0x02].active = TRUE;
	cpu->pio.devices[0x02].code = (devp) port2;

	STDINT_t* stdint = INT83P_init(cpu);
	cpu->pio.devices[0x03].active = TRUE;
	cpu->pio.devices[0x03].aux = stdint;
	cpu->pio.devices[0x03].code = (devp) port3;

	cpu->pio.devices[0x04].active = TRUE;
	cpu->pio.devices[0x04].aux = stdint;
	cpu->pio.devices[0x04].code = (devp) port4;

	cpu->pio.devices[0x05].active = TRUE;
	cpu->pio.devices[0x05].aux = assist;
	cpu->pio.devices[0x05].code = (devp) port5;

	cpu->pio.devices[0x06].active = TRUE;
	cpu->pio.devices[0x06].code = (devp) port6;

	cpu->pio.devices[0x07].active = TRUE;
	cpu->pio.devices[0x07].code = (devp) port7;

	LCD_t *lcd = LCD_init(cpu, TI_83P);
	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp) LCD_command;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp) LCD_data;

	cpu->pio.devices[0x14].active = TRUE;
	cpu->pio.devices[0x14].code = (devp) port14;
	//protected means flash = unlocked, this would be a problem
	//cpu->pio.devices[0x14].protected_port = TRUE;

	cpu->pio.devices[0x16].active = TRUE;
	cpu->pio.devices[0x16].code = (devp) port16;
	cpu->pio.devices[0x16].protected_port = TRUE;

	//shadows
	cpu->pio.devices[0x21].active = TRUE;
	cpu->pio.devices[0x21].code = (devp) port2;
	cpu->pio.devices[0x26].active = TRUE;
	cpu->pio.devices[0x26].aux = stdint;
	cpu->pio.devices[0x26].code = (devp) port3;
	cpu->pio.devices[0x27].active = TRUE;
	cpu->pio.devices[0x27].code = (devp) port7;
	
	cpu->pio.lcd		= lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= link;
	cpu->pio.stdint		= stdint;
	//a little hacky but it will work
	//STP: No it won't, it crashes savestates dumbass
	//BuckeyeDude: fixed savestates
	cpu->pio.se_aux		= (SE_AUX_t *) assist;
	
	cpu->pio.model		= TI_83P;
	
	Append_interrupt_device(cpu, 0x00, 1);
	Append_interrupt_device(cpu, 0x03, 8);
	Append_interrupt_device(cpu, 0x11, 128);
	return 0;
}


int memory_init_83p(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));

	mc->mem_read_break_callback = mem_debug_callback;
	mc->mem_write_break_callback = mem_debug_callback;
#ifdef WINVER
	mc->breakpoint_manager_callback = check_break_callback;
#endif

	// page protection for the 83p
	mc->protected_page_set = 0;
	
	// Set Number of Pages here
	mc->flash_pages = 32;
	mc->ram_pages = 2;

	mc->flash_version = 1;
	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	memset(mc->flash, 0xFF, mc->flash_size);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);

	if (!mc->flash || !mc->ram) {
		_tprintf_s(_T("Couldn't allocate memory in memory_init_83p\n"));
		return 1;
	}

	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;

	// Organize bank states here
	
	//	Address								page	write?	ram?	no exec?
	bank_state_t banks[5] = {
		{mc->flash, 						0, 		FALSE,	FALSE, 	FALSE},
		{mc->flash + 0x1f * PAGE_SIZE,		0x1f, 	FALSE, 	FALSE, 	FALSE},
		{mc->flash + 0x1f * PAGE_SIZE,		0x1f, 	FALSE, 	FALSE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	FALSE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};
	memcpy(mc->normal_banks, banks, sizeof(banks));
	update_bootmap_pages(mc);
	mc->banks					= mc->normal_banks;
	return 0;
}



