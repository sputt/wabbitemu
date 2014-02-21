#include "stdafx.h"

#include "83psehw.h"
#include "lcd.h"
#include "colorlcd.h"
#include "keys.h"
#include "link.h"
#include "device.h"

/*
	NOTE ABOUT 83+SE AND 84+SE:
		There is no difference in hardware code between the 83+SE
	and the 84+SE.  The reason is that there should be zero code interaction
	so making separate hardware models would be overkill.
*/

//Interrupts on SE calculators are based on the crystal timers
// however for now this will do.
static double timer_freq83pse[4] = { 1.0 / 512.0, 1.0 / 227.0, 1.0 / 158.0, 1.0 / 108.0 };

void UpdateDelays(CPU_t *cpu, DELAY_t *delay);

void port0_83pse(CPU_t *cpu, device_t *dev) {
	link_t * link = (link_t *) dev->aux;

	if (cpu->input) {
		cpu->bus = ((link->host & 0x03) | (link->client[0] & 0x03)) ^ 0x03;
		cpu->bus += cpu->link_write << 4;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ((link->host & 0x01) != (cpu->bus & 0x01)) {
			if (link->audio.init && link->audio.enabled) 
				FlippedLeft(cpu, cpu->bus & 0x01);
		}
		if ((link->host & 0x02) != (cpu->bus & 0x02)) {
			if (link->audio.init && link->audio.enabled) 
				FlippedRight(cpu, (cpu->bus & 0x02) >> 1);
		}		

		cpu->link_write = link->host = cpu->bus & 0x03;
		cpu->output = FALSE;
	}

	if (link->audio.init && link->audio.enabled) {
		nextsample(cpu);
	}
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
	LCD_t *lcd = (LCD_t *) cpu->pio.lcd;
	if (cpu->input) {
		cpu->bus =  (cpu->pio.model >= TI_84P ? 0xE1 : 0xC1) | (cpu->mem_c->flash_locked ? 0 : 4) 
			| (((tc_tstates(cpu->timer_c) - cpu->pio.lcd->last_tstate) > lcd->lcd_delay) ? 2 : 0);
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;

		STDINT_t * stdint = cpu->pio.stdint;
		if (!(cpu->bus & BIT(0))) {
			stdint->on_latch = FALSE;
		}

		if (!(cpu->bus & BIT(1))) {
			while ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
				stdint->lastchk1 += stdint->timermax1;
			}
		}

		if (!(cpu->bus & BIT(2))) {
			while ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) {
				stdint->lastchk2 += stdint->timermax2;
			}
		}
	}
}

void port3_83pse(CPU_t *cpu, device_t *dev) {
	STDINT_t * stdint = (STDINT_t *) dev->aux;

	if (cpu->input) {
		cpu->bus = stdint->intactive;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if ( (!(stdint->intactive & BIT(3))) && (cpu->bus & BIT(3)) && cpu->pio.model < TI_84PCSE) {
			cpu->pio.lcd->active = TRUE;		//I'm worried about this
			/*
			Normally when the calc is set to a low power state and hits a halt
			the LCD ram disconnects from the LCD.  It does not physically power
			off the LCD.  However for the sake of ease, its powering off here.
			should normally be fine, since no one should rely on a powered on
			disconnected LCD.
			*/
		}
		
		if ((cpu->bus & BIT(0)) == 0) {
			stdint->on_latch = FALSE;
		}
		
		stdint->intactive = cpu->bus;
		cpu->output = FALSE;
	}
	
	/*Read note above*/
	if (!(stdint->intactive & BIT(3))  && (cpu->halt == TRUE) && cpu->pio.model < TI_84PCSE) {
		cpu->pio.lcd->active = FALSE;
	}

/*
	Standard interrupt
	occurs on a base frequency
	when mask timer continues to tick but 
	does not generate an interrupt. */
	if (stdint->intactive & BIT(1)) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
			cpu->interrupt = TRUE;
		}
	}
	else if ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
		//		stdint->lastchk1 = ceil((tc_elapsed(cpu->timer_c) - stdint->lastchk1)/stdint->timermax1)*stdint->timermax1;
		while ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1) {
			stdint->lastchk1 += stdint->timermax1;
		}
	}

/*
	Double speed standard interrupt
	occurs (1/(4*frequency)) second after standard timer.
	when mask timer continues to tick but 
	does not generate an interrupt. */
	if (stdint->intactive & BIT(2)) {
		if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) {
			cpu->interrupt = TRUE;
		}
	} else if ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) {
//		stdint->lastchk2 = ceil((tc_elapsed(cpu->timer_c) - stdint->lastchk2)/stdint->timermax2)*stdint->timermax2;
		while ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2) {
			stdint->lastchk2 += stdint->timermax2;
		}
	}
	
	
	if ((stdint->intactive & BIT(0)) && (cpu->pio.keypad->on_pressed & KEY_VALUE_MASK) && (stdint->on_backup & KEY_VALUE_MASK) == 0)  {
		stdint->on_latch = TRUE;
	}

	stdint->on_backup = cpu->pio.keypad->on_pressed;
	if (stdint->on_latch) {
		cpu->interrupt = TRUE;
	}
}

