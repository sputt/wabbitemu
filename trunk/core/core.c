#include "core.h"
#include "calc.h"
#include "device.h"
#include "alu.h"
#include "indexcb.h"
#include "control.h"


#include "optable.h"

u_char check_break(memc *mem, uint16_t addr) {
	bank_t *bank = &mem->banks[mc_bank(addr)];
	return mem->breaks[bank->ram][PAGE_SIZE * bank->page + mc_base(addr)] & 1;
}

u_char check_mem_write_break(memc *mem, uint16_t addr) {
	bank_t *bank = &mem->banks[mc_bank(addr)];
	return mem->breaks[bank->ram][PAGE_SIZE * bank->page + mc_base(addr)] & 2;
}

u_char check_mem_read_break(memc *mem, uint16_t addr) {
	bank_t *bank = &mem->banks[mc_bank(addr)];
	return mem->breaks[bank->ram][PAGE_SIZE * bank->page + mc_base(addr)] & 4;
}

unsigned char mem_read(memc *mem, unsigned short addr) {
	if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 3) && (mc_base(addr) >= (0x10000 - 64*mem->port27_remap_count)) && mc_base(addr) >= 0xFB64) {
		return mem->ram[0*PAGE_SIZE + mc_base(addr)];
	}
	if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 2) && (mc_base(addr) < 64*mem->port28_remap_count)) {
		return mem->ram[1*PAGE_SIZE + mc_base(addr)];
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

void set_break(memc *mem, BOOL ram, int page, uint16_t addr) {
	mem->breaks[ram % 2][PAGE_SIZE * page + mc_base(addr)] |= 1;
}

void set_mem_write_break(memc *mem, BOOL ram, int page, uint16_t addr) {
	mem->breaks[ram % 2][PAGE_SIZE * page + mc_base(addr)] |= 2;
}

void set_mem_read_break(memc *mem, BOOL ram, int page, uint16_t addr) {
	mem->breaks[ram % 2][PAGE_SIZE * page + mc_base(addr)] |= 4;
}

void clear_break(memc *mem, BOOL ram, int page, uint16_t addr) {
	mem->breaks[ram % 2][PAGE_SIZE * page + mc_base(addr)] &= 6;
}

void clear_mem_write_break(memc *mem, BOOL ram, int page, uint16_t addr) {
	mem->breaks[ram % 2][PAGE_SIZE * page + mc_base(addr)] &= 5;
}

void clear_mem_read_break(memc *mem, BOOL ram, int page, uint16_t addr) {
	mem->breaks[ram % 2][PAGE_SIZE * page + mc_base(addr)] &= 3;
}

unsigned char mem_write(memc *mem, unsigned short addr, char data) {
	if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 3) && (mc_base(addr) >= (0x10000 - 64*mem->port27_remap_count)) && mc_base(addr) >= 0xFB64) {
		return mem->ram[0*PAGE_SIZE + mc_base(addr)] = data;
	}
	if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 2) && (mc_base(addr) < 64*mem->port28_remap_count)) {
		return mem->ram[1*PAGE_SIZE + mc_base(addr)] = data;
	}
	return  *(mem->banks[mc_bank(addr)].addr + mc_base(addr)) = data;
}

inline unsigned short read2bytes(memc *mem, unsigned short addr) {
	return (mem_read(mem,addr)+(mem_read(mem,addr+1)<<8));
}

inline unsigned short mem_read16(memc *mem, unsigned short addr) {
	return (mem_read(mem,addr)+(mem_read(mem,addr+1)<<8));
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
	mem_c->port28_remap_count = 0;
	return 0;
}

static void handle_pio(CPU_t *cpu) {
	int i;
	for (i = 0; cpu->pio.interrupt[i] != -1; i++) {
		if (cpu->pio.skip_factor[i]) {
			if (cpu->pio.skip_count[i] == 0x00) device_control(cpu, cpu->pio.interrupt[i]);
			cpu->pio.skip_count[i] = (cpu->pio.skip_count[i]+1)%cpu->pio.skip_factor[i];
		}
	}
}

