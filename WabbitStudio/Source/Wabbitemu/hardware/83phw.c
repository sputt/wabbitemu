#include "stdafx.h"

#include "lcd.h"
#include "keys.h"
#include "83phw.h"
#include "link.h"
#include "device.h"
#include <math.h>

static double timer_freq83p[4] = {1.0f/560.0f, 1.0f/248.0f, 1.0f/170.0f, 1.0f/118.0f};

#define TI_83P		6

static void port0(CPU_t *cpu, device_t *dev) {
	link_t * link = (link_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = ((link->host&0x03)|(link->client[0]&0x03))^0x03;
		cpu->input = FALSE;
	} else if (cpu->output) {
#ifdef WINVER // lazy me
		if ((link->host&0x01) != (cpu->bus&0x01)) {
			FlippedLeft(cpu,(cpu->bus&0x01));		//sound .. #$%# you
		}
		if ((link->host&0x02) != (cpu->bus&0x02)) {
			FlippedRight(cpu,((cpu->bus&0x02)>>1));	//sound, not portable
		}
#endif		

		link->host = cpu->bus&0x03;
		cpu->output = FALSE;
	}
#ifdef WINVER // :P
	if (link->audio.init && link->audio.enabled) nextsample(cpu);
#endif
}

static void port2(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x3B | ((cpu->mem_c->flash_locked)?4:0);
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
	ocurrs on a base frequency
	when mask timer continues to tick but 
	does not generate an interrupt. */
	if (stdint->intactive & 0x02) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1)
			cpu->interrupt = TRUE;
	} else if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
//		stdint->lastchk1 = ceil((tc_elapsed(cpu->timer_c) - stdint->lastchk1)/stdint->timermax1)*stdint->timermax1;
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
	} else if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) {
//		stdint->lastchk2 = ceil((tc_elapsed(cpu->timer_c) - stdint->lastchk2)/stdint->timermax2)*stdint->timermax2;
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
		/* but for practicallity its close enough for now. */
		int freq = ((cpu->bus & 6) >> 1);
		stdint->timermax1 = stdint->freq[freq];
		stdint->timermax2 = ( stdint->freq[freq] / 2.0f );
		stdint->lastchk2  = stdint->lastchk1 + ( stdint->freq[freq] / 4.0f );

		if ( ( cpu->bus & 1 ) == 1) {
			if ( !cpu->mem_c->boot_mapped ) {
				memmove(&cpu->mem_c->banks[2],&cpu->mem_c->banks[1],sizeof(bank_state_t)*3);
				memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[4],sizeof(bank_state_t)*1);
			}
			cpu->mem_c->boot_mapped = TRUE;
		}else {
			if ( cpu->mem_c->boot_mapped ) {
				memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[2],sizeof(bank_state_t)*3);
			}
			cpu->mem_c->boot_mapped = FALSE;
		}
		cpu->output = FALSE;
	}	
}

static void port6(CPU_t *cpu, device_t *dev) {
	if ( cpu->mem_c->boot_mapped ) {
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[2],sizeof(bank_state_t)*3);
	}
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
			cpu->mem_c->banks[1].read_only	= cpu->mem_c->banks[1].page == 0x1f;
			cpu->mem_c->banks[1].no_exec	= FALSE;
		}
		cpu->output = FALSE;
	}
	if (cpu->mem_c->boot_mapped) {
		memmove(&cpu->mem_c->banks[2],&cpu->mem_c->banks[1],sizeof(bank_state_t)*3);
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[4],sizeof(bank_state_t)*1);
	}
}

static void port7(CPU_t *cpu, device_t *dev) {
	if (cpu->mem_c->boot_mapped) {
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[2],sizeof(bank_state_t)*3);
	}
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
			cpu->mem_c->banks[2].read_only	= FALSE;
			cpu->mem_c->banks[2].no_exec	= FALSE;
			if (cpu->mem_c->banks[2].page == 0x1f) cpu->mem_c->banks[2].read_only = TRUE;
		}
		cpu->output = FALSE;
	}
	if (cpu->mem_c->boot_mapped) {
		memmove(&cpu->mem_c->banks[2],&cpu->mem_c->banks[1],sizeof(bank_state_t)*3);
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[4],sizeof(bank_state_t)*1);
	}
}


static void port14(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = cpu->mem_c->flash_locked;
		cpu->input = FALSE;
	} else if (cpu->output) {
		int bank = cpu->pc>>14;
		if (!cpu->mem_c->banks[bank].ram) {
			if ( cpu->mem_c->banks[bank].page > 0x1B ) {		
				cpu->mem_c->flash_locked = ( cpu->bus & 0x01 );
			}
		}
		cpu->output = FALSE;
	}
}


