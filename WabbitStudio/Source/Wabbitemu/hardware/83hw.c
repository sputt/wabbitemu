#include "stdafx.h"

#include "83hw.h"
#include "lcd.h"
#include "keys.h"
#include "link.h"
#include "device.h"

static double timer_freq83[4] = {1.0f / 600.0f, 1.0f / 257.14f, 1.0f / 163.63f, 1.0f / 120.0f};

#define SWAP_BANK	0xFF
#define ROM0_8		0xFE
#define ROM1_8		0xFD
#define ROM1_9		0xFC
#define ROM0		0x00
#define RAM0		0x08
#define RAM1		0x09

unsigned char banks83[16][4] = {
	{ ROM0,		SWAP_BANK,	ROM0_8,		RAM0	},
	{ ROM0,		SWAP_BANK,	ROM1_8,		RAM0	},
	{ ROM0,		SWAP_BANK,	RAM0,		RAM0	},
	{ ROM0,		SWAP_BANK,	RAM1,		RAM0	},

	{ ROM0,		SWAP_BANK,	ROM0_8,		RAM0	},
	{ ROM0,		SWAP_BANK,	ROM1_8,		RAM0	},
	{ ROM0,		SWAP_BANK,	RAM0,		RAM0	},
	{ ROM0,		SWAP_BANK,	RAM1,		RAM0	},

	{ ROM0,		ROM0_8,		SWAP_BANK,	ROM0_8	},
	{ ROM0,		ROM0_8,		SWAP_BANK,	ROM1_9	},
	{ ROM0,		ROM0_8,		SWAP_BANK,	RAM0	},
	{ ROM0,		ROM0_8,		SWAP_BANK,	RAM1	},

	{ ROM0,		RAM0,		RAM1,		ROM0_8	},
	{ ROM0,		RAM0,		RAM1,		ROM1_9	},
	{ ROM0,		RAM0,		RAM1,		RAM0	},
	{ ROM0,		RAM0,		RAM1,		RAM1	}};


void setpage83(CPU_t *cpu) {
	STDINT_t *stdint = cpu->pio.stdint;
	int xy		= (stdint->xy & 0x10) >> 4;
	int ram		= (stdint->mem & 0x40) >> 6;
	int page	= (stdint->mem & 0x07) + ((stdint->xy & 0x10) >> 0x01) ;
	int rpage	= page % cpu->mem_c->ram_pages;
	int fpage	= page % cpu->mem_c->flash_pages;
	int mem		= ((stdint->mem & 0x08) >> 3) + ((stdint->mem & 0x80) >> 6) + (ram << 2) + ((cpu->mem_c->boot_mapped == TRUE) ? 8 : 0);
	int i;
	
	for(i = 0; i < 4; i++) {
		switch(banks83[mem][i]) {
			case ROM0: {
				cpu->mem_c->banks[i].addr			= cpu->mem_c->flash;
				cpu->mem_c->banks[i].page			= 0x00;
				cpu->mem_c->banks[i].read_only		= FALSE;
				cpu->mem_c->banks[i].ram			= FALSE;
				cpu->mem_c->banks[i].no_exec		= FALSE;
				break;
			}
			case RAM0: {
				cpu->mem_c->banks[i].addr			= cpu->mem_c->ram;
				cpu->mem_c->banks[i].page			= 0x00;
				cpu->mem_c->banks[i].read_only		= FALSE;
				cpu->mem_c->banks[i].ram			= TRUE;
				cpu->mem_c->banks[i].no_exec		= FALSE;
				break;
			}
			case RAM1: {
				cpu->mem_c->banks[i].addr			= cpu->mem_c->ram + PAGE_SIZE;
				cpu->mem_c->banks[i].page			= 0x01;
				cpu->mem_c->banks[i].read_only		= FALSE;
				cpu->mem_c->banks[i].ram			= TRUE;
				cpu->mem_c->banks[i].no_exec		= FALSE;
				break;
			}
			case ROM0_8: {
				if (xy) {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->flash + 0x08 * PAGE_SIZE;
					cpu->mem_c->banks[i].page			= 0x08;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= FALSE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				} else {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->flash;
					cpu->mem_c->banks[i].page			= 0x00;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= FALSE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				}
				break;
			}
			case ROM1_8: {
				if (xy) {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->flash + 0x08 * PAGE_SIZE;
					cpu->mem_c->banks[i].page			= 0x08;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= FALSE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				} else {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->flash + PAGE_SIZE;
					cpu->mem_c->banks[i].page			= 0x01;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= FALSE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				}
				break;
			}
			case ROM1_9: {
				if (xy) {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->flash + 0x09 * PAGE_SIZE;
					cpu->mem_c->banks[i].page			= 0x09;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= FALSE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				} else {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->flash + PAGE_SIZE;
					cpu->mem_c->banks[i].page			= 0x01;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= FALSE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				}
				break;
			}
			case SWAP_BANK: {
				if (ram) {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->ram + rpage * PAGE_SIZE;
					cpu->mem_c->banks[i].page			= rpage;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= TRUE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				} else {
					cpu->mem_c->banks[i].addr			= cpu->mem_c->flash + fpage * PAGE_SIZE;
					cpu->mem_c->banks[i].page			= fpage;
					cpu->mem_c->banks[i].read_only		= FALSE;
					cpu->mem_c->banks[i].ram			= FALSE;
					cpu->mem_c->banks[i].no_exec		= FALSE;
				}
				break;
			}
		}
	}
}
	

