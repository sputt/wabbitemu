#include "stdafx.h"

#include "core.h"
#include "calc.h"
#include "device.h"
#include "alu.h"
#include "indexcb.h"
#include "control.h"
#include "83phw.h"
#include "83psehw.h"
#include "optable.h"
#ifdef WITH_REVERSE
#include "alu_reverse.h"
#include "indexcb_reverse.h"
#include "control_reverse.h"
#include "optable_reverse.h"
#include "reverse_info.h"
#include "reverse_info_table.h"
#endif

unsigned char mem_read(memc *mem, unsigned short addr) {
	if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 3) && (addr >= (0x10000 - 64*mem->port27_remap_count)) && addr >= 0xFB64) {
		return mem->ram[0*PAGE_SIZE + mc_base(addr)];
	}
	if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 2) && (mc_base(addr) < 64*mem->port28_remap_count)) {
		return mem->ram[1*PAGE_SIZE + mc_base(addr)];
	}
	//handle missing ram pages
	if (mem->ram_version == 2) {
		if (mem->banks[mc_bank(addr)].ram == TRUE && mem->banks[mc_bank(addr)].page > 2) {
			return mem->ram[2 * PAGE_SIZE + mc_base(addr)];
		}
	}
	return *(mem->banks[mc_bank(addr)].addr + mc_base(addr));
}

// Fetches a byte using a "wide" unique address
uint8_t wmem_read(memc *mem, waddr_t waddr) {
	if (waddr.is_ram) {
		return mem->ram[waddr.page * PAGE_SIZE + (waddr.addr % PAGE_SIZE)];
	} else {
		return mem->flash[waddr.page * PAGE_SIZE + (waddr.addr % PAGE_SIZE)];
	}
}
uint16_t wmem_read16(memc *mem, waddr_t waddr) {
	if (waddr.is_ram) {
		return mem->ram[waddr.page * PAGE_SIZE + (waddr.addr % PAGE_SIZE)] +
			(mem->ram[waddr.page * PAGE_SIZE + ((waddr.addr + 1) % PAGE_SIZE)] << 8);
	} else {
		return mem->flash[waddr.page * PAGE_SIZE + (waddr.addr % PAGE_SIZE)] +
			(mem->flash[waddr.page * PAGE_SIZE + ((waddr.addr + 1) % PAGE_SIZE)] << 8);
	}
}
uint8_t wmem_write(memc *mem, waddr_t waddr, uint8_t data) {
	if (waddr.is_ram) {
		return mem->ram[waddr.page * PAGE_SIZE + waddr.addr] = data;
	} else {
		return mem->flash[waddr.page * PAGE_SIZE + waddr.addr] = data;
	}
}

/*
 * Convert a Z80 address to a waddr
 */
waddr_t addr_to_waddr(memc *mem_c, uint16_t addr) {
	waddr_t waddr;
	bank_t *bank = &mem_c->banks[mc_bank(addr)];

	waddr.addr = addr;
	waddr.page = bank->page;
	waddr.is_ram = bank->ram;

	return waddr;
}

BOOL check_break(memc *mem, waddr_t waddr) {
	if (!(mem->breaks[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] & NORMAL_BREAK))
		return FALSE;
#ifdef WINVER
	if (mem->breakpoint_manager_callback)
		return mem->breakpoint_manager_callback(mem, NORMAL_BREAK, waddr);
#endif
	return TRUE;
}
BOOL check_mem_write_break(memc *mem, waddr_t waddr) {
	if (!(mem->breaks[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] & MEM_WRITE_BREAK))
		return FALSE;
#ifdef WINVER
	if (mem->breakpoint_manager_callback)
		return mem->breakpoint_manager_callback(mem, MEM_WRITE_BREAK, waddr);
#endif
	return TRUE;
}
BOOL check_mem_read_break(memc *mem, waddr_t waddr) {
	if (!(mem->breaks[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] & MEM_READ_BREAK))
		return FALSE;
#ifdef WINVER
	if (mem->breakpoint_manager_callback)
		return mem->breakpoint_manager_callback(mem, MEM_READ_BREAK, waddr);
#endif
	return TRUE;
}

extern void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);
extern void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);

void set_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] |= NORMAL_BREAK;
#ifdef WINVER
	add_breakpoint(mem, NORMAL_BREAK, waddr);
#endif
}
void set_mem_write_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] |= MEM_WRITE_BREAK;
#ifdef WINVER
	add_breakpoint(mem, MEM_WRITE_BREAK, waddr);