void port4_83pse(CPU_t *cpu, device_t *dev) {
	STDINT_t *stdint = (STDINT_t *) dev->aux;
	XTAL_t *xtal = &cpu->pio.se_aux->xtal;
	if (cpu->input) {
		unsigned char result = 0;
		if (stdint->on_latch) {
			result += BIT(0);
		}

		if ((stdint->intactive & BIT(1)) && ((tc_elapsed(cpu->timer_c) - stdint->lastchk1) > stdint->timermax1)) {
			result += BIT(1);
		}

		if ((stdint->intactive & BIT(2)) && ((tc_elapsed(cpu->timer_c) - stdint->lastchk2) > stdint->timermax2)) {
			result += BIT(2);
		}

		if (!cpu->pio.keypad->on_pressed) {
			result += BIT(3);
		}
		
		if (xtal->timers[0].underflow) {
			result += BIT(5);
		}

		if (xtal->timers[1].underflow) {
			result += BIT(6);
		}

		if (xtal->timers[2].underflow) {
			result += BIT(7);
		}
		
		cpu->bus = result;
		cpu->input = FALSE;
	} else if (cpu->output) {
		/* I'm not sure if this is how the interrupts work. */
		/* but for practicality its close enough for now. */
		int freq = ((cpu->bus & 6) >> 1);
		stdint->timermax1 = stdint->freq[freq];
		stdint->timermax2 = ( stdint->freq[freq] / 2.0f );
		stdint->lastchk2  = stdint->lastchk1 + ( stdint->freq[freq] / 4.0f );

		if (cpu->bus & BIT(0)) {
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

void port5_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (unsigned char) cpu->mem_c->banks[3].page;
		cpu->input = FALSE;
	} else if (cpu->output) {
		change_page(cpu->mem_c, 3, (cpu->bus & 0x7f) % cpu->mem_c->ram_pages, TRUE);
		cpu->output = FALSE;
	}
}

static void update_bank_1(CPU_t *cpu) {
	BOOL ram = (cpu->mem_c->port06 >> 7) & 1;
	int page;
	if (ram) {
		page = cpu->mem_c->port06 & (cpu->mem_c->ram_pages - 1);
		change_page(cpu->mem_c, 1, (u_char)page, ram);
	} else {
		page = (cpu->mem_c->port06 & 0x7F) | (cpu->mem_c->port0E << 7);
		change_page(cpu->mem_c, 1, (u_char)(page & (cpu->mem_c->flash_pages - 1)), ram);
	}
}

static void update_bank_2(CPU_t *cpu) {
	BOOL ram = (cpu->mem_c->port07 >> 7) & 1;
	int page;
	if (ram) {
		page = cpu->mem_c->port07 & (cpu->mem_c->ram_pages - 1);
		change_page(cpu->mem_c, 2, (u_char)page, ram);
	} else {
		page = (cpu->mem_c->port07 & 0x7F) | (cpu->mem_c->port0F << 7);
		change_page(cpu->mem_c, 2, (u_char)(page & (cpu->mem_c->flash_pages - 1)), ram);
	}
}

void port6_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (unsigned char)((cpu->mem_c->banks[1].ram << 7) + (cpu->mem_c->banks[1].page & 0x7F));
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->port06 = cpu->bus;
		update_bank_1(cpu);
		cpu->output = FALSE;
	}
}

void port7_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (unsigned char)((cpu->mem_c->banks[2].ram << 7) + (cpu->mem_c->banks[2].page & 0x7F));
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->mem_c->port07 = cpu->bus;
		update_bank_2(cpu);
		cpu->output = FALSE;
	}
}

//Flash locking port
//0 if locked, 1 if unlocked
void port14_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = !cpu->mem_c->flash_locked;
		cpu->input = FALSE;
	} else if (cpu->output) {
		if (is_priveleged_page(cpu)) {
			cpu->mem_c->flash_locked = !(cpu->bus & BIT(0));
		}
		cpu->output = FALSE;
	}
}

void port15_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		if (cpu->pio.model == TI_83PSE) {
			cpu->bus = 0x33;
		} else if (cpu->pio.model == TI_84PCSE) {
			cpu->bus = 0x45;
		} else if (!cpu->mem_c->ram_version) {
			cpu->bus = 0x44;
		} else {
			cpu->bus = 0x55;
		}

		cpu->input = FALSE;
	} else if (cpu->output) {
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
	LINKASSIST_t *assist = (LINKASSIST_t *) dev->aux;
	
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
			Modify_interrupt_device(cpu, 0x09, 0);
		} else Modify_interrupt_device(cpu, 0x09, 3);
		cpu->output = FALSE;
	}
}