void port00_82(CPU_t *cpu, device_t *dev) {
	link_t* link = (link_t *) dev->aux;
	//STDINT_t* stdint = (STDINT_t *) cpu->pio.devices[0x02].aux;
	
	if (cpu->input) {
		cpu->bus = ((link->host & 0x03)) << 2;
		cpu->bus += (((link->host & 0x03) | (link->client[0] & 0x03)) ^ 0x03);
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ((link->host & 0x01) != ((cpu->bus & 0x04) >> 2)) {
			FlippedLeft(cpu, (cpu->bus & 0x04) >> 2);
		}
		if ((link->host&0x02) != ((cpu->bus & 0x08) >> 2)) {
			FlippedRight(cpu, (cpu->bus & 0x08) >> 3);
		}
		link->host = (cpu->bus & 0x0C) >> 2;
//		setpage83(cpu);
		cpu->output = FALSE;
	}

	if (link->audio.init && link->audio.enabled) {
		nextsample(cpu);
	}
}

void port00_83(CPU_t *cpu, device_t *dev) {
	link_t* link = (link_t *) dev->aux;
	STDINT_t* stdint = (STDINT_t *) cpu->pio.devices[0x02].aux;
	
	if (cpu->input) {
		cpu->bus = (link->host & 0x03) ^ 0x03;
		cpu->bus += (((link->host & 0x03) | (link->client[0] & 0x03)) ^ 0x03) << 2;
		cpu->bus += stdint->xy;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ((link->host & 0x01) != (cpu->bus & 0x01)) {
			FlippedLeft(cpu, cpu->bus & 0x01);
		}

		if ((link->host & 0x02) != (cpu->bus & 0x02)) {
			FlippedRight(cpu, (cpu->bus & 0x02) >> 1);
		}

		link->host = cpu->bus & 0x03;
		stdint->xy = cpu->bus & 0x10;
		setpage83(cpu);
		cpu->output = FALSE;
	}

	nextsample(cpu);
}

void port02_83(CPU_t *cpu, device_t *dev) {
	STDINT_t* mem83 = (STDINT_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = mem83->mem;
		cpu->input = FALSE;
	} else if (cpu->output) {
		mem83->mem = cpu->bus;
		cpu->output = FALSE;
		setpage83(cpu);
	}
}

