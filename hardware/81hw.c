#include "stdafx.h"

#include "81hw.h"
#include "lcd.h"
#include "keys.h"
#include "device.h"

static double timer_freq81[4] = { 1.0 / 800.0, 1.0 / 400.0, 3.0 / 800.0, 1.0 / 200.0 };

// 81 screen offset
static void port0(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0;
		cpu->input = FALSE;
	} else if (cpu->output) {
		LCD_t *lcd = (LCD_t *)cpu->pio.lcd;
		lcd->screen_addr = (0x100 * ((cpu->bus % 0x20) + 0xE0));
		dev->aux = lcd;
		port10(cpu, dev);
		cpu->output = FALSE;
		device_t devt;
		devt.aux = cpu->pio.lcd;
		cpu->pio.lcd->data(cpu, &devt);
	}
	return;
}

// Contrast v1.x
static void port2(CPU_t *cpu, device_t *dev) {
	LCD_t *lcd = (LCD_t *) dev->aux;
	if (cpu->input) {
		cpu->input = FALSE;
	} else if (cpu->output) {
		lcd->base.contrast = cpu->bus & 0x1F;
		if (lcd->base.contrast >= LCD_MAX_CONTRAST) {
			lcd->base.contrast = LCD_MAX_CONTRAST - 1;
		}

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
		
		stdint->intactive = cpu->bus & (BIT(2) | BIT(0));
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
		cpu->bus = 0x00;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
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

static void port7(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x00;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

static void port10(CPU_t *cpu, device_t *dev) {
	LCD_t *lcd = (LCD_t *)dev->aux;
	int screen_addr = lcd->screen_addr;
	// Output the entire LCD
	unsigned char *base_addr = cpu->mem_c->banks[mc_bank(screen_addr)].addr + mc_base(screen_addr);
	int k = 0, l = 0;
	for (int j = 0; j < LCD_HEIGHT; j++) {	
		for (int i = 0; i < 12; i++, k++, l++) {
			lcd->display[k] = *(base_addr + l);
		}
		k += 4;
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
	stdint->on_backup = 0;
	stdint->on_latch = FALSE;
	return stdint;
}

int memory_init_81(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));
	
	/* Set Number of Pages here */
	mc->flash_pages = 2;
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
		{mc->flash+0x1*PAGE_SIZE,			0x1, 	FALSE, 	FALSE, 	FALSE},
		{mc->flash+0x1*PAGE_SIZE,			0x1, 	FALSE, 	FALSE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	FALSE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->normal_banks, banks, sizeof(banks));
	mc->banks = mc->normal_banks;
	return 0;
}

int device_init_81(CPU_t *cpu) {
	ClearDevices(cpu);

	LCD_t *lcd = LCD_init(cpu, TI_81);
	cpu->pio.devices[0x00].active = TRUE;
	cpu->pio.devices[0x00].aux = NULL;
	cpu->pio.devices[0x00].code = (devp) &port0;

	keypad_t *keyp = keypad_init();
	cpu->pio.devices[0x01].active = TRUE;
	cpu->pio.devices[0x01].aux = keyp;
	cpu->pio.devices[0x01].code = (devp) &keypad;

	cpu->pio.devices[0x02].active = TRUE;
	cpu->pio.devices[0x02].code = (devp) &port2;
	cpu->pio.devices[0x02].aux = lcd;

	STDINT_t *stdint = INT81_init(cpu);
	cpu->pio.devices[0x03].active = TRUE;
	cpu->pio.devices[0x03].aux = stdint;
	cpu->pio.devices[0x03].code = (devp) &port3;

	cpu->pio.devices[0x04].active = TRUE;
	cpu->pio.devices[0x04].code = (devp) &port4;
	cpu->pio.devices[0x04].aux = lcd;

	cpu->pio.devices[0x05].active = TRUE;
	cpu->pio.devices[0x05].code = (devp) &port5;

	cpu->pio.devices[0x06].active = TRUE;
	cpu->pio.devices[0x06].code = (devp) &port6;

	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp) &port10;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp)lcd->base.data;
	
	cpu->pio.lcd		= (LCDBase_t *) lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= NULL;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= NULL;
	cpu->pio.model		= TI_81;
	
	//Append_interrupt_device(cpu, 0x00, 1);
	Append_interrupt_device(cpu, 0x03, 1);
	Append_interrupt_device(cpu, 0x10, 255);
	Append_interrupt_device(cpu, 0x11, 255);
	//Append_interrupt_device(cpu, 0x11, 128);
	return 0;
}