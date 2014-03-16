#include "stdafx.h"

#include "core.h"
#include "device.h"
#include "alu.h"
#include "indexcb.h"
#include "control.h"
#include "optable.h"
#ifdef WITH_REVERSE
#include "alu_reverse.h"
#include "indexcb_reverse.h"
#include "control_reverse.h"
#include "optable_reverse.h"
#include "reverse_info.h"
#include "reverse_info_table.h"
#endif

#define FLASH_BYTE_PROGRAM 0xA0
#define FLASH_BYTE_ERASE 0x80
#define FLASH_BYTE_FASTMODE 0x20
#define FLASH_BYTE_AUTOSELECT 0x90
#define FLASH_BYTE_FASTMODE_EXIT 0x90
#define FLASH_BYTE_FASTMODE_PROG 0xA0

unsigned char mem_read(memc *mem, unsigned short addr) {
	if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 3) && (addr >= (0x10000 - 64 * mem->port27_remap_count)) && addr >= 0xFB64) {
		return mem->ram[0 * PAGE_SIZE + mc_base(addr)];
	}
	if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 2) && (mc_base(addr) < 64 * mem->port28_remap_count)) {
		return mem->ram[1 * PAGE_SIZE + mc_base(addr)];
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
waddr_t addr16_to_waddr(memc *mem_c, uint16_t addr) {
	waddr_t waddr;
	bank_t *bank = &mem_c->banks[mc_bank(addr)];

	waddr.addr = addr;
	waddr.page = (uint8_t) bank->page;
	waddr.is_ram = bank->ram;

	return waddr;
}

/*
* Convert a 32 bit address to a waddr
*/
waddr_t addr32_to_waddr(unsigned int addr, BOOL is_ram) {
	waddr_t waddr;

	waddr.page = (uint8_t)(addr / PAGE_SIZE);
	waddr.addr = (uint16_t)(addr % PAGE_SIZE);
	waddr.is_ram = is_ram;

	return waddr;
}


BOOL check_break(memc *mem, waddr_t waddr) {
	if (!(mem->breaks[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] & NORMAL_BREAK))
		return FALSE;
	if (mem->breakpoint_manager_callback) {
		return mem->breakpoint_manager_callback(mem, NORMAL_BREAK, waddr);
	}
	return TRUE;
}
BOOL check_mem_write_break(memc *mem, waddr_t waddr) {
	if (!(mem->breaks[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] & MEM_WRITE_BREAK))
		return FALSE;
	if (mem->breakpoint_manager_callback) {
		return mem->breakpoint_manager_callback(mem, MEM_WRITE_BREAK, waddr);
	}
	return TRUE;
}
BOOL check_mem_read_break(memc *mem, waddr_t waddr) {
	if (!(mem->breaks[waddr.is_ram][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] & MEM_READ_BREAK))
		return FALSE;
	if (mem->breakpoint_manager_callback) {
		return mem->breakpoint_manager_callback(mem, MEM_READ_BREAK, waddr);
	}
	return TRUE;
}

extern void add_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);
extern void rem_breakpoint(memc *mem, BREAK_TYPE type, waddr_t waddr);

void set_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] |= NORMAL_BREAK;
	add_breakpoint(mem, NORMAL_BREAK, waddr);
}
void set_mem_write_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] |= MEM_WRITE_BREAK;
	add_breakpoint(mem, MEM_WRITE_BREAK, waddr);
}
void set_mem_read_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] |= MEM_READ_BREAK;
	add_breakpoint(mem, MEM_READ_BREAK, waddr);
}

void clear_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] &= CLEAR_NORMAL_BREAK;
	rem_breakpoint(mem, NORMAL_BREAK, waddr);
}
void clear_mem_write_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] &= CLEAR_MEM_WRITE_BREAK;
	rem_breakpoint(mem, MEM_WRITE_BREAK, waddr);
}
void clear_mem_read_break(memc *mem, waddr_t waddr) {
	mem->breaks[waddr.is_ram % 2][PAGE_SIZE * waddr.page + mc_base(waddr.addr)] &= CLEAR_MEM_READ_BREAK;
	rem_breakpoint(mem, MEM_READ_BREAK, waddr);
}