#endif
}
void set_mem_read_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] |= MEM_READ_BREAK;
#ifdef WINVER
	add_breakpoint(mem, MEM_READ_BREAK, waddr);
#endif
}

void clear_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] &= CLEAR_NORMAL_BREAK;
#ifdef WINVER
	rem_breakpoint(mem, NORMAL_BREAK, waddr);
#endif
}
void clear_mem_write_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] &= CLEAR_MEM_WRITE_BREAK;
	#ifdef WINVER
	rem_breakpoint(mem, MEM_WRITE_BREAK, waddr);
#endif
}
void clear_mem_read_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] &= CLEAR_MEM_READ_BREAK;
#ifdef WINVER
	rem_breakpoint(mem, MEM_READ_BREAK, waddr);
#endif
}

unsigned char mem_write(memc *mem, unsigned short addr, char data) {
	if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 3) && (addr >= (0x10000 - 64*mem->port27_remap_count)) && addr >= 0xFB64) {
		return mem->ram[0*PAGE_SIZE + mc_base(addr)] = data;
	}
	if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 2) && (mc_base(addr) < 64*mem->port28_remap_count)) {
		return mem->ram[1*PAGE_SIZE + mc_base(addr)] = data;
	}
	//handle missing ram pages
	if (mem->ram_version == 2) {
		if (mem->banks[mc_bank(addr)].ram && mem->banks[mc_bank(addr)].page > 2) {
			return mem->ram[2*PAGE_SIZE + mc_base(addr)] = data;
		}
	}
	return  *(mem->banks[mc_bank(addr)].addr + mc_base(addr)) = data;
}

inline unsigned short read2bytes(memc *mem, unsigned short addr) {
	return (mem_read(mem, addr) + (mem_read(mem, addr + 1) << 8));
}

unsigned short mem_read16(memc *mem, unsigned short addr) {
	return (mem_read(mem, addr) + (mem_read(mem, addr + 1) << 8));
}

/* Initialize a timer context */
int tc_init(timerc *tc, int timer_freq) {
	tc->tstates = 0;
	tc->elapsed = 0;
	tc->freq = timer_freq;
	return 0;
}

int CPU_init(CPU_t *cpu, memc *mem_c, timerc *timer_c) {
	memset(cpu, 0, sizeof(CPU_t));
	cpu->sp = 0xF000;
	cpu->mem_c = mem_c;
	cpu->timer_c = timer_c;
	mem_c->port27_remap_count = 0;
	mem_c->port28_remap_count = 0;
	cpu->exe_violation_callback = mem_debug_callback;
#ifdef WITH_REVERSE
	cpu->prev_instruction = cpu->prev_instruction_list;
#endif
	return 0;
}

static void handle_pio(CPU_t *cpu) {
	for (int i = cpu->pio.num_interrupt; i >= 0 ; i--) {
		interrupt_t *intVal = &cpu->pio.interrupt[i];
		if (intVal->skip_factor) {
			if (!intVal->skip_count) {
				device_control(cpu, intVal->interrupt_val);
			}
			//cpu->pio.skip_count[i] = (skip_count + 1) % skip_factor;
			intVal->skip_count++;
			if (intVal->skip_count == intVal->skip_factor) {
				intVal->skip_count = 0;
			}
		}
	}
}

BOOL is_priveleged_page(CPU_t *cpu) {
	//privileged pages are as follows
	// TI 83+		= 1C, 1D, 1F
	// TI 83+SE		= 7C, 7D, 7F
	// TI-84+		= 2F, 3C, 3D, 3F
	// TI 84+SE		= 6F, 7C, 7D, 7F
	bank_state_t *bank = &cpu->mem_c->banks[mc_bank(cpu->pc)];
	if (bank->ram) {
		return FALSE;
	}
	int maxPages = cpu->mem_c->flash_pages;
	int page = bank->page;
	//this includes page XE (which should be ok because its the cert). If its a problem i can fix later
	return ((page >= maxPages - 4 && page != maxPages - 2) || (((cpu->pio.model >= TI_84P) && page == maxPages - 0x11)));
}

