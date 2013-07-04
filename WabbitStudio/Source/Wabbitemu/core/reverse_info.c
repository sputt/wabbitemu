#include "stdafx.h"

#ifdef WITH_REVERSE
#include "core.h"
//ALU.c

void adc_hl_reg16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->hl;
}

void sbc_hl_reg16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->hl;
}

void rld_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
	cpu->prev_instruction->data2 = mem_read(cpu->mem_c, cpu->hl);
}

void rrd_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
	cpu->prev_instruction->data2 = mem_read(cpu->mem_c, cpu->hl);
}

void res_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}

}

void set_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}


void rl_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void rlc_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void rr_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}
void rrc_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void sll_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}
void sla_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void sra_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}
void srl_reg_reverse_info(CPU_t *cpu) {
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 0x05:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 0x06:
			cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void and_reg8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void and_num8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}



void or_reg8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void or_num8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void xor_reg8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void xor_num8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void cpl_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void daa_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void adc_a_reg8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void add_a_reg8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void adc_a_num8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void add_a_num8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void sbc_a_reg8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void sub_a_reg8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void sbc_a_num8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void sub_a_num8_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void add_hl_reg16_reverse_info(CPU_t *cpu) {
	if (!cpu->prefix) {
		cpu->prev_instruction->data1 = cpu->hl;
	} else if (cpu->prefix == 0xDD) {
		cpu->prev_instruction->data1 = cpu->ix;
	} else {
		cpu->prev_instruction->data1 = cpu->iy;
	}
}

void rla_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
	
}
void rlca_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void rra_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
	
}
void rrca_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;	
}
//end alu.c

//Control.c
void ldd_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = mem_read(cpu->mem_c, cpu->de);
}

void lddr_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = mem_read(cpu->mem_c, cpu->de);
}

void ldi_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = mem_read(cpu->mem_c, cpu->de);
}

void ldir_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = mem_read(cpu->mem_c, cpu->de);
}

void ld_mem16_reg16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = mem_read(cpu->mem_c, cpu->pc);
	cpu->prev_instruction->data1 |= mem_read(cpu->mem_c, cpu->pc + 1) << 8;
}

void ld_reg16_mem16_reverse_info(CPU_t *cpu) {
	int test = (mem_read(cpu->mem_c, cpu->pc) >> 4) & 3;

	switch (test) {
		case 00:
			cpu->prev_instruction->data1 = cpu->bc;
			break;
		case 01:
			cpu->prev_instruction->data1 = cpu->de;
			break;
		case 02:
			cpu->prev_instruction->data1 = cpu->hl;
			break;
		case 03:
			cpu->prev_instruction->data1 = cpu->sp;
			break;
	}
}

void IM0_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->imode;
}
void IM1_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->imode;
}
void IM2_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->imode;
}

void ind_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
}

void indr_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
}
void ini_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
}

void inir_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);	
}

void ld_i_a_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->i;
}
void ld_r_a_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->r;
}
void ld_a_i_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void ld_a_r_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void reti_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data2 = cpu->iff1;
	cpu->prev_instruction->data1 = cpu->pc;
}
void retn_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data2 = cpu->iff1;
	cpu->prev_instruction->data1 = cpu->pc;
}

void rst_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
	cpu->prev_instruction->lower_data2 = mem_read(cpu->mem_c, cpu->sp);
	cpu->prev_instruction->upper_data2 = mem_read(cpu->mem_c, cpu->sp + 1);
}

void in_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}

void ret_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}
void ret_condition_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}
void call_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
	cpu->prev_instruction->lower_data2 = mem_read(cpu->mem_c, cpu->sp);
	cpu->prev_instruction->upper_data2 = mem_read(cpu->mem_c, cpu->sp + 1);
}

void call_condition_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data2 = cpu->pc;
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->sp);
	cpu->prev_instruction->upper_data1 = mem_read(cpu->mem_c, cpu->sp + 1);
}

void push_reg16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->sp);
	cpu->prev_instruction->upper_data1 = mem_read(cpu->mem_c, cpu->sp + 1);
}

void pop_reg16_reverse_info(CPU_t *cpu) {
	switch ((cpu->bus >> 4) & 0x03) {
		case 0:
			cpu->prev_instruction->data1 = cpu->bc;
			break;
		case 1:
			cpu->prev_instruction->data1 = cpu->de;
			break;
		case 2:
			if (!cpu->prefix) {
				cpu->prev_instruction->data1 = cpu->hl;
			} else if (cpu->prefix == 0xDD) {
				cpu->prev_instruction->data1 = cpu->ix;
			} else {
				cpu->prev_instruction->data1 = cpu->iy;
			}
			break;
		case 3:
			cpu->prev_instruction->data1 = cpu->af;
			break;
	}
}

void ld_sp_hl_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->sp;
}

void ld_mem16_hlf_reverse_info(CPU_t *cpu) {
	unsigned short reg = mem_read(cpu->mem_c, cpu->pc - 1);
	reg |= mem_read(cpu->mem_c, cpu->pc - 2) << 8;

	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, reg);
	cpu->prev_instruction->lower_data2 = mem_read(cpu->mem_c, --reg);
}

