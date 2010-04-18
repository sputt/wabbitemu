#include "lcd.h"
#include "keys.h"
#include "83psehw.h"
#include "link.h"
#include "device.h"
#include "calc.h"
#include <math.h>

/*
	NOTE ABOUT 83+SE AND 84+SE:
		There is no difference in hardware code between the 83+SE
	and the 84+SE.  The reason is that there should be zero code interaction
	so makeing seperate hardware models would be overkill.
*/

//Interrupts on SE calculators are based on the crystal timers
// however for now this will do.
static double timer_freq83pse[4] = {1.0/512.0, 1.0/227.0, 1.0/156.0, 1.0/108.0};


void port0_83pse(CPU_t *cpu, device_t *dev) {
	link_t * link = (link_t *) dev->aux;

	if (cpu->input) {
		cpu->bus = ((link->host&0x03)|(link->client[0]&0x03))^0x03;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ((link->host&0x01) != (cpu->bus&0x01)) {
			#ifdef WINVER
			if (link->audio.init && link->audio.enabled) 
				FlippedLeft(cpu,(cpu->bus&0x01));
			#endif
		}
		if ((link->host&0x02) != (cpu->bus&0x02)) {
			#ifdef WINVER
			if (link->audio.init && link->audio.enabled) 
				FlippedRight(cpu,((cpu->bus&0x02)>>1));
			#endif
		}		

		link->host = cpu->bus&0x03;
		cpu->output = FALSE;
	}
	#ifdef WINVER
	if (link->audio.init && link->audio.enabled) nextsample(cpu);
	#endif
}

//------------------------
// bit 0 - battery test (not implemented)
// Bit 1 - LCD wait (not implemented)
// bit 2 - flash lock
// bit 3 - not used
// bit 4 - not used
// bit 5 - 83+se or 84+
// bit 6 - not used
// bit 7 - SE or Basic
void port2_83pse(CPU_t *cpu, device_t *dev) {

	if (cpu->input) {
		cpu->bus = 0xC3 | ((cpu->mem_c->flash_locked)?4:0);
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}


void port3_83pse(CPU_t *cpu, device_t *dev) {
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
	
	
	if ((stdint->intactive & 0x01) && (cpu->pio.keypad->on_pressed & KEY_VALUE_MASK) && (stdint->on_backup & KEY_VALUE_MASK) == 0)  {
		stdint->on_latch = TRUE;
	}
	stdint->on_backup = cpu->pio.keypad->on_pressed;
	if (stdint->on_latch)
		cpu->interrupt = TRUE;
}




void port4_83pse(CPU_t *cpu, device_t *dev) {
	STDINT_t * stdint = (STDINT_t *) dev->aux;
	XTAL_t* xtal = &cpu->pio.se_aux->xtal;
	if (cpu->input) {
		unsigned char result = 0;
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) result += 2;
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) result += 4;
		if (stdint->on_latch) result += 1;
		if (!cpu->pio.keypad->on_pressed) result += 8;
		
		if (xtal->timers[0].underflow) result += 32;
		if (xtal->timers[1].underflow) result += 64;
		if (xtal->timers[2].underflow) result += 128;
		
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


void port5_83pse(CPU_t *cpu, device_t *dev) {
	if ( cpu->mem_c->boot_mapped ) {
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[2],sizeof(bank_state_t)*3);
	}
	if ( cpu->input ) {
		cpu->bus = cpu->mem_c->banks[3].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->banks[3].page		= ((cpu->bus&0x7f) % cpu->mem_c->ram_pages );
		cpu->mem_c->banks[3].addr		= cpu->mem_c->ram+(cpu->mem_c->banks[3].page*16384);
		cpu->mem_c->banks[3].read_only	= FALSE;
		// the next line isn't correct...
		cpu->mem_c->banks[3].no_exec	= (cpu->mem_c->banks[3].page%2)^1;

		cpu->output = FALSE;
	}
	if (cpu->mem_c->boot_mapped) {
		memmove(&cpu->mem_c->banks[2],&cpu->mem_c->banks[1],sizeof(bank_state_t)*3);
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[4],sizeof(bank_state_t)*1);
	}
}

