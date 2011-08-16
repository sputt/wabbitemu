#include "stdafx.h"

#include "core.h"
#include "alu.h"

//---------------------------------------------
// ED OPCODES

void neg(CPU_t *cpu) {
	tc_add(cpu->timer_c, 8);
	int result = -cpu->a;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(0,cpu->a,0) + 
		 x3chk(result)+ vchksub(0,cpu->a,result) + 
		 SUB_INSTR +  carrychk(result);
	cpu->a = result;
}

void adc_hl_reg16(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 15);
	switch (((cpu->bus)>>4)&0x03) {
		case 0x00:
			reg = cpu->bc;
			break;
		case 0x01:
			reg = cpu->de;
			break;
		case 0x02:
			reg = cpu->hl;
			break;
		case 0x03:
			reg = cpu->sp;
			break;
	}
	result = cpu->hl+reg+ (cpu->f&CARRY_MASK);
	cpu->f = signchk16(result) + zerochk16(result) + 
		 x5chk16(result) + hcaddchk16(cpu->hl,reg,cpu->f&CARRY_MASK) + 
		 x3chk16(result)+ vchkadd16(cpu->hl,reg,result) + 
		 ADD_INSTR +  carrychk16(result);
	cpu->hl = result;
}

void sbc_hl_reg16(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 15);
	switch (((cpu->bus)>>4)&0x03) {
		case 0x00:
			reg = cpu->bc;
			break;
		case 0x01:
			reg = cpu->de;
			break;
		case 0x02:
			reg = cpu->hl;
			break;
		case 0x03:
			reg = cpu->sp;
			break;
	}
	result = cpu->hl - reg - (cpu->f&CARRY_MASK);

	cpu->f = signchk16(result) + zerochk16(result) + 
		 x5chk16(result) + hcsubchk16(cpu->hl,reg,cpu->f&CARRY_MASK) + 
		 x3chk16(result)+ vchksub16(cpu->hl, reg, result) +                //DOUBLE CHECK!!!!
		 SUB_INSTR +  carrychk16(result);
	cpu->hl = result;
}



void cpd(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 16);
	reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl--;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
}

void cpdr(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 16);
	reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl--;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
	if ((cpu->f&PV_MASK)!=0 && (cpu->f&ZERO_MASK)==0 ) {
		cpu->pc-=2;
		tc_add(cpu->timer_c, 21-16);
	}
		
}

void cpi(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 16);
	reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl++;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
}

void cpir(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 16);
	reg = CPU_mem_read(cpu,cpu->hl);
	result = cpu->a - reg;
	cpu->bc--;
	cpu->hl++;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg-((cpu->f&HC_MASK)>>4)) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg-((cpu->f&HC_MASK)>>4))+ doparity(cpu->bc!=0) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
	if ((cpu->f&PV_MASK)!=0 && (cpu->f&ZERO_MASK)==0 ) {
		cpu->pc-=2;
		tc_add(cpu->timer_c, 21-16);
	}
		
}


void rld(CPU_t *cpu) {
	int result;

	tc_add(cpu->timer_c,18);
	result = (CPU_mem_read(cpu,cpu->hl)<<4)+(cpu->a&0x0f);
	CPU_mem_write(cpu,cpu->hl,result&0xff);
	cpu->a = (cpu->a&0xF0)+((result>>8)&0x0F);
	cpu->f = signchk(cpu->a) + zerochk(cpu->a) +
		 x5chk(cpu->a) + x3chk(cpu->a)+ 
		 parity(cpu->a) + unaffect(CARRY_MASK);
}
void rrd(CPU_t *cpu) {
	int result,tmp;
	tc_add(cpu->timer_c,18);
	result = (CPU_mem_read(cpu,cpu->hl)>>4)+((cpu->a&0x0f)<<4);
	tmp = cpu->bus;
	CPU_mem_write(cpu,cpu->hl,result&0xff);
	cpu->a = (cpu->a&0xF0)+(tmp&0x0F);
	cpu->f = signchk(cpu->a) + zerochk(cpu->a) +
		 x5chk(cpu->a) + x3chk(cpu->a)+ 
		 parity(cpu->a) + unaffect(CARRY_MASK);
}