void port03_83(CPU_t *cpu, device_t *dev) {
	STDINT_t * stdint = (STDINT_t *) dev->aux;
	
	if (cpu->input) {
		unsigned char result = 0;
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
			result += 2;
		}
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) {
			result += 4;
		}
		if (cpu->pio.keypad->on_pressed) {
			result += 1;
		} else {
			result += 8;
		}
		cpu->bus = result;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ( (!(stdint->intactive & 0x08)) && (cpu->bus & 0x08) ) {
			cpu->pio.lcd->active = TRUE;		//I'm worried about this
			/*
			Normally when the calc is set to a low power state and hits a halt
			the LCD ram disconnects from the LCD.  It does not physically power
			off the LCD.  However for the sake of ease, its powering off here.
			should normally be fine, since no one should rely on a powered on
			disconnected lcd.
			*/
		}
		if ((cpu->bus & 0x01) == 0) {
			stdint->on_latch = FALSE;
		}
		
		stdint->intactive = cpu->bus;
		cpu->output = FALSE;
	}
	
	/*Read note above*/
	if (!(stdint->intactive & 0x08)  && (cpu->halt==TRUE)) {
		cpu->pio.lcd->active = FALSE;
	}

/*
	Standard interrupt
	ocurrs on a base frequency
	when mask timer continues to tick but 
	does not generate an interrupt. */
	if (stdint->intactive & 0x02) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1)
			cpu->interrupt = TRUE;
	} else {
		while ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1)
			stdint->lastchk1 += stdint->timermax1;
	}

/*
	Double speed standard interrupt
	ocurrs (1/(4*frequency)) second after standard timer.
	when mask timer continues to tick but 
	does not generate an interrupt. */
	if (stdint->intactive & 0x04) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2)
			cpu->interrupt = TRUE;
	} else {
		while ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2)
			stdint->lastchk2 += stdint->timermax2;
	}
	
	if ((stdint->intactive & 0x01) && (cpu->pio.keypad->on_pressed & KEY_VALUE_MASK) && (stdint->on_backup & KEY_VALUE_MASK) == 0)  {
		stdint->on_latch = TRUE;
	}
	stdint->on_backup = cpu->pio.keypad->on_pressed;
	if (stdint->on_latch)
		cpu->interrupt = TRUE;
}

void port04_83(CPU_t *cpu, device_t *dev) {
	link_t * link = (link_t *) cpu->pio.devices[0x00].aux;
	STDINT_t * stdint = (STDINT_t *) dev->aux;
	
	if (cpu->input) {
		cpu->bus = ((link->host&0x03)|(link->client[0]&0x03))^0x03;
		cpu->bus += ((link->host&0x03)<<2);
		cpu->bus += stdint->xy;
		cpu->input = FALSE;
	} else if (cpu->output) {
		/* I'm not sure if this is how the interrupts work. */
		/* but for practicallity its close enough for now. */
		int freq = ((cpu->bus & 6) >> 1);
		stdint->timermax1 = stdint->freq[freq];
		stdint->timermax2 = stdint->freq[freq] / 2.0f;
		stdint->lastchk2  = stdint->lastchk1 + (stdint->freq[freq] / 4.0f);

		if ((cpu->bus & 1) == 1) {
			cpu->mem_c->boot_mapped = TRUE;
		} else {
			cpu->mem_c->boot_mapped = FALSE;
		}
		setpage83(cpu);
		cpu->output = FALSE;
	}	
}