void port9_83pse(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t* assist = (LINKASSIST_t *) dev->aux;
	link_t * link = (link_t *) cpu->pio.link;
	
	if (!cpu->input && !cpu->output) {
		if (!(assist->link_enable & 0x80)) {

			if (assist->sending) {
				assist->ready = FALSE;
				switch ((LinkRead) & 0x03) {
					case 00:	// bit Acknowledged
						if (link->host) {
							assist->bit++;
						}
						link->host = 0;
						break;
					case 03:
						if (assist->bit >= 8) {
							assist->sending = FALSE;
							assist->ready = TRUE;
							assist->bit = 0;
						} else if (assist->out & (1 << assist->bit)) {
							link->host = 2;
						} else {
							link->host = 1;
						}
						break;
				}
			} else {
				if (LinkRead != 3) assist->ready = FALSE;
				switch ((LinkRead) & 0x03) {
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
			if ((assist->link_enable & 0x01) && assist->read) {
				cpu->interrupt = TRUE;
			}
			if ((assist->link_enable & 0x02) && assist->ready) {
				cpu->interrupt = TRUE;
			}
			if ((assist->link_enable & 0x04) && assist->error) {
				cpu->interrupt = TRUE;
			}
		}
	}
	
	if (cpu->input) {
		cpu->bus = 0x00;
		if (!(assist->link_enable & 0x80)) {
			if ((assist->link_enable & 0x01) && assist->read) {
				cpu->bus += 1;
			}
			if ((assist->link_enable & 0x02) && assist->ready) {
				cpu->bus += 2;
			}
			if ((assist->link_enable & 0x04) && assist->error) {
				cpu->bus += 4;
			}
			if (assist->receiving) {
				cpu->bus += 8;
			}
			if (assist->read) {
				cpu->bus += 16;
			}
			if (assist->ready) {
				cpu->bus += 32;
			}
			if (assist->error) {
				cpu->bus += 64;
			}
			if (assist->sending) {
				cpu->bus += 128;
			}
		}
		cpu->input = FALSE;
		assist->error	= FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

void port0A_83pse(CPU_t *cpu, device_t *dev) {
	LINKASSIST_t *assist = (LINKASSIST_t *) dev->aux;
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
	LINKASSIST_t *assist = (LINKASSIST_t *) dev->aux;
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

void port0E_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->input = FALSE;
		cpu->bus = cpu->mem_c->port0E;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->mem_c->port0E = cpu->bus & 0x3;
		update_bank_1(cpu);
	}
}

void port0F_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->input = FALSE;
		cpu->bus = cpu->mem_c->port0F;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->mem_c->port0F = cpu->bus & 0x3;
		update_bank_2(cpu);
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
	unsigned long reg = 0;	
	
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
	reg = ( (reg << s) | (reg >> (32 - s)));
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
				cpu->bus = (calc_md5(md5) >> ((DEV_INDEX(dev) - 0x1C) * 8)) & 0xFF;
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
				md5->reg[DEV_INDEX(dev) - 0x18] =
					(md5->reg[DEV_INDEX(dev) - 0x18] >> 8) | (cpu->bus << 24);
				break;
			case 0x1E:
				md5->s = cpu->bus & 0x1F;
				break;
			case 0x1F:
				md5->mode = cpu->bus & 0x03;
				break;
		}
		cpu->output = FALSE;
	}
}

uint8_t GetCPUSpeed(CPU_t *cpu) {
	switch (cpu->timer_c->freq) {
			case MHZ_6:
				return 0x00;
			case MHZ_15:
				return 0x01;
			case MHZ_20:
				return 0x02;
			case MHZ_25:
				return 0x03;
		}
	return 0;
}

void port20_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = GetCPUSpeed(cpu);
		cpu->input = FALSE;
	} else if (cpu->output) {
		int val = cpu->timer_c->timer_version > 0 ? cpu->bus & 3 : cpu->bus & 1;
		switch (val) {
			case 1:
				cpu->timer_c->freq = MHZ_15;
				break;
			case 2:
				cpu->timer_c->freq = MHZ_20;
				break;
			case 3:
				cpu->timer_c->freq = MHZ_25;
				break;
			default:
				cpu->timer_c->freq = MHZ_6;
				break;
		}
		UpdateDelays(cpu, &cpu->pio.se_aux->delay);
		cpu->output = FALSE;
	}
}

void port21_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (unsigned char) (cpu->model_bits + ((cpu->mem_c->prot_mode) >> 4));
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->model_bits = cpu->bus & 0x3;
		cpu->mem_c->prot_mode = (RAM_PROT_MODE) ((cpu->bus & 0x30) >> 4);
	}
}

void port22_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = cpu->mem_c->flash_lower & 0xFF;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->mem_c->flash_lower = (cpu->mem_c->flash_lower & 0xFF00) | cpu->bus;
	}
}

void port23_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = cpu->mem_c->flash_upper & 0xFF;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->mem_c->flash_upper = (cpu->mem_c->flash_upper & 0xFF00) | cpu->bus;
	}
}

void port24_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->input = FALSE;
		cpu->bus = cpu->mem_c->port24;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->mem_c->port24 = cpu->bus;
		cpu->mem_c->flash_upper = (cpu->mem_c->flash_upper & 0xFF) | (cpu->bus & BIT(1) << 7);
		cpu->mem_c->flash_lower = (cpu->mem_c->flash_lower & 0xFF) | (cpu->bus & BIT(0) << 8);
	}
}

void port25_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (unsigned char)(cpu->mem_c->ram_lower / 0x400);
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->mem_c->ram_lower = cpu->bus * 0x400;
	}
}
void port26_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		cpu->bus = (unsigned char)((cpu->mem_c->ram_upper - 0x3FF) / 0x400);
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
		cpu->mem_c->ram_upper = cpu->bus * 0x400 + 0x3FF;
	}
}

