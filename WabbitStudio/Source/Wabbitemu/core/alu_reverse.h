#ifndef ALU_REVERSE_H
#define ALU_REVERSE_H

#define SIGN_MASK 0x80
#define ZERO_MASK 0x40
#define X5_MASK 0x20
#define HC_MASK 0x10
#define X3_MASK 0x08
#define PV_MASK 0x04
#define N_MASK 0x02
#define CARRY_MASK 0x01

#define unaffect( mask )	((cpu->f)&(mask))

#define dosign( tval )		(( tval )?SIGN_MASK:0)
#define dozero( tval )		(( tval )?ZERO_MASK:0)
#define dox5( tval )		(( tval )?X5_MASK:0)
#define dohc( tval )		(( tval )?HC_MASK:0)
#define dox3( tval )		(( tval )?X3_MASK:0)
#define doparity( tval )	(( tval )?PV_MASK:0)
#define dooverflow( tval )	(( tval )?PV_MASK:0)
#define ADD_INSTR 0
#define SUB_INSTR N_MASK
#define carry( tval )		(( tval )?CARRY_MASK:0)

#define signchk( tval )		((tval)&0x80) 
#define zerochk( zchk )		dozero( (((zchk)&0xFF)==0) ) 
#define x5chk( tval )		((tval)&0x20)
#define hcaddchk( opr1 , opr2 , carry)		((((opr1)&0x0F)+((opr2)&0x0F) + carry)&0x10)
#define hcsubchk( opr1 , opr2 , carry)		((((opr1)&0x0F)-((opr2)&0x0F) - carry)&0x10)
#define x3chk( tval )		((tval)&0x08)
#define vchkadd(opr1,opr2,res) ( ((((opr1) & 0x80) == ((opr2) & 0x80) ) & (((opr1) & 0x80) != ((res) & 0x80))) << 2)
#define vchksub(opr1,opr2,res) ( ((((opr1) & 0x80) != ((opr2) & 0x80) ) & (((opr1) & 0x80) != ((res) & 0x80))) << 2)
#define parity( opr1 )          ((((((opr1)>>0)^((opr1)>>1)^((opr1)>>2)^((opr1)>>3)^((opr1)>>4)^((opr1)>>5)^((opr1)>>6)^((opr1)>>7))&1)^1)*PV_MASK) 
//add or sub
#define carrychk(tval)		(((tval) & 0x100) >> 8)


#define signchk16( tval )		(((tval)>>8)&0x80)
#define zerochk16( tval )		dozero(((tval)&0xFFFF)==0)
#define x5chk16( tval )			(((tval)>>8)&0x20)
#define hcaddchk16( opr1 , opr2 , carry)	(((((opr1)&0xfff)+((opr2)&0xfff)+(carry))&0x1000)>>8)
#define hcsubchk16( opr1 , opr2 , carry)	(((((opr1)&0xfff)-((opr2)&0xfff)-(carry))&0x1000)>>8)
#define x3chk16( tval )			(((tval)>>8)&8)
#define vchkadd16(opr1,opr2,res) ( ((((opr1) & 0x8000) == ((opr2) & 0x8000) ) & (((opr1) & 0x8000) != ((res) & 0x8000))) << 2)
#define vchksub16(opr1,opr2,res) ( ((((opr1) & 0x8000) != ((opr2) & 0x8000) ) & (((opr1) & 0x8000) != ((res) & 0x8000))) << 2)
//add sub
#define carrychk16(tval)		(((tval) & 0x10000) >> 16)

void add_a_num8_reverse(CPU_t*);
void adc_a_num8_reverse(CPU_t*);
void sub_a_num8_reverse(CPU_t*);
void sbc_a_num8_reverse(CPU_t*);
void adc_a_reg8_reverse(CPU_t *);
void add_a_reg8_reverse(CPU_t *);
void sbc_a_reg8_reverse(CPU_t *);
void sub_a_reg8_reverse(CPU_t *);
void dec_reg8_reverse(CPU_t *);
void inc_reg8_reverse(CPU_t *);


void dec_reg16_reverse(CPU_t *);
void inc_reg16_reverse(CPU_t *);
void add_hl_reg16_reverse(CPU_t *);
void adc_hl_reg16_reverse(CPU_t *);
void sbc_hl_reg16_reverse(CPU_t *);

void and_reg8_reverse(CPU_t *);
void and_num8_reverse(CPU_t *);

void cp_reg8_reverse(CPU_t *);
void cp_num8_reverse(CPU_t *);
void cpd_reverse(CPU_t *);
void cpdr_reverse(CPU_t *);
void cpi_reverse(CPU_t *);
void cpir_reverse(CPU_t *);


void cpl_reverse(CPU_t *);
void daa_reverse(CPU_t *);


void or_reg8_reverse(CPU_t *);
void or_num8_reverse(CPU_t *);

void xor_num8_reverse(CPU_t *);
void xor_reg8_reverse(CPU_t *);

void rlca_reverse(CPU_t *);
void rla_reverse(CPU_t *);
void rrca_reverse(CPU_t *);
void rra_reverse(CPU_t *);

void rld_reverse(CPU_t *);
void rrd_reverse(CPU_t *);

//CB opcodes

void bit_reverse(CPU_t *);
void res_reverse(CPU_t *);
void set_reverse(CPU_t *);
void rl_reg_reverse(CPU_t *);
void rlc_reg_reverse(CPU_t *);
void rr_reg_reverse(CPU_t *);
void rrc_reg_reverse(CPU_t *);

void srl_reg_reverse(CPU_t *);
void sra_reg_reverse(CPU_t *);
void sla_reg_reverse(CPU_t *);
void sll_reg_reverse(CPU_t *);

#endif