void port14_83(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 1;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

/*----------------------------------------------*/
/*												*/
/*				Intialize						*/
/*												*/
/*----------------------------------------------*/


STDINT_t* INT83_init(timer_context_t *timer_c) {
	STDINT_t * stdint = (STDINT_t *) malloc(sizeof(STDINT_t));
	if (!stdint) {
		printf("Couldn't allocate memory for standard interrupt\n");
		exit(1);
	}
	
	stdint->freq[0] = timer_freq83[0];
	stdint->freq[1] = timer_freq83[1];
	stdint->freq[2] = timer_freq83[2];
	stdint->freq[3] = timer_freq83[3];
	
	stdint->intactive = 0;
	stdint->timermax1 = stdint->freq[3];
	stdint->lastchk1 = tc_elapsed(timer_c);
	stdint->timermax2 = stdint->freq[3]/2.0f;
	stdint->lastchk2 = tc_elapsed(timer_c)+stdint->freq[3]/4.0f;
	
	
	stdint->mem	=0;
	stdint->xy	=0;
	
	
/*  MAY NEED TO INTIALIZE xy and mem */	
	
	return stdint;
}

link_t* link83_init() {
	link_t * link = (link_t *) malloc(sizeof(link_t));
	if (!link) {
		printf("Couldn't allocate memory for link\n");
		exit(1);
	}
	link->host		= 0;			//neither lines set
	link->client	= &link->host;	//nothing plugged in.
	return link;
}


int device_init_83(CPU_t *cpu, BOOL bad82) {
	ClearDevices(cpu);

	link_t * link = link83_init();
	cpu->pio.devices[0x00].active = TRUE;
	cpu->pio.devices[0x00].aux = link;
	if (bad82 == 1) {
		puts("82 port");
		cpu->pio.devices[0x00].code = (devp) port00_82;
	} else {
		puts("83 port");
		cpu->pio.devices[0x00].code = (devp) port00_83;
	}
	keypad_t *keyp = keypad_init();
	cpu->pio.devices[0x01].active = TRUE;
	cpu->pio.devices[0x01].aux = keyp;
	cpu->pio.devices[0x01].code = (devp) keypad;
	
	STDINT_t* stdint = INT83_init(cpu->timer_c);
	cpu->pio.devices[0x02].active = TRUE;
	cpu->pio.devices[0x02].aux = stdint;
	cpu->pio.devices[0x02].code = (devp) port02_83;
	
	cpu->pio.devices[0x03].active = TRUE;
	cpu->pio.devices[0x03].aux = stdint;
	cpu->pio.devices[0x03].code = (devp) port03_83;

	cpu->pio.devices[0x04].active = TRUE;
	cpu->pio.devices[0x04].aux = stdint;
	cpu->pio.devices[0x04].code = (devp) port04_83;

	LCD_t *lcd = LCD_init(cpu, TI_83);
	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp) lcd->base.command;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp) lcd->base.data;

	cpu->pio.devices[0x14].active = TRUE;
	cpu->pio.devices[0x14].code = (devp) port14_83;

	cpu->pio.lcd		= (LCDBase_t *) lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= link;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= NULL;
	cpu->pio.model		= TI_83;

	Append_interrupt_device(cpu, 0x00, 1);
	Append_interrupt_device(cpu, 0x03, 8);
	Append_interrupt_device(cpu, 0x11, 138);
	return 0;
}


int memory_init_83(memc *mc) {
	memset(mc, 0, sizeof(memc));
	
	/* Set Number of Pages here */
	mc->flash_pages = 16;
	mc->ram_pages = 2;


	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	memset(mc->flash, 0xFF, mc->flash_size);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);

	if (!mc->flash || !mc->ram) {
		_tprintf_s(_T("Couldn't allocate memory in memory_init_83\n"));
		return 1;
	}
	mc->flash_version = 0;
	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;

	/* Organize bank states here */
	bank_state_t banks[5] = {
		{mc->flash, 						0, 		FALSE,	FALSE, 	FALSE},
		{mc->flash+0x00*PAGE_SIZE, 			0x00, 	FALSE, 	FALSE, 	FALSE},
		{mc->ram+0x01*PAGE_SIZE,		 	0x01, 	FALSE, 	TRUE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	FALSE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->normal_banks, banks, sizeof(banks));
	mc->banks = mc->normal_banks;
	return 0;
}



