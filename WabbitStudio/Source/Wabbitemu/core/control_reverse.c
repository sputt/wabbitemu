#ifdef WITH_REVERSE
#include "stdafx.h"

#include "core.h"
#include "control.h"
#include "device.h"
#include "alu.h"

//-----------------------------
// ED OPCODES

void ednop_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
}

void ldd_reverse(CPU_t *cpu) {
	int reg;
	tc_sub(cpu->timer_c, 16);

	cpu->de++;
	cpu->hl++;
	cpu->bc++;
	reg = cpu->prev_instruction->data1;
	CPU_mem_write(cpu, cpu->de, reg);
}

void lddr_reverse(CPU_t *cpu) {
	int reg;
	tc_sub(cpu->timer_c, 16);

	if (cpu->bc) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21 - 16);
	}

	cpu->bc++;
	cpu->hl++;
	cpu->de++;
	reg = cpu->prev_instruction->data1;
	CPU_mem_write(cpu, cpu->de, reg);
}

void ldi_reverse(CPU_t *cpu) {
	int tmp;
	int reg;
	tc_sub(cpu->timer_c, 16);

	cpu->bc++;
	cpu->hl--;
	cpu->de--;

	reg = cpu->prev_instruction->data1;
	CPU_mem_write(cpu,cpu->de, reg);
}

void ldir_reverse(CPU_t *cpu) {
	int tmp;
	int reg;
	tc_sub(cpu->timer_c, 16);

	if (cpu->bc) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21 - 16);
	}

	cpu->bc--;
	cpu->hl++;
	cpu->de++;
	
	reg = cpu->prev_instruction->data1;
	CPU_mem_write(cpu, cpu->de, reg);
}

void ld_mem16_reg16_reverse(CPU_t *cpu) {
	int result = cpu->prev_instruction->data1;
	unsigned short address = mem_read(cpu->mem_c, --cpu->pc) << 8;
	address |= mem_read(cpu->mem_c, --cpu->pc);

	CPU_mem_write(cpu, address++, result & 0xFF);
	CPU_mem_write(cpu, address, result >> 8);
	tc_sub(cpu->timer_c, 20);
}

void ld_reg16_mem16_reverse(CPU_t *cpu) {
	cpu->pc -= 2;
	int test = (mem_read(cpu->mem_c, cpu->pc) >> 4) & 3;
	unsigned short result = cpu->prev_instruction->data1;

	switch (test) {
		case 00:
			cpu->bc = result;
			break;
		case 01:
			cpu->de = result;
			break;
		case 02:
			cpu->hl = result;
			break;
		case 03:
			cpu->sp = result;
			break;
	}
	tc_sub(cpu->timer_c, 20);
}

void IM0_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->imode = cpu->prev_instruction->lower_data1;
}
void IM1_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->imode = cpu->prev_instruction->lower_data1;
}
void IM2_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->imode = cpu->prev_instruction->lower_data1;
}

void in_reg_c_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 12);
	cpu->f = cpu->prev_instruction->flag;
}


void ind_reverse(CPU_t *cpu) {
	int result, tmp;
	tc_sub(cpu->timer_c, 16);
	cpu->f = cpu->prev_instruction->flag;
	cpu->b++;
	cpu->hl++;

	CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
	
}

void indr_reverse(CPU_t *cpu) {
	int result,tmp;
	tc_sub(cpu->timer_c,16);
	if (cpu->b != 0) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21-16);
	}
	cpu->f = cpu->prev_instruction->flag;
	cpu->b--;
	cpu->hl--;
	CPU_mem_write(cpu,cpu->hl,cpu->prev_instruction->lower_data1);	
}
void ini_reverse(CPU_t *cpu) {
	int result,tmp;
	tc_sub(cpu->timer_c, 16);
	cpu->f = cpu->prev_instruction->flag;
	cpu->b++;
	cpu->hl--;

	CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
}

void inir_reverse(CPU_t *cpu) {
	int result,tmp;
	tc_sub(cpu->timer_c,16);
	if (cpu->b != 0) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21-16);
	}
	cpu->f = cpu->prev_instruction->flag;
	cpu->b++;
	cpu->hl--;

	CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);	
}