void flashwrite83p(CPU_t *cpu, unsigned short addr, unsigned char data) {
	int bank = addr>>14;
	switch( cpu->mem_c->step ) {
		case 0:
			if ( ( addr & 0x0FFF ) == 0x0AAA ) {
				if ( data == 0xAA ) cpu->mem_c->step++;
			}
			break;
		case 1:
			if ( ( addr & 0x0FFF ) == 0x0555 ) {
				if (data==0x55) cpu->mem_c->step++;
				else endflash(cpu);
			} else endflash(cpu);
			break;
		case 2:
			if ( ( addr & 0x0FFF ) == 0x0AAA ) {
				if ( data == 0xA0 ) {
					cpu->mem_c->cmd=0xA0;		//Program
					cpu->mem_c->step++;
				} else if ( data == 0x80 ) {
					cpu->mem_c->cmd=0x80;		//Erase
					cpu->mem_c->step++;
				} else endflash(cpu);
			} else endflash(cpu);
			break;
		case 3:
			if ( cpu->mem_c->cmd == 0xA0 && cpu->mem_c->step == 3) {
				(*(cpu->mem_c->banks[bank].addr +(addr&0x3fff))) &= data;  //AND LOGIC!!
//				if (cpu->mem_c->banks[bank].page == 0x1E) printf("\n");
//				if (cpu->mem_c->banks[bank].page == 0x1E || cpu->mem_c->banks[bank].page == 0x08 ) {
//					printf("Address: %02X:%04X  <- %02X  \n",cpu->mem_c->banks[bank].page ,addr&0x3fff,data);
//				}
//				if (cpu->mem_c->banks[bank].page == 0x1E) printf("\n");
				endflash(cpu);
			}
			if ( ( addr & 0x0FFF ) == 0x0AAA ) {
				if (data==0xAA) cpu->mem_c->step++;
			}
			if (data == 0xF0) endflash(cpu);
			break;
		case 4:
			if ( ( addr & 0x0FFF ) == 0x0555 ) {
				if ( data == 0x55 ) cpu->mem_c->step++;
			}
			if (data == 0xF0) endflash(cpu);
			break;
		case 5:
			if ( ( addr & 0x0FFF ) == 0x0AAA ) {
				if (data==0x10) {			//Erase entire chip...Im not sure if 
					int i;					//boot page is included, so I'll leave it off
					for( i = 0; i < ( cpu->mem_c->flash_size - 16384 ) ; i++ ) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				} 
			}
			if (data == 0xF0) endflash(cpu);
			if (data == 0x30) {		//erase sectors
				int i;
				int spage = (cpu->mem_c->banks[bank].page<<1) + ((addr>>13)&0x01);
				if (spage<56) {
					int startaddr = ( ( spage & 0x00F8 ) * 0x2000 );
					int endaddr   = ( startaddr + 0x10000 );
					for(i=startaddr; i<endaddr ;i++) {
						cpu->mem_c->flash[i]=0xFF;
					}
				} else if (spage<60) {
					for( i=0x70000; i < 0x78000; i++ ) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				} else if (spage<61) {
//					printf("\nAddress: 1E:0000 -- ERASED\n");
					for( i=0x78000; i < 0x7A000; i++ ) {
						cpu->mem_c->flash[i] = 0xFF;

					}
				} else if (spage<62) {
//											printf("\nAddress: 1E:2000 -- ERASED\n");
					for( i=0x7A000; i<0x7C000; i++ ) {
						cpu->mem_c->flash[i]=0xFF;
					}
				} else if (spage<64) {
/*
// I comment this off because this is the boot page
// it suppose to be write protected...
					for(i=0x7C000;i<0x80000;i++) {
						cpu->mem_c->flash[i]=0xFF;
					}
*/
				}
			}
			endflash(cpu);
			break;
		default:
			endflash(cpu);
			break;
	}
}



/*----------------------------------------------*/
/*												*/
/*				Intialize						*/
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
	stdint->timermax2 = stdint->freq[3]/2.0f;
	stdint->lastchk2 = tc_elapsed(cpu->timer_c)+stdint->freq[3]/4.0f;
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


	link_t * link = link83p_init(cpu);
	cpu->pio.devices[0x00].active = TRUE;
	cpu->pio.devices[0x00].aux = link;
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

	cpu->pio.devices[0x06].active = TRUE;
	cpu->pio.devices[0x06].code = (devp) port6;

	cpu->pio.devices[0x07].active = TRUE;
	cpu->pio.devices[0x07].code = (devp) port7;

	LCD_t *lcd = LCD_init(cpu,TI_83P);
	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp) LCD_command;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp) LCD_data;

	cpu->pio.devices[0x14].active = TRUE;
	cpu->pio.devices[0x14].code = (devp) port14;
	
	cpu->pio.lcd		= lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= link;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= NULL;
	
	cpu->pio.model		= TI_83P;
	
	Append_interrupt_device(cpu,0x00,1);
	Append_interrupt_device(cpu,0x03,8);
	Append_interrupt_device(cpu,0x11,128);
	return 0;
}


int memory_init_83p(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));
	
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