static BOOL is_allowed_exec(CPU_t *cpu) {
	bank_state_t  *bank = &cpu->mem_c->banks[mc_bank(cpu->pc)];
	if (cpu->pio.model <= TI_83P) {
		int protected_val;
		if (bank->ram) {
			protected_val = cpu->mem_c->protected_page[3];
			if ((protected_val & 0x01) && bank->page == 0)
				return FALSE;
			if ((protected_val & 0x20) && bank->page == 1)
				return FALSE;
			return TRUE;
		} else if (bank->page < 0x08)
			return TRUE;
		else if (bank->page >= 0x1C)
			return TRUE;
		protected_val = cpu->mem_c->protected_page[(bank->page - 8) / 8];
		//yay for awesome looking code :D
		//basically this checks whether the bit corresponding to the page
		//is set indicating no exec is allowed
		return !(protected_val & (0x01 << ((bank->page - 8) % 8)));
	} else {
		memc *mem = cpu->mem_c;
		if (!bank->ram)	{		//if its flash and between page limits
			return bank->page <= mem->flash_lower || mem->flash_enabled ||
				(!mem->flash_disabled && bank->page > mem->flash_upper) ||
				is_priveleged_page(cpu);
		}
		if (bank->page & (2 >> (cpu->mem_c->prot_mode + 1)))
			return TRUE;		//we know were in ram so lets check if the page is allowed in the mem protected mode
								//execution is allowed on 2^(mode+1)
		//finally we check ports 25/26 to see if its ok to execute on this page
		int global_addr = bank->page * PAGE_SIZE + (cpu->pc & 0x3FFF);
		if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(cpu->pc) == 3) && (cpu->pc >= (0x10000 - 64*mem->port27_remap_count)) && cpu->pc >= 0xFB64)
			global_addr = 0*PAGE_SIZE + mc_base(cpu->pc);
		else if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(cpu->pc) == 2) && (mc_base(cpu->pc) < 64*mem->port28_remap_count))
			global_addr = 1*PAGE_SIZE + mc_base(cpu->pc);
		if (global_addr < cpu->mem_c->ram_lower || global_addr > cpu->mem_c->ram_upper)
			return FALSE;
		return TRUE;
	}
}

void change_page(CPU_t *cpu, int bank, char page, BOOL ram) {
	cpu->mem_c->normal_banks[bank].ram			= ram;
	if (ram) {
		cpu->mem_c->normal_banks[bank].page		= page;
		cpu->mem_c->normal_banks[bank].addr		= cpu->mem_c->ram + (page * PAGE_SIZE);
		cpu->mem_c->normal_banks[bank].read_only	= FALSE;
		cpu->mem_c->normal_banks[bank].no_exec		= FALSE;
	} else {
		cpu->mem_c->normal_banks[bank].page		= page;
		cpu->mem_c->normal_banks[bank].addr		= cpu->mem_c->flash + (page * PAGE_SIZE);
		cpu->mem_c->normal_banks[bank].read_only	= page == cpu->mem_c->flash_pages - 1;
		cpu->mem_c->normal_banks[bank].no_exec		= FALSE;
	}
	update_bootmap_pages(cpu->mem_c);
}

void update_bootmap_pages(memc *mem_c) {
	memcpy(mem_c->bootmap_banks, mem_c->normal_banks, sizeof(bank_state_t));

	mem_c->bootmap_banks[1].page		= mem_c->normal_banks[1].page & 0xFE;
	mem_c->bootmap_banks[1].addr		= (mem_c->normal_banks[1].ram ? mem_c->ram : mem_c->flash) + (mem_c->bootmap_banks[1].page * PAGE_SIZE);
	mem_c->bootmap_banks[1].read_only	= FALSE;
	mem_c->bootmap_banks[1].no_exec		= FALSE;
	mem_c->bootmap_banks[1].ram			= mem_c->normal_banks[1].ram;
		
	mem_c->bootmap_banks[2].page		= mem_c->normal_banks[1].page | (!mem_c->flash_version == 1);
	mem_c->bootmap_banks[2].addr		= (mem_c->normal_banks[1].ram ? mem_c->ram : mem_c->flash) + (mem_c->bootmap_banks[2].page * PAGE_SIZE);
	mem_c->bootmap_banks[2].read_only	= FALSE;
	mem_c->bootmap_banks[2].no_exec		= FALSE;
	mem_c->bootmap_banks[2].ram			= mem_c->normal_banks[1].ram;
	
	mem_c->bootmap_banks[3].page		= mem_c->normal_banks[2].page;
	mem_c->bootmap_banks[3].addr		= (mem_c->normal_banks[2].ram ? mem_c->ram : mem_c->flash) + (mem_c->bootmap_banks[3].page * PAGE_SIZE);
	mem_c->bootmap_banks[3].read_only	= FALSE;
	mem_c->bootmap_banks[3].no_exec		= FALSE;
	mem_c->bootmap_banks[3].ram			= mem_c->normal_banks[2].ram;
}

static int CPU_opcode_fetch(CPU_t *cpu) {
	int bank_num = mc_bank(cpu->pc);
	bank_state_t *bank = &cpu->mem_c->banks[bank_num];
	//the boot page is mapped to bank 0 to start
	//if code is run from an address of whatever page is mapped to port 6
	//then the page is changed to page 0. why? who the fuck knows
	if (!cpu->mem_c->hasChangedPage0 && !bank->ram && (bank_num == 1 || (cpu->mem_c->boot_mapped && bank_num == 2)))
	{
		change_page(cpu, 0, 0, FALSE);
		cpu->mem_c->hasChangedPage0 = TRUE;
	}
	if (!is_allowed_exec(cpu)) {
		if (break_on_exe_violation) {
			cpu->exe_violation_callback(cpu);
		} else {
			CPU_reset(cpu);
		}
	}
	if (!bank->ram) {									//I DON'T THINK THIS IS CORRECT
		endflash(cpu);									//However it shouldn't be a problem
	}													//assuming you know how to write to flash
	cpu->bus = mem_read(cpu->mem_c, cpu->pc);
	if (bank->ram) {
		SEtc_add(cpu->timer_c, cpu->mem_c->read_OP_ram_tstates);
	} else {
		SEtc_add(cpu->timer_c, cpu->mem_c->read_OP_flash_tstates);
	}
	cpu->pc++;
	cpu->r = (cpu->r & 0x80) + ((cpu->r + 1) & 0x7F);		//note: prefix opcodes inc the r reg to. so bit 7,h has 2 incs.
	return cpu->bus;
}

unsigned char CPU_mem_read(CPU_t *cpu, unsigned short addr) {
	if (check_mem_read_break(cpu->mem_c, addr_to_waddr(cpu->mem_c, addr))) {
		cpu->mem_c->mem_read_break_callback(cpu);
	}
	cpu->bus = mem_read(cpu->mem_c, addr);

	if (cpu->mem_c->banks[mc_bank(addr)].ram) {
		SEtc_add(cpu->timer_c, cpu->mem_c->read_NOP_ram_tstates);
	} else {
		/*if (cpu->mem_c->step > 4) cpu->bus = 0xFF; // Flash status read, apparently
		else cpu->mem_c->step = 0;*/				// calc84: says this is better
		if (cpu->mem_c->cmd == 0x90 && cpu->mem_c->step == 3) {
			if ((addr & 0x3FFF) == 0) {
				//1 indicates an AMD chip
				//manufacturer id
				cpu->bus = 1;
			} else if ((addr & 0x3FFF) == 2) {
			//B9 for new 512 K chips, 23 for old 512 K chips, C4 for 2 MB chips, and DA for 1 MB chips.
				switch (cpu->pio.model) {
					case TI_84P:
						cpu->bus = 0xDA;
						break;
					case TI_83PSE:
					case TI_84PSE:
						cpu->bus = 0xC4;
						break;
					default:
						if (cpu->cpu_version == 1) {
							cpu->bus = 0x23;
						} else {
							cpu->bus = 0xB9;
						}
						break;
				}
			} else if ((addr & 0x3FFF) == 4) {
				cpu->bus = 0;
			}
			cpu->mem_c->cmd = 0;
			cpu->mem_c->step = 0;
		}

		SEtc_add(cpu->timer_c, cpu->mem_c->read_NOP_flash_tstates);
	}

	return cpu->bus;
}

static void flashwrite(CPU_t *cpu, unsigned short addr, unsigned char data) {
	int bank = mc_bank(addr);
	switch(cpu->mem_c->step) {
		case 0:
			if (data == 0xF0) {
				endflash(cpu);
			} else if ((addr & 0x0FFF) == 0x0AAA) {
				if (data == 0xAA) {
					cpu->mem_c->step++;
				} else {
					endflash_break(cpu);
				}
			} else {
				endflash_break(cpu);
			}
			break;
		case 1:
			if ((addr & 0x0FFF) == 0x0555) {
				if (data == 0x55) {
					cpu->mem_c->step++;
				} else {
					endflash_break(cpu);
				}
			} else {
				endflash(cpu);
			}
			break;
		case 2:
			if ((addr & 0x0FFF) == 0x0AAA) {
				if (data == 0xA0) {
					cpu->mem_c->cmd = 0xA0;		//Program
					cpu->mem_c->step++;
				} else if (data == 0x80) {
					cpu->mem_c->cmd = 0x80;		//Erase
					cpu->mem_c->step++;
				} else if (data == 0x20 && cpu->mem_c->flash_version != 1) {
					cpu->mem_c->cmd = 0x20;		//Fast mode
					cpu->mem_c->step = 6;
				} else if (data == 0x90) {
					cpu->mem_c->cmd = 0x90;		//Auto select
					cpu->mem_c->step++;
				} else {
					endflash_break(cpu);
				}
			} else {
				endflash_break(cpu);
			}
			break;
		case 3: {
			int value = 0;
			if (cpu->mem_c->cmd == 0xA0) {
				value = *(cpu->mem_c->banks[bank].addr + mc_base(addr));
				(*(cpu->mem_c->banks[bank].addr + mc_base(addr))) &= data;  //AND LOGIC!!
				if ((~((~value) | (~data))) != value) {
					value = (~value) & 0x80 | 0x20;
				}
				endflash(cpu);
			}
			if ((addr & 0x0FFF) == 0x0AAA) {
				if (data == 0xAA) {
					cpu->mem_c->step++;
				}
			}
			if (data == 0xF0) { 
				if (value) {
					cpu->bus = value;
				}
				endflash(cpu);
			}
			break;
		}
		case 4:
			if ((addr & 0x0FFF) == 0x0555) {
				if (data == 0x55) {
					cpu->mem_c->step++;
				}
			}
			if (data == 0xF0) {
				endflash(cpu);
			}
			break;
		case 5:
			if ((addr & 0x0FFF) == 0x0AAA) {
				if (data == 0x10) {			//Erase entire chip...Im not sure if 
					int i;					//boot page is included, so I'll leave it off.
					//DrDnar 7/8/11: boot sector is included
					for(i = 0; i < cpu->mem_c->flash_size; i++) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				} 
			}
			if (data == 0xF0) {
				endflash(cpu);
			}
			if (data == 0x30) {		//erase sectors
				int i;
				int spage = (cpu->mem_c->banks[bank].page << 1) + ((addr >> 13) & 0x01);
				int pages = cpu->mem_c->flash_pages;
				int totalPages = pages * 2;

				if (spage < totalPages - 8) {
					int startaddr = (spage & 0x00F8) * 0x2000;
					int endaddr   = startaddr + 0x10000;
					for (i = startaddr; i < endaddr; i++) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				} else if (spage < totalPages - 4) {
					for (i = (pages - 4) * PAGE_SIZE; i < (pages - 2) * PAGE_SIZE; i++ ) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				} else if (spage < totalPages - 3) {
					for (i = (pages - 2) * PAGE_SIZE; i < (pages - 2) * PAGE_SIZE + PAGE_SIZE / 2; i++) {
						cpu->mem_c->flash[i] = 0xFF;

					}
				} else if (spage < totalPages - 2) {
					for (i = (pages - 2) * PAGE_SIZE + PAGE_SIZE / 2; i < (pages - 1) * PAGE_SIZE; i++) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				} else if (spage < totalPages) {
// I comment this off because this is the boot page
// it suppose to be write protected...
//BuckeyeDude 6/27/11: new info has been discovered boot code is writeable under certain conditions
					for (i = (pages - 1) * PAGE_SIZE; i < pages * PAGE_SIZE; i++) {
						cpu->mem_c->flash[i] = 0xFF;
					}
				}
			}
			endflash(cpu);
			break;
		case 6:
			if (data == 0x90) {
				cpu->mem_c->step = 7;	//check if exit fast mode
			} else if (data == 0xA0) {
				cpu->mem_c->step = 8;	//write byte in fast mode
			} else if (data == 0xF0) {
				endflash(cpu);
			}
			break;
		case 7:
			if (data == 0xF0) {
				endflash(cpu);
			} else {
				cpu->mem_c->step = 6;
			}
			break;
		case 8: {
			int value = *(cpu->mem_c->banks[bank].addr + mc_base(addr));
			(*(cpu->mem_c->banks[bank].addr + mc_base(addr))) &= data;  //AND LOGIC!!
			if ((~((~value) | (~data))) != value) {
				cpu->bus = (~value) & 0x80 | 0x20;
			}
			cpu->mem_c->step = 6;
			break;
		}
		default:
			endflash_break(cpu);
			break;
	}
}

unsigned char CPU_mem_write(CPU_t *cpu, unsigned short addr, unsigned char data) {
	if (check_mem_write_break(cpu->mem_c, addr_to_waddr(cpu->mem_c, addr))) {
		cpu->mem_c->mem_write_break_callback(cpu);
	}
	bank_state_t *bank = &cpu->mem_c->banks[mc_bank(addr)];

	if (bank->ram) {
		if (!bank->read_only) {
			mem_write(cpu->mem_c, addr, data);
		}
		SEtc_add(cpu->timer_c, cpu->mem_c->write_ram_tstates);
	} else {
		int page = bank->page;
		if (!cpu->mem_c->flash_locked && cpu->pio.model >= TI_73 && //1) {
			(((page != 0x3F && page != 0x2F) || (cpu->pio.se_aux->model_bits & 0x3)) &&
			((page != 0x7F && page != 0x6F) || (cpu->pio.se_aux->model_bits & 0x2) || !(cpu->pio.se_aux->model_bits & 0x1)))) {
			flashwrite(cpu, addr, data);
		} else if (break_on_invalid_flash) {
			cpu->mem_c->mem_write_break_callback(cpu);
		}

		SEtc_add(cpu->timer_c, cpu->mem_c->write_flash_tstates);
	}

	return cpu->bus = data;
}

#ifdef WITH_REVERSE
static int CPU_opcode_fetch_reverse(CPU_t *cpu) {
	cpu->r = cpu->prev_instruction->r;
	cpu->pc--;

	int bank_num = mc_bank(cpu->pc);
	bank_state_t  bank = cpu->mem_c->banks[bank_num];

	if (bank.ram) {
		SEtc_sub(cpu->timer_c, cpu->mem_c->read_OP_ram_tstates);
	} else {
		SEtc_sub(cpu->timer_c, cpu->mem_c->read_OP_flash_tstates);
	}

	cpu->bus = cpu->prev_instruction->bus;
	return cpu->bus;
}

static void CPU_opcode_run_reverse(CPU_t *cpu) {
	opcode_reverse[cpu->bus](cpu);
}

static void CPU_CB_opcode_run_reverse(CPU_t *cpu) {
	if (cpu->prefix) {
		CPU_opcode_fetch_reverse(cpu);
		char backup = cpu->bus;
		CPU_mem_read(cpu, --cpu->pc);
		char offset = cpu->bus;
		cpu->bus = backup;
		ICB_opcode_reverse[cpu->bus](cpu, offset);
	} else {
		CPU_opcode_fetch_reverse(cpu);
		CBtab_reverse[cpu->bus](cpu);
	}
}

static void CPU_ED_opcode_run_reverse(CPU_t *cpu) {
	CPU_opcode_fetch_reverse(cpu);
	EDtab_reverse[cpu->bus](cpu);
}


int CPU_step_reverse(CPU_t* cpu) {
	//if (cpu->interrupt && !cpu->ei_block) handle_interrupt(cpu);

	//handle_pio(cpu);
	if (--cpu->reverse_instr < 0 && cpu->reverse_wrap) {
		cpu->reverse_instr = ARRAYSIZE(cpu->prev_instruction_list) - 1;
	}
	cpu->prev_instruction = &cpu->prev_instruction_list[cpu->reverse_instr];
	if (cpu->halt == FALSE) {
		if (cpu->bus == 0xDD || cpu->bus == 0xFD) {
			cpu->prefix = cpu->bus;
			CPU_opcode_fetch_reverse(cpu);
			CPU_opcode_run_reverse(cpu);
			cpu->prefix = 0;
		} else {
			CPU_opcode_run_reverse(cpu);
			cpu->f = cpu->prev_instruction->flag;
			CPU_opcode_fetch_reverse(cpu);
		}
	} else {
		/* If the CPU is in halt */
		tc_sub(cpu->timer_c, 4 * HALT_SCALE);
		cpu->r = cpu->prev_instruction->r;
		cpu->reverse_instr--;
		if (cpu->reverse_instr < 0) {
			cpu->reverse_instr = ARRAYSIZE(cpu->prev_instruction_list) - 1;
		}
	}

	//cpu->interrupt = 0;
	//cpu->ei_block = FALSE;
	return 0;
}