void ld_i_a_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 9);
	cpu->i = cpu->prev_instruction->lower_data1;
}
void ld_r_a_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 9);
	cpu->r = cpu->prev_instruction->lower_data1;
}
void ld_a_i_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 9);
	cpu->a = cpu->prev_instruction->lower_data1;
	cpu->f = cpu->prev_instruction->flag;
}
void ld_a_r_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 9);
	cpu->a = cpu->prev_instruction->lower_data1;
	cpu->f = cpu->prev_instruction->flag;
}

void out_reg_reverse(CPU_t *cpu) { 
	 tc_sub(cpu->timer_c, 12); 
}

void outd_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 16);
	cpu->f = cpu->prev_instruction->flag;
	cpu->b++;
	cpu->hl++;	
}

void otdr_reverse(CPU_t *cpu) {
	int result,tmp;
	tc_sub(cpu->timer_c, 16);
	
	if (cpu->b != 0) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21-16);
	}
	cpu->f = cpu->prev_instruction->flag;

	cpu->b++;
	cpu->hl++;
}
void outi_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 16);
	cpu->f = cpu->prev_instruction->flag;
	cpu->b++;
	cpu->hl--;
}

void otir_reverse(CPU_t *cpu) {
	int result,tmp;
	tc_sub(cpu->timer_c, 16);
	if (cpu->b != 0) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21-16);
	}
	cpu->f = cpu->prev_instruction->flag;
	cpu->b++;
	cpu->hl--;
}

void reti_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 14);
	cpu->sp -= 2;
	cpu->iff1 = cpu->prev_instruction->lower_data2;
	cpu->pc = cpu->prev_instruction->data1;
}
void retn_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 14);
	cpu->sp -= 2;
	cpu->iff1 = cpu->prev_instruction->lower_data2;
	cpu->pc = cpu->prev_instruction->data1;
}

//
//----------------


//-----------------
// SCF
void scf_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c,4);
	cpu->f = cpu->prev_instruction->flag;
}
//-----------------
// CCF
void ccf_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c,4);
	cpu->f = cpu->prev_instruction->flag;
}

void rst_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 11);
	cpu->pc = cpu->prev_instruction->data1;
	CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->lower_data2);
	CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->upper_data2);
}

void out_reverse(CPU_t *cpu) {
	cpu->pc--;
	tc_sub(cpu->timer_c, 11);
}
void in_reverse(CPU_t *cpu) {
	cpu->pc--;
	tc_sub(cpu->timer_c, 11);
	cpu->a = cpu->prev_instruction->lower_data1;
}

void ret_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 10);
	cpu->pc = cpu->prev_instruction->data1;
	cpu->sp -= 2;
}
void ret_condition_reverse(CPU_t *cpu) {
	int succeed = FALSE;
	tc_sub(cpu->timer_c, 5);
	switch ((cpu->bus >> 3) & 0x07) {
		case 0:	if (!(cpu->f & ZERO_MASK)) succeed = TRUE;
				break;
		case 1:	if ((cpu->f & ZERO_MASK)) succeed = TRUE;
				break;
		case 2:	if (!(cpu->f & CARRY_MASK)) succeed = TRUE;
				break;
		case 3:	if ((cpu->f & CARRY_MASK)) succeed = TRUE;
				break;
		case 4:	if (!(cpu->f & PV_MASK)) succeed = TRUE;
				break;
		case 5:	if ((cpu->f & PV_MASK)) succeed = TRUE;
				break;
		case 6:	if (!(cpu->f & SIGN_MASK)) succeed = TRUE;
				break;
		case 7:	if ((cpu->f & SIGN_MASK)) succeed = TRUE;
				break;
	}
	if (!succeed) {
		cpu->pc = cpu->prev_instruction->data1;
		cpu->sp -= 2;
		tc_sub(cpu->timer_c, 6);
	}
}
void call_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 17);
	cpu->pc = cpu->prev_instruction->data1;
	CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->lower_data2);
	CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->upper_data2);

	cpu->pc -= 2;
}