//-----------------------------------------
// CB OPCODES


//------------------
// Bit num,reg
void bit(CPU_t *cpu) {
	int result;
	int reg;
	int dbus = cpu->bus;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&0x07) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			reg = cpu->h;
			break;
		case 0x05:
			reg = cpu->l;
			break;
		case 0x06:
			reg = CPU_mem_read(cpu,cpu->hl);
			tc_add(cpu->timer_c,4);
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = reg & (1<<((dbus>>3)&0x07));
	int xchk;
	if ( (dbus&0x07)!=0x06 ) xchk = result;
	else xchk = cpu->h;
		xchk = result;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(xchk) + HC_MASK + 
		 x3chk(xchk)+ parity(result) + unaffect(CARRY_MASK);
}
//------------------
// RES num,reg
void res(CPU_t *cpu) {
	int reg;
	int bit =	(~(1<<((cpu->bus>>3)&0x07)))&0xFF;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&0x07) {
		case 0x00:
			cpu->b &= bit;
			break;
		case 0x01:
			cpu->c &= bit;
			break;
		case 0x02:
			cpu->d &= bit;
			break;
		case 0x03:
			cpu->e &= bit;
			break;
		case 0x04:
			cpu->h &= bit;
			break;
		case 0x05:
			cpu->l &= bit;
			break;
		case 0x06:
			reg = CPU_mem_read(cpu,cpu->hl);
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,reg&bit);
			break;
		case 0x07:
			cpu->a &= bit;
			break;
	}

}
//------------------
// SET num,reg
void set(CPU_t *cpu) {
	int reg;
	int bit = (1<<((cpu->bus>>3)&0x7))&0xFF;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&0x07) {
		case 0x00:
			cpu->b |= bit;
			break;
		case 0x01:
			cpu->c |= bit;
			break;
		case 0x02:
			cpu->d |= bit;
			break;
		case 0x03:
			cpu->e |= bit;
			break;
		case 0x04:
			cpu->h |= bit;
			break;
		case 0x05:
			cpu->l |= bit;
			break;
		case 0x06:
			reg = CPU_mem_read(cpu,cpu->hl);
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,reg|bit);
			break;
		case 0x07:
			cpu->a |= bit;
			break;
	}

}


