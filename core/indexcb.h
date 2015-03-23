#include "core.h"
#include "alu.h"
#include "control.h"

int bit_ind(CPU_t*, char);
int res_ind(CPU_t*, char);
int set_ind(CPU_t*, char);

int rr_ind(CPU_t*, char);
int rrc_ind(CPU_t*, char);
int rl_ind(CPU_t*, char);
int rlc_ind(CPU_t*, char);

int srl_ind(CPU_t*, char);
int sll_ind(CPU_t*, char);

int sra_ind(CPU_t*, char);
int sla_ind(CPU_t*, char);
