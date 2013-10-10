#ifndef REVERSE_INFO_H
#define REVERSE_INFO_H

static void CPU_CB_opcode_run_reverse_info(CPU_t*);
static void CPU_ED_opcode_run_reverse_info(CPU_t*);

void nop_reverse_info(CPU_t *cpu);
void nop_ind_reverse_info(CPU_t *cpu, char offset);

// alu.c
void add_a_num8_reverse_info(CPU_t*);
void adc_a_num8_reverse_info(CPU_t*);
void sub_a_num8_reverse_info(CPU_t*);
void sbc_a_num8_reverse_info(CPU_t*);
void adc_a_reg8_reverse_info(CPU_t *);
void add_a_reg8_reverse_info(CPU_t *);
void sbc_a_reg8_reverse_info(CPU_t *);
void sub_a_reg8_reverse_info(CPU_t *);
void dec_reg8_reverse_info(CPU_t *);
void inc_reg8_reverse_info(CPU_t *);


void dec_reg16_reverse_info(CPU_t *);
void inc_reg16_reverse_info(CPU_t *);
void add_hl_reg16_reverse_info(CPU_t *);
void adc_hl_reg16_reverse_info(CPU_t *);
void sbc_hl_reg16_reverse_info(CPU_t *);

void and_reg8_reverse_info(CPU_t *);
void and_num8_reverse_info(CPU_t *);

void cp_reg8_reverse_info(CPU_t *);
void cp_num8_reverse_info(CPU_t *);
void cpd_reverse_info(CPU_t *);
void cpdr_reverse_info(CPU_t *);
void cpi_reverse_info(CPU_t *);
void cpir_reverse_info(CPU_t *);


void cpl_reverse_info(CPU_t *);
void daa_reverse_info(CPU_t *);


void or_reg8_reverse_info(CPU_t *);
void or_num8_reverse_info(CPU_t *);

void xor_num8_reverse_info(CPU_t *);
void xor_reg8_reverse_info(CPU_t *);

void rlca_reverse_info(CPU_t *);
void rla_reverse_info(CPU_t *);
void rrca_reverse_info(CPU_t *);
void rra_reverse_info(CPU_t *);

void rld_reverse_info(CPU_t *);
void rrd_reverse_info(CPU_t *);

//CB opcodes

void res_reverse_info(CPU_t *);
void set_reverse_info(CPU_t *);
void rl_reg_reverse_info(CPU_t *);
void rlc_reg_reverse_info(CPU_t *);
void rr_reg_reverse_info(CPU_t *);
void rrc_reg_reverse_info(CPU_t *);

void srl_reg_reverse_info(CPU_t *);
void sra_reg_reverse_info(CPU_t *);
void sla_reg_reverse_info(CPU_t *);
void sll_reg_reverse_info(CPU_t *);

// control.c
void rst_reverse_info(CPU_t *);
void ret_reverse_info(CPU_t *);
void out_reverse_info(CPU_t *);
void in_reverse_info(CPU_t *);
void call_reverse_info(CPU_t *);
void call_condition_reverse_info(CPU_t *);
void push_reg16_reverse_info(CPU_t *);
void pop_reg16_reverse_info(CPU_t *);
void ld_sp_hl_reverse_info(CPU_t *);
void ld_mem16_hlf_reverse_info(CPU_t *);
void ld_hlf_mem16_reverse_info(CPU_t *);
void ld_hl_num16_reverse_info(CPU_t *);
void ld_de_num16_reverse_info(CPU_t *);
void ld_bc_num16_reverse_info(CPU_t *);
void ld_sp_num16_reverse_info(CPU_t *);
void ld_a_mem16_reverse_info(CPU_t *);
void ld_a_bc_reverse_info(CPU_t *);
void ld_a_de_reverse_info(CPU_t *);
void ld_mem16_a_reverse_info(CPU_t *);
void ld_bc_a_reverse_info(CPU_t *);
void ld_de_a_reverse_info(CPU_t *);
void ld_r_num8_reverse_info(CPU_t *);
void ld_r_r_reverse_info(CPU_t *);
void halt_reverse_info(CPU_t *);
void ex_sp_hl_reverse_info(CPU_t *);
void ex_de_hl_reverse_info(CPU_t *);
void exx_reverse_info(CPU_t *);
void ex_af_afp_reverse_info(CPU_t *);
void jp_hl_reverse_info(CPU_t *);
void jp_reverse_info(CPU_t *);
void jr_reverse_info(CPU_t *);
void jp_condition_reverse_info(CPU_t *);
void jr_condition_reverse_info(CPU_t *);
void djnz_reverse_info(CPU_t *);
void ret_condition_reverse_info(CPU_t *);
void ccf_reverse_info(CPU_t *);
void scf_reverse_info(CPU_t *);
void out_reverse_info(CPU_t *cpu);
void in_reverse_info(CPU_t *cpu);

void IM0_reverse_info(CPU_t *);
void IM1_reverse_info(CPU_t *);
void IM2_reverse_info(CPU_t *);
void ei_reverse_info(CPU_t *);
void di_reverse_info(CPU_t *);
void ldd_reverse_info(CPU_t *);
void lddr_reverse_info(CPU_t *);
void ldi_reverse_info(CPU_t *);
void ldir_reverse_info(CPU_t *);
void neg_reverse_info(CPU_t *);
void nop_reverse_info(CPU_t *);
void ld_mem16_reg16_reverse_info(CPU_t *);
void ld_reg16_mem16_reverse_info(CPU_t *);
void in_reg_c_reverse_info(CPU_t *);
void ind_reverse_info(CPU_t *);
void indr_reverse_info(CPU_t *);
void ini_reverse_info(CPU_t *);
void inir_reverse_info(CPU_t *);
void ld_i_a_reverse_info(CPU_t *);
void ld_r_a_reverse_info(CPU_t *);
void ld_a_i_reverse_info(CPU_t *);
void ld_a_r_reverse_info(CPU_t *);
void out_reg_reverse_info(CPU_t *);
void outd_reverse_info(CPU_t *);
void otdr_reverse_info(CPU_t *);
void outi_reverse_info(CPU_t *);
void otir_reverse_info(CPU_t *);
void reti_reverse_info(CPU_t *);
void retn_reverse_info(CPU_t *);
void ednop_reverse_info(CPU_t *);

// indexcb.c
void res_ind_reverse_info(CPU_t*, char);
void set_ind_reverse_info(CPU_t*, char);

void rr_ind_reverse_info(CPU_t*, char);
void rrc_ind_reverse_info(CPU_t*, char);
void rl_ind_reverse_info(CPU_t*, char);
void rlc_ind_reverse_info(CPU_t*, char);

void srl_ind_reverse_info(CPU_t*, char);
void sll_ind_reverse_info(CPU_t*, char);

void sra_ind_reverse_info(CPU_t*, char);
void sla_ind_reverse_info(CPU_t*, char);

#endif