void rl_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&0x07) {
		case 0x00:
			carry = (cpu->b>>7)&0x01;
			result = cpu->b = ((cpu->b<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&0x01;
			result = cpu->c = ((cpu->c<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&0x01;
			result = cpu->d = ((cpu->d<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&0x01;
			result = cpu->e = ((cpu->e<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&0x01;
			result = cpu->h = ((cpu->h<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&0x01;
			result = cpu->l = ((cpu->l<<1)+(cpu->f&0x01))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&0x01;
			result = ((result<<1)+(cpu->f&0x01))&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&0x01;
			result = cpu->a = ((cpu->a<<1)+(cpu->f&0x01))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}

void rlc_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&0x07) {
		case 0x00:
			carry = (cpu->b>>7)&1;
			result = cpu->b = ((cpu->b<<1)+carry)&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&1;
			result = cpu->c = ((cpu->c<<1)+carry)&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&1;
			result = cpu->d = ((cpu->d<<1)+carry)&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&1;
			result = cpu->e = ((cpu->e<<1)+carry)&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&1;
			result = cpu->h = ((cpu->h<<1)+carry)&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&1;
			result = cpu->l = ((cpu->l<<1)+carry)&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&1;
			result = ((result<<1)+carry)&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&1;
			result = cpu->a = ((cpu->a<<1)+carry)&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}

void rr_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x01:
			carry = cpu->c&1;
			result = cpu->c = ((cpu->c>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x02:
			carry = cpu->d&1;
			result = cpu->d = ((cpu->d>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x03:
			carry = cpu->e&1;
			result = cpu->e = ((cpu->e>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x04:
			carry = cpu->h&1;
			result = cpu->h = ((cpu->h>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x05:
			carry = cpu->l&1;
			result = cpu->l = ((cpu->l>>1)+((cpu->f&1)<<7))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = result&1;
			result = ((result>>1)+((cpu->f&1)<<7))&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = cpu->a&1;
			result = cpu->a = ((cpu->a>>1)+((cpu->f&1)<<7))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}
void rrc_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1)+(carry<<7))&0xFF;
			break;
		case 0x01:
			carry = cpu->c&1;
			result = cpu->c = ((cpu->c>>1)+(carry<<7))&0xFF;
			break;
		case 0x02:
			carry = cpu->d&1;
			result = cpu->d = ((cpu->d>>1)+(carry<<7))&0xFF;
			break;
		case 0x03:
			carry = cpu->e&1;
			result = cpu->e = ((cpu->e>>1)+(carry<<7))&0xFF;
			break;
		case 0x04:
			carry = cpu->h&1;
			result = cpu->h = ((cpu->h>>1)+(carry<<7))&0xFF;
			break;
		case 0x05:
			carry = cpu->l&1;
			result = cpu->l = ((cpu->l>>1)+(carry<<7))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = result&1;
			result = ((result>>1)+(carry<<7))&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = cpu->a&1;
			result = cpu->a = ((cpu->a>>1)+(carry<<7))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}

void sll_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = (cpu->b>>7)&1;
			result = cpu->b = ((cpu->b<<1)+1)&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&1;
			result = cpu->c = ((cpu->c<<1)+1)&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&1;
			result = cpu->d = ((cpu->d<<1)+1)&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&1;
			result = cpu->e = ((cpu->e<<1)+1)&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&1;
			result = cpu->h = ((cpu->h<<1)+1)&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&1;
			result = cpu->l = ((cpu->l<<1)+1)&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&1;
			result = ((result<<1)+1)&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&1;
			result = cpu->a = ((cpu->a<<1)+1)&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}
void sla_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = (cpu->b>>7)&1;
			result = cpu->b = ((cpu->b<<1))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c>>7)&1;
			result = cpu->c = ((cpu->c<<1))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d>>7)&1;
			result = cpu->d = ((cpu->d<<1))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e>>7)&1;
			result = cpu->e = ((cpu->e<<1))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h>>7)&1;
			result = cpu->h = ((cpu->h<<1))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l>>7)&1;
			result = cpu->l = ((cpu->l<<1))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result>>7)&1;
			result = ((result<<1))&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a>>7)&1;
			result = cpu->a = ((cpu->a<<1))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}

void sra_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1)+(cpu->b&128))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c)&1;
			result = cpu->c = ((cpu->c>>1)+(cpu->c&128))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d)&1;
			result = cpu->d = ((cpu->d>>1)+(cpu->d&128))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e)&1;
			result = cpu->e = ((cpu->e>>1)+(cpu->e&128))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h)&1;
			result = cpu->h = ((cpu->h>>1)+(cpu->h&128))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l)&1;
			result = cpu->l = ((cpu->l>>1)+(cpu->l&128))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result)&1;
			result = ((result>>1)+(result&128))&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a)&1;
			result = cpu->a = ((cpu->a>>1)+(cpu->a&128))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}