static int CPU_opcode_fetch(CPU_t *cpu) {
	int bank = mc_bank(cpu->pc);
	// TI Execution prevention will reset to 0x0000 if you execute on an even RAM page (handle also the port28 remap for SEs)
	if (cpu->mem_c->banks[bank].no_exec && !(!cpu->mem_c->boot_mapped && (bank == 2) && (mc_base(cpu->pc) < 64*cpu->mem_c->port28_remap_count))) {
		// plant a reset command if this was a non execute area
		cpu->bus = 0xC7;	// rst 00h, may want to place interrupt and hardware resets.
	} else {
		if (!cpu->mem_c->banks[bank].ram) endflash(cpu);	//I DON'T THINK THIS IS CORRECT
		cpu->bus = mem_read(cpu->mem_c, cpu->pc);			//However it shouldn't be a problem
															//assuming you know how to write to flash
	}
	if (cpu->mem_c->banks[bank].ram) {
		SEtc_add(cpu->timer_c, cpu->mem_c->read_OP_ram_tstates);
	} else {
		SEtc_add(cpu->timer_c, cpu->mem_c->read_OP_flash_tstates);
	}
	cpu->pc++;
	cpu->r = (cpu->r&0x80) + ((cpu->r+1)&0x7F);		//note: prefix opcodes inc the r reg to. so bit 7,h has 2 incs.
	return cpu->bus;
}

int CPU_mem_read(CPU_t *cpu, unsigned short addr) {
#ifdef DATA_BREAK
	if (check_mem_read_break(cpu->mem_c, addr))
	{
		calcs[gslot].running = FALSE;
		bank_t *bank = &calcs[gslot].mem_c.banks[mc_bank(calcs[gslot].cpu.pc)];
		if (calcs[gslot].ole_callback != NULL) {
			PostMessage(calcs[gslot].ole_callback, WM_USER, bank->ram<<16 | bank->page, calcs[gslot].cpu.pc);
			printf("postmessage called!\n");
		} else {
			gui_debug(gslot);
		}
	}
#endif
	cpu->bus = mem_read(cpu->mem_c, addr);

	if (cpu->mem_c->banks[mc_bank(addr)].ram) {
		SEtc_add(cpu->timer_c, cpu->mem_c->read_NOP_ram_tstates);
	} else {
		if (cpu->mem_c->step > 4) cpu->bus = 0xFF; // Flash status read, apparently
		else cpu->mem_c->step = 0;

		SEtc_add(cpu->timer_c, cpu->mem_c->read_NOP_flash_tstates);
	}

	return cpu->bus;
}

int CPU_mem_write(CPU_t *cpu, unsigned short addr, unsigned char data) {
#ifdef DATA_BREAK
	if (check_mem_write_break(cpu->mem_c, addr))
	{
		calcs[gslot].running = FALSE;
		bank_t *bank = &calcs[gslot].mem_c.banks[mc_bank(calcs[gslot].cpu.pc)];
		if (calcs[gslot].ole_callback != NULL) {
			PostMessage(calcs[gslot].ole_callback, WM_USER, bank->ram<<16 | bank->page, calcs[gslot].cpu.pc);
			printf("postmessage called!\n");
		} else {
			gui_debug(gslot);
		}
	}
#endif
	int bank = mc_bank(addr);

	if (cpu->mem_c->banks[bank].ram) {
		if (!cpu->mem_c->banks[bank].read_only) mem_write(cpu->mem_c, addr, data);

		SEtc_add(cpu->timer_c, cpu->mem_c->write_ram_tstates);
	} else {
		if (cpu->mem_c->flash_locked) {
			switch(cpu->mem_c->flash_version) {
				case 00:
					break;
				case 01:	//TI83+
					flashwrite83p(cpu,addr,data);	// in a seperate function for now, flash writes aren't the same across calcs
					break;
				case 02:	//TI83+SE, TI84+SE
					flashwrite83pse(cpu,addr,data);	// in a seperate function for now, flash writes aren't the same across calcs
					break;
				case 03:	//TI84+
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
		CPU_mem_read(cpu,cpu->pc++);				//read the offset, NOT INST
		char offset = cpu->bus;
		CPU_opcode_fetch(cpu);						//cb opcode, this is an INST
		cpu->r=((cpu->r-1)&0x7f)+(cpu->r&0x80);		//CHEAP BUG FIX
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
			unsigned short vector = (cpu->i<<8) + cpu->bus;
			int reg = CPU_mem_read(cpu,vector++) + ( CPU_mem_read(cpu,vector)<<8 );
			CPU_mem_write(cpu, --cpu->sp, (cpu->pc>>8)&0xFF);
			CPU_mem_write(cpu, --cpu->sp, cpu->pc&0xFF);
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
		tc_add(cpu->timer_c, 4*HALT_SCALE);
		cpu->r = (cpu->r&0x80) + ((cpu->r+1*HALT_SCALE)&0x7F);
	}

	handle_pio(cpu);

	if (cpu->interrupt && !cpu->ei_block) handle_interrupt(cpu);
	return 0;
}

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
