#ifdef WITH_REVERSE
#include "stdafx.h"

#include "core.h"
#include "alu_reverse.h"

//---------------------------------------------
// ED OPCODES

void neg_reverse(CPU_t *cpu) {
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = -cpu->a;
	tc_sub(cpu->timer_c, 8);
}

void adc_hl_reg16_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 15);
	cpu->f = cpu->prev_instruction->flag;
	cpu->hl = cpu->prev_instruction->data1;
}

void sbc_hl_reg16_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 15);
	cpu->f = cpu->prev_instruction->flag;
	cpu->hl = cpu->prev_instruction->data1;
}

void cpd_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 16);
	cpu->f = cpu->prev_instruction->flag;
	cpu->hl++;
	cpu->bc++;
}

void cpdr_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 16);
	if ((cpu->f & PV_MASK) && !(cpu->f & ZERO_MASK)) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21 - 16);
	}
	cpu->f = cpu->prev_instruction->flag;
	cpu->bc++;
	cpu->hl++;	
}

void cpi_reverse(CPU_t *cpu) {
	int result;
	int reg;
	tc_sub(cpu->timer_c, 16);
	cpu->f = cpu->prev_instruction->flag;

	cpu->hl--;
	cpu->bc++;
}

void cpir_reverse(CPU_t *cpu) {
	int result;
	int reg;
	tc_sub(cpu->timer_c, 16);
	
	if ((cpu->f & PV_MASK) && !(cpu->f & ZERO_MASK)) {
		cpu->pc += 2;
		tc_sub(cpu->timer_c, 21 - 16);
	}
	cpu->f = cpu->prev_instruction->flag;
	cpu->hl--;
	cpu->bc++;
}


void rld_reverse(CPU_t *cpu) {
	int result;

	tc_sub(cpu->timer_c, 18);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->data2);
}

void rrd_reverse(CPU_t *cpu) {
	int result, tmp;
	tc_sub(cpu->timer_c, 18);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->data2);
}


//-----------------------------------------
// CB OPCODES


//------------------
// Bit num,reg
void bit_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;
}
//------------------
// RES num,reg
void res_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c, 7);
			CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}

}
//------------------
// SET num,reg
void set_reverse(CPU_t *cpu) {
	int reg;
	tc_sub(cpu->timer_c, 8);
	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}

}


void rl_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c, 7);
			CPU_mem_write(cpu,cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}

void rlc_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 0x07) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c, 7);
			CPU_mem_write(cpu,cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}

void rr_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;
	switch (cpu->bus & 7) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c, 7);
			CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}
void rrc_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 7) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c, 7);
			CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}

void sll_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 7) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}
void sla_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 7) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c, 7);
			CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}

void sra_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 7) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c,7);
			CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}
void srl_reg_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 8);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 7) {
		case 0x00:
			cpu->b = cpu->prev_instruction->lower_data1;
			break;
		case 0x01:
			cpu->c = cpu->prev_instruction->lower_data1;
			break;
		case 0x02:
			cpu->d = cpu->prev_instruction->lower_data1;
			break;
		case 0x03:
			cpu->e = cpu->prev_instruction->lower_data1;
			break;
		case 0x04:
			cpu->h = cpu->prev_instruction->lower_data1;
			break;
		case 0x05:
			cpu->l = cpu->prev_instruction->lower_data1;
			break;
		case 0x06:
			tc_sub(cpu->timer_c,7);
			CPU_mem_write(cpu, cpu->hl, cpu->prev_instruction->lower_data1);
			break;
		case 0x07:
			cpu->a = cpu->prev_instruction->lower_data1;
			break;
	}
}
// END CB OPCODES
//---------------------------------------


//-----------------
// and reg8
void and_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;

	switch (cpu->bus & 7) {
		case 0x04:
		case 0x05:
				tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				cpu->pc--;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
	
}



//-----------------
// and num8
void and_num8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	cpu->pc--;
}