void srl_reg(CPU_t *cpu) {
	int result;
	int carry;
	tc_add(cpu->timer_c,8);
	switch ((cpu->bus)&7) {
		case 0x00:
			carry = cpu->b&1;
			result = cpu->b = ((cpu->b>>1))&0xFF;
			break;
		case 0x01:
			carry = (cpu->c)&1;
			result = cpu->c = ((cpu->c>>1))&0xFF;
			break;
		case 0x02:
			carry = (cpu->d)&1;
			result = cpu->d = ((cpu->d>>1))&0xFF;
			break;
		case 0x03:
			carry = (cpu->e)&1;
			result = cpu->e = ((cpu->e>>1))&0xFF;
			break;
		case 0x04:
			carry = (cpu->h)&1;
			result = cpu->h = ((cpu->h>>1))&0xFF;
			break;
		case 0x05:
			carry = (cpu->l)&1;
			result = cpu->l = ((cpu->l>>1))&0xFF;
			break;
		case 0x06:
			result = CPU_mem_read(cpu,cpu->hl);
			carry = (result)&1;
			result = ((result>>1))&0xFF;
			tc_add(cpu->timer_c,7);
			CPU_mem_write(cpu,cpu->hl,result);
			break;
		case 0x07:
			carry = (cpu->a)&1;
			result = cpu->a = ((cpu->a>>1))&0xFF;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + x3chk(result)+ 
		 parity(result) + carry;
}
// END CB OPCODES
//---------------------------------------


//-----------------
// and reg8
void and_reg8(CPU_t *cpu) {
	int result;
	int reg;

	tc_add(cpu->timer_c,4);
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; tc_add(cpu->timer_c,4);,
				reg = cpu->iyh; tc_add(cpu->timer_c,4);
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; tc_add(cpu->timer_c, 4);,
				reg = cpu->iyl; tc_add(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				tc_add(cpu->timer_c,3);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				tc_add(cpu->timer_c, 15);
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a & reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + HC_MASK + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
}



//-----------------
// and num8
void and_num8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c,7);
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a&reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + HC_MASK + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
}



//-----------------
// or reg8
void or_reg8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c,4);
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; tc_add(cpu->timer_c,4);,
				reg = cpu->iyh; tc_add(cpu->timer_c,4);
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; tc_add(cpu->timer_c, 4);,
				reg = cpu->iyl; tc_add(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				tc_add(cpu->timer_c,3);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				tc_add(cpu->timer_c, 15);
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a | reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
}

//-----------------
// or num8
void or_num8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c,7);
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a|reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
}

//-----------------
// xor reg8
void xor_reg8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c,4);
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; tc_add(cpu->timer_c,4);,
				reg = cpu->iyh; tc_add(cpu->timer_c,4);
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; tc_add(cpu->timer_c, 4);,
				reg = cpu->iyl; tc_add(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				tc_add(cpu->timer_c,3);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				tc_add(cpu->timer_c, 15);
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a^reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
}


//-----------------
// xor num8
void xor_num8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c,7);
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a^reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + 
		 x3chk(result)+ parity(result);
	cpu->a = result;
}

void cp_reg8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 4);
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; tc_add(cpu->timer_c,4);,
				reg = cpu->iyh; tc_add(cpu->timer_c,4);
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; tc_add(cpu->timer_c, 4);,
				reg = cpu->iyl; tc_add(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				tc_add(cpu->timer_c,3);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				tc_add(cpu->timer_c, 15);
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a - reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);
}

void cp_num8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 7);
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a - reg;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(reg) + hcsubchk(cpu->a,reg,0) + 
		 x3chk(reg)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);
}

void cpl(CPU_t *cpu) {
	int result;
	tc_add(cpu->timer_c, 4);
	result = (~cpu->a)&255;
	cpu->f =unaffect( SIGN_MASK+ZERO_MASK+PV_MASK+CARRY_MASK) +
		 x5chk(result) + HC_MASK + x3chk(result)+ N_MASK;
	cpu->a = result;
}