void port6_83pse(CPU_t *cpu, device_t *dev) {
	if ( cpu->mem_c->boot_mapped ) {
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[2],sizeof(bank_state_t)*3);
	}
	if ( cpu->input ) {
		cpu->bus = ((cpu->mem_c->banks[1].ram)<<7)+cpu->mem_c->banks[1].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->banks[1].ram = (cpu->bus>>7)&1;
		if (cpu->mem_c->banks[1].ram) {
			cpu->mem_c->banks[1].page		= ((cpu->bus&0x7f) % cpu->mem_c->ram_pages );
			cpu->mem_c->banks[1].addr		= cpu->mem_c->ram+(cpu->mem_c->banks[1].page*16384);
			cpu->mem_c->banks[1].read_only	= FALSE;
			// the next line isn't correct...
			cpu->mem_c->banks[1].no_exec	= (cpu->mem_c->banks[1].page%2)^1;
		} else {
			cpu->mem_c->banks[1].page		= ((cpu->bus&0x7f) % cpu->mem_c->flash_pages);
			cpu->mem_c->banks[1].addr		= cpu->mem_c->flash+(cpu->mem_c->banks[1].page*16384);
			cpu->mem_c->banks[1].read_only	= FALSE;
			cpu->mem_c->banks[1].no_exec	= FALSE;
			if (cpu->mem_c->banks[1].page==0x7f) cpu->mem_c->banks[1].read_only=TRUE;
		}
		cpu->output = FALSE;
	}
	if (cpu->mem_c->boot_mapped) {
		memmove(&cpu->mem_c->banks[2],&cpu->mem_c->banks[1],sizeof(bank_state_t)*3);
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[4],sizeof(bank_state_t)*1);
	}
}

void port7_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->mem_c->boot_mapped) {
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[2],sizeof(bank_state_t)*3);
	}
	if (cpu->input) {
		cpu->bus = ((cpu->mem_c->banks[2].ram)<<7)+cpu->mem_c->banks[2].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->banks[2].ram = (cpu->bus>>7)&1;
		if (cpu->mem_c->banks[2].ram) {
			cpu->mem_c->banks[2].page		= ((cpu->bus&0x7f) % cpu->mem_c->ram_pages);
			cpu->mem_c->banks[2].addr		= cpu->mem_c->ram+(cpu->mem_c->banks[2].page*16384);
			cpu->mem_c->banks[2].read_only	= FALSE;
			// the next line isn't correct...
			cpu->mem_c->banks[2].no_exec	= (cpu->mem_c->banks[2].page%2)^1;
		} else {
			cpu->mem_c->banks[2].page		= ((cpu->bus&0x7f) % cpu->mem_c->flash_pages);
			cpu->mem_c->banks[2].addr		= cpu->mem_c->flash+(cpu->mem_c->banks[2].page*16384);
			cpu->mem_c->banks[2].read_only	= FALSE;
			cpu->mem_c->banks[2].no_exec	= FALSE;
			if (cpu->mem_c->banks[2].page==0x1f) cpu->mem_c->banks[2].read_only=TRUE;
		}
		cpu->output = FALSE;
	}
	if (cpu->mem_c->boot_mapped) {
		memmove(&cpu->mem_c->banks[2],&cpu->mem_c->banks[1],sizeof(bank_state_t)*3);
		memmove(&cpu->mem_c->banks[1],&cpu->mem_c->banks[4],sizeof(bank_state_t)*1);
	}
}


void port14_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = cpu->mem_c->flash_locked;
		cpu->input = FALSE;
	} else if (cpu->output) {
		int bank = cpu->pc>>14;
		if (!cpu->mem_c->banks[bank].ram) {
			if ( cpu->mem_c->banks[bank].page > 0x7B ) {		
				cpu->mem_c->flash_locked = ( cpu->bus & 0x01 );
			}
		}
		cpu->output = FALSE;
	}
}


//########################################################
//########################################################
//##
//##	START OF 83+SE SPECIFIC CODE
//##
//########################################################
//########################################################


void port8_83pse(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t* assist = (LINKASSIST_t *) dev->aux;
	
	if (cpu->input) {
		cpu->bus = assist->link_enable & 0x87;
		cpu->input = FALSE;
	} else if (cpu->output) {
		assist->link_enable = cpu->bus;
		if (assist->link_enable & 0x80) {
			assist->receiving	= FALSE;
			assist->read		= FALSE;
			assist->ready		= TRUE;
			assist->error		= FALSE;
			assist->sending		= FALSE;
			Modify_interrupt_device(cpu,0x09,0);
		} else Modify_interrupt_device(cpu,0x09,3);
		cpu->output = FALSE;
	}
}