//-----------------
// or reg8
void or_reg8_reverse(CPU_t *cpu) {
	int result;
	int reg;
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;

	switch (cpu->bus &7) {
		case 0x04:
		case 0x05:
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				cpu->pc--;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
}

//-----------------
// or num8
void or_num8_reverse(CPU_t *cpu) {
	int result;
	int reg;
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	cpu->pc--;
}

//-----------------
// xor reg8
void xor_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;

	switch (cpu->bus & 7) {
		case 0x04:
		case 0x05:
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c,3);
			} else {
				cpu->pc++;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
}


//-----------------
// xor num8
void xor_num8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	cpu->pc--;
}

void cp_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;

	switch (cpu->bus & 7) {
		case 0x04:
		case 0x05:
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				cpu->pc--;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
}

void cp_num8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->pc--;
}

void cpl_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
}

void daa_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
}

void dec_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	switch ((cpu->bus >> 3) & 7) {
		case 0x00:
			cpu->b++;
			break;
		case 0x01:
			cpu->c++;
			break;
		case 0x02:
			cpu->d++;
			break;
		case 0x03:
			cpu->e++;
			break;
		case 0x04:
			if (!cpu->prefix) {
				cpu->h++;
			} else if (cpu->prefix == 0xDD) {
				cpu->ixh++;
				tc_sub(cpu->timer_c, 4);
			} else {
				cpu->iyh++;
				tc_sub(cpu->timer_c, 4);
			}
			break;
		case 0x05:
			if (!cpu->prefix) {
				cpu->l++;
			} else if (cpu->prefix == 0xDD) {
				cpu->ixl++;
				tc_sub(cpu->timer_c, 4);
			} else {
				cpu->iyl++;
				tc_sub(cpu->timer_c, 4);
			}
			break;
		case 0x06:
			if (!cpu->prefix) {
				int reg = CPU_mem_read(cpu, cpu->hl);
				reg++;
				CPU_mem_write(cpu,cpu->hl, reg);
				tc_sub(cpu->timer_c, 7);
			} else {
				char offset = CPU_mem_read(cpu, --cpu->pc);
				if (cpu->prefix == 0xDD) {
					int reg = CPU_mem_read(cpu,cpu->ix+offset);
					reg++;
					CPU_mem_write(cpu,cpu->ix+offset, reg);
					tc_sub(cpu->timer_c, 19 - 4);
				} else {
					int reg = CPU_mem_read(cpu,cpu->iy+offset);
					reg++;
					CPU_mem_write(cpu,cpu->iy+offset, reg);
					tc_sub(cpu->timer_c, 19 - 4);
				}
			}
			break;
		case 0x07:
			cpu->a++;
			break;
	}
}


void inc_reg8_reverse(CPU_t *cpu) {
	cpu->f = cpu->prev_instruction->flag;
	switch ((cpu->bus >> 3) & 7) {
		case 0x00:
			cpu->b--;
			break;
		case 0x01:
			cpu->c--;
			break;
		case 0x02:
			cpu->d--;
			break;
		case 0x03:
			cpu->e--;
			break;
		case 0x04:
			if (!cpu->prefix) {
				cpu->h--;
			} else if (cpu->prefix == 0xDD) {
				cpu->ixh--;
				tc_sub(cpu->timer_c, 4);
			} else {
				cpu->iyh--;
				tc_sub(cpu->timer_c, 4);
			}
			break;
		case 0x05:
			if (!cpu->prefix) {
				cpu->l--;
			} else if (cpu->prefix == 0xDD) {
				cpu->ixl--;
				tc_sub(cpu->timer_c, 4);
			} else {
				cpu->iyl--;
				tc_sub(cpu->timer_c, 4);
			}
			break;
		case 0x06:
			if (!cpu->prefix) {
				int reg = CPU_mem_read(cpu, cpu->hl);
				reg--;
				CPU_mem_write(cpu,cpu->hl, reg);
				tc_sub(cpu->timer_c, 7);
			} else {
				char offset = CPU_mem_read(cpu, --cpu->pc);
				if (cpu->prefix == 0xDD) {
					int reg = CPU_mem_read(cpu,cpu->ix+offset);
					reg--;
					CPU_mem_write(cpu,cpu->ix+offset, reg);
					tc_sub(cpu->timer_c, 19 - 4);
				} else {
					int reg = CPU_mem_read(cpu,cpu->iy+offset);
					reg--;
					CPU_mem_write(cpu,cpu->iy+offset, reg);
					tc_sub(cpu->timer_c, 19 - 4);
				}
			}
			break;
		case 0x07:
			cpu->a--;
			break;
	}
}

