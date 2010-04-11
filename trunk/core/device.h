#ifndef DEVICE_H
#define DEVICE_H
#include "core.h"

#define DEV_INDEX(zdev) (zdev - cpu->pio.devices)

int device_output(CPU_t *, unsigned char);
int device_input(CPU_t *, unsigned char);
int device_control(CPU_t *cpu, unsigned char dev);
void Append_interrupt_device(CPU_t *, int, int);
void Modify_interrupt_device(CPU_t *, int, int);
void ClearDevices(CPU_t*);
#endif
