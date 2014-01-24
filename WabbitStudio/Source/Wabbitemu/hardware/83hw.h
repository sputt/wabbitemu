#ifndef TI83HW_H
#define TI83HW_H
#include "core.h"
#include "ti_stdint.h"

#define PAGE_SIZE 16384


#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))



STDINT_t* INT83_init(CPU_t*);
int device_init_83(CPU_t*, BOOL);
int memory_init_83(memc *);

void port0_83(CPU_t *, device_t *);
void port2_83(CPU_t *, device_t *);
void port3_83(CPU_t *, device_t *);
void port4_83(CPU_t *, device_t *);
void port6_83(CPU_t *, device_t *);
void port7_83(CPU_t *, device_t *);
void port14_83(CPU_t *, device_t *);


#endif 