void adc_a_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;

	switch (cpu->bus & 7) {
		case 0x04:
		case 0x05:
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				cpu->pc--;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
}
void add_a_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;

	switch (cpu->bus & 7) {
		case 0x04:
		case 0x05:
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				cpu->pc--;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
}


void adc_a_num8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->pc--;
	cpu->a = cpu->prev_instruction->lower_data1;
}
void add_a_num8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->pc--;
	cpu->a = cpu->prev_instruction->lower_data1;
}

void sbc_a_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;

	switch ((cpu->bus)&7) {
		case 0x04:
		case 0x05:
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				cpu->pc--;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
}
void sub_a_reg8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;

	switch ((cpu->bus)&7) {
		case 0x04:
		case 0x05:
			tc_sub(cpu->timer_c, 4);
			break;
		case 0x06:
			if (!cpu->prefix) {
				tc_sub(cpu->timer_c, 3);
			} else {
				cpu->pc--;
				tc_sub(cpu->timer_c, 15);
			}
			break;
	}
}

void sbc_a_num8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	cpu->pc--;
}
void sub_a_num8_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 7);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	cpu->pc--;
}


void dec_reg16_reverse(CPU_t *cpu) {
	int time = 6;
	switch ((cpu->bus >> 4) & 3) {
		case 0x00:
			cpu->bc++;
			break;
		case 0x01:
			cpu->de++;
			break;
		case 0x02:
			index_ext(
				cpu->hl++;,
				cpu->ix++; time+=4;,
				cpu->iy++; time+=4;
			)
			break;
		case 0x03:
			cpu->sp++;
			break;
	}
	tc_sub(cpu->timer_c, time);
}

void inc_reg16_reverse(CPU_t *cpu) {
	int time = 6;
	switch ((cpu->bus >> 4) & 3) {
		case 0x00:
			cpu->bc--;
			break;
		case 0x01:
			cpu->de--;
			break;
		case 0x02:
			index_ext(
				cpu->hl--;,
				cpu->ix--; time+=4;,
				cpu->iy--; time+=4;
			)
			break;
		case 0x03:
			cpu->sp--;
			break;
	}
	tc_sub(cpu->timer_c, time);
}

void add_hl_reg16_reverse(CPU_t *cpu) {
	cpu->f = cpu->prev_instruction->flag;

	int time = 11; 
	switch ((cpu->bus >> 4) & 3) {
		case 0x02:
			if (cpu->prefix == 0xDD) {
				time+=4;
			} else if (cpu->prefix == 0xFD) {
				time+=4;
			}
			break;
	}

	if (!cpu->prefix) {
		cpu->hl = cpu->prev_instruction->data1;
	} else if (cpu->prefix == 0xDD) {
		cpu->ix = cpu->prev_instruction->data1;
	} else {
		cpu->iy = cpu->prev_instruction->data1;
	}

	tc_sub(cpu->timer_c, time);
}


void rla_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	
}
void rlca_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
}
void rra_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	
}
void rrca_reverse(CPU_t *cpu) {
	tc_sub(cpu->timer_c, 4);
	cpu->f = cpu->prev_instruction->flag;
	cpu->a = cpu->prev_instruction->lower_data1;
	
}
#endif