void UpdateDelays(CPU_t *cpu, DELAY_t *delay) {
	// Update memory access delays
	int enable = delay->reg[GetCPUSpeed(cpu)];
	int select = delay->reg[0x2E - 0x29];
	cpu->mem_c->read_OP_flash_tstates = (enable & 1) && (select & 0x01);
	cpu->mem_c->read_NOP_flash_tstates = (enable & 1) && (select & 0x02);
	cpu->mem_c->write_flash_tstates = (enable & 1) && (select & 0x04);
	cpu->mem_c->read_OP_ram_tstates = (enable & 2) && (select & 0x10);
	cpu->mem_c->read_NOP_ram_tstates = (enable & 2) && (select & 0x20);
	cpu->mem_c->write_ram_tstates = (enable & 2) && (select & 0x40);
}

void delay_ports(CPU_t *cpu, device_t *dev) {
	DELAY_t *delay = (DELAY_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = delay->reg[(DEV_INDEX(dev) - 0x29)];
		cpu->input = FALSE;
	} else if (cpu->output) {
		delay->reg[(DEV_INDEX(dev) - 0x29)] = cpu->bus;
		cpu->output = FALSE;
		UpdateDelays(cpu, delay);
	}
}


void mod_timer(CPU_t *cpu, XTAL_t* xtal) {
	int a, b, c;
	a = ((xtal->timers[0].clock & 0xC0) >> 6) & 0x03;
	b = ((xtal->timers[1].clock & 0xC0) >> 6) & 0x03;
	c = ((xtal->timers[2].clock & 0xC0) >> 6) & 0x03;

	switch (a | b | c) {
		case 0:
			Modify_interrupt_device(cpu, 0x32, 0);
			break;
		case 1:
			//Modify_interrupt_device(cpu, 0x32, 8);
			Modify_interrupt_device(cpu, 0x32, 1);
			break;
		case 2:
		case 3:
			Modify_interrupt_device(cpu, 0x32, 1);
			break;
	}
}

void port_chunk_remap_83pse(CPU_t *cpu, device_t *dev) {
	int *count = (int *) dev->aux;
	
	if (cpu->input) {
		cpu->bus = (unsigned char)*count;
		cpu->input = FALSE;
	} else {
		*count = cpu->bus;
		cpu->output = FALSE;
	}
}


