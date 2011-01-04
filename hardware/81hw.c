#include "stdafx.h"

#include "81hw.h"
#include "lcd.h"
#include "keys.h"
#include "device.h"
#include "calc.h"

static double timer_freq81[4] = { 1.0f / 560.0f, 1.0f / 248.0f, 1.0f / 170.0f, 1.0f / 118.0f };

static void port2(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x00;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

static void port3(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x08;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

static void port5(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x00;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

static void port6(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x00;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

static STDINT_t* INT81_init(CPU_t* cpu) {
	STDINT_t * stdint = (STDINT_t *) malloc(sizeof(STDINT_t));
	if (!stdint) {
		printf("Couldn't allocate memory for standard interrupt\n");
		return NULL;
	}
	
	memcpy(stdint->freq, timer_freq81, 4 * sizeof(stdint->freq[0]));
	
	stdint->intactive = 0;
	stdint->timermax1 = stdint->freq[3];
	stdint->lastchk1 = tc_elapsed(cpu->timer_c);
	stdint->timermax2 = stdint->freq[3] / 2.0f;
	stdint->lastchk2 = tc_elapsed(cpu->timer_c)+stdint->freq[3] / 4.0f;
	stdint->on_backup = 0;
	stdint->on_latch = FALSE;
	return stdint;
}

int memory_init_81(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));

	mc->mem_read_break_callback = mem_debug_callback;
	mc->mem_write_break_callback = mem_debug_callback;
	
	/* Set Number of Pages here */
	mc->flash_pages = 32;
	mc->ram_pages = 2;

	mc->flash_version = 1;
	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	memset(mc->flash, 0xFF, mc->flash_size);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (unsigned char *)calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);

	if (!mc->flash || !mc->ram) {
		_tprintf_s(_T("Couldn't allocate memory in memory_init_83p\n"));
		return 1;
	}

	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;

	/* Organize bank states here */
	
	/*	Address								page	write?	ram?	no exec?	*/
	bank_state_t banks[5] = {
		{mc->flash, 						0, 		FALSE,	FALSE, 	FALSE},
		{mc->flash+0x1f*PAGE_SIZE,			0x1f, 	FALSE, 	FALSE, 	FALSE},
		{mc->flash+0x1f*PAGE_SIZE,			0x1f, 	FALSE, 	FALSE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	FALSE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->banks, banks, sizeof(banks));
	return 0;
}

int device_init_81(CPU_t *cpu) {
	ClearDevices(cpu);

	keypad_t *keyp = keypad_init(cpu);
	cpu->pio.devices[0x01].active = TRUE;
	cpu->pio.devices[0x01].aux = keyp;
	cpu->pio.devices[0x01].code = (devp) keypad;

	cpu->pio.devices[0x02].active = TRUE;
	cpu->pio.devices[0x02].code = (devp) port2;

	cpu->pio.devices[0x03].active = TRUE;
	cpu->pio.devices[0x03].code = (devp) port3;

	cpu->pio.devices[0x06].active = TRUE;
	cpu->pio.devices[0x06].code = (devp) port6;


	LCD_t *lcd = LCD_init(cpu, TI_81);
	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp) LCD_command;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp) LCD_data;
	
	STDINT_t *stdint = INT81_init(cpu);
	cpu->pio.lcd		= lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= NULL;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= NULL;
	cpu->pio.model		= TI_81;
	
	/*Append_interrupt_device(cpu, 0x00, 1);
	Append_interrupt_device(cpu, 0x03, 8);
	Append_interrupt_device(cpu, 0x11, 128);*/
	return 0;
}