static void CPU_CB_opcode_run_reverse_info(CPU_t *cpu) {
	if (cpu->prefix) {
		char offset = mem_read(cpu->mem_c, cpu->pc);
		char temp = mem_read(cpu->mem_c, cpu->pc + 1);
		if (ICB_opcode_reverse_info[temp]) {
			ICB_opcode_reverse_info[temp](cpu, offset);
		}
	} else {
		char temp = mem_read(cpu->mem_c, cpu->pc);
		if (CBtab_reverse_info[temp]) {
			CBtab_reverse_info[temp](cpu);
		}
	}
}

static void CPU_ED_opcode_run_reverse_info(CPU_t *cpu) {
	if (EDtab_reverse_info[cpu->bus]) {
		EDtab_reverse_info[cpu->bus](cpu);
	}
}

void CPU_add_prev_instr(CPU_t *cpu) {
	cpu->prev_instruction->flag = cpu->f;
	cpu->prev_instruction->bus = cpu->bus;
	cpu->prev_instruction->r = cpu->r;
	if (++cpu->reverse_instr >= ARRAYSIZE(cpu->prev_instruction_list)) {
		cpu->reverse_instr = 0;
		cpu->reverse_wrap = TRUE;
	}
	cpu->prev_instruction = &cpu->prev_instruction_list[cpu->reverse_instr];
}
#endif

static void CPU_opcode_run(CPU_t *cpu) {
#ifdef WITH_REVERSE
	if (opcode_reverse_info[cpu->bus]) {
		opcode_reverse_info[cpu->bus](cpu);
	}
#endif
	opcode[cpu->bus](cpu);
}

static void CPU_CB_opcode_run(CPU_t *cpu) {
	if (cpu->prefix) {
		CPU_mem_read(cpu, cpu->pc++);				//read the offset, NOT INST
		char offset = cpu->bus;
		CPU_opcode_fetch(cpu);						//CB opcode, this is an INST
		cpu->r = ((cpu->r - 1) & 0x7f) + (cpu->r & 0x80);
		ICB_opcode[cpu->bus](cpu,offset);
	} else {
		CPU_opcode_fetch(cpu);
		CBtab[cpu->bus](cpu);
	}
}

static void CPU_ED_opcode_run(CPU_t *cpu) {
	CPU_opcode_fetch(cpu);
	EDtab[cpu->bus](cpu);
}

static void handle_interrupt(CPU_t *cpu) {
	if (cpu->iff1) {
		cpu->iff1 = FALSE;
		cpu->iff2 = FALSE;
		if (cpu->imode == 0) {
			/* should execute whatever is on the bus...*/
			cpu->halt = FALSE;
			CPU_opcode_run(cpu);
		} else if (cpu->imode == 1) {
			//
			tc_add(cpu->timer_c, 8);
			cpu->halt = FALSE;
			cpu->bus = 0xFF;
#ifdef WITH_REVERSE
			CPU_add_prev_instr(cpu);
#endif
			CPU_opcode_run(cpu);
		} else if (cpu->imode == 2) {
			tc_add(cpu->timer_c, 19);
			cpu->halt = FALSE;
			unsigned short vector = (cpu->i << 8) + cpu->bus;
			int reg = CPU_mem_read(cpu,vector++) + (CPU_mem_read(cpu,vector) << 8);
			CPU_mem_write(cpu, --cpu->sp, (cpu->pc >> 8) & 0xFF);
			CPU_mem_write(cpu, --cpu->sp, cpu->pc & 0xFF);
			cpu->pc = reg;
		}
	}
}

BOOL is_link_instruction(CPU_t *cpu) {
	BYTE b1 = mem_read(cpu->mem_c, cpu->pc);
	BYTE b2 = mem_read(cpu->mem_c, cpu->pc + 1);
	return
		// out (n),a; in a,(n)
		(((b2 & 0x17) == 0x00) && ((b1 & 0xf7) == 0xd3)) ||
		// out (c),r; in r,(c)
		((b1 == 0xed) && ((cpu->c & 0x17) == 0x00) && ((b2 & 0xc6) == 0x40));
}