void call_condition_reverse(CPU_t *cpu) {
	int succeed = FALSE;
	int condition = (cpu->bus >> 3) & 0x07;
	unsigned short address = CPU_mem_read(cpu, --cpu->pc) << 8;
	address |= CPU_mem_read(cpu, --cpu->pc);
	
	switch (condition) {
		case 0:	if (!(cpu->f & ZERO_MASK)) succeed = TRUE;
				break;
		case 1:	if ((cpu->f & ZERO_MASK)) succeed = TRUE;
				break;
		case 2:	if (!(cpu->f & CARRY_MASK)) succeed = TRUE;
				break;
		case 3:	if ((cpu->f & CARRY_MASK)) succeed = TRUE;
				break;
		case 4:	if (!(cpu->f & PV_MASK)) succeed = TRUE;
				break;
		case 5:	if ((cpu->f & PV_MASK)) succeed = TRUE;
				break;
		case 6:	if (!(cpu->f & SIGN_MASK)) succeed = TRUE;
				break;
		case 7:	if ((cpu->f & SIGN_MASK)) succeed = TRUE;
				break;
	}
	tc_sub(cpu->timer_c, 10);
	if (!succeed) {
		cpu->pc = cpu->prev_instruction->data2;
		CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->lower_data1);
		CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->upper_data1);
		tc_sub(cpu->timer_c, 7);
	}
}

void push_reg16_reverse(CPU_t *cpu) {
	int time = 11;
	CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->lower_data1);
	CPU_mem_write(cpu, cpu->sp++, cpu->prev_instruction->upper_data1);
	
	if (((cpu->bus >> 4) & 0x03) == 2) {
		if (cpu->prefix) {
			time +=4;
		}
	}
	
	tc_sub(cpu->timer_c, time);
}

void pop_reg16_reverse(CPU_t *cpu) {
	unsigned short reg = cpu->prev_instruction->data1;
	int time = 10;
	
	switch ((cpu->bus >> 4) & 0x03) {
		case 0:
			cpu->bc = reg;
			break;
		case 1:
			cpu->de = reg;
			break;
		case 2:
			if (!cpu->prefix) {
				cpu->hl = reg;
			} else if (cpu->prefix == 0xDD) {
				cpu->ix = reg;
				time +=4;
			} else {
				cpu->iy = reg;
				time +=4;
			}
			break;
		case 3:
			cpu->af = reg;
			break;
	}
	cpu->sp -= 2;
	tc_sub(cpu->timer_c, time);
}

void ld_sp_hl_reverse(CPU_t *cpu) {
	if (!cpu->prefix) {
		cpu->sp = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c, 6);
	} else {
		cpu->sp = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c, 10);
	}
}
void ld_mem16_hlf_reverse(CPU_t *cpu) {
	unsigned short reg = CPU_mem_read(cpu, --cpu->pc);
	reg |= CPU_mem_read(cpu, --cpu->pc) << 8;

	if (!cpu->prefix) {
		CPU_mem_write(cpu, reg, cpu->prev_instruction->lower_data1);
		CPU_mem_write(cpu, --reg, cpu->prev_instruction->upper_data1);
		tc_sub(cpu->timer_c, 16);
	} else if (cpu->prefix == 0xDD) {
		CPU_mem_write(cpu, reg, cpu->prev_instruction->lower_data1);
		CPU_mem_write(cpu, --reg, cpu->prev_instruction->upper_data1);
		tc_sub(cpu->timer_c, 20);
	} else {
		CPU_mem_write(cpu, reg, cpu->prev_instruction->lower_data1);
		CPU_mem_write(cpu, --reg, cpu->prev_instruction->upper_data1);
		tc_sub(cpu->timer_c, 20);
	}		
}

void ld_hlf_mem16_reverse(CPU_t *cpu) {
	cpu->pc -= 2;
	if (!cpu->prefix) {
		cpu->hl = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c,16);
	} else if (cpu->prefix == 0xDD) {
		cpu->ix = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c,20);
	} else {
		cpu->iy = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c,20);
	}		
}

