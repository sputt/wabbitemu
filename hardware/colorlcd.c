#include "stdafx.h"

#include "colorlcd.h"

ColorLCD_t *ColorLCD_init(CPU_t *cpu, int model) {
	ColorLCD_t* lcd = (ColorLCD_t *)malloc(sizeof(ColorLCD_t));
	if (!lcd) {
		printf("Couldn't allocate memory for LCD\n");
		exit(1);
	}

	switch (model) {
	case TI_84PCSE:

		break;
	default:
		return NULL;
	}
}

void ColorLCD_command(CPU_t *cpu, device_t *device) {
	ColorLCD_t *lcd = (ColorLCD_t *) device->aux;
}

void ColorLCD_data(CPU_t *cpu, device_t *device) {
	ColorLCD_t *lcd = (ColorLCD_t *)device->aux;
}