unsigned char mem_write(memc *mem, unsigned short addr, char data) {
	if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 3) && (addr >= (0x10000 - 64 * mem->port27_remap_count)) && addr >= 0xFB64) {
		return mem->ram[0 * PAGE_SIZE + mc_base(addr)] = data;
	}
	if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(addr) == 2) && (mc_base(addr) < 64 * mem->port28_remap_count)) {
		return mem->ram[1 * PAGE_SIZE + mc_base(addr)] = data;
	}
	//handle missing ram pages
	if (mem->ram_version == 2) {
		if (mem->banks[mc_bank(addr)].ram && mem->banks[mc_bank(addr)].page > 2) {
			return mem->ram[2 * PAGE_SIZE + mc_base(addr)] = data;
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
	mem_c->flash_write_delay = 200;
#ifdef WITH_REVERSE
	cpu->prev_instruction = cpu->prev_instruction_list;
#endif
	return 0;
}

/* Clear RAM and start calculator at $0000
* 10/20/10 Calc84 says that starting at $0000 is wrong we need to start on the boot page,
* 6/29/11 which turns out to be in bank 0 so we start at $0000 anyway.
*/
int CPU_reset(CPU_t *cpu) {
	cpu->sp = 0;
	cpu->interrupt = FALSE;
	cpu->imode = 1;
	cpu->ei_block = FALSE;
	cpu->iff1 = FALSE;
	cpu->iff2 = FALSE;
	cpu->halt = FALSE;
	cpu->read = FALSE;
	cpu->write = FALSE;
	cpu->output = FALSE;
	cpu->input = FALSE;
	cpu->prefix = 0;
	cpu->pc = 0;
	cpu->mem_c->port27_remap_count = 0;
	cpu->mem_c->port28_remap_count = 0;
	cpu->mem_c->ram_lower = 0x00 * 0x400;
	cpu->mem_c->ram_upper = 0x00 * 0x400 + 0x3FF;
	cpu->mem_c->banks = cpu->mem_c->normal_banks;
	cpu->mem_c->boot_mapped = FALSE;
	cpu->mem_c->hasChangedPage0 = FALSE;
	memset(cpu->mem_c->protected_page, 0, sizeof(cpu->mem_c->protected_page));
	cpu->mem_c->protected_page_set = 0;
#ifdef WITH_REVERSE
	cpu->prev_instruction = cpu->prev_instruction_list;
	memset(cpu->prev_instruction_list, 0, sizeof(cpu->prev_instruction_list));
	cpu->reverse_instr = 0;
#endif
	
	switch (cpu->pio.model) {
	case TI_81: {
		bank_state_t banks[5] = {
			{ cpu->mem_c->flash, 0, FALSE, FALSE, FALSE },
			{ cpu->mem_c->flash + 0x1 * PAGE_SIZE, 0x1, FALSE, FALSE, FALSE },
			{ cpu->mem_c->flash + 0x1 * PAGE_SIZE, 0x1, FALSE, FALSE, FALSE },
			{ cpu->mem_c->ram, 0, FALSE, TRUE, FALSE },
			{ NULL, 0, FALSE, FALSE, FALSE }
		};
		memcpy(cpu->mem_c->normal_banks, banks, sizeof(banks));
		break;
	}
	case TI_82:
	case TI_83: {
		bank_state_t banks[5] = {
			{ cpu->mem_c->flash, 0, FALSE, FALSE, FALSE },
			{ cpu->mem_c->flash + 0x00 * PAGE_SIZE, 0x00, FALSE, FALSE, FALSE },
			{ cpu->mem_c->ram + 0x01 * PAGE_SIZE, 0x01, FALSE, TRUE, FALSE },
			{ cpu->mem_c->ram, 0, FALSE, TRUE, FALSE },
			{ NULL, 0, FALSE, FALSE, FALSE }
		};
		memcpy(cpu->mem_c->normal_banks, banks, sizeof(banks));
		break;
	}
	case TI_85:
	case TI_86: {
		bank_state_t banks[5] = {
			{ cpu->mem_c->flash, 0, FALSE, FALSE, FALSE },
			{ cpu->mem_c->flash + 0x0F * PAGE_SIZE, 0x0F, FALSE, FALSE, FALSE },
			{ cpu->mem_c->flash, 0, FALSE, FALSE, FALSE },
			{ cpu->mem_c->ram, 0, FALSE, TRUE, FALSE },
			{ NULL, 0, FALSE, FALSE, FALSE }
		};

		memcpy(cpu->mem_c->normal_banks, banks, sizeof(banks));
		break;
	}
	case TI_73:
	case TI_83P:
	case TI_83PSE:
	case TI_84PSE: 
	case TI_84P:
	case TI_84PCSE: {
		int bootPage = cpu->mem_c->flash_pages - 1;
		/*	Address		page	write?	ram?	no exec?	*/
		bank_state_t banks[5] = {
			{ cpu->mem_c->flash + bootPage * PAGE_SIZE, bootPage, FALSE, FALSE, FALSE },
			{ cpu->mem_c->flash, 0, FALSE, FALSE, FALSE },
			{ cpu->mem_c->flash, 0, FALSE, FALSE, FALSE },
			{ cpu->mem_c->ram, 0, FALSE, TRUE, FALSE },
			{ NULL, 0, FALSE, FALSE, FALSE }
		};

		memcpy(cpu->mem_c->normal_banks, banks, sizeof(banks));
		break;
	}
	}
	return 0;
}

static void handle_pio(CPU_t *cpu) {
	for (int i = cpu->pio.num_interrupt; i >= 0; i--) {
		interrupt_t *intVal = &cpu->pio.interrupt[i];
		intVal->skip_count--;
		if (!intVal->skip_count) {
			device_control(cpu, intVal->interrupt_val);
			intVal->skip_count = intVal->skip_factor;
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
			if ((protected_val & 0x01) && bank->page == 0) {
				return FALSE;
			} else if ((protected_val & 0x20) && bank->page == 1) {
				return FALSE;
			}
			return TRUE;
		} else if (bank->page < 0x08) {
			return TRUE;
		} else if (bank->page >= 0x1C) {
			return TRUE;
		}
		protected_val = cpu->mem_c->protected_page[(bank->page - 8) / 8];
		// basically this checks whether the bit corresponding to the page
		// is set indicating no exec is allowed
		return !(protected_val & (0x01 << ((bank->page - 8) % 8)));
	} else {
		memc *mem = cpu->mem_c;
		if (!bank->ram)	{		//if its flash and between page limits
			return bank->page <= mem->flash_lower || bank->page > mem->flash_upper;
		}
		if (bank->page & (2 >> (cpu->mem_c->prot_mode + 1)))
			return TRUE;		//we know were in ram so lets check if the page is allowed in the mem protected mode
		//execution is allowed on 2^(mode+1)
		//finally we check ports 25/26 to see if its ok to execute on this page
		int global_addr = bank->page * PAGE_SIZE + (cpu->pc & 0x3FFF);
		if ((mem->port27_remap_count > 0) && !mem->boot_mapped && (mc_bank(cpu->pc) == 3) && (cpu->pc >= (0x10000 - 64 * mem->port27_remap_count)) && cpu->pc >= 0xFB64)
			global_addr = 0 * PAGE_SIZE + mc_base(cpu->pc);
		else if ((mem->port28_remap_count > 0) && !mem->boot_mapped && (mc_bank(cpu->pc) == 2) && (mc_base(cpu->pc) < 64 * mem->port28_remap_count))
			global_addr = 1 * PAGE_SIZE + mc_base(cpu->pc);
		if (global_addr < cpu->mem_c->ram_lower || global_addr > cpu->mem_c->ram_upper)
			return FALSE;
		return TRUE;
	}
}

void change_page(memc *mem, int bank, u_char page, BOOL ram) {
	mem->normal_banks[bank].ram = ram;
	if (ram) {
		mem->normal_banks[bank].page = page;
		mem->normal_banks[bank].addr = mem->ram + (page * PAGE_SIZE);
		mem->normal_banks[bank].read_only = FALSE;
		mem->normal_banks[bank].no_exec = FALSE;
	}
	else {
		mem->normal_banks[bank].page = page;
		mem->normal_banks[bank].addr = mem->flash + (page * PAGE_SIZE);
		mem->normal_banks[bank].read_only = page == mem->flash_pages - 1;
		mem->normal_banks[bank].no_exec = FALSE;
	}
	update_bootmap_pages(mem);
}

void update_bootmap_pages(memc *mem_c) {
	memcpy(mem_c->bootmap_banks, mem_c->normal_banks, sizeof(bank_state_t));

	mem_c->bootmap_banks[1].page = mem_c->normal_banks[1].page & 0xFE;
	mem_c->bootmap_banks[1].addr = (mem_c->normal_banks[1].ram ? mem_c->ram : mem_c->flash) + (mem_c->bootmap_banks[1].page * PAGE_SIZE);
	mem_c->bootmap_banks[1].read_only = FALSE;
	mem_c->bootmap_banks[1].no_exec = FALSE;
	mem_c->bootmap_banks[1].ram = mem_c->normal_banks[1].ram;

	mem_c->bootmap_banks[2].page = mem_c->normal_banks[1].page | (!mem_c->flash_version == 1);
	mem_c->bootmap_banks[2].addr = (mem_c->normal_banks[1].ram ? mem_c->ram : mem_c->flash) + (mem_c->bootmap_banks[2].page * PAGE_SIZE);
	mem_c->bootmap_banks[2].read_only = FALSE;
	mem_c->bootmap_banks[2].no_exec = FALSE;
	mem_c->bootmap_banks[2].ram = mem_c->normal_banks[1].ram;

	mem_c->bootmap_banks[3].page = mem_c->normal_banks[2].page;
	mem_c->bootmap_banks[3].addr = (mem_c->normal_banks[2].ram ? mem_c->ram : mem_c->flash) + (mem_c->bootmap_banks[3].page * PAGE_SIZE);
	mem_c->bootmap_banks[3].read_only = FALSE;
	mem_c->bootmap_banks[3].no_exec = FALSE;
	mem_c->bootmap_banks[3].ram = mem_c->normal_banks[2].ram;
}

static void endflash(memc *mem_c) {
	if (mem_c->step != FLASH_ERROR) {
		mem_c->step = FLASH_READ;
	}
}

static void endflash_break(CPU_t *cpu) {
	if (cpu->invalid_flash_callback) {
		cpu->invalid_flash_callback(cpu);
	}
	endflash(cpu->mem_c);
}

static int CPU_opcode_fetch(CPU_t *cpu) {
	int bank_num = mc_bank(cpu->pc);
	bank_state_t *bank = &cpu->mem_c->banks[bank_num];
	//the boot page is mapped to bank 0 to start
	//if code is run from an address of whatever page is mapped to port 6
	//then the page is changed to page 0. why? who the fuck knows
	if (!cpu->mem_c->hasChangedPage0 && !bank->ram && (bank_num == 1 || (cpu->mem_c->boot_mapped && bank_num == 2)))
	{
		change_page(cpu->mem_c, 0, 0, FALSE);
		cpu->mem_c->hasChangedPage0 = TRUE;
	}
	if (!is_allowed_exec(cpu)) {
		if (cpu->exe_violation_callback) {
			cpu->exe_violation_callback(cpu);
		} else {
			CPU_reset(cpu);
		}
	}
	if (!bank->ram && cpu->mem_c->step != FLASH_READ) {	//I DON'T THINK THIS IS CORRECT
		endflash_break(cpu);							//However it shouldn't be a problem
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


static unsigned char flash_autoselect(CPU_t *cpu, unsigned short addr) {
	int offset = addr & 0x3FFF;
	if (offset == 0) {
		//1 indicates an AMD chip
		//manufacturer id
		return 1;
	} else if (offset == 2) {
		//B9 for new 512 K chips, 23 for old 512 K chips, C4 for 2 MB chips, and DA for 1 MB chips.
		switch (cpu->pio.model) {
		case TI_84P:
			return 0xDA;
		case TI_83PSE:
		case TI_84PSE:
			return 0xC4;
		default:
			if (cpu->cpu_version == 1) {
				return 0x23;
			} else {
				return 0xB9;
			}
		}
	} else if (offset == 4) {
		return 0;
	} else {
		endflash_break(cpu);
		return 0;
	}
}

static unsigned char flash_read(CPU_t *cpu, unsigned short addr) {
	memc *mem_c = cpu->mem_c;

	if (mem_c->flash_error) {
		unsigned char error_value = ((~cpu->mem_c->flash_write_byte & 0x80) | 0x20);
		error_value |= cpu->mem_c->flash_toggles;
		cpu->mem_c->flash_toggles ^= 0x40;
		mem_c->flash_error = FALSE;
		return error_value;
	} else if (mem_c->step == FLASH_READ || mem_c->step == FLASH_FASTMODE) {
		return mem_read(cpu->mem_c, addr);
	} else if (mem_c->step == FLASH_AUTOSELECT) {
		return flash_autoselect(cpu, addr);
	} else {
		// flash read during writing to flash is bad, consider it invalid
		// I'm pretty sure this is the correct behavior, but haven't tested it
		endflash_break(cpu);
		return mem_read(cpu->mem_c, addr);
	}
}

unsigned char CPU_mem_read(CPU_t *cpu, unsigned short addr) {
	if (check_mem_read_break(cpu->mem_c, addr16_to_waddr(cpu->mem_c, addr))) {
		if (cpu->mem_read_break_callback) {
			cpu->mem_read_break_callback(cpu);
		}
	}

	if (cpu->mem_c->banks[mc_bank(addr)].ram) {
		cpu->bus = mem_read(cpu->mem_c, addr);
		SEtc_add(cpu->timer_c, cpu->mem_c->read_NOP_ram_tstates);
	}
	else {
		cpu->bus = flash_read(cpu, addr);
		SEtc_add(cpu->timer_c, cpu->mem_c->read_NOP_flash_tstates);
	}

	return cpu->bus;
}

static void flash_write_byte(memc *mem_c, unsigned short addr, unsigned char data) {
	int bankNum = mc_bank(addr);
	bank_t bank = mem_c->banks[bankNum];
	BYTE *write_location = bank.addr + mc_base(addr);
	(*write_location) &= data;  //AND LOGIC!!
	mem_c->flash_write_byte = data;
	if ((*write_location) != data) {
		mem_c->flash_error = TRUE;
	}
	mem_c->step = FLASH_READ;
}

static void flash_write(CPU_t *cpu, unsigned short addr, unsigned char data) {
	memc *mem_c = cpu->mem_c;
	int bank = mc_bank(addr);

	// F0 means we were exiting flash write sequence
	if (data == 0xF0 && mem_c->step != FLASH_PROGRAM && mem_c->step != FLASH_FASTMODE_PROG) {
		endflash(mem_c);
		return;
	}

	mem_c->flash_error = FALSE;
	switch (mem_c->step) {
	case FLASH_READ:
		if (((addr & 0x0FFF) == 0x0AAA) && (data == 0xAA)) {
			mem_c->step = FLASH_AA;
		} else {
			endflash_break(cpu);
		}
		break;
	case FLASH_AA:
		if (((addr & 0x0FFF) == 0x0555) && (data == 0x55)) {
			mem_c->step = FLASH_55;
		} else {
			endflash_break(cpu);
		}
		break;
	case FLASH_55:
		if ((addr & 0x0FFF) == 0x0AAA) {
			switch (data) {
			case FLASH_BYTE_PROGRAM:
				mem_c->step = FLASH_PROGRAM;
				break;
			case FLASH_BYTE_ERASE:
				mem_c->step = FLASH_ERASE;
				break;
			case FLASH_BYTE_FASTMODE:
				if (mem_c->flash_version != 1) {
					mem_c->step = FLASH_FASTMODE;
				} else {
					endflash_break(cpu);
				}
				break;
			case FLASH_BYTE_AUTOSELECT:
				mem_c->step = FLASH_AUTOSELECT;
				break;
			default:
				endflash_break(cpu);
				break;
			}
		} else {
			endflash_break(cpu);
		}
		break;
	case FLASH_PROGRAM: {
		flash_write_byte(mem_c, addr, data);
		if (check_mem_write_break(cpu->mem_c, addr16_to_waddr(cpu->mem_c, addr))) {
			if (cpu->mem_write_break_callback) {
				cpu->mem_write_break_callback(cpu);
			}
		}

		endflash(mem_c);
		break;
	}
	case FLASH_ERASE:
		if (((addr & 0x0FFF) == 0x0AAA) && (data == 0xAA)) {
			mem_c->step = FLASH_ERASE_AA;
		} else {
			endflash_break(cpu);
		}
		break;
	case FLASH_ERASE_AA:
		if (((addr & 0x0FFF) == 0x0555) && (data == 0x55)) {
			mem_c->step = FLASH_ERASE_55;
		} else {
			endflash_break(cpu);
		}
		break;
	case FLASH_ERASE_55:
		if (((addr & 0x0FFF) == 0x0AAA) && (data == 0x10)) {
			// Erase entire chip...I'm not sure if 
			// boot page is included, so I'll leave it off.
			// DrDnar 7/8/11: boot sector is included
			for (int i = 0; i < cpu->mem_c->flash_size; i++) {
				cpu->mem_c->flash[i] = 0xFF;

				if (check_mem_write_break(cpu->mem_c, addr32_to_waddr(i, FALSE))) {
					if (cpu->mem_write_break_callback) {
						cpu->mem_write_break_callback(cpu);
					}
				}
			}
		} else if (data == 0x30) {
			// erase sectors
			int spage = (mem_c->banks[bank].page << 1) + ((addr >> 13) & 0x01);
			int pages = mem_c->flash_pages;
			int totalPages = pages * 2;
			int startaddr, endaddr;

			if (spage < totalPages - 8) {
				startaddr = (spage & 0x01FF) * 0x2000;
				endaddr = startaddr + PAGE_SIZE * 4;
			} else if (spage < totalPages - 4) {
				startaddr = (pages - 4) * PAGE_SIZE;
				endaddr = (pages - 2) * PAGE_SIZE;
			} else if (spage < totalPages - 3) {
				startaddr = (pages - 2) * PAGE_SIZE;
				endaddr = (pages - 2) * PAGE_SIZE + PAGE_SIZE / 2;
			} else if (spage < totalPages - 2) {
				startaddr = (pages - 2) * PAGE_SIZE + PAGE_SIZE / 2;
				endaddr = (pages - 1) * PAGE_SIZE;
			} else if (spage < totalPages) {
				// I comment this off because this is the boot page
				// it suppose to be write protected...
				// BuckeyeDude 6/27/11: new info has been discovered boot code
				// is writable under certain conditions
				startaddr = (pages - 1) * PAGE_SIZE;
				endaddr = pages * PAGE_SIZE;
			} else {
				endflash_break(cpu);
				break;
			}

			for (int i = startaddr; i < endaddr; i++) {
				mem_c->flash[i] = 0xFF;

				if (check_mem_write_break(cpu->mem_c, addr32_to_waddr(i, FALSE))) {
					if (cpu->mem_write_break_callback) {
						cpu->mem_write_break_callback(cpu);
					}
				}
			}
		} else {
			endflash_break(cpu);
		}
		endflash(mem_c);
		break;
	case FLASH_FASTMODE:
		if (data == FLASH_BYTE_FASTMODE_EXIT) {
			mem_c->step = FLASH_FASTMODE_EXIT;
		} else if (data == FLASH_BYTE_FASTMODE_PROG) {
			mem_c->step = FLASH_FASTMODE_PROG;
		} else {
			endflash_break(cpu);
		}
		break;
	case FLASH_FASTMODE_EXIT:
		// we already handle the case data == 0xF0 above
		// so just reset back to fastmode if you try do do
		// anything but exit
		mem_c->step = FLASH_FASTMODE;
		break;
	case FLASH_FASTMODE_PROG: {
		flash_write_byte(cpu->mem_c, addr, data);
		if (check_mem_write_break(cpu->mem_c, addr16_to_waddr(cpu->mem_c, addr))) {
			if (cpu->mem_write_break_callback) {
				cpu->mem_write_break_callback(cpu);
			}
		}
		break;
	}
	default:
		endflash_break(cpu);
		break;
	}
}

BOOL check_flash_write_valid(CPU_t *cpu, int page) {
	return !cpu->mem_c->flash_locked && cpu->pio.model >= TI_73 &&
		(((page != 0x3F && page != 0x2F) || (cpu->model_bits & 0x3)) &&
		((page != 0x7F && page != 0x6F) || (cpu->model_bits & 0x2) || !(cpu->model_bits & 0x1)));
}

void CPU_mem_write(CPU_t *cpu, unsigned short addr, unsigned char data) {
	bank_state_t *bank = &cpu->mem_c->banks[mc_bank(addr)];

	if (bank->ram) {
		if (!bank->read_only) {
			mem_write(cpu->mem_c, addr, data);
			if (check_mem_write_break(cpu->mem_c, addr16_to_waddr(cpu->mem_c, addr))) {
				if (cpu->mem_write_break_callback) {
					cpu->mem_write_break_callback(cpu);
				}
			}
		}
		SEtc_add(cpu->timer_c, cpu->mem_c->write_ram_tstates);
	} else {
		if (check_flash_write_valid(cpu, bank->page)) {
			flash_write(cpu, addr, data);
		} else if (cpu->invalid_flash_callback) {
			cpu->invalid_flash_callback(cpu);
		}

		SEtc_add(cpu->timer_c, cpu->mem_c->write_flash_tstates);
	}

	cpu->bus = data;
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
		ICB_opcode[cpu->bus](cpu, offset);
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
			unsigned short reg = CPU_mem_read(cpu, vector++);
			reg += CPU_mem_read(cpu, vector) << 8;
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

	if (cpu->interrupt && !cpu->ei_block) {
		handle_interrupt(cpu);
	}

	return 0;
}

void handle_profiling(CPU_t *cpu, uint64_t oldTStates, uint16_t oldPC) {
	uint64_t time = tc_tstates(cpu->timer_c) - oldTStates;
	cpu->profiler.totalTime += time;
	bank_t bank = cpu->mem_c->banks[mc_bank(oldPC)];
	int block = (oldPC % PAGE_SIZE) / cpu->profiler.blockSize;

	if (bank.ram) {
		uint64_t(*ram_data)[PROFILER_NUM_BLOCKS] = 
			(uint64_t(*)[PROFILER_NUM_BLOCKS]) cpu->profiler.ram_data;
		ram_data[bank.page][block] += time;
	} else {
		uint64_t(*flash_data)[PROFILER_NUM_BLOCKS] = 
			(uint64_t(*)[PROFILER_NUM_BLOCKS]) cpu->profiler.flash_data;
		flash_data[bank.page][block] += time;
	}
}

int CPU_step(CPU_t* cpu) {
	cpu->interrupt = 0;
	cpu->ei_block = FALSE;
	unsigned short old_pc = cpu->old_pc = cpu->pc;
	unsigned long long old_tstates = tc_tstates(cpu->timer_c);

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

	if (cpu->interrupt && !cpu->ei_block) {
		//if an interrupt is generated during a ld a, r or ld a, i
		//then the PV flag should be reset
		u_char edprefix = mem_read(cpu->mem_c, cpu->pc - 2);
		u_char instruction = mem_read(cpu->mem_c, cpu->pc - 1);
		if (edprefix == 0xED && (instruction == 0x57 || instruction == 0x5F)) {
			cpu->f &= ~PV_MASK;
		}
		handle_interrupt(cpu);
	}

	if (cpu->profiler.running) {
		handle_profiling(cpu, old_tstates, old_pc);
	}

	return 0;
}

CPU_t* CPU_clone(CPU_t *cpu) {
	CPU_t *new_cpu = (CPU_t *)malloc(sizeof(CPU_t));
	memcpy(new_cpu, cpu, sizeof(CPU_t));
	return new_cpu;
}
