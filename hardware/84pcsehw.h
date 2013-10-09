#ifndef TI84PCSEHW_H
#define TI84PCSEHW_H
#include "core.h"
#include "ti_stdint.h"

STDINT_t *INT83PSE_init(CPU_t*);
int device_init_84pcse(CPU_t*);
int memory_init_84pcse(memc *);

//void port0_83pse(CPU_t *, device_t *);
//void port2_83pse(CPU_t *, device_t *);
//void port3_83pse(CPU_t *, device_t *);
//void port4_83pse(CPU_t *, device_t *);
//void port6_83pse(CPU_t *, device_t *);
//void port7_83pse(CPU_t *, device_t *);
//void port14_83pse(CPU_t *, device_t *);
//int GetCPUSpeed(CPU_t *);

#endif 