void port30_83pse(CPU_t *cpu, device_t *dev) {
	XTAL_t* xtal = (XTAL_t *) dev->aux;
	TIMER_t* timer = &xtal->timers[(DEV_INDEX(dev) - 0x30) / 3];
	
	if (cpu->input) {
		cpu->bus = timer->clock;
		cpu->input = FALSE;
	} else if (cpu->output) {
		timer->clock = cpu->bus;
		timer->active = FALSE;
		timer->generate	= FALSE;
		switch ((timer->clock & 0xC0) >> 6) {
			case 0x00: {
				timer->divsor = 0.0f;
				break;
			}
			case 0x01: {
				switch (timer->clock & 0x07) {
					case 0x00:
						timer->divsor = 3.0;
						break;
					case 0x01:
						timer->divsor = 32.0;
						break;
					case 0x02:
						timer->divsor = 327.000;
						break;
					case 0x03:
						timer->divsor = 3276.00;
						break;
					case 0x04:
						timer->divsor = 1.0;
						break;
					case 0x05:
						timer->divsor = 16.0;
						break;
					case 0x06:
						timer->divsor = 256.0;
						break;
					case 0x07:
						timer->divsor = 4096.0;
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
				for(i = 0; i < 6; i++) {
					if (timer->clock & mask) break;
					mask = mask>>1;
					timer->divsor /= 2.0f;
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
	TIMER_t* timer = &xtal->timers[(DEV_INDEX(dev) - 0x30) / 3];
	if (cpu->input) {
		cpu->bus  = timer->loop ? 1 : 0;
		cpu->bus += timer->interrupt ? 2 : 0;
		cpu->bus += timer->underflow ? 4 : 0;
		cpu->input = FALSE;
	} else if (cpu->output) {
		timer->loop			= cpu->bus & 0x01 ? 1 : 0;
		timer->interrupt	= cpu->bus & 0x02 ? 1 : 0;
		timer->underflow	= FALSE;
		timer->generate		= FALSE;
		cpu->output = FALSE;
	}
}



void handlextal(CPU_t *cpu,XTAL_t* xtal) {
	TIMER_t* timer = &xtal->timers[0];
	
	// overall xtal timer ticking
	xtal->ticks = (unsigned long long)(tc_elapsed(cpu->timer_c) * 32768.0);
	xtal->lastTime = tc_elapsed(cpu->timer_c);
	
	for (int i = 0; i < NumElm(xtal->timers); i++)
	{
		timer = &xtal->timers[i];
		if (timer->active)
		{
			switch(((timer->clock & 0xC0) >> 6) & 0x03) {
				case 0:
					break;
				case 1:
					if ((timer->lastTicks + timer->divsor) < xtal->ticks ) {
						timer->lastTicks += timer->divsor;
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
					while(timer->lastTstates + ((unsigned long long) timer->divsor) < (unsigned long long) tc_tstates(cpu->timer_c) ) {
						
						timer->lastTstates += (unsigned long long) timer->divsor;
						timer->count--;
						if (!timer->count) {
							if (!timer->underflow) {
								timer->count = timer->max;
								if (!timer->loop)
									timer->active = FALSE;
							}
							if (timer->interrupt) timer->generate = TRUE;
							timer->underflow = TRUE;
						}
					}
					break;
			}
		}
						
		if (timer->generate && !cpu->halt)
			cpu->interrupt = TRUE;
	}
}
		
void port32_83pse(CPU_t *cpu, device_t *dev) {
	XTAL_t* xtal = (XTAL_t *) dev->aux;
	TIMER_t* timer = &xtal->timers[(DEV_INDEX(dev)-0x30)/3];
	
	handlextal(cpu, xtal);	
		
	if (cpu->input) {
		cpu->bus = timer->count;
		cpu->input = FALSE;

	} else if (cpu->output) {
		timer->count = cpu->bus;
		timer->max = cpu->bus;
		if (timer->clock & 0xC0) timer->active = TRUE;
		timer->lastTstates = tc_tstates(cpu->timer_c);
		timer->lastTicks = (double) xtal->ticks;
		mod_timer(cpu, xtal);
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
			clock->base = clock->base+((unsigned long) (tc_elapsed(cpu->timer_c)-clock->lasttime));
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
		if (clock->enable & 0x01) {
			time = clock->base + ((unsigned long)(tc_elapsed(cpu->timer_c) - clock->lasttime));
		} else {
			time = clock->base;
		}

		cpu->bus = (time >> ((DEV_INDEX(dev) - 0x45) * 8)) & 0xFF;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

#define USB_LINE_INTERRUPT_MASK BIT(2);
#define USB_PROTOCOL_INTERRUPT_MASK BIT(4);

void GenerateUSBEvent(CPU_t *cpu, USB_t *usb, int bit, BOOL lowToHigh) {
	if (lowToHigh) {
		usb->USBEvents |= BIT(bit);
		usb->USBEvents &= ~BIT(bit - 1);
	} else {
		usb->USBEvents |= BIT(bit);
		usb->USBEvents &= ~BIT(bit + 1);
	}
	usb->LineInterrupt = TRUE;
	//is this all i need?
	cpu->interrupt = TRUE;
}

void port3A_83pse(CPU_t *cpu, device_t *dev) {
	if (cpu->input) {
		// if bit 7 is reset, calc thinks its charging
		if (cpu->pio.model == TI_84PCSE) {
			cpu->bus = BIT(7) | BIT(5);
		} else {
			cpu->bus = BIT(2);
		}

		cpu->input = FALSE;
	} else if (cpu->output) {
		
		cpu->output = FALSE;
	}
}

void port4A_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = usb->Port4A;
		if ((usb->Port54 & BIT(2)) && (usb->Port54 & BIT(6)) && (usb->Port4C & BIT(3)) && (usb->USBLineState & VBUS_HIGH_MASK))
			cpu->bus += BIT(0);
		if (!((usb->Port54 & BIT(2)) && (usb->Port54 & BIT(6)) && (usb->Port4C & BIT(3)) && (usb->USBLineState & VBUS_HIGH_MASK)))
			cpu->bus += BIT(2);
		cpu->input = FALSE;
	} else if (cpu->output) {
		usb->Port4A = cpu->bus & (BIT(3) | BIT(4) | BIT(5));
		if (cpu->bus & BIT(3)) {
			if (!(usb->USBLineState & BIT(3))) {
				GenerateUSBEvent(cpu, usb, 3, TRUE);
			}
			usb->USBLineState |= VBUS_HIGH_MASK;
		}
		cpu->output = FALSE;
	}
}

void port4C_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = 0x2 | usb->Port4C;
		if (usb->Port54 & BIT(2))
			cpu->bus |= BIT(4);
		if (!(usb->Port54 & BIT(6)))
			cpu->bus |= BIT(5);
		if (usb->Port54 & BIT(7))
			cpu->bus |= BIT(6);
		cpu->input = FALSE;
	} else if (cpu->output) {
		usb->Port4C = cpu->bus & BIT(3);
		cpu->output = FALSE;
	}
}

void port4D_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = (unsigned char)usb->USBLineState;
		// not too worried about this next stuff, no good reading D+/-
		if (usb->Port54 & BIT(2) && usb->Port54 & BIT(6) && usb->Port4C & BIT(3) && usb->USBLineState & VBUS_HIGH_MASK)
			cpu->bus |= BIT(1) & ~BIT(0);
		else
			cpu->bus |= BIT(0) & ~BIT(1);

		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

void port54_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = usb->Port54 & (BIT(0) | BIT(1) | BIT(2) | BIT(6) | BIT(7));
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
		usb->Port54 = cpu->bus & (BIT(0) | BIT(1) | BIT(2) | BIT(6) | BIT(7));
		usb->USBPowered = !(cpu->bus & (BIT(1)));
	}
}

void port55_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux; 
	if (cpu->input) {
		// default value. unknown event in bit 0, bits 1 and 3 are "normally set"
		cpu->bus = BIT(0) | BIT(1) | BIT(3);
		// if the bit is cleared, the interrupt was triggered
		if (!usb->LineInterrupt) {
			cpu->bus += USB_LINE_INTERRUPT_MASK;
		}

		if (!usb->ProtocolInterrupt) {
			cpu->bus += USB_PROTOCOL_INTERRUPT_MASK;
		}

		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

void port56_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = (unsigned char)usb->USBEvents;
		cpu->input = FALSE;
	} else if (cpu->output) {
		cpu->output = FALSE;
	}
}

void port57_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = (unsigned char)usb->USBEventMask;
		cpu->input = FALSE;
	} else if (cpu->output) {
		usb->USBEventMask = cpu->bus;
		cpu->output = FALSE;
	}
}

