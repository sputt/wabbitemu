#ifndef CONTROL_REVERSE_H
#define CONTROL_REVERSE_H

#include "core.h"

void rst_reverse(CPU_t *);
void ret_reverse(CPU_t *);
void out_reverse(CPU_t *);
void in_reverse(CPU_t *);
void call_reverse(CPU_t *);
void call_condition_reverse(CPU_t *);
void push_reg16_reverse(CPU_t *);
void pop_reg16_reverse(CPU_t *);
void ld_sp_hl_reverse(CPU_t *);
void ld_mem16_hlf_reverse(CPU_t *);
void ld_hlf_mem16_reverse(CPU_t *);
void ld_hl_num16_reverse(CPU_t *);
void ld_de_num16_reverse(CPU_t *);
void ld_bc_num16_reverse(CPU_t *);
void ld_sp_num16_reverse(CPU_t *);
void ld_a_mem16_reverse(CPU_t *);
void ld_a_bc_reverse(CPU_t *);
void ld_a_de_reverse(CPU_t *);
void ld_mem16_a_reverse(CPU_t *);
void ld_bc_a_reverse(CPU_t *);
void ld_de_a_reverse(CPU_t *);
void ld_r_num8_reverse(CPU_t *);
void ld_r_r_reverse(CPU_t *);
void halt_reverse(CPU_t *);
void ex_sp_hl_reverse(CPU_t *);
void ex_de_hl_reverse(CPU_t *);
void exx_reverse(CPU_t *);
void ex_af_afp_reverse(CPU_t *);
void jp_hl_reverse(CPU_t *);
void jp_reverse(CPU_t *);
void jr_reverse(CPU_t *);
void jp_condition_reverse(CPU_t *);
void jr_condition_reverse(CPU_t *);
void djnz_reverse(CPU_t *);
void ret_condition_reverse(CPU_t *);
void ccf_reverse(CPU_t *);
void scf_reverse(CPU_t *);


void IM0_reverse(CPU_t *);
void IM1_reverse(CPU_t *);
void IM2_reverse(CPU_t *);
void ei_reverse(CPU_t *);
void di_reverse(CPU_t *);
void ldd_reverse(CPU_t *);
void lddr_reverse(CPU_t *);
void ldi_reverse(CPU_t *);
void ldir_reverse(CPU_t *);
void neg_reverse(CPU_t *);
void nop_reverse(CPU_t *);
void ld_mem16_reg16_reverse(CPU_t *);
void ld_reg16_mem16_reverse(CPU_t *);
void in_reg_c_reverse(CPU_t *);
void ind_reverse(CPU_t *);
void indr_reverse(CPU_t *);
void ini_reverse(CPU_t *);
void inir_reverse(CPU_t *);
void ld_i_a_reverse(CPU_t *);
void ld_r_a_reverse(CPU_t *);
void ld_a_i_reverse(CPU_t *);
void ld_a_r_reverse(CPU_t *);
void out_reg_reverse(CPU_t *);
void outd_reverse(CPU_t *);
void otdr_reverse(CPU_t *);
void outi_reverse(CPU_t *);
void otir_reverse(CPU_t *);
void reti_reverse(CPU_t *);
void retn_reverse(CPU_t *);
void ednop_reverse(CPU_t *);


#endif
