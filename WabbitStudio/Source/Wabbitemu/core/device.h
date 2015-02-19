#ifndef DEVICE_H
#define DEVICE_H
#include "core.h"

#define DEV_INDEX(zdev) (zdev - cpu->pio.devices)

int device_output(CPU_t *, unsigned char);
int device_input(CPU_t *, unsigned char);
void handle_pio(CPU_t *cpu);
void Append_interrupt_device(CPU_t *, unsigned char, unsigned char);
void Modify_interrupt_device(CPU_t *, unsigned char, unsigned char);
void ClearDevices(CPU_t*);
#endif
