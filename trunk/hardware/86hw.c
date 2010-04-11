#include "lcd.h"
#include "keys.h"
#include "86hw.h"
#include "link.h"
#include "device.h"
#include <math.h>

static double timer_freq[4] = {1.0/200.0};

static int screen_addr = 0xFC00;
static void port10(CPU_t *, device_t *);

// 86 screen offset
static void port0(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0;
		cpu->input = FALSE;
	} else if (cpu->output) {
		screen_addr = 0x100 * ((cpu->bus % 0x40) + 0xC0);
		port10(cpu, dev);
		cpu->output = FALSE;
		LCD_data(cpu, dev);
	}
	return;
}

// Contrast
static void port2(CPU_t *cpu, device_t *dev) {
	return;
}

static void port3(CPU_t *cpu, device_t *dev) {
	STDINT_t * stdint = dev->aux;
	
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
		dev->aux = cpu->bus;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->bus = (uint8_t) dev->aux;
		cpu->output = FALSE;
	}
}

// ROM port
static void port5(CPU_t *cpu, device_t *dev) {
	if ( cpu->input ) {
		cpu->bus = ((cpu->mem_c->banks[1].ram)<<6)+cpu->mem_c->banks[1].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->banks[1].ram = (cpu->bus>>6)&1;
		if (cpu->mem_c->banks[1].ram) {
			cpu->mem_c->banks[1].page		= ((cpu->bus&0x1f) % cpu->mem_c->ram_pages );
			cpu->mem_c->banks[1].addr		= cpu->mem_c->ram+(cpu->mem_c->banks[1].page*16384);
			cpu->mem_c->banks[1].read_only	= FALSE;
			cpu->mem_c->banks[1].no_exec	= FALSE;
		} else {
			cpu->mem_c->banks[1].page		= ((cpu->bus&0x1f) % cpu->mem_c->flash_pages);
			cpu->mem_c->banks[1].addr		= cpu->mem_c->flash+(cpu->mem_c->banks[1].page*16384);
			cpu->mem_c->banks[1].read_only	= TRUE;
			cpu->mem_c->banks[1].no_exec	= FALSE;
			if (cpu->mem_c->banks[1].page==0x1f) cpu->mem_c->banks[1].read_only=TRUE;
		}
		cpu->output = FALSE;
	}
}

// RAM port
static void port6(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = ((cpu->mem_c->banks[2].ram)<<6)+cpu->mem_c->banks[2].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->banks[2].ram = (cpu->bus>>6)&1;
		if (cpu->mem_c->banks[2].ram) {
			cpu->mem_c->banks[2].page		= ((cpu->bus&0x1f) % cpu->mem_c->ram_pages);
			cpu->mem_c->banks[2].addr		= cpu->mem_c->ram+(cpu->mem_c->banks[2].page*16384);
			cpu->mem_c->banks[2].read_only	= FALSE;
			cpu->mem_c->banks[2].no_exec	= FALSE;
		} else {
			cpu->mem_c->banks[2].page		= ((cpu->bus&0x1f) % cpu->mem_c->flash_pages);
			cpu->mem_c->banks[2].addr		= cpu->mem_c->flash+(cpu->mem_c->banks[2].page*16384);
			cpu->mem_c->banks[2].read_only	= TRUE;
			cpu->mem_c->banks[2].no_exec	= FALSE;
			if (cpu->mem_c->banks[2].page==0x1f) cpu->mem_c->banks[2].read_only=TRUE;
		}
		cpu->output = FALSE;
	}
}

static void port7(CPU_t *cpu, device_t *dev) {
	link_t * link = dev->aux;
	if (cpu->input) {
		cpu->bus = (((link->host&0x03)|(link->client[0]&0x03))^0x03);
		cpu->input = FALSE;
	} else if (cpu->output) {
		link->host = (cpu->bus >> 4) & 0x03;
		cpu->output = FALSE;
	}
}


static void port10(CPU_t *cpu, device_t *dev) {
	// Output the entire LCD
	LCD_t *lcd = cpu->pio.lcd;
	memcpy(lcd->display, 
			cpu->mem_c->banks[mc_bank(screen_addr)].addr + mc_base(screen_addr), 
			DISPLAY_SIZE);

}

/*----------------------------------------------*/
/*												*/
/*				Intialize						*/
/*												*/
/*----------------------------------------------*/



static STDINT_t* INT86_init(CPU_t* cpu) {
	STDINT_t * stdint = malloc(sizeof(STDINT_t));
	if (!stdint) {
		printf("Couldn't allocate memory for standard interrupt\n");
		return NULL;
	}
	
	memcpy(stdint->freq, timer_freq, 4 * sizeof(stdint->freq[0]));
	stdint->intactive = 0;
	stdint->timermax1 = stdint->freq[0];
	stdint->lastchk1 = tc_elapsed(cpu->timer_c);
	stdint->on_backup = 0;
	stdint->on_latch = FALSE;
	return stdint;
}

static link_t* link_init(CPU_t* cpu) {
	link_t * link = malloc(sizeof(link_t));
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

	keypad_t *keyp = keypad_init(cpu);
	STDINT_t* stdint = INT86_init(cpu);
	link_t * link = link_init(cpu);
	LCD_t *lcd = LCD_init(cpu, TI_86);
	
	cpu->pio.devices[0x00] = (device_t) {TRUE, NULL, lcd, (devp) &port0};
	cpu->pio.devices[0x01] = (device_t) {TRUE, NULL, keyp, (devp) &keypad};
	cpu->pio.devices[0x02] = (device_t) {TRUE, NULL, NULL, (devp) &port2};
	cpu->pio.devices[0x03] = (device_t) {TRUE, NULL, stdint, (devp) &port3};
	cpu->pio.devices[0x04] = (device_t) {TRUE, NULL, 0, (devp) &port4};
	// ROM page swap
	cpu->pio.devices[0x05] = (device_t) {TRUE, NULL, stdint, (devp) &port5};
	// RAM page swap
	cpu->pio.devices[0x06] = (device_t) {TRUE, NULL, NULL, (devp) &port6};
	cpu->pio.devices[0x07] = (device_t) {TRUE, NULL, link, (devp) &port7};
	cpu->pio.devices[0x10] = (device_t) {TRUE, NULL, lcd, (devp) &port10};
	cpu->pio.devices[0x11] = (device_t) {TRUE, NULL, lcd, (devp) &LCD_data};
	
	cpu->pio.lcd		= lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= link;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= NULL;
	
	cpu->pio.model		= TI_86;
	
	// Interrupt
	Append_interrupt_device(cpu, 0x03, 1);
	// LCD
	Append_interrupt_device(cpu, 0x10, 16000);
	Append_interrupt_device(cpu, 0x11, 16000);
	
	return 0;
}


int memory_init_86(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));
	
	/* Set Number of Pages here */
	mc->flash_pages = 16;
	mc->ram_pages = 8;

	mc->flash_version = 1;
	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = calloc(mc->flash_pages, PAGE_SIZE);
	memset(mc->flash, 0xFF, mc->flash_size);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = calloc(mc->ram_pages, PAGE_SIZE);

	if (!mc->flash || !mc->ram) {
		printf("Couldn't allocate memory in memory_init_86\n");
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

	memcpy(mc->banks, banks, sizeof(banks));
	return 0;
}



