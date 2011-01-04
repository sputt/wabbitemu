#ifndef TI83PSEHW_H
#define TI83PSEHW_H
#include "core.h"
#include "ti_stdint.h"

#define LinkRead (((cpu->pio.link->host & 0x03) | (cpu->pio.link->client[0] & 0x03))^3)
#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))

typedef struct TIMER {
	/* determines which clock if any is used for time */
	unsigned long long lastTstates;
	double lastTicks;
	double divsor;
	BOOL loop;
	BOOL interrupt;
	BOOL underflow;
	BOOL generate;
	BOOL active;
	unsigned char clock;
	unsigned char count;
	unsigned char max;
} TIMER_t;

typedef struct XTAL {
	double lastTime;			/* actual real time of last tick */
	unsigned long long ticks;	/* ticks of the xtal timer */
	TIMER_t timers[3];
} XTAL_t;

typedef struct LINKASSIST {
	unsigned char link_enable;
	unsigned char in;
	unsigned char out;
	unsigned char working;
	BOOL receiving;
	BOOL read;
	BOOL ready;
	BOOL error;
	BOOL sending;
	double last_access;
	int bit;
} LINKASSIST_t;

typedef struct MD5 {
	/* 32 bit registers */
	union {
		struct {
			unsigned long a;
			unsigned long b;
			unsigned long c;
			unsigned long d;
			unsigned long x;
			unsigned long ac;
		};
		unsigned long reg[6];
	};
	unsigned char s;
	unsigned char mode ;
} MD5_t;

typedef struct DELAY {
	union {
		struct {
			unsigned char lcd1;
			unsigned char lcd2;
			unsigned char lcd3;
			unsigned char lcd4;
			unsigned char unkown;
			unsigned char mad; /*memory access delay*/
			unsigned char lcdwait;
		};
		unsigned char reg[7];
	};
} DELAY_t;

typedef struct CLOCK {
	unsigned char enable;
	unsigned long set;
	unsigned long base;
	double lasttime;
} CLOCK_t;

typedef struct USB {
	unsigned int DPlus;
	unsigned int DMinus;
	unsigned int ID;
	unsigned int Vbus;
	unsigned int USBEvents;			//whether interrupts should be generated when USB lines change

} USB_t;

typedef struct SE_AUX {
	CLOCK_t clock;
	DELAY_t delay;
	MD5_t md5;
	LINKASSIST_t linka;
	XTAL_t xtal;
	USB_t usb;
} SE_AUX_t;

STDINT_t *INT83PSE_init(CPU_t*);
int device_init_83pse(CPU_t*);
int memory_init_83pse(memc *);
int memory_init_84p(memc *);

void port0_83pse(CPU_t *, device_t *);
void port2_83pse(CPU_t *, device_t *);
void port3_83pse(CPU_t *, device_t *);
void port4_83pse(CPU_t *, device_t *);
void port6_83pse(CPU_t *, device_t *);
void port7_83pse(CPU_t *, device_t *);
void port14_83pse(CPU_t *, device_t *);
void flashwrite83pse(CPU_t *, unsigned short, unsigned char);
void flashwrite84p(CPU_t *, unsigned short, unsigned char);

#endif 