void daa(CPU_t *cpu) {
	int result = cpu->a;
	tc_add(cpu->timer_c, 4);
	if ( (cpu->f&N_MASK)!=0 ) {
		if ( (cpu->f&HC_MASK)!=0 || (cpu->a&0x0f)>9 ) result -= 0x06;
		if ( (cpu->f&CARRY_MASK)!=0 || (cpu->a > 0x99) ) result -= 0x60;
	} else {
		if ( (cpu->f&HC_MASK)!=0 || (cpu->a&0x0f)>9 ) result += 0x06;
		if ( (cpu->f&CARRY_MASK)!=0 || (cpu->a > 0x99) ) result += 0x60;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + (cpu->a&0x10 ^ result&0x10) + 
		 x3chk(result)+ parity(result) + 
		 unaffect( N_MASK ) + (cpu->f&CARRY_MASK | ((cpu->a>0x99)?CARRY_MASK:0));
	cpu->a = result;
}




void dec_reg8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 4);
	switch ((cpu->bus>>3)&7) {
		case 0x00:
			reg = cpu->b;
			cpu->b--;
			result = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			cpu->c--;
			result = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			cpu->d--;
			result = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			cpu->e--;
			result = cpu->e;
			break;
		case 0x04:
			if (!cpu->prefix) {
				reg = cpu->h;
				cpu->h--;
				result = cpu->h;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixh;
				cpu->ixh--;
				result = cpu->ixh;
				tc_add(cpu->timer_c,4);
			} else {
				reg = cpu->iyh;
				cpu->iyh--;
				result = cpu->iyh;
				tc_add(cpu->timer_c,4);
			}
			break;
		case 0x05:
			if (!cpu->prefix) {
				reg = cpu->l;
				cpu->l--;
				result = cpu->l;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixl;
				cpu->ixl--;
				result = cpu->ixl;
				tc_add(cpu->timer_c,4);
			} else {
				reg = cpu->iyl;
				cpu->iyl--;
				result = cpu->iyl;
				tc_add(cpu->timer_c,4);
			}
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				result = reg;
				result--;
				CPU_mem_write(cpu,cpu->hl,result);
				tc_add(cpu->timer_c,7);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu,cpu->ix+offset);
					result = reg;
					result--;
					CPU_mem_write(cpu,cpu->ix+offset,result);
					tc_add(cpu->timer_c,19-4);
				} else {
					reg = CPU_mem_read(cpu,cpu->iy+offset);
					result = reg;
					result--;
					CPU_mem_write(cpu,cpu->iy+offset,result);
					tc_add(cpu->timer_c,19-4);
				}
			}
			break;
		case 0x07:
			reg = cpu->a;
			cpu->a--;
			result = cpu->a;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(reg,1,0) + 
		 x3chk(result)+ vchksub(reg,1,result) + 
		 SUB_INSTR +  unaffect(CARRY_MASK);
}


