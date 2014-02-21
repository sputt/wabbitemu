#include "stdafx.h"

#include "86hw.h"
#include "lcd.h"
#include "keys.h"
#include "link.h"
#include "device.h"

static double timer_freq[4] = { 1.0 / 800.0, 1.0 / 400.0, 3.0 / 800.0, 1.0 / 200.0 };

//this would make it impossible to open multiple 86s...
//static int screen_addr = 0xFC00;
static void port10(CPU_t *, device_t *);

// 86 screen offset
static void port0(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0;
		cpu->input = FALSE;
	} else if (cpu->output) {
		LCD_t *lcd = (LCD_t *)cpu->pio.lcd;
		lcd->screen_addr = 0x100 * ((cpu->bus % 0x40) + 0xC0);
		port10(cpu, dev);
		cpu->output = FALSE;
		device_t lcd_dev;
		lcd_dev.aux = cpu->pio.lcd;
		cpu->pio.lcd->data(cpu, &lcd_dev);
	}
	return;
}

// Contrast
// ranges from 0 to 31
static void port2(CPU_t *cpu, device_t *dev) {
	LCD_t *lcd = (LCD_t *) dev->aux;
	if (cpu->input) {
		cpu->input = FALSE;
	} else if (cpu->output) {
		//lcd->contrast ranges from 24 - 64
		//HACK: still not sure exactly how this works :P
		lcd->base.contrast = lcd->base_level - 15 + cpu->bus;
		if (lcd->base.contrast > 64)
			lcd->base.contrast = 64;
		cpu->output = FALSE;
	}
	return;
}

static void port3(CPU_t *cpu, device_t *dev) {
	STDINT_t * stdint = (STDINT_t *) dev->aux;
	
	if (cpu->input) {
		unsigned char result = 0;
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) result += 4;
		if (cpu->pio.lcd->active) result += 2;
		if (stdint->on_latch) result += 1;
		else result += 8;
		
		cpu->bus = result;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if (cpu->bus & 0x08) {
			cpu->pio.lcd->active = TRUE;		//I'm worried about this
		} else {
			cpu->pio.lcd->active = FALSE;
		}
		
		if ((cpu->bus & 0x01) == 0)
			stdint->on_latch = FALSE;
		
		stdint->intactive = cpu->bus;
		cpu->output = FALSE;
	}
	
	if (!(stdint->intactive & 0x04) && cpu->pio.lcd->active == TRUE) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
			cpu->interrupt = TRUE;
			while ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1)
				stdint->lastchk1 += stdint->timermax1;
		}
	}

	if ((stdint->intactive & 0x01) && (cpu->pio.keypad->on_pressed & KEY_VALUE_MASK) && (stdint->on_backup & KEY_VALUE_MASK) == 0)  {
		stdint->on_latch = TRUE;
	}
	stdint->on_backup = cpu->pio.keypad->on_pressed;
	if (stdint->on_latch)
		cpu->interrupt = TRUE;
}




static void port4(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 1;
		cpu->input = FALSE;
	} else if (cpu->output) {
		dev->aux = (void *) (size_t) cpu->bus;
		int freq = (cpu->bus >> 1) & 0x3;
		cpu->pio.stdint->timermax1 = cpu->pio.stdint->freq[freq];
		cpu->pio.stdint->lastchk1 = tc_elapsed(cpu->timer_c);
		int lcd_mode = (cpu->bus >> 3) & 0x3;
		if (lcd_mode == 0) {
			cpu->pio.lcd->width = 80;
		} else {
			cpu->pio.lcd->width = 32 * lcd_mode + 64;
		}

		
		cpu->output = FALSE;
	}
}

// ROM port
static void port5(CPU_t *cpu, device_t *dev) {
	if ( cpu->input ) {
		cpu->bus = (cpu->mem_c->banks[1].ram << 6) + cpu->mem_c->banks[1].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->banks[1].ram = (cpu->bus >> 6) & 1;
		if (cpu->mem_c->banks[1].ram) {
			cpu->mem_c->banks[1].page		= (cpu->bus & 0x1f) % cpu->mem_c->ram_pages;
			cpu->mem_c->banks[1].addr		= cpu->mem_c->ram+(cpu->mem_c->banks[1].page * PAGE_SIZE);
			cpu->mem_c->banks[1].read_only	= FALSE;
			cpu->mem_c->banks[1].no_exec	= FALSE;
		} else {
			cpu->mem_c->banks[1].page		= (cpu->bus & 0x1f) % cpu->mem_c->flash_pages;
			cpu->mem_c->banks[1].addr		= cpu->mem_c->flash+(cpu->mem_c->banks[1].page * PAGE_SIZE);
			cpu->mem_c->banks[1].read_only	= TRUE;
			cpu->mem_c->banks[1].no_exec	= FALSE;
			if (cpu->mem_c->banks[1].page == 0x1f) cpu->mem_c->banks[1].read_only = TRUE;
		}
		cpu->output = FALSE;
	}
}