void port9_83pse(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t* assist = (LINKASSIST_t *) dev->aux;
	link_t * link = (link_t *) cpu->pio.link;
	
	if (!cpu->input && !cpu->output) {
		if (!(assist->link_enable&0x80)) {

			if (assist->sending) {
				assist->ready = FALSE;
				//if (!isinit(cpu)) {
				if (0) {
					assist->sending = FALSE;
					assist->ready = TRUE;
					assist->error	= TRUE;
				} else {
//				{
					switch ( (LinkRead) & 0x03) {
						case 00:	// bit Acknoledged
							if (link->host) {
								assist->bit++;
							}
							link->host = 0;
							break;
						case 03:
							if (assist->bit >=8) {
								assist->sending = FALSE;
								assist->ready = TRUE;
								assist->bit = 0;
							} else if (assist->out & (1<<assist->bit)) {
								link->host = 2;
							} else {
								link->host = 1;
							}
							break;
					}
				}
			} else {
				if (LinkRead!=3) assist->ready = FALSE;
				switch ( (LinkRead) & 0x03) {
					case 01:
						if (assist->bit < 8) {
							if (!assist->receiving) {
								assist->bit = 0;
							}
							assist->receiving = TRUE;
							if (!link->host) {
								assist->working  = (assist->working>>1) + 128;
								assist->bit++;
								link->host = 1;
							} else {
								link->host = 0;
							}
						} else if (assist->bit == 8) {
							if (link->host) {
								link->host = 0;
							}
							if ( !assist->read ) {
								assist->receiving = FALSE;
								assist->read = TRUE;
								assist->in = assist->working;
								assist->bit = 0;
							}
						}
						break;
					case 02:
						if (assist->bit < 8) {
							if (!assist->receiving) {
								assist->bit = 0;
							}
							assist->receiving = TRUE;
							if (!link->host) {
								assist->working  = (assist->working>>1);
								assist->bit++;
								link->host = 2;
							} else {
								link->host = 0;
							}
						} else if (assist->bit == 8) {
							if (link->host) {
								link->host = 0;
							}
							if ( !assist->read ) {
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
			if ( (assist->link_enable&0x01) && assist->read ) {
				cpu->interrupt = TRUE;
			}
			if ( (assist->link_enable&0x02) && assist->ready ) {
				cpu->interrupt = TRUE;
			}
			if ( (assist->link_enable&0x04) && assist->error ) {
				cpu->interrupt = TRUE;
			}
		}
	}
	
	if (cpu->input) {
		cpu->bus = 0x00;
		if (!(assist->link_enable&0x80)) {
			if ( (assist->link_enable&0x01) && assist->read ) {
				cpu->bus +=1;
			}
			if ( (assist->link_enable&0x02) && assist->ready ) {
				cpu->bus +=2;
			}
			if ( (assist->link_enable&0x04) && assist->error ) {
				cpu->bus +=4;
			}
			if (assist->receiving) {
				cpu->bus +=8;
			}
			if (assist->read) {
				cpu->bus +=16;
			}
			if (assist->ready) {
				cpu->bus +=32;
			}
			if (assist->error) {
				cpu->bus +=64;
			}
			if (assist->sending) {
				cpu->bus +=128;
			}
		}
		cpu->input = FALSE;
		assist->error	= FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

void port0A_83pse(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t* assist = (LINKASSIST_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = assist->in;
		assist->read = FALSE;
		cpu->input = FALSE;
	} else if (cpu->output) {
		assist->read = FALSE;
		cpu->output = FALSE;
	}
}

void port0D_83pse(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t* assist = (LINKASSIST_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = assist->out;
		assist->ready = FALSE;
		cpu->input = FALSE;
	} else if (cpu->output) {
		assist->out = cpu->bus;
		assist->ready = FALSE;
		assist->sending = TRUE;
		assist->bit = 0;
		cpu->output = FALSE;
	}
}



unsigned long calc_md5(MD5_t* md5) {
/* just don't want to type the struct every time */
	unsigned long a = md5->a;
	unsigned long b = md5->b;
	unsigned long c = md5->c;
	unsigned long d = md5->d;
	unsigned long x = md5->x;
	unsigned long ac = md5->ac;
	unsigned char s = md5->s & 0x1f;
	unsigned long reg;	
	
	switch(md5->mode) {
		case 00:
			reg = (b & c) | ((~b) & d);
			break;
		case 01:
			reg = (b & d) | (c & (~d));
			break;
		case 02:
			reg = b^c^d;
			break;
		case 03:
			reg = c^(b|(~d));
			break;
	}
	reg += a + x + ac;
	reg &= 0xFFFFFFFF;
	reg = ( (reg<<s) | (reg>>(32-s)) );
	reg += b;
	return reg;
}


void md5ports(CPU_t *cpu, device_t *dev) {
	MD5_t* md5 = (MD5_t *) dev->aux;
	if (cpu->input) {
		switch(DEV_INDEX(dev)) {
			case 0x18:
			case 0x19:
			case 0x1A:
			case 0x1B:
				cpu->bus = 0;
				break;
			case 0x1C:
			case 0x1D:
			case 0x1E:
			case 0x1F:
				cpu->bus = (calc_md5(md5)>>((DEV_INDEX(dev)-0x1C)*8))&0xFF;
				break;
		}
		cpu->input = FALSE;
	} else if (cpu->output) {
		switch(DEV_INDEX(dev)) {
			case 0x18:
			case 0x19:
			case 0x1A:
			case 0x1B:
			case 0x1C:
			case 0x1D:
				md5->reg[DEV_INDEX(dev)-0x18] =
					(md5->reg[DEV_INDEX(dev)-0x18]>>8) | (cpu->bus<<24);
				break;
			case 0x1E:
				md5->s = cpu->bus&0x1F;
				break;
			case 0x1F:
				md5->mode = cpu->bus&0x03;
				break;
		}
		cpu->output = FALSE;
	}
}



void port20_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		if (cpu->timer_c->freq <= MHZ_6) {
			cpu->bus = 0x00;
		} else {
			cpu->bus = 0x01;
		}
		cpu->input = FALSE;
	} else if (cpu->output) {
		if (cpu->bus%4 == 0) {
			cpu->timer_c->freq = MHZ_6;
		} else {
			cpu->timer_c->freq = MHZ_15;
		}
		cpu->output = FALSE;
	}
}


void port21_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x01;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

void port21_84p(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = 0x00;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

void port22_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = cpu->mem_c->lower;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->lower = cpu->bus;
		cpu->output = FALSE;
	}
}

void port23_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = cpu->mem_c->upper;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->upper = cpu->bus;
		cpu->output = FALSE;
	}
}

void delay_ports(CPU_t *cpu, device_t *dev) {
	DELAY_t* delay = (DELAY_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = delay->reg[(DEV_INDEX(dev)-0x29)];
		cpu->input = FALSE;
	} else if (cpu->output) {
		delay->reg[(DEV_INDEX(dev)-0x29)] = cpu->bus;
		cpu->output = FALSE;
	}
}



void mod_timer(CPU_t *cpu,XTAL_t* xtal) {
	int a,b,c;
	a = ((xtal->timers[0].clock&0xC0)>>6)&0x03;
	b = ((xtal->timers[1].clock&0xC0)>>6)&0x03;
	c = ((xtal->timers[2].clock&0xC0)>>6)&0x03;

	switch (a|b|c) {
		case 0:
			Modify_interrupt_device(cpu,0x32,0);
			break;
		case 1:
			Modify_interrupt_device(cpu,0x32,8);
			break;
		case 2:
		case 3:
			Modify_interrupt_device(cpu,0x32,1);
			break;
	}
}

void port_chunk_remap_83pse(CPU_t *cpu, device_t *dev) {
	int *count = (int *) dev->aux;
	
	if (cpu->input) {
		cpu->bus = *count;
		cpu->input = FALSE;
	} else {
		*count = cpu->bus;
		cpu->output = FALSE;
	}
}


void port30_83pse(CPU_t *cpu, device_t *dev) {
	XTAL_t* xtal = (XTAL_t *) dev->aux;
	TIMER_t* timer = &xtal->timers[(DEV_INDEX(dev)-0x30)/3];
	
	if (cpu->input) {
		cpu->bus = timer->clock;
		cpu->input = FALSE;
	} else if (cpu->output) {
		timer->clock = cpu->bus;
		timer->active = FALSE;
		timer->generate	= FALSE;
		switch ( (timer->clock&0xC0)>>6) {
			case 0x00:
			{
				timer->divsor = 0.0f;
				break;
			}
			case 0x01:
			{
				switch (timer->clock&0x07) {
					case 0x00:
						timer->divsor = 3.0f;
						break;
					case 0x01:
						timer->divsor = 32.0f;
						break;
					case 0x02:
						timer->divsor = 327.000f;
						break;
					case 0x03:
						timer->divsor = 3276.00f;
						break;
					case 0x04:
						timer->divsor = 1.0f;
						break;
					case 0x05:
						timer->divsor = 16.0f;
						break;
					case 0x06:
						timer->divsor = 256.0f;
						break;
					case 0x07:
						timer->divsor = 4096.0f;
						break;
				}
				break;
			}
			case 0x02:
			case 0x03:
			{
				int i;
				int mask = 0x20;
				timer->divsor = 64.0f;
				for(i=0;i<6; i++) {
					if (timer->clock & mask) break;
					mask = mask>>1;
					timer->divsor /=2.0f;
					
				}
				break;
			}
		}
		mod_timer(cpu,xtal);
		cpu->output = FALSE;
	}
}

void port31_83pse(CPU_t *cpu, device_t *dev) {
	XTAL_t* xtal = (XTAL_t *) dev->aux;
	TIMER_t* timer = &xtal->timers[(DEV_INDEX(dev)-0x30)/3];
	if (cpu->input) {
		cpu->bus  = (timer->loop?1:0);
		cpu->bus += (timer->interrupt?2:0);
		cpu->bus += (timer->underflow?4:0);
		cpu->input = FALSE;
	} else if (cpu->output) {
		timer->loop			= (cpu->bus&0x01)?1:0;
		timer->interrupt	= (cpu->bus&0x02)?1:0;
		timer->underflow	= FALSE;
		timer->generate		= FALSE;
		cpu->output = FALSE;
	}
}



void handlextal(CPU_t *cpu,XTAL_t* xtal) {
	TIMER_t* timer = &xtal->timers[0];


	
	/* overall xtal timer ticking */
/*	
	if (tc_elapsed(cpu->timer_c) > ( xtal->lastTime + (1.0f / 32768.0f ) ) ){
		xtal->lastTime += (1.0f / 32768.0f );
		xtal->ticks++;

	}
*/
	xtal->ticks = tc_elapsed(cpu->timer_c)*32768.0f;
	xtal->lastTime = ((double)xtal->ticks / 32768.0f);
	


	int i;
	for(i=0;i<3;i++) {
		timer = &xtal->timers[i];
		if (timer->active) {
			
			switch(((timer->clock&0xC0)>>6)&0x03) {
				case 0:
					break;
				case 1:
					if ( (timer->lastTicks+timer->divsor) < xtal->ticks ) {
						timer->lastTicks+=timer->divsor;
						timer->count--;
						if (!timer->count) {
							if (!timer->underflow) {
								timer->count = timer->max;
								if (!timer->loop) {
									timer->active = FALSE;
								}
							}
							if (timer->interrupt) timer->generate = TRUE;
							timer->underflow = TRUE;
						}
					}
					break;
				case 2:
				case 3:
					while( timer->lastTstates+((long long)timer->divsor) < tc_tstates(cpu->timer_c) ) {
						
						timer->lastTstates+=timer->divsor;
						timer->count--;
						if (!timer->count) {
							if (!timer->underflow) {
								timer->count = timer->max;
								if (!timer->loop) {
									timer->active = FALSE;
								}
							}
							if (timer->interrupt) timer->generate = TRUE;
							timer->underflow = TRUE;
						}
					}
					break;
			}
		}
						
		if (timer->generate && !cpu->halt) {
			cpu->interrupt = TRUE;
		}
	}
}

void port32_83pse(CPU_t *cpu, device_t *dev) {
	XTAL_t* xtal = (XTAL_t *) dev->aux;
	TIMER_t* timer = &xtal->timers[(DEV_INDEX(dev)-0x30)/3];
	
	

	handlextal(cpu,xtal);
		
		
	if (cpu->input) {
		cpu->bus = timer->count;
		cpu->input = FALSE;

	} else if (cpu->output) {
		timer->count = cpu->bus;
		timer->max = cpu->bus;
		if (timer->clock&0xC0) timer->active = TRUE;
		timer->lastTstates = tc_tstates(cpu->timer_c);
		timer->lastTicks = xtal->ticks;
		mod_timer(cpu,xtal);
		cpu->output = FALSE;
	}
	
//	handlextal(cpu,xtal);
}





//########################################################
//########################################################
//##
//##	START OF 84+SE SPECIFIC CODE
//##
//########################################################
//########################################################




void clock_enable(CPU_t *cpu, device_t *dev) {
	CLOCK_t* clock = (CLOCK_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = clock->enable&0x03;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ( (clock->enable&0x02)==0 && (cpu->bus&0x02)==2) {
			clock->base = clock->set;
		}
		if ( (clock->enable&0x01)==0 && (cpu->bus&0x01)==1) {
			clock->lasttime = tc_elapsed(cpu->timer_c);
		}
		if ( (clock->enable&0x01)==1 && (cpu->bus&0x01)==0) {
			clock->base = clock->base+(tc_elapsed(cpu->timer_c)-clock->lasttime);
		}
		clock->enable= cpu->bus&0x03;

		cpu->output = FALSE;
	}
}

void clock_set(CPU_t *cpu, device_t *dev) {
	CLOCK_t* clock = (CLOCK_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = (clock->set>>((DEV_INDEX(dev)-0x41)*8))&0xFF;
		cpu->input = FALSE;
	} else if (cpu->output) {
		clock->set = 
			( clock->set & ~(0xFF<<((DEV_INDEX(dev)-0x41)*8)) ) | 
			( cpu->bus<<((DEV_INDEX(dev)-0x41)*8) );
		clock->lasttime = tc_elapsed(cpu->timer_c);
		cpu->output = FALSE;
	}
}
	
	
void clock_read(CPU_t *cpu, device_t *dev) {
	CLOCK_t* clock = (CLOCK_t *) dev->aux;
	if (cpu->input) {
		unsigned long time;
		if (clock->enable&0x01) {
			time = clock->base+(tc_elapsed(cpu->timer_c)-clock->lasttime);
		} else {
			time = clock->base;
		}

		cpu->bus = (time>>((DEV_INDEX(dev)-0x45)*8))&0xFF;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}
	
	
	
void fake_usb(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		switch(DEV_INDEX(dev)) {
/* double checked TILEM for proper values, thanks Ben, Dan_E*/
			case 0x4C:
				cpu->bus =  0x22;
				break;
			case 0x4D:
				cpu->bus =  0xA5;
				break;
			case 0x55:
				cpu->bus =  0x1F;
				break;
			case 0x56:
				cpu->bus =  0x00;
				break;
			case 0x57:
				cpu->bus =  0x50;
				break;
		}	
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}
	
	


void flashwrite83pse(CPU_t *cpu, unsigned short addr, unsigned char data) {
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
					printf("\n");
					cpu->mem_c->cmd=0x80;		//Erase
					cpu->mem_c->step++;
				} else if ( data == 0x20 ) {
					puts("Fast");
					cpu->mem_c->cmd=0x20;		//Fastmode
					cpu->mem_c->step=6;
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
//				if (cpu->mem_c->banks[bank].page == 0x7E) printf("%04X <= %02X \n",addr,data);
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

				printf("Spage = %03d , Page = %02X , Addr = %04X \n",spage, spage/2,( ( spage & 0x00F8 ) * 0x2000 ) );
				if (spage<248) {
					int startaddr = ( ( spage & 0x00F8 ) * 0x2000 );
					int endaddr   = ( startaddr + 0x10000 );
					for(i=startaddr; i<endaddr ;i++) {
						cpu->mem_c->flash[i]=0xFF;
					}
				} else if (spage<252) {
					for( i=0x1F0000; i < 0x1F8000; i++ ) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				} else if (spage<253) {
//					printf("\nAddress: 1E:0000 -- ERASED\n");
					for( i=0x1F8000; i < 0x1FA000; i++ ) {
						cpu->mem_c->flash[i] = 0xFF;

					}
				} else if (spage<254) {
//											printf("\nAddress: 1E:2000 -- ERASED\n");
					for( i=0x1FA000; i< 0x1FC000; i++ ) {
						cpu->mem_c->flash[i]=0xFF;
					}
				} else if (spage<256) {
// I comment this off because this is the boot page
// it suppose to be write protected...
//					for(i=0x7C000;i<0x80000;i++) {
//						cpu->mem_c->flash[i]=0xFF;
//					}
				}

			}
			break;
		case 6:
			if (data == 0x90 ) {
				cpu->mem_c->step=7;	//check if exit fastmode
			} else if (data == 0xA0 ) {
				cpu->mem_c->step=8;	//write byte in fastmode
			}
			break;
		case 7:
			if (data == 0xF0) {
				endflash(cpu);
			} else cpu->mem_c->step=6;
			break;
		case 8:
			(*(cpu->mem_c->banks[bank].addr +(addr&0x3fff))) &= data;  //AND LOGIC!!
			cpu->mem_c->step=6;
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



STDINT_t* INT83PSE_init(CPU_t* cpu) {
	STDINT_t * stdint = (STDINT_t *) malloc(sizeof(STDINT_t));
	if (!stdint) {
		printf("Couldn't allocate memory for standard interrupt\n");
		exit(1);
	}
	
	stdint->freq[0] = timer_freq83pse[0];
	stdint->freq[1] = timer_freq83pse[1];
	stdint->freq[2] = timer_freq83pse[2];
	stdint->freq[3] = timer_freq83pse[3];
	
	stdint->intactive = 0;
	stdint->timermax1 = stdint->freq[3];
	stdint->lastchk1 = tc_elapsed(cpu->timer_c);
	stdint->timermax2 = stdint->freq[3]/2.0f;
	stdint->lastchk2 = tc_elapsed(cpu->timer_c)+stdint->freq[3]/4.0f;
	
	stdint->on_backup = 0;
	stdint->on_latch = FALSE;
	return stdint;
}

link_t* link83pse_init(CPU_t* cpu) {
	link_t * link = (link_t *) malloc(sizeof(link_t)); 
	if (!link) {
		printf("Couldn't allocate memory for link\n");
		exit(1);
	}
	link->host		= 0;			//neither lines set
	link->client	= &link->host;	//nothing plugged in.


	return link;
}




SE_AUX_t* SE_AUX_init(CPU_t* cpu) {
	SE_AUX_t* se_aux = (SE_AUX_t *) malloc(sizeof(SE_AUX_t));
	if (!se_aux) {
		printf("Couldn't allocate memory for SE structs\n");
		exit(1);
	}
	memset(se_aux,0,sizeof(SE_AUX_t));
	se_aux->xtal.timers[0].divsor = 1;
	se_aux->xtal.timers[1].divsor = 1;
	se_aux->xtal.timers[2].divsor = 1;
	se_aux->linka.link_enable = 0x80;
	return se_aux;
}

int device_init_83pse(CPU_t *cpu) {
	ClearDevices(cpu);

/* link */
	link_t * link = link83pse_init(cpu);
	cpu->pio.devices[0x00].active = TRUE;
	cpu->pio.devices[0x00].aux = link;
	cpu->pio.devices[0x00].code = (devp) port0_83pse;

/* Key pad */
	keypad_t *keyp = keypad_init(cpu);
	cpu->pio.devices[0x01].active = TRUE;
	cpu->pio.devices[0x01].aux = keyp;
	cpu->pio.devices[0x01].code = (devp) keypad;

/* status */
	cpu->pio.devices[0x02].active = TRUE;
	cpu->pio.devices[0x02].code = (devp) port2_83pse;

/* standard interrupts */
	STDINT_t* stdint = INT83PSE_init(cpu);
	cpu->pio.devices[0x03].active = TRUE;
	cpu->pio.devices[0x03].aux = stdint;
	cpu->pio.devices[0x03].code = (devp) port3_83pse;

	cpu->pio.devices[0x04].active = TRUE;
	cpu->pio.devices[0x04].aux = stdint;
	cpu->pio.devices[0x04].code = (devp) port4_83pse;
	
	
/* memory mapping */
	cpu->pio.devices[0x05].active = TRUE;
	cpu->pio.devices[0x05].code = (devp) port5_83pse;

	cpu->pio.devices[0x06].active = TRUE;
	cpu->pio.devices[0x06].code = (devp) port6_83pse;

	cpu->pio.devices[0x07].active = TRUE;
	cpu->pio.devices[0x07].code = (devp) port7_83pse;
	
	
	SE_AUX_t* se_aux = SE_AUX_init(cpu);
	
// SE link assist
	cpu->pio.devices[0x08].active = TRUE;
	cpu->pio.devices[0x08].aux = &se_aux->linka;
	cpu->pio.devices[0x08].code = (devp) port8_83pse;

	cpu->pio.devices[0x09].active = TRUE;
	cpu->pio.devices[0x09].aux = &se_aux->linka;
	cpu->pio.devices[0x09].code = (devp) port9_83pse;

	cpu->pio.devices[0x0A].active = TRUE;
	cpu->pio.devices[0x0A].aux = &se_aux->linka;
	cpu->pio.devices[0x0A].code = (devp) port0A_83pse;

	cpu->pio.devices[0x0D].active = TRUE;
	cpu->pio.devices[0x0D].aux = &se_aux->linka;
	cpu->pio.devices[0x0D].code = (devp) port0D_83pse;
	
/* LCD */
	LCD_t *lcd = LCD_init(cpu,TI_83PSE);
	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp) LCD_command;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp) LCD_data;

/* Flash locking */
	cpu->pio.devices[0x14].active = TRUE;
	cpu->pio.devices[0x14].code = (devp) port14_83pse;
	

// MD5
	for (int i = 0x18; i <= 0x1F; i++) {
		cpu->pio.devices[i].active = TRUE;
		cpu->pio.devices[i].aux = &se_aux->md5;
		cpu->pio.devices[i].code = (devp) md5ports;
	}

/* speed */
	cpu->pio.devices[0x20].active = TRUE;
	cpu->pio.devices[0x20].aux = NULL;
	cpu->pio.devices[0x20].code = (devp) port20_83pse;

	
	
/* page locking */
	cpu->pio.devices[0x21].active = TRUE;
	cpu->pio.devices[0x21].aux = NULL;
	cpu->pio.devices[0x21].code = (devp) port21_83pse;

	cpu->pio.devices[0x22].active = TRUE;
	cpu->pio.devices[0x22].aux = NULL;
	cpu->pio.devices[0x22].code = (devp) port22_83pse;

	cpu->pio.devices[0x23].active = TRUE;
	cpu->pio.devices[0x23].aux = NULL;
	cpu->pio.devices[0x23].code = (devp) port23_83pse;


// weird ass remap stuff
	cpu->pio.devices[0x27].active = TRUE;
	cpu->pio.devices[0x27].aux = &cpu->mem_c->port27_remap_count;
	cpu->pio.devices[0x27].code = (devp) &port_chunk_remap_83pse;

	cpu->pio.devices[0x28].active = TRUE;
	cpu->pio.devices[0x28].aux = &cpu->mem_c->port28_remap_count;
	cpu->pio.devices[0x28].code = (devp) &port_chunk_remap_83pse;
	
//delay ports
	for (int i = 0x29; i <= 0x2F; i++) {
		cpu->pio.devices[i].active = TRUE;
		cpu->pio.devices[i].aux = &se_aux->delay;
		cpu->pio.devices[i].code = (devp) delay_ports;
	}

// timer 1
	cpu->pio.devices[0x30].active = TRUE;
	cpu->pio.devices[0x30].aux = &se_aux->xtal;
	cpu->pio.devices[0x30].code = (devp) &port30_83pse;

	cpu->pio.devices[0x31].active = TRUE;
	cpu->pio.devices[0x31].aux = &se_aux->xtal;
	cpu->pio.devices[0x31].code = (devp) &port31_83pse;

	cpu->pio.devices[0x32].active = TRUE;
	cpu->pio.devices[0x32].aux = &se_aux->xtal;
	cpu->pio.devices[0x32].code = (devp) &port32_83pse;

// timer 2
	cpu->pio.devices[0x33].active = TRUE;
	cpu->pio.devices[0x33].aux = &se_aux->xtal;
	cpu->pio.devices[0x33].code = (devp) &port30_83pse;

	cpu->pio.devices[0x34].active = TRUE;
	cpu->pio.devices[0x34].aux = &se_aux->xtal;
	cpu->pio.devices[0x34].code = (devp) &port31_83pse;

	cpu->pio.devices[0x35].active = TRUE;
	cpu->pio.devices[0x35].aux = &se_aux->xtal;
	cpu->pio.devices[0x35].code = (devp) &port32_83pse;

// timer 3
	cpu->pio.devices[0x36].active = TRUE;
	cpu->pio.devices[0x36].aux = &se_aux->xtal;
	cpu->pio.devices[0x36].code = (devp) &port30_83pse;

	cpu->pio.devices[0x37].active = TRUE;
	cpu->pio.devices[0x37].aux = &se_aux->xtal;
	cpu->pio.devices[0x37].code = (devp) &port31_83pse;

	cpu->pio.devices[0x38].active = TRUE;
	cpu->pio.devices[0x38].aux = &se_aux->xtal;
	cpu->pio.devices[0x38].code = (devp) &port32_83pse;



// Clock 
	cpu->pio.devices[0x40].active = TRUE;
	cpu->pio.devices[0x40].aux = &se_aux->clock;
	cpu->pio.devices[0x40].code = (devp) &clock_enable;

	cpu->pio.devices[0x41].active = TRUE;
	cpu->pio.devices[0x41].aux = &se_aux->clock;
	cpu->pio.devices[0x41].code = (devp) &clock_set;

	cpu->pio.devices[0x42].active = TRUE;
	cpu->pio.devices[0x42].aux = &se_aux->clock;
	cpu->pio.devices[0x42].code = (devp) &clock_set;

	cpu->pio.devices[0x43].active = TRUE;
	cpu->pio.devices[0x43].aux = &se_aux->clock;
	cpu->pio.devices[0x43].code = (devp) &clock_set;

	cpu->pio.devices[0x44].active = TRUE;
	cpu->pio.devices[0x44].aux = &se_aux->clock;
	cpu->pio.devices[0x44].code = (devp) &clock_set;

	cpu->pio.devices[0x45].active = TRUE;
	cpu->pio.devices[0x45].aux = &se_aux->clock;
	cpu->pio.devices[0x45].code = (devp) &clock_read;

	cpu->pio.devices[0x46].active = TRUE;
	cpu->pio.devices[0x46].aux = &se_aux->clock;
	cpu->pio.devices[0x46].code = (devp) &clock_read;

	cpu->pio.devices[0x47].active = TRUE;
	cpu->pio.devices[0x47].aux = &se_aux->clock;
	cpu->pio.devices[0x47].code = (devp) &clock_read;

	cpu->pio.devices[0x48].active = TRUE;
	cpu->pio.devices[0x48].aux = &se_aux->clock;
	cpu->pio.devices[0x48].code = (devp) &clock_read;	



/*Fake USB*/
	cpu->pio.devices[0x4C].active = TRUE;
	cpu->pio.devices[0x4C].aux = NULL;
	cpu->pio.devices[0x4C].code = (devp) &fake_usb;	

	cpu->pio.devices[0x4D].active = TRUE;
	cpu->pio.devices[0x4D].aux = NULL;
	cpu->pio.devices[0x4D].code = (devp) &fake_usb;

	cpu->pio.devices[0x55].active = TRUE;
	cpu->pio.devices[0x55].aux = NULL;
	cpu->pio.devices[0x55].code = (devp) &fake_usb;

	cpu->pio.devices[0x56].active = TRUE;
	cpu->pio.devices[0x56].aux = NULL;
	cpu->pio.devices[0x56].code = (devp) &fake_usb;

	cpu->pio.devices[0x57].active = TRUE;
	cpu->pio.devices[0x57].aux = NULL;
	cpu->pio.devices[0x57].code = (devp) &fake_usb;

	
	
	cpu->pio.lcd		= lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= link;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= se_aux;

	cpu->pio.model		= TI_83PSE;
	
	Append_interrupt_device(cpu,0x00,1);
	Append_interrupt_device(cpu,0x03,8);
	Append_interrupt_device(cpu,0x11,128);
	Append_interrupt_device(cpu,0x09,3);
	Append_interrupt_device(cpu,0x32,8);
	return 0;
}


int memory_init_83pse(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));
	
	/* Set Number of Pages here */
	mc->flash_pages = 128;
	mc->ram_pages = 8;

	mc->flash_version = 2;
	mc->upper = 0x60;
	mc->lower = 0x10;
	
	
	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (u_char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (u_char *) calloc(mc->flash_pages, PAGE_SIZE);
	memset(mc->flash, 0xFF, mc->flash_size);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (u_char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (u_char *) calloc(mc->ram_pages, PAGE_SIZE);
	
	if (!mc->flash || !mc->ram ) {
		printf("Couldn't allocate memory in memory_init_83pse\n");
		return 1;
	}

	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;

	/* Organize bank states here */
	
	/*	Address								page	write?	ram?	no exec?	*/
	bank_state_t banks[5] = {
		{mc->flash, 						0, 		FALSE,	FALSE, 	FALSE},
		{mc->flash+0x7f*PAGE_SIZE,			0x7f, 	FALSE, 	FALSE, 	FALSE},
		{mc->flash+0x7f*PAGE_SIZE,			0x7f, 	FALSE, 	FALSE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	TRUE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->banks, banks, sizeof(banks));
	return 0;
}