void inc_reg8(CPU_t *cpu) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 4);
	switch ((cpu->bus>>3)&7) {
		case 0x00:
			reg = cpu->b;
			cpu->b++;
			result = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			cpu->c++;
			result = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			cpu->d++;
			result = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			cpu->e++;
			result = cpu->e;
			break;
		case 0x04:
			if (!cpu->prefix) {
				reg = cpu->h;
				cpu->h++;
				result = cpu->h;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixh;
				cpu->ixh++;
				result = cpu->ixh;
				tc_add(cpu->timer_c,4);
			} else {
				reg = cpu->iyh;
				cpu->iyh++;
				result = cpu->iyh;
				tc_add(cpu->timer_c,4);
			}
			break;
		case 0x05:
			if (!cpu->prefix) {
				reg = cpu->l;
				cpu->l++;
				result = cpu->l;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ixl;
				cpu->ixl++;
				result = cpu->ixl;
				tc_add(cpu->timer_c,4);
			} else {
				reg = cpu->iyl;
				cpu->iyl++;
				result = cpu->iyl;
				tc_add(cpu->timer_c,4);
			}
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				result = reg;
				result++;
				CPU_mem_write(cpu,cpu->hl,result);
				tc_add(cpu->timer_c,7);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu,cpu->ix+offset);
					result = reg;
					result++;
					CPU_mem_write(cpu,cpu->ix+offset,result);
					tc_add(cpu->timer_c,19-4);
				} else {
					reg = CPU_mem_read(cpu,cpu->iy+offset);
					result = reg;
					result++;
					CPU_mem_write(cpu,cpu->iy+offset,result);
					tc_add(cpu->timer_c,19-4);
				}
			}
			break;
		case 0x07:
			reg = cpu->a;
			cpu->a++;
			result = cpu->a;
			break;
	}
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcaddchk(reg,1,0) + 
		 x3chk(result)+vchkadd(reg,1,result) + 
		 ADD_INSTR +  unaffect(CARRY_MASK);
}
//-----------------
// add a,reg8
// add a,(hl)
// adc a,reg8
// adc a,(hl)
void add_a_reg(CPU_t *cpu, int carry) {
	int result;
	int reg;
	tc_add(cpu->timer_c,4);
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; tc_add(cpu->timer_c,4);,
				reg = cpu->iyh; tc_add(cpu->timer_c,4);
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; tc_add(cpu->timer_c, 4);,
				reg = cpu->iyl; tc_add(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				tc_add(cpu->timer_c,3);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				tc_add(cpu->timer_c, 15);
			}
			break;
		case 0x07:
			reg = cpu->a;
			break;
	}
	result = cpu->a+reg+carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcaddchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchkadd(cpu->a,reg,result) + 
		 ADD_INSTR +  carrychk(result);
	cpu->a = result;
}
void adc_a_reg8(CPU_t *cpu) {
	add_a_reg(cpu, cpu->f & 1);
}
void add_a_reg8(CPU_t *cpu) {
	add_a_reg(cpu, 0);
}


//-----------------
// add a,num8
// adc a,num8
void add_a_num(CPU_t *cpu, int carry) {
	int result;
	int reg;
	tc_add(cpu->timer_c,7);
	reg = CPU_mem_read(cpu,cpu->pc++);	//THIS IS NOT AN OPCODE READ
	result = cpu->a + reg + carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcaddchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchkadd(cpu->a,reg,result) + 
		 ADD_INSTR +  carrychk(result);
	cpu->a = result;
}
void adc_a_num8(CPU_t *cpu) {
	add_a_num(cpu, cpu->f & 1);
}
void add_a_num8(CPU_t *cpu) {
	add_a_num(cpu,0);
}


//-----------------
// sub reg8
// sub (hl)
// sbc a,reg8
// sbc a,(hl)
void sub_a_reg(CPU_t *cpu, int carry) {
	int result;
	int reg;
	tc_add(cpu->timer_c, 4);
	switch ((cpu->bus)&7) {
		case 0x00:
			reg = cpu->b;
			break;
		case 0x01:
			reg = cpu->c;
			break;
		case 0x02:
			reg = cpu->d;
			break;
		case 0x03:
			reg = cpu->e;
			break;
		case 0x04:
			index_ext (
				reg = cpu->h;,
				reg = cpu->ixh; tc_add(cpu->timer_c,4);,
				reg = cpu->iyh; tc_add(cpu->timer_c,4);
			)
			break;
		case 0x05:
			index_ext (
				reg = cpu->l;,
				reg = cpu->ixl; tc_add(cpu->timer_c, 4);,
				reg = cpu->iyl; tc_add(cpu->timer_c, 4);
			)
			break;
		case 0x06:
			if (!cpu->prefix) {
				reg = CPU_mem_read(cpu,cpu->hl);
				tc_add(cpu->timer_c,3);
			} else {
				char offset = CPU_mem_read(cpu, cpu->pc++);
				if (cpu->prefix == 0xDD) {
					reg = CPU_mem_read(cpu, cpu->ix + offset);
				} else {
					reg = CPU_mem_read(cpu, cpu->iy + offset);
				}
				tc_add(cpu->timer_c, 15);
			}
			break;
		case 0x07:
		default:
			reg = cpu->a;
			break;
	}
	result = cpu->a - reg - carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);
	cpu->a = result;
}
void sbc_a_reg8(CPU_t *cpu) {
	sub_a_reg(cpu,(cpu->f)&1);
}
void sub_a_reg8(CPU_t *cpu) {
	sub_a_reg(cpu,0);
}