// RAM port
static void port6(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (cpu->mem_c->banks[2].ram << 6) + cpu->mem_c->banks[2].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->banks[2].ram = (cpu->bus >> 6) & 1;
		if (cpu->mem_c->banks[2].ram) {
			cpu->mem_c->banks[2].page		= cpu->bus & 0x07;
			cpu->mem_c->banks[2].addr		= cpu->mem_c->ram + (cpu->mem_c->banks[2].page * PAGE_SIZE);
			cpu->mem_c->banks[2].read_only	= FALSE;
			cpu->mem_c->banks[2].no_exec	= FALSE;
		} else {
			cpu->mem_c->banks[2].page		= cpu->bus & 0x0f;
			cpu->mem_c->banks[2].addr		= cpu->mem_c->flash + (cpu->mem_c->banks[2].page * PAGE_SIZE);
			cpu->mem_c->banks[2].read_only	= TRUE;
			cpu->mem_c->banks[2].no_exec	= FALSE;
			if (cpu->mem_c->banks[2].page == 0x1f) {
				cpu->mem_c->banks[2].read_only = TRUE;
			}
		}
		cpu->output = FALSE;
	}
}

static void port7(CPU_t *cpu, device_t *dev) {
	link_t * link = (link_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = (((link->host & 0x03) | (link->client[0] & 0x03)) ^ 0x03);
		cpu->input = FALSE;
	} else if (cpu->output) {
		link->host = (cpu->bus >> 4) & 0x03;
		cpu->output = FALSE;
	}
}


static void port10(CPU_t *cpu, device_t *dev) {
	LCD_t *lcd = (LCD_t *)dev->aux;
	int screen_addr = lcd->screen_addr;
	// Output the entire LCD
	memcpy(lcd->display, cpu->mem_c->banks[mc_bank(screen_addr)].addr + mc_base(screen_addr), DISPLAY_SIZE);

}

/*----------------------------------------------*/
/*												*/
/*				Intialize						*/
/*												*/
/*----------------------------------------------*/



static STDINT_t* INT86_init(CPU_t* cpu) {
	STDINT_t * stdint = (STDINT_t *) malloc(sizeof(STDINT_t));
	if (!stdint) {
		printf("Couldn't allocate memory for standard interrupt\n");
		return NULL;
	}
	
	memcpy(stdint->freq, timer_freq, sizeof(timer_freq));
	stdint->intactive = 0;
	stdint->timermax1 = stdint->freq[3];
	stdint->lastchk1 = tc_elapsed(cpu->timer_c);
	stdint->on_backup = 0;
	stdint->on_latch = FALSE;
	return stdint;
}

static link_t* link86_init() {
	link_t * link = (link_t *) malloc(sizeof(link_t));
	if (!link) {
		printf("Couldn't allocate memory for link\n");
		exit(1);
	}
	link->host		= 0;			//neither lines set
	link->client	= &link->host;	//nothing plugged in.

	return link;
}


int device_init_86(CPU_t *cpu) {
	ClearDevices(cpu);

	keypad_t *keyp = keypad_init();
	STDINT_t* stdint = INT86_init(cpu);
	link_t * link = link86_init();
	LCD_t *lcd = LCD_init(cpu, TI_86);
	
	cpu->pio.devices[0x00].active = TRUE;
	cpu->pio.devices[0x00].aux = lcd;
	cpu->pio.devices[0x00].code = (devp) &port0;

	cpu->pio.devices[0x01].active = TRUE;
	cpu->pio.devices[0x01].aux = keyp;
	cpu->pio.devices[0x01].code = (devp) &keypad;

	cpu->pio.devices[0x02].active = TRUE;
	cpu->pio.devices[0x02].aux = lcd;
	cpu->pio.devices[0x02].code = (devp) &port2;

	cpu->pio.devices[0x03].active = TRUE;
	cpu->pio.devices[0x03].aux = stdint;
	cpu->pio.devices[0x03].code = (devp) &port3;

	cpu->pio.devices[0x04].active = TRUE;
	cpu->pio.devices[0x04].aux = 0;
	cpu->pio.devices[0x04].code = (devp) &port4;

	// ROM page swap
	cpu->pio.devices[0x05].active = TRUE;
	cpu->pio.devices[0x05].aux = stdint;
	cpu->pio.devices[0x05].code = (devp) &port5;

	// RAM page swap
	cpu->pio.devices[0x06].active = TRUE;
	cpu->pio.devices[0x06].aux = NULL;
	cpu->pio.devices[0x06].code = (devp) &port6;

	cpu->pio.devices[0x07].active = TRUE;
	cpu->pio.devices[0x07].aux = link;
	cpu->pio.devices[0x07].code = (devp) &port7;

	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp) &port10;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp) lcd->base.data;

	cpu->pio.lcd		= (LCDBase_t *)lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= link;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= NULL;
	
	cpu->pio.model		= TI_86;
	
	// Interrupt
	Append_interrupt_device(cpu, 0x03, 1);
	// LCD
	Append_interrupt_device(cpu, 0x10, 255);
	Append_interrupt_device(cpu, 0x11, 255);
	
	return 0;
}


int memory_init_86(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));

	/* Set Number of Pages here */
	mc->flash_pages = 16;
	mc->ram_pages = 8;

	mc->flash_version = 1;
	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	memset(mc->flash, 0xFF, mc->flash_size);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);

	if (!mc->flash || !mc->ram) {
		_tprintf_s(_T("Couldn't allocate memory in memory_init_86\n"));
		return 1;
	}

	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;

	/* Organize bank states here */
	
	/*	Address								page	write?	ram?	no exec?	*/
	bank_state_t banks[5] = {
		{mc->flash, 						0, 		FALSE,	FALSE, 	FALSE},
		{mc->flash+0x0F*PAGE_SIZE,			0x0F, 	FALSE, 	FALSE, 	FALSE},
		{mc->flash,							0, 		FALSE, 	FALSE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	FALSE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->normal_banks, banks, sizeof(banks));
	mc->banks = mc->normal_banks;
	return 0;
}



