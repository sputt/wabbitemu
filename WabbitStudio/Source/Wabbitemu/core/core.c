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
		return mem->ram[waddr.page * PAGE_SIZE + waddr.addr];
	} else {
		return mem->flash[waddr.page * PAGE_SIZE + waddr.addr];
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

BOOL check_break(memc *mem, waddr waddr) {
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
		if (mem->banks[mc_bank(addr)].ram == TRUE && mem->banks[mc_bank(addr)].page > 2) {
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
	return 0;
}

static void handle_pio(CPU_t *cpu) {
	int i;
	for (i = 0; cpu->pio.interrupt[i] != -1; i++) {
		if (cpu->pio.skip_factor[i]) {
			if (cpu->pio.skip_count[i] == 0x00) device_control(cpu, cpu->pio.interrupt[i]);
			cpu->pio.skip_count[i] = (cpu->pio.skip_count[i] + 1) % cpu->pio.skip_factor[i];
		}
	}
}

BOOL is_priveleged_page(CPU_t *cpu) {
	//priveleged pages are as follows
	// TI 83+		= 1C, 1D, 1F
	// TI 83+SE		= 7C, 7D, 7F
	// TI-84+		= 2F, 3C, 3D, 3F
	// TI 84+SE		= 6F, 7C, 7D, 7F
	bank_state_t bank = cpu->mem_c->banks[mc_bank(cpu->pc)];
	//this includes page XE (which should be ok because its the cert). If its a problem i can fix later
	return ((bank.page >= cpu->mem_c->flash_pages - 4 && bank.page != cpu->mem_c->flash_pages - 2) || (((cpu->pio.model >= TI_84P) && bank.page == cpu->mem_c->flash_pages - 0x11)));
}

static BOOL is_allowed_exec(CPU_t *cpu) {
	bank_state_t  bank = cpu->mem_c->banks[mc_bank(cpu->pc)];
	if (cpu->pio.model <= TI_83P) {
		int protected_val, group_offset;
		if (bank.ram) {
			protected_val = cpu->mem_c->protected_page[3];
			if ((protected_val & 0x01) && bank.page == 0)
				return FALSE;
			if ((protected_val & 0x20) && bank.page == 1)
				return FALSE;
			return TRUE;
		} else if (bank.page < 0x08)
			return TRUE;
		else if (bank.page >= 0x1C)
			return TRUE;
		protected_val = cpu->mem_c->protected_page[(bank.page - 8) / 8];
		//yay for awesome looking code :D
		//basically this checks whether the bit corresponding to the page
		//is set indicating no exec is allowed
		return !(protected_val & (0x01 << ((bank.page - 8) % 8)));
	} else {
		if (!bank.ram)			//if its flash and between page limits
		return bank.page > cpu->mem_c->flash_upper || bank.page <= cpu->mem_c->flash_lower;
		if (bank.page & (2 >> (cpu->mem_c->prot_mode + 1)))
			return TRUE;		//we know were in ram so lets check if the page is allowed in the mem prot mode
								//execution is allowed on 2^(mode+1)
		memc *mem = cpu->mem_c;
		//finally we check ports 25/26 to see if its ok to execute on this page
		int global_addr = bank.page * 0x4000 + (cpu->pc & 0x3FFF);
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
		
	mem_c->bootmap_banks[2].page		= mem_c->normal_banks[1].page | !mem_c->flash_version == 1;
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
	bank_state_t  bank = cpu->mem_c->banks[bank_num];
	//the boot page is mapped to bank 0 to start
	//if code is run from an address of whatever page is mapped to port 6
	//then the page is changed to page 0. why? who the fuck knows
	if (cpu->mem_c->banks[0].page != 0 && (cpu->mem_c->boot_mapped && (bank_num == 1 || bank_num == 2) && !bank.ram) || ((!cpu->mem_c->boot_mapped && bank_num == 1) && !bank.ram))
		change_page(cpu, 0, 0, FALSE);
	if (!is_allowed_exec(cpu)) {
		if (break_on_exe_violation)
			cpu->exe_violation_callback(cpu);
		CPU_reset(cpu);
	}
	if (!bank.ram) endflash(cpu);						//I DON'T THINK THIS IS CORRECT
	cpu->bus = mem_read(cpu->mem_c, cpu->pc);			//However it shouldn't be a problem
															//assuming you know how to write to flash
	if (bank.ram)
		SEtc_add(cpu->timer_c, cpu->mem_c->read_OP_ram_tstates);
	else
		SEtc_add(cpu->timer_c, cpu->mem_c->read_OP_flash_tstates);
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
						if (cpu->cpu_version == 1)
							cpu->bus = 0x23;
						else
							cpu->bus = 0xB9;
						break;
				}
			} else if ((addr & 0x3FFF) == 4) {
				cpu->bus = 0;
			}
		}

		SEtc_add(cpu->timer_c, cpu->mem_c->read_NOP_flash_tstates);
	}

	return cpu->bus;
}

unsigned char CPU_mem_write(CPU_t *cpu, unsigned short addr, unsigned char data) {
	if (check_mem_write_break(cpu->mem_c, addr_to_waddr(cpu->mem_c, addr))) {
		cpu->mem_c->mem_write_break_callback(cpu);
	}
	int bank = mc_bank(addr);

	if (cpu->mem_c->banks[bank].ram) {
		if (!cpu->mem_c->banks[bank].read_only) mem_write(cpu->mem_c, addr, data);

		SEtc_add(cpu->timer_c, cpu->mem_c->write_ram_tstates);
	} else {
		if (!cpu->mem_c->flash_locked && // 1) {
			(((cpu->mem_c->banks[bank].page != 0x3F && cpu->mem_c->banks[bank].page != 0x2F) || cpu->pio.se_aux->model_bits & 0x3) &&
			((cpu->mem_c->banks[bank].page != 0x7F && cpu->mem_c->banks[bank].page != 0x6F) || cpu->pio.se_aux->model_bits & 0x2 || !(cpu->pio.se_aux->model_bits & 0x1)))) {
			switch(cpu->mem_c->flash_version) {
				case 00:
					break;
				case 01:	//TI83+
					flashwrite83p(cpu, addr, data);		// in a seperate function for now, flash writes aren't the same across calcs
					break;
				case 02:	//TI83+SE, TI84+SE
					flashwrite83pse(cpu, addr, data);	// in a seperate function for now, flash writes aren't the same across calcs
					break;
				case 03:	//TI84+
					flashwrite84p(cpu, addr, data);
					break;
			}
		}

		SEtc_add(cpu->timer_c, cpu->mem_c->write_flash_tstates);
	}

	return cpu->bus = data;
}

static void CPU_opcode_run(CPU_t *cpu) {
	opcode[cpu->bus](cpu);
}

static void CPU_CB_opcode_run(CPU_t *cpu) {
	if (cpu->prefix) {
		CPU_mem_read(cpu, cpu->pc++);				//read the offset, NOT INST
		char offset = cpu->bus;
		CPU_opcode_fetch(cpu);						//cb opcode, this is an INST
		cpu->r = ((cpu->r - 1) & 0x7f) + (cpu->r & 0x80);		//CHEAP BUG FIX
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

int CPU_step(CPU_t* cpu) {
	cpu->interrupt = 0;
	cpu->ei_block = FALSE;

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
		#define HALT_SCALE	3
		tc_add(cpu->timer_c, 4 * HALT_SCALE);
		cpu->r = (cpu->r & 0x80) + ((cpu->r+1 * HALT_SCALE) & 0x7F);
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