int CPU_connected_step(CPU_t *cpu) {
	cpu->interrupt = 0;
	cpu->ei_block = FALSE;

	if (cpu->halt == FALSE) {
		if (is_link_instruction(cpu)) {// && cpu->linking_time + cpu->timer_c->freq / 25 < cpu->timer_c->tstates) {
			cpu->is_link_instruction = TRUE;
			cpu->linking_time = cpu->timer_c->tstates;
			return 2;
		}
		CPU_opcode_fetch(cpu);
		if (cpu->bus == 0xDD || cpu->bus == 0xFD) {
			cpu->prefix = cpu->bus;
			CPU_opcode_fetch(cpu);
			CPU_opcode_run(cpu);
			cpu->prefix = 0;
		} else {
#ifdef WITH_REVERSE
			CPU_add_prev_instr(cpu);
#endif
			CPU_opcode_run(cpu);
		}
	} else {
		/* If the CPU is in halt */
		tc_add(cpu->timer_c, 4 * HALT_SCALE);
		cpu->r = (cpu->r & 0x80) + ((cpu->r + 1 * HALT_SCALE) & 0x7F);
	}

	handle_pio(cpu);

	if (cpu->interrupt && !cpu->ei_block) handle_interrupt(cpu);
	return 0;
}

int CPU_step(CPU_t* cpu) {
	cpu->interrupt = 0;
	cpu->ei_block = FALSE;

#ifdef WITH_REVERSE
	CPU_add_prev_instr(cpu);
#endif
	if (cpu->halt == FALSE) {
		CPU_opcode_fetch(cpu);
		if (cpu->bus == 0xDD || cpu->bus == 0xFD) {
			cpu->prefix = cpu->bus;
			CPU_opcode_fetch(cpu);
			CPU_opcode_run(cpu);
			cpu->prefix = 0;
		} else {
			CPU_opcode_run(cpu);
		}
	} else {
		/* If the CPU is in halt */
		tc_add(cpu->timer_c, 4 * HALT_SCALE);
		cpu->r = (cpu->r & 0x80) + ((cpu->r + 1 * HALT_SCALE) & 0x7F);
	}

	handle_pio(cpu);

	if (cpu->interrupt && !cpu->ei_block) handle_interrupt(cpu);
	return 0;
}

CPU_t* CPU_clone(CPU_t *cpu) {
	CPU_t *new_cpu = (CPU_t *) malloc(sizeof(CPU_t));
	memcpy(new_cpu, cpu, sizeof(CPU_t));
	return new_cpu;
}

#ifndef MACVER
#ifdef DEBUG
void displayreg(CPU_t *cpu) {
	puts("");
	printf("AF %0.4X\tAF' %0.4X\n", cpu->af, cpu->afp);
	printf("BC %0.4X\tBC' %0.4X\n", cpu->bc, cpu->bcp);
	printf("DE %0.4X\tDE' %0.4X\n", cpu->de, cpu->dep);
	printf("HL %0.4X\tHL' %0.4X\n", cpu->hl, cpu->hlp);
	printf("IX %0.4X\tIY  %0.4X\n", cpu->ix, cpu->iy);
	printf("PC %0.4X\tSP  %0.4X\n", cpu->pc, cpu->sp);

	printf("(BC) %0.4X\t(BC') %0.4X\n", read2bytes(cpu->mem_c,cpu->bc), read2bytes(cpu->mem_c,cpu->bcp));
	printf("(DE) %0.4X\t(DE') %0.4X\n", read2bytes(cpu->mem_c,cpu->de), read2bytes(cpu->mem_c,cpu->dep));
	printf("(HL) %0.4X\t(HL') %0.4X\n", read2bytes(cpu->mem_c,cpu->hl), read2bytes(cpu->mem_c,cpu->hlp));
	printf("(IX) %0.4X\t(IY)  %0.4X\n", read2bytes(cpu->mem_c,cpu->ix), read2bytes(cpu->mem_c,cpu->iy));
	printf("(PC) %0.4X\t(SP)  %0.4X\n", read2bytes(cpu->mem_c,cpu->pc), read2bytes(cpu->mem_c,cpu->sp));

	printf("I  %0.2X\tR  %0.2X\n", cpu->i, cpu->r);
	printf("Bus %0.2X\tim %d\n", cpu->bus,cpu->imode);
	printf("iff1: %d\tiff2: %d\n",cpu->iff1,cpu->iff2);
	printf("halt: %d\tinter: %d\n",cpu->halt,cpu->interrupt);
	printf("Tstates = %ld\n",tc_tstates(cpu->timer_c));
	printf("Seconds = %Lf\n",tc_elapsed(cpu->timer_c));
	puts("");
}
#endif
#endif