void ld_hl_num16_reverse(CPU_t *cpu) {
	cpu->pc -= 2;

	if (!cpu->prefix) {
		cpu->hl = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c, 10);
	} else if (cpu->prefix == 0xDD) {
		cpu->ix = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c, 14);
	} else {
		cpu->iy = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c, 14);
	}	
}
void ld_de_num16_reverse(CPU_t *cpu) {
	cpu->pc -= 2;
	cpu->de = cpu->prev_instruction->data1;

	tc_sub(cpu->timer_c, 10);
}
void ld_bc_num16_reverse(CPU_t *cpu) {
	cpu->pc -= 2;
	cpu->bc = cpu->prev_instruction->data1;
	tc_sub(cpu->timer_c, 10);
}
void ld_sp_num16_reverse(CPU_t *cpu) {
	cpu->pc -= 2;
	cpu->sp = cpu->prev_instruction->data1;
	tc_sub(cpu->timer_c,10);
}
void ld_a_mem16_reverse(CPU_t *cpu) {
	cpu->pc -= 2;
	cpu->a = cpu->prev_instruction->lower_data1;
	tc_sub(cpu->timer_c, 13);
}
void ld_a_bc_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->a = cpu->prev_instruction->lower_data1;
}
void ld_a_de_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->a = cpu->prev_instruction->lower_data1;
}
void ld_mem16_a_reverse(CPU_t *cpu) {
	unsigned short address = CPU_mem_read(cpu, --cpu->pc) << 8;
	address |= CPU_mem_read(cpu, --cpu->pc);
	CPU_mem_write(cpu, address, cpu->prev_instruction->lower_data1);

	tc_sub(cpu->timer_c, 13);
}
void ld_bc_a_reverse(CPU_t *cpu) {
	CPU_mem_write(cpu, cpu->bc, cpu->prev_instruction->lower_data1);
	tc_sub(cpu->timer_c, 7);
}
void ld_de_a_reverse(CPU_t *cpu) {
	CPU_mem_write(cpu, cpu->de, cpu->prev_instruction->lower_data1);
	tc_sub(cpu->timer_c, 7);
}

void ld_r_num8_reverse(CPU_t *cpu) {
	int reg;
	int test = (cpu->bus >> 3) & 7;
	tc_sub(cpu->timer_c, 7);
	reg = cpu->prev_instruction->lower_data1;
	switch(test) {
		case 0x00:
			cpu->b = reg;
			break;
		case 0x01:
			cpu->c = reg;
			break;
		case 0x02:
			cpu->d = reg;
			break;
		case 0x03:
			cpu->e = reg;
			break;
		case 0x04:
			index_ext (
				cpu->h = reg;,
				cpu->ixh = reg; tc_sub(cpu->timer_c, 4);,
				cpu->iyh = reg; tc_sub(cpu->timer_c, 4);
			)
			break;
		case 0x05:
			index_ext (
				cpu->l = reg;,
				cpu->ixl = reg; tc_sub(cpu->timer_c, 4);,
				cpu->iyl = reg; tc_sub(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				CPU_mem_write(cpu,cpu->hl, reg);
				tc_sub(cpu->timer_c, 3);
			} else {
				char offset = reg;
				reg = CPU_mem_read(cpu, --cpu->pc);
				if (cpu->prefix == 0xDD) {
					CPU_mem_write(cpu, cpu->ix + offset, reg);
				} else {
					CPU_mem_write(cpu, cpu->iy + offset, reg);
				}
				tc_sub(cpu->timer_c, 7);
			}
			break;
		case 0x07:
			cpu->a = reg;
			break;
	}
}

void ld_r_r_reverse(CPU_t *cpu) {
	int test = (cpu->bus >> 3) & 7;
	int test2 = cpu->bus & 7;
	tc_sub(cpu->timer_c, 4);

	int reg = cpu->prev_instruction->data1;
	switch(test2) {
		case 0x04:
		case 0x05:
			if (cpu->prefix && test == 6)
				break;
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				tc_sub(cpu->timer_c, 7);
			}
			break;
	}

	switch(test) {
		case 0x00:
			cpu->b = reg;
			break;
		case 0x01:
			cpu->c = reg;
			break;
		case 0x02:
			cpu->d = reg;
			break;
		case 0x03:
			cpu->e = reg;
			break;
		case 0x04:
			reg = cpu->prev_instruction->data1;
			if (cpu->prefix && test2 == 6)
				break;
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x05:
			if (cpu->prefix && test2 == 6) {
				cpu->l = reg;
				break;
			}
			index_ext (
				cpu->l = reg;,
				cpu->ixl = reg; tc_sub(cpu->timer_c, 4);,
				cpu->iyl = reg; tc_sub(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				CPU_mem_write(cpu,cpu->hl, reg);
				tc_sub(cpu->timer_c, 3);
			} else {
				char offset = CPU_mem_read(cpu, --cpu->pc);
				if (cpu->prefix == 0xDD) {
					CPU_mem_write(cpu, cpu->ix + offset, reg);
				} else {
					CPU_mem_write(cpu, cpu->iy + offset, reg);
				}
				tc_sub(cpu->timer_c, 7);
			}
			break;
		case 0x07:
			cpu->a = reg;
			break;
	}
}

void halt_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->halt = FALSE;
//	cpu->pc--;			//I'll tell you later...just to sync up.
}