void port5B_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = 0x00;
		if (usb->ProtocolInterruptEnabled) {
			cpu->bus += BIT(0);
		}

		cpu->input = FALSE;
	} else if (cpu->output) {
		usb->ProtocolInterruptEnabled = cpu->bus & BIT(0);
		cpu->output = FALSE;
	}
}

void port80_83pse(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux;
	if (cpu->input) {
		cpu->bus = (unsigned char)(usb->DevAddress & ~BIT(7));
		cpu->input = FALSE;
	} else if (cpu->output) {
		usb->DevAddress = cpu->bus & ~BIT(7);
		cpu->output = FALSE;
	}
}


//void port_83pse(CPU_t *cpu, device_t *dev) {
//	USB_t *usb = (USB_t *) dev->aux;
//	if (cpu->input) {
//		cpu->input = FALSE;
//	} else if (cpu->output) {
//		cpu->output = FALSE;
//	}
//}
	
void fake_usb(CPU_t *cpu, device_t *dev) {
	USB_t *usb = (USB_t *) dev->aux; 
	if (cpu->input) {
		switch(DEV_INDEX(dev)) {
/* double checked TILEM for proper values, thanks Ben, Dan_E*/
			case 0x4C:
				cpu->bus =  0x22;
				break;
			case 0x4D:
				cpu->bus = (unsigned char)usb->USBLineState;
				break;
			case 0x56:
				cpu->bus =  0x00;
				break;
			case 0x57:
				cpu->bus = (unsigned char)usb->USBEvents;
				break;
			case 0x80:
				cpu->bus = usb->DevAddress & 0x7F;
				break;
		}	
		cpu->input = FALSE;
	} else if (cpu->output) {
		switch (DEV_INDEX(dev)) {
			case 0x4D:
				usb->USBLineState = cpu->bus;
				break;
			case 0x56:
				usb->USBEvents = cpu->bus;
				break;
			case 0x80:
				usb->DevAddress = cpu->bus & 0x7F;
				break;
		}
		cpu->output = FALSE;
	}
}

/*----------------------------------------------*/
/*												*/
/*				Initialize						*/
/*												*/
/*----------------------------------------------*/

STDINT_t* INT83PSE_init(CPU_t* cpu, int model) {
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
	stdint->timermax2 = stdint->freq[3] / 2.0f;
	stdint->lastchk2 = tc_elapsed(cpu->timer_c) + stdint->freq[3] / 4.0f;
	
	stdint->on_backup = 0;
	stdint->on_latch = FALSE;
	return stdint;
}

link_t* link83pse_init() {
	link_t * link = (link_t *) malloc(sizeof(link_t)); 
	if (!link) {
		printf("Couldn't allocate memory for link\n");
		exit(1);
	}
	link->host		= 0;			//neither lines set
	link->client	= &link->host;	//nothing plugged in.
	return link;
}

SE_AUX_t* SE_AUX_init() {
	SE_AUX_t* se_aux = (SE_AUX_t *) malloc(sizeof(SE_AUX_t));
	if (!se_aux) {
		printf("Couldn't allocate memory for SE structs\n");
		exit(1);
	}
	memset(se_aux, 0, sizeof(SE_AUX_t));
	se_aux->xtal.timers[0].divsor = 1;
	se_aux->xtal.timers[1].divsor = 1;
	se_aux->xtal.timers[2].divsor = 1;
	se_aux->linka.link_enable = 0x80;
	se_aux->usb.USBEvents = 0x50;
	se_aux->usb.USBLineState = 0xA5;
	se_aux->usb.LineInterrupt = FALSE;
	se_aux->usb.ProtocolInterrupt = FALSE;
	return se_aux;
}