//-----------------
// sub num8
// sbc a,num8
void sub_a_num(CPU_t *cpu,int carry) {
	int result;
	int reg;
	tc_add(cpu->timer_c,7);
	reg = CPU_mem_read(cpu,cpu->pc++);
	result = cpu->a - reg - carry;
	cpu->f = signchk(result) + zerochk(result) +
		 x5chk(result) + hcsubchk(cpu->a,reg,carry) + 
		 x3chk(result)+ vchksub(cpu->a,reg,result) + 
		 SUB_INSTR +  carrychk(result);
	cpu->a = result;
}
void sbc_a_num8(CPU_t *cpu) {
	sub_a_num(cpu, cpu->f & 1);
}
void sub_a_num8(CPU_t *cpu) {
	sub_a_num(cpu, 0);
}


void dec_reg16(CPU_t *cpu) {
	
	int time = 6;
	switch (((cpu->bus)>>4)&3) {
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
	tc_add(cpu->timer_c, time);
}

void inc_reg16(CPU_t *cpu) {
	
	int time = 6;
	switch (((cpu->bus)>>4)&3) {
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
	tc_add(cpu->timer_c, time);
}

void add_hl_reg16(CPU_t *cpu) {
	int result;
	int base, reg;

	int time = 11; 
	switch (((cpu->bus)>>4)&3) {
		case 0x00:
			reg = cpu->bc;
			break;
		case 0x01:
			reg = cpu->de;
			break;
		case 0x02:
			if (!cpu->prefix) {
				reg = cpu->hl;
			} else if (cpu->prefix == 0xDD) {
				reg = cpu->ix;
				time+=4;
			} else {
				reg = cpu->iy;
				time+=4;
			}
			break;
		case 0x03:
			reg = cpu->sp;
			break;
	}

	if (!cpu->prefix) {
		base = cpu->hl;
		cpu->hl = result = base + reg;
	} else if (cpu->prefix == 0xDD) {
		base = cpu->ix;
		cpu->ix = result = base + reg;
	} else {
		base = cpu->iy;
		cpu->iy = result = base + reg;
	}

	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk16(result) + hcaddchk16(base, reg, 0) + 
		 x3chk16(result)+ 
		 ADD_INSTR +  carrychk16( base + reg);
	tc_add(cpu->timer_c, time);
}


void rla(CPU_t *cpu) {
	int result = ((cpu->a<<1)+(cpu->f&1))&255;
	tc_add(cpu->timer_c,4);
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ (((cpu->a)>>7)&1);
	cpu->a =result;
	
}
void rlca(CPU_t *cpu) {
	int result = ((cpu->a<<1)+(((cpu->a)>>7)&1))&255;
	tc_add(cpu->timer_c,4);
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ (((cpu->a)>>7)&1);
	cpu->a =result;
}
void rra(CPU_t *cpu) {
	int result = ((cpu->a>>1)+((cpu->f&1)<<7))&255;
	tc_add(cpu->timer_c,4);
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ ((cpu->a)&1);
	cpu->a =result;
	
}
void rrca(CPU_t *cpu) {
	int result = ((cpu->a>>1)+(((cpu->a)<<7)&128))&255;
	tc_add(cpu->timer_c,4);
	cpu->f = unaffect(SIGN_MASK +ZERO_MASK+PV_MASK) + 
		 x5chk(result) + x3chk(result)+ ((cpu->a)&1);
	cpu->a =result;
	
}