void nop_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
}

void ex_sp_hl_reverse(CPU_t *cpu) {
	unsigned short reg = CPU_mem_read(cpu, cpu->sp);
	reg |= CPU_mem_read(cpu, cpu->sp + 1) << 8;
	
	if (!cpu->prefix) {
		tc_sub(cpu->timer_c, 19);
		CPU_mem_write(cpu,cpu->sp + 1, cpu->h);
		CPU_mem_write(cpu,cpu->sp, cpu->l);
		cpu->hl = reg;
	} else {
		if (cpu->prefix == 0xDD) {
			tc_sub(cpu->timer_c, 23);
			CPU_mem_write(cpu,cpu->sp + 1, cpu->ixh);
			CPU_mem_write(cpu,cpu->sp, cpu->ixl);
			cpu->ix = reg;
		} else {
			CPU_mem_write(cpu,cpu->sp + 1, cpu->iyh);
			CPU_mem_write(cpu,cpu->sp, cpu->iyl);
			cpu->iy = reg;
		}
	}
}



#define swappair( regpair1 , regpair2 ) \
reg = regpair1; \
regpair1 = regpair2; \
regpair2 = reg;

void ex_de_hl_reverse(CPU_t *cpu) {
	unsigned short reg;
	swappair(cpu->hl, cpu->de);
	tc_sub(cpu->timer_c, 4);
}

void exx_reverse(CPU_t *cpu) {
	unsigned short reg;
	swappair(cpu->hl, cpu->hlp);
	swappair(cpu->bc, cpu->bcp);
	swappair(cpu->de, cpu->dep);
	tc_sub(cpu->timer_c, 4);
}

void ex_af_afp_reverse(CPU_t *cpu) {
	unsigned short reg;
	swappair(cpu->af, cpu->afp);
	tc_sub(cpu->timer_c,4);
}	

void jp_hl_reverse(CPU_t *cpu) {
	if (!cpu->prefix) {
		cpu->pc = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c, 4);
	} else {
		cpu->pc = cpu->prev_instruction->data1;
		tc_sub(cpu->timer_c, 8);
	}
}

void jp_reverse(CPU_t *cpu) {
	cpu->pc = cpu->prev_instruction->data1;
	tc_sub(cpu->timer_c, 10);
}

void jr_reverse(CPU_t *cpu) {
	cpu->pc = cpu->prev_instruction->data1;
	tc_sub(cpu->timer_c, 12);
}

void jp_condition_reverse(CPU_t *cpu) {
	cpu->pc = cpu->prev_instruction->data1;

	tc_sub(cpu->timer_c, 10);
}

void jr_condition_reverse(CPU_t *cpu) {
	cpu->pc = cpu->prev_instruction->data1;
	int condition = (CPU_mem_read(cpu, cpu->pc) >> 3) & 3;

	tc_sub(cpu->timer_c, 7);
	switch (condition) {
		case 0:
			if (!(ZERO_MASK & cpu->f))
				tc_sub(cpu->timer_c, 5);
			break;
		case 1:
			if (ZERO_MASK & cpu->f)
				tc_sub(cpu->timer_c, 5);
			break;
		case 2:
			if (!(CARRY_MASK & cpu->f))
				tc_sub(cpu->timer_c, 5);
			break;
		case 3:
			if (CARRY_MASK & cpu->f)
				tc_sub(cpu->timer_c, 5);
			break;
	}
}

void djnz_reverse(CPU_t *cpu) {
	cpu->pc = cpu->prev_instruction->data1;
	tc_sub(cpu->timer_c, 8);
	if (!cpu->b++) {
		tc_sub(cpu->timer_c, 5);
	}
}

void ei_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);

	cpu->iff1 = cpu->prev_instruction->lower_data1;
	cpu->iff2 = cpu->prev_instruction->lower_data2;
	cpu->ei_block = cpu->prev_instruction->upper_data1;
}

void di_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->iff1 = cpu->prev_instruction->lower_data1;
	cpu->iff2 = cpu->prev_instruction->lower_data2;
}
#endif