int device_init_83pse(CPU_t *cpu, int model) {
	ClearDevices(cpu);

/* link */
	link_t * link = link83pse_init();
	cpu->pio.devices[0x00].active = TRUE;
	cpu->pio.devices[0x00].aux = link;
	cpu->pio.devices[0x00].code = (devp) port0_83pse;

/* Key pad */
	keypad_t *keyp = keypad_init();
	cpu->pio.devices[0x01].active = TRUE;
	cpu->pio.devices[0x01].aux = keyp;
	cpu->pio.devices[0x01].code = (devp) keypad;

/* status */
	cpu->pio.devices[0x02].active = TRUE;
	cpu->pio.devices[0x02].code = (devp) port2_83pse;

/* standard interrupts */
	STDINT_t* stdint = INT83PSE_init(cpu, model);
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
	
	
	SE_AUX_t *se_aux = SE_AUX_init();
	
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

	cpu->pio.devices[0x0E].active = TRUE;
	cpu->pio.devices[0x0E].aux = &cpu->mem_c->port0E;
	cpu->pio.devices[0x0E].code = (devp) port0E_83pse;

	cpu->pio.devices[0x0F].active = TRUE;
	cpu->pio.devices[0x0F].aux = &cpu->mem_c->port0F;
	cpu->pio.devices[0x0F].code = (devp) port0F_83pse;
	
/* LCD */
	LCDBase_t *lcd;
	if (model == TI_84PCSE) {
		ColorLCD_t *colorlcd = ColorLCD_init(cpu, TI_84PCSE);
		lcd = (LCDBase_t *)colorlcd;
	} else {
		LCD_t *greylcd = LCD_init(cpu, TI_83PSE);
		lcd = (LCDBase_t *)greylcd;
	}

	cpu->pio.devices[0x10].active = TRUE;
	cpu->pio.devices[0x10].aux = lcd;
	cpu->pio.devices[0x10].code = (devp)lcd->command;

	cpu->pio.devices[0x11].active = TRUE;
	cpu->pio.devices[0x11].aux = lcd;
	cpu->pio.devices[0x11].code = (devp)lcd->data;

/* Flash locking */
	cpu->pio.devices[0x14].active = TRUE;
	cpu->pio.devices[0x14].code = (devp) port14_83pse;
	//I know this seems silly but the way we check for protected ports is done by flash
	//is unlocked and this is how you unlock flash so...
	//cpu->pio.devices[0x14].protected_port = TRUE;
	cpu->pio.devices[0x15].active = TRUE;
	cpu->pio.devices[0x15].code = (devp) port15_83pse;

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
	
	
/* page locking and hardware */
	cpu->pio.devices[0x21].active = TRUE;
	cpu->pio.devices[0x21].code = (devp) port21_83pse;
	cpu->pio.devices[0x21].protected_port = TRUE;

	cpu->pio.devices[0x22].active = TRUE;
	cpu->pio.devices[0x22].code = (devp) port22_83pse;
	cpu->pio.devices[0x22].protected_port = TRUE;

	cpu->pio.devices[0x23].active = TRUE;
	cpu->pio.devices[0x23].code = (devp) port23_83pse;
	cpu->pio.devices[0x23].protected_port = TRUE;

	cpu->pio.devices[0x24].active = TRUE;
	cpu->pio.devices[0x24].aux = &cpu->mem_c->port24;
	cpu->pio.devices[0x24].code = (devp) port24_83pse;
	cpu->pio.devices[0x24].protected_port = TRUE;

	cpu->pio.devices[0x25].active = TRUE;
	cpu->pio.devices[0x25].code = (devp) port25_83pse;
	cpu->pio.devices[0x25].protected_port = TRUE;

	cpu->pio.devices[0x26].active = TRUE;
	cpu->pio.devices[0x26].code = (devp) port26_83pse;
	cpu->pio.devices[0x26].protected_port = TRUE;


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


	cpu->pio.devices[0x3A].active = TRUE;
	cpu->pio.devices[0x3A].code = (devp)&port3A_83pse;

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
	cpu->pio.devices[0x4A].active = TRUE;
	cpu->pio.devices[0x4A].aux = &se_aux->usb;
	cpu->pio.devices[0x4A].code = (devp) &port4A_83pse;	

	cpu->pio.devices[0x4C].active = TRUE;
	cpu->pio.devices[0x4C].aux = &se_aux->usb;
	cpu->pio.devices[0x4C].code = (devp) &port4C_83pse;	

	cpu->pio.devices[0x4D].active = TRUE;
	cpu->pio.devices[0x4D].aux = &se_aux->usb;
	cpu->pio.devices[0x4D].code = (devp) &port4D_83pse;

	cpu->pio.devices[0x55].active = TRUE;
	cpu->pio.devices[0x55].aux = &se_aux->usb;
	cpu->pio.devices[0x55].code = (devp) &port54_83pse;

	cpu->pio.devices[0x55].active = TRUE;
	cpu->pio.devices[0x55].aux = &se_aux->usb;
	cpu->pio.devices[0x55].code = (devp) &port55_83pse;

	cpu->pio.devices[0x56].active = TRUE;
	cpu->pio.devices[0x56].aux = &se_aux->usb;
	cpu->pio.devices[0x56].code = (devp) &port56_83pse;

	cpu->pio.devices[0x57].active = TRUE;
	cpu->pio.devices[0x57].aux = &se_aux->usb;
	cpu->pio.devices[0x57].code = (devp) &port57_83pse;

	cpu->pio.devices[0x5B].active = TRUE;
	cpu->pio.devices[0x5B].aux = &se_aux->usb;
	cpu->pio.devices[0x5B].code = (devp) &port5B_83pse;

	cpu->pio.devices[0x80].active = TRUE;
	cpu->pio.devices[0x80].aux = &se_aux->usb;
	cpu->pio.devices[0x80].code = (devp) &port80_83pse;
	
	cpu->pio.lcd		= (LCDBase_t *)lcd;
	cpu->pio.keypad		= keyp;
	cpu->pio.link		= link;
	cpu->pio.stdint		= stdint;
	cpu->pio.se_aux		= se_aux;
	cpu->pio.model		= model;
	
	
	/*Append_interrupt_device(cpu, 0x00, 1);
	Append_interrupt_device(cpu, 0x03, 8);
	Append_interrupt_device(cpu, 0x11, 128);
	Append_interrupt_device(cpu, 0x09, 3);
	Append_interrupt_device(cpu, 0x32, 8);*/
	
	// Turning off the instruction skip stuff temporarily
	// To attempt to diagnose some timing issues
	//BuckeyeDude: 7/27/11 this destroys cpu, need to find the correct
	//way to do these timings
	Append_interrupt_device(cpu, 0x00, 1);
	Append_interrupt_device(cpu, 0x03, 1);
	Append_interrupt_device(cpu, 0x11, 128);
	Append_interrupt_device(cpu, 0x09, 1);
	Append_interrupt_device(cpu, 0x32, 1);
	return 0;
}


int memory_init_83pse(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));

	/* Set Number of Pages here */
	mc->flash_pages = 128;
	mc->ram_pages = 8;

	mc->flash_version = 2;
	mc->flash_upper = 0x60;
	mc->flash_lower = 0x10;
	
	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_lower = 0x00 * 0x400;
	mc->ram_upper = 0x00 * 0x400 + 0x3FF;
	
	if (!mc->flash || !mc->ram ) {
		_tprintf_s(_T("Couldn't allocate memory in memory_init_83pse\n"));
		return 1;
	}
	memset(mc->flash, 0xFF, mc->flash_size);

	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;
	mc->prot_mode				= MODE0;

	/* Organize bank states here */
	
	/*	Address								page	write?	ram?	no exec?	*/
	bank_state_t banks[5] = {
		{mc->flash +  0x07f * PAGE_SIZE, 	0x7f, 	FALSE,	FALSE, 	FALSE},
		{mc->flash,							0,		FALSE, 	FALSE, 	FALSE},
		{mc->flash,							0,	 	FALSE, 	FALSE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	FALSE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->normal_banks, banks, sizeof(banks));
	update_bootmap_pages(mc);
	mc->banks					= mc->normal_banks;
	return 0;
}

int memory_init_84p(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));

	/* Set Number of Pages here */
	mc->flash_pages = 64;
	mc->ram_pages = 8;

	mc->flash_version = 3;
	mc->flash_upper = 0x30;
	mc->flash_lower = 0x10;

	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	memset(mc->flash, 0xFF, mc->flash_size);

	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);

	if (!mc->flash || !mc->ram ) {
		_tprintf_s(_T("Couldn't allocate memory in memory_init_84p\n"));
		return 1;
	}

	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;
	mc->prot_mode				= MODE0;

	/* Organize bank states here */

	/*	Address								page	write?	ram?	no exec?	*/
	bank_state_t banks[5] = {
		{mc->flash + 0x3f * PAGE_SIZE,	0x3f,	FALSE,	FALSE, 	FALSE},
		{mc->flash,						0,	 	FALSE, 	FALSE, 	FALSE},
		{mc->flash,						0, 		FALSE, 	FALSE, 	FALSE},
		{mc->ram,						0,		FALSE,	TRUE,	FALSE},
		{NULL,							0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->normal_banks, banks, sizeof(banks));
	update_bootmap_pages(mc);
	mc->banks					= mc->normal_banks;
	return 0;
}

int memory_init_84pcse(memc *mc) {
	memset(mc, 0, sizeof(memory_context_t));

	/* Set Number of Pages here */
	mc->flash_pages = 256;
	mc->ram_pages = 8;

	mc->flash_version = 2;
	mc->flash_upper = 0xC0;
	mc->flash_lower = 0x10;
	
	mc->flash_size = mc->flash_pages * PAGE_SIZE;
	mc->flash = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	mc->flash_break = (unsigned char *) calloc(mc->flash_pages, PAGE_SIZE);
	
	mc->ram_size = mc->ram_pages * PAGE_SIZE;
	mc->ram = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_break = (unsigned char *) calloc(mc->ram_pages, PAGE_SIZE);
	mc->ram_lower = 0x00 * 0x400;
	mc->ram_upper = 0x00 * 0x400 + 0x3FF;
	
	if (!mc->flash || !mc->ram ) {
		_tprintf_s(_T("Couldn't allocate memory in memory_init_84pcse\n"));
		return 1;
	}
	memset(mc->flash, 0xFF, mc->flash_size);

	mc->boot_mapped				= FALSE;
	mc->flash_locked			= TRUE;
	mc->prot_mode				= MODE0;

	/* Organize bank states here */
	
	/*	Address								page	write?	ram?	no exec?	*/
	bank_state_t banks[] = {
		{mc->flash +  0x0FF * PAGE_SIZE, 	0xFF, 	FALSE,	FALSE, 	FALSE},
		{mc->flash,							0,		FALSE, 	FALSE, 	FALSE},
		{mc->flash,							0,	 	FALSE, 	FALSE, 	FALSE},
		{mc->ram,							0,		FALSE,	TRUE,	FALSE},
		{NULL,								0,		FALSE,	FALSE,	FALSE}
	};

	memcpy(mc->normal_banks, banks, sizeof(banks));
	update_bootmap_pages(mc);
	mc->banks					= mc->normal_banks;
	return 0;
}