void ld_hlf_mem16_reverse_info(CPU_t *cpu) {
	if (!cpu->prefix) {
		cpu->prev_instruction->data1 = cpu->hl;
	} else if (cpu->prefix == 0xDD) {
		cpu->prev_instruction->data1 = cpu->ix;
	} else {
		cpu->prev_instruction->data1 = cpu->iy;
	}		
}

void ld_hl_num16_reverse_info(CPU_t *cpu) {
	if (!cpu->prefix) {
		cpu->prev_instruction->data1 = cpu->hl;
	} else if (cpu->prefix == 0xDD) {
		cpu->prev_instruction->data1 = cpu->ix;
	} else {
		cpu->prev_instruction->data1 = cpu->iy;
	}
}
void ld_de_num16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->de;
}
void ld_bc_num16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->bc;
}
void ld_sp_num16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->sp;
}
void ld_a_mem16_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void ld_a_bc_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void ld_a_de_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->a;
}
void ld_mem16_a_reverse_info(CPU_t *cpu) {
	unsigned short address = mem_read(cpu->mem_c, cpu->pc) << 8;
	address |= mem_read(cpu->mem_c, cpu->pc + 1);
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, address);
}
void ld_bc_a_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->bc);
}
void ld_de_a_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->de);
}

void ld_r_num8_reverse_info(CPU_t *cpu) {
	int test = (cpu->bus >> 3) & 7;
	switch(test) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			index_ext (
				cpu->prev_instruction->lower_data1 = cpu->h;,
				cpu->prev_instruction->lower_data1 = cpu->ixh;,
				cpu->prev_instruction->lower_data1 = cpu->iyh;
			)
			break;
		case 0x05:
			index_ext (
				cpu->prev_instruction->lower_data1 = cpu->l;,
				cpu->prev_instruction->lower_data1 = cpu->ixl;,
				cpu->prev_instruction->lower_data1 = cpu->iyl;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			} else {
				char offset = mem_read(cpu->mem_c, cpu->pc);
				if (cpu->prefix == 0xDD) {
					cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
				} else {
					cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
				}
			}
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void ld_r_r_reverse_info(CPU_t *cpu) {
	int test = (cpu->bus >> 3) & 7;

	switch(test) {
		case 0x00:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 0x01:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 0x02:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 0x03:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 0x04:
			index_ext (
				cpu->prev_instruction->lower_data1 = cpu->h;,
				cpu->prev_instruction->lower_data1 = cpu->ixh;,
				cpu->prev_instruction->lower_data1 = cpu->iyh;
			)
			break;
		case 0x05:
			index_ext (
				cpu->prev_instruction->lower_data1 = cpu->l;,
				cpu->prev_instruction->lower_data1 = cpu->ixl;,
				cpu->prev_instruction->lower_data1 = cpu->iyl;
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->hl);
			} else {
				char offset = mem_read(cpu->mem_c, cpu->pc);
				if (cpu->prefix == 0xDD) {
					cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
				} else {
					cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
				}
			}
			break;
		case 0x07:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void halt_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->halt;
}

void jp_hl_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}

void jp_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}

void jr_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}

void jr_condition_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}

void jp_condition_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}

void djnz_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->data1 = cpu->pc;
}

void ei_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->iff1;
	cpu->prev_instruction->lower_data2 = cpu->iff2;
	cpu->prev_instruction->upper_data1 = cpu->ei_block;
}

void di_reverse_info(CPU_t *cpu) {
	cpu->prev_instruction->lower_data1 = cpu->iff1;
	cpu->prev_instruction->lower_data2 = cpu->iff2;
}
//End control.c

//indexcb.c

void res_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data2 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data2 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}

void set_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data2 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data2 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data1 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data1 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data1 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data1 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data1 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data1 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data1 = cpu->a;
			break;
	}
}


void rl_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	}
}

void rlc_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	}
}

void rr_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	}
}

void rrc_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	}
}

void sll_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	} 
}

void srl_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	}
}

void sla_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	}
}

void sra_ind_reverse_info(CPU_t *cpu, char offset) {
	int save = cpu->bus & 0x07;
	
	if (cpu->prefix == IX_PREFIX) {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->ix + offset);
	} else {
		cpu->prev_instruction->lower_data1 = mem_read(cpu->mem_c, cpu->iy + offset);
	}
	
	switch(save) {
		case 0:
			cpu->prev_instruction->lower_data2 = cpu->b;
			break;
		case 1:
			cpu->prev_instruction->lower_data2 = cpu->c;
			break;
		case 2:
			cpu->prev_instruction->lower_data2 = cpu->d;
			break;
		case 3:
			cpu->prev_instruction->lower_data2 = cpu->e;
			break;
		case 4:
			cpu->prev_instruction->lower_data2 = cpu->h;
			break;
		case 5:
			cpu->prev_instruction->lower_data2 = cpu->l;
			break;
		case 7:
			cpu->prev_instruction->lower_data2 = cpu->a;
			break;
	}
}
//end indexcb.c
#endif