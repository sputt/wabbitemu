#ifndef CORE_H
#define CORE_H

#include "coretypes.h"

#define TI_82		1
#define TI_83		2
#define TI_85		3
#define TI_86		4
#define TI_73		5
#define TI_83P		6
#define TI_83PSE	7
#define TI_84P		8
#define TI_84PSE	9

/*defines the start of the app page*/
/*this page starts in HIGH mem and grows to LOW */
#define TI_73_APPPAGE		0x15	/*Ummm...*/
#define TI_83P_APPPAGE		0x15
#define TI_83PSE_APPPAGE	0x69
#define TI_84P_APPPAGE		0x29	/*Ummm...*/
#define TI_84PSE_APPPAGE	0x69


/*defines the Number of user archive pages*/
#define TI_73_USERPAGES		0x0A	/*Ummm...*/
#define TI_83P_USERPAGES	0x0A
#define TI_83PSE_USERPAGES	0x60
#define TI_84P_USERPAGES	0x1E	/*Ummm...*/
#define TI_84PSE_USERPAGES	0x60

#ifndef PAGE_SIZE
#define PAGE_SIZE 16384
#endif

#define NumElm(array) (sizeof (array) / sizeof ((array)[0]))

#define FPS 50
// ticks per frame
#define TPF (1000 / FPS)
// 86 speed
#define MHZ_4_8 4800000
// typical 83+ speed,  Mode 0 on SE 6300000
#define MHZ_6 6000000
// Mode 1 on the SE
#define MHZ_15 15000000
// Planned Mode 2 on the SE
#define MHZ_20 20000000
// Planned Mode 3 on the SE
#define MHZ_25 25000000

#define MHZ_9 9437184


#define IX_PREFIX 0xDD
#define IY_PREFIX 0xFD

/* 	Macro to form a register pair
 *		Can refer to pair by
 *		either contituent given
 *	Example:
 *		regpair(h, l, hl);
 *		---
 *		struct.h
 *		struct.l
 *		struct.hl					*/
#ifdef __BIG_ENDIAN__
#define regpair(name1,name2,fullname) \
union { \
	struct { \
		unsigned char name1; \
		unsigned char name2; \
	}; \
	unsigned short fullname; \
}
#else
#define regpair(name1,name2,fullname) \
union { \
	struct { \
		unsigned char name2; \
		unsigned char name1; \
	}; \
	unsigned short fullname; \
}
#endif

/* 	Determines frequency provided
 *	to the CPU */
typedef struct timer_context {
	long long tstates;
	unsigned long freq;
	double elapsed;		//this isn't used if using long long only
	double lasttime; //<--this isn't used anymore
} timer_context_t, timerc;

/* Bank unit for a partition */
typedef struct bank_state {
	unsigned char * addr;		//Pointer to offset of memory.(already paged)
	int page;					//Current 16kb page
	BOOL read_only;				//You can not write to this page(not even if the flash is unlocked)
	BOOL ram;					//This is on the ram chip(also effect write method for flash)
	BOOL no_exec;				//You can not execute on this page
} bank_state_t, bank_t;

/* Memory address translation */
#define MC_BANK_MASK	0xC000
#define MC_BASE_MASK	(~MC_BANK_MASK)

#define mc_bank(addr_z) (addr_z >> 14)
#define mc_base(addr_z) (addr_z & MC_BASE_MASK)

// all the information required to address a byte of memory
typedef struct waddr {
	BOOL is_ram;
	uint8_t page;
	uint16_t addr;
} waddr_t;

typedef struct memory_context {
	/* to be defined */
	//unsigned char (*flash)[PAGE_SIZE];	//Pointer to flash memory
	//unsigned char (*ram)[PAGE_SIZE];		//Pointer to ram
	u_char *flash;
	u_char *ram;
	union {
		struct {
			u_char *flash_break;
			u_char *ram_break;
		};
		u_char *breaks[2];
	};

	int flash_size;
	int flash_pages;
	int ram_size;
	int ram_pages;
	int step;					// These 3 are for flash programming
	unsigned char cmd;			// step tells what cycle of the command you are on,

	bank_state_t banks[5];		//Current state of each bank
								// structure 5 is used to preserve the 4th in boot map
	BOOL boot_mapped;			//Special mapping used in boot that rotates location of pages
	BOOL flash_locked;			//Whether flash is writeable or not.

	int flash_version;

	int read_OP_flash_tstates;	//These are for delays on SEs, typically they should be 0.
	int read_NOP_flash_tstates;
	int write_flash_tstates;
	int read_OP_ram_tstates;
	int read_NOP_ram_tstates;
	int write_ram_tstates;

	unsigned char upper;
	unsigned char lower;

	int port27_remap_count;		// amount of 64 byte chunks remapped from RAM page 0 to bank 3
	int port28_remap_count;		// amount of 64 byte chunks remapped from RAM page 1 to bank 1
} memory_context_t, memc;

/* Input/Output device mapping */
typedef void (*devp)(void*, void*);
typedef struct device {
	BOOL active;
	memory_context_t *mem_c;
	void *aux;
	devp code;
} device_t;


typedef struct pio_context {
	int model;
	struct LCD *lcd;
	struct keypad *keypad;
	struct link *link;
	struct STDINT *stdint;
	struct SE_AUX *se_aux;
	/* list other cross model devices here */

	device_t devices[256];
	int interrupt[256];
	unsigned int skip_factor[256];
	unsigned int skip_count[256];
} pio_context_t, pioc;

typedef struct CPU {
	/* Register bank 0 */
	regpair(a, f, af);
	regpair(b, c, bc);
	regpair(d, e, de);
	regpair(h, l, hl);
	/* Register bank 1 */
	regpair(ap, fp, afp);
	regpair(bp, cp, bcp);
	regpair(dp, ep, dep);
	regpair(hp, lp, hlp);
	/* Remaining CPU registers */
	regpair(ixh, ixl, ix);
	regpair(iyh, iyl, iy);
	unsigned short pc, sp;
	unsigned char i, r, bus;
	int imode;
	BOOL interrupt;
	BOOL ei_block;
	BOOL iff1,iff2;
	BOOL halt;
	BOOL read, write, output, input;
	int prefix;
	pioc pio;
	memc *mem_c;
	timerc *timer_c;
} CPU_t;

typedef void (*opcodep)(CPU_t*);
typedef void (*index_opcodep)(CPU_t*, char);

unsigned char mem_read(memc*, unsigned short);
uint8_t wmem_read(memc*, waddr_t);
unsigned short mem_read16(memc*, unsigned short);
unsigned char mem_write(memc*, unsigned short, char);
waddr_t addr_to_waddr(memc*, uint16_t);

void set_break(memc *, BOOL, int, uint16_t);
void set_mem_write_break(memc *, BOOL, int, uint16_t);
void set_mem_read_break(memc *, BOOL, int, uint16_t);
void clear_break(memc *mem, BOOL ram, int page, uint16_t addr);
void clear_mem_write_break(memc *, BOOL, int, uint16_t);
void clear_mem_read_break(memc *, BOOL, int, uint16_t);
u_char check_break(memc *, uint16_t);
u_char check_mem_write_break(memc *, uint16_t);
u_char check_mem_read_break(memc *, uint16_t);

int tc_init(timerc*, int);
int CPU_init(CPU_t*, memc*, timerc*);
int CPU_step(CPU_t*);
unsigned char CPU_mem_read(CPU_t *cpu, unsigned short addr);
unsigned char CPU_mem_write(CPU_t *cpu, unsigned short addr, unsigned char data);


#ifdef DEBUG
void displayreg(CPU_t *);
#endif



#define tc_add( timer_z , num ) \
	(timer_z)->tstates += num; \
	(timer_z)->elapsed += ((double)(num))/((double)(timer_z)->freq);

#define tc_elapsed( timer_z ) \
	((timer_z)->elapsed)

#define tc_tstates( timer_z ) \
	((timer_z)->tstates)

#ifdef UseSEtiming
#define SEtc_add( timer_z , num ) \
	timer_z->tstates += num; \
	timer_z->elapsed += ((double)(num))/((double)(timer_z)->freq);
#else
#define SEtc_add( timer_z , num )
#endif


#define endflash(cpu_v) cpu_v->mem_c->step=0;

#define addschar(address_m, offset_m) ( ( (unsigned short) address_m ) + ( (char) offset_m ) )


#define index_ext(hlcase,ixcase,iycase) \
if (!cpu->prefix) { \
	hlcase \
} else if (cpu->prefix == 0xDD) { \
	ixcase \
} else { \
	iycase \
}

#endif
