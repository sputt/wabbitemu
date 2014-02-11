#include "stdafx.h"

#include "device.h"

void ClearDevices(CPU_t* cpu) {
	int i;
	for (i = 0; i < ARRAYSIZE(cpu->pio.interrupt); i++) {
		cpu->pio.devices[i].active = FALSE;
		interrupt_t *intVal = &cpu->pio.interrupt[i];
		intVal->interrupt_val = -1;
		intVal->skip_factor = 1;
		intVal->skip_count = intVal->skip_factor;
	}
	cpu->pio.num_interrupt = 0;
}

int device_output(CPU_t *cpu, unsigned char dev) {
	if (cpu->pio.devices[dev].active) {
		cpu->output = TRUE;
		if (!cpu->pio.devices[dev].protected_port || !cpu->mem_c->flash_locked)
			cpu->pio.devices[dev].code(cpu, &(cpu->pio.devices[dev]));
		if (cpu->pio.devices[dev].breakpoint)
			cpu->pio.breakpoint_callback(cpu, &(cpu->pio.devices[dev]));
		if (cpu->output) {
			/* Device is not responding */
			cpu->output = FALSE;
			return 1;
		}
	}
	return 0;
}

int device_input(CPU_t *cpu, unsigned char dev) {
	if (cpu->pio.devices[dev].active) {
		cpu->input = TRUE;
		if (cpu->pio.devices[dev].breakpoint)
			cpu->pio.breakpoint_callback(cpu, &(cpu->pio.devices[dev]));
		cpu->pio.devices[dev].code(cpu, &(cpu->pio.devices[dev]));
		if (cpu->input) {
			/* Device is not responding */
			cpu->input = FALSE;
			cpu->bus = 0xFF;
			return 1;
		}
	} else  {
		cpu->bus = 0xFF;
		return 1;
	}
	return 0;
}

void Append_interrupt_device(CPU_t *cpu, int port, int skip) {
	interrupt_t *intVal = &cpu->pio.interrupt[cpu->pio.num_interrupt];
	intVal->interrupt_val = port;
	intVal->skip_factor = skip;
	cpu->pio.num_interrupt++;
}

void Modify_interrupt_device(CPU_t *cpu, int port, int skip) {
	for(int i = 0; i < cpu->pio.num_interrupt; i++) {
		if (cpu->pio.interrupt[i].interrupt_val == port) {
			cpu->pio.interrupt[i].skip_factor = skip;
			break;
		}
	}
}

int device_control(CPU_t *cpu, unsigned char dev) {
	device_t *device = &cpu->pio.devices[dev];
	if (device->active) {
		device->code(cpu, device);
	}
	return 0;
}

