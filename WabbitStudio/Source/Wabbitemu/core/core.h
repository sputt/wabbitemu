#ifndef CORE_H
#define CORE_H

#include "coretypes.h"
//#include "stdafx.h"

#define TI_81		0
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

#ifdef MACVER
#define FPS 30
#else
#define FPS 50
#endif
// ticks per frame
#define TPF (CLOCKS_PER_SEC / FPS)
// 81 speed
#define MHZ_2 2000000
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
 *		either constituent given
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
		BYTE name2; \
		BYTE name1; \
	}; \
	WORD fullname; \
}
#endif

/* 	Determines frequency provided
 *	to the CPU */
typedef struct timer_context {
	uint64_t tstates;
	uint32_t freq;
	double elapsed;		//this isn't used if using long long only (see next line)
	double lasttime;	//<--this isn't used anymore (except for sound and interrupts)
	int timer_version;
} timer_context_t, timerc;

/* Bank unit for a partition */
typedef struct bank_state {
	unsigned char *addr;		//Pointer to offset of memory.(already paged)
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

typedef enum {
	MODE0 = 0,			//Execution is allowed on pages 81h, 83h, 85h, 87h, 89h, 8Bh, 8Dh, and 8Fh. 
	MODE1 = 1,			//Execution is allowed on pages 81h, 85h, 89h, and 8Dh. 
	MODE2 = 2,			//Execution is allowed on pages 81h and 89h. 
	MODE3 = 3,			//Execution is allowed on pages 81h only. 
} RAM_PROT_MODE;

typedef enum {
	NORMAL_BREAK = 0x1,
	MEM_WRITE_BREAK = 0x2,
	MEM_READ_BREAK = 0x4,
	CLEAR_NORMAL_BREAK = ~NORMAL_BREAK,
	CLEAR_MEM_WRITE_BREAK = ~MEM_WRITE_BREAK,
	CLEAR_MEM_READ_BREAK = ~MEM_READ_BREAK,
} BREAK_TYPE;

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

#ifdef WINVER
	BOOL (*breakpoint_manager_callback)(memory_context *, BREAK_TYPE, waddr_t);
#endif

	int flash_size;
	int flash_pages;
	int ram_size;
	int ram_pages;
	int step;					// These 4 are for flash programming
	unsigned char cmd;			// step tells what cycle of the command you are on,
	uint64_t flash_last_write;	// last time flash was written to
	uint64_t flash_write_delay;	// number of tstates to delay before allowing flash read/write
	BOOL flash_locked;			//Whether flash is writeable or not.

	bank_state_t *banks;		//pointer to the correct bank state currently
	bank_state_t normal_banks[5];		//Current state of each bank
								// structure 5 is used to preserve the 4th in boot map
	bank_state_t bootmap_banks[5];			//used to hold a backup of the banks when this is boot mapped
	BOOL boot_mapped;			//Special mapping used in boot that changes how paging works
	BOOL hasChangedPage0;		//Check if bootcode is still mapped to page 0 or not
	int protected_page_set;		//Special for the 83p, used to determine which group of pages you are referring to
	int protected_page[4];		//Special for the 83p, used to determine which page of a set to protect
	RAM_PROT_MODE prot_mode;

	int flash_version;
	int ram_version;

	int read_OP_flash_tstates;	//These are for delays on SEs, typically they should be 0.
	int read_NOP_flash_tstates;
	int write_flash_tstates;
	int read_OP_ram_tstates;
	int read_NOP_ram_tstates;
	int write_ram_tstates;

	unsigned char flash_upper;
	unsigned char flash_lower;

	unsigned short ram_upper;
	unsigned short ram_lower;

	int port0E;
	int port0F;
	union {
		struct {
			BOOL flash_enabled : 1;
			BOOL flash_disabled : 1;
		};
		uint8_t port24;
	};

	int port27_remap_count;		// amount of 64 byte chunks remapped from RAM page 0 to bank 3
	int port28_remap_count;		// amount of 64 byte chunks remapped from RAM page 1 to bank 1

	void (*mem_read_break_callback)(void *);
	void (*mem_write_break_callback)(void *);
} memory_context_t, memc;

/* Input/Output device mapping */
typedef void (*devp)(void *, void *);
typedef struct device {
	BOOL active;
	memory_context_t *mem_c;
	void *aux;
	devp code;
	BOOL breakpoint;
	BOOL protected_port;
} device_t;

typedef struct interrupt {
	unsigned char interrupt_val;
	unsigned char skip_factor;
	unsigned char skip_count;
} interrupt_t;

typedef struct pio_context {
	int model;
	struct LCD *lcd;
	struct keypad *keypad;
	struct link *link;
	struct STDINT *stdint;
	struct SE_AUX *se_aux;
	/* list other cross model devices here */

	device_t devices[256];
	interrupt_t interrupt[256];
	int num_interrupt;
	devp breakpoint_callback;
} pio_context_t, pioc;

typedef struct reverse_time {
	BYTE flag;
	regpair(upper_data1, lower_data1, data1);
	regpair(upper_data2, lower_data2, data2);
	BYTE bus;
	BYTE r;
} reverse_time_t;

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
	unsigned char i, r, bus, link_write;
	int imode;
	BOOL interrupt;
	BOOL ei_block;
	BOOL iff1, iff2;
	BOOL halt;
	BOOL read, write, output, input;
	int prefix;
	pioc pio;
	memc *mem_c;
	timerc *timer_c;
	void (*exe_violation_callback)(void *);
	int cpu_version;
	reverse_time_t prev_instruction_list[512];
	reverse_time_t *prev_instruction;
	int reverse_instr;
	BOOL reverse_wrap;
	BOOL do_opcode_callback;
	BOOL is_link_instruction;
	unsigned long long linking_time;
	unsigned long long hasHitEnter;
} CPU_t;

typedef void (*opcodep)(CPU_t*);
typedef void (*index_opcodep)(CPU_t*, char);

inline unsigned char mem_read(memc*, unsigned short);
uint8_t wmem_read(memc*, waddr_t);
uint16_t wmem_read16(memc *mem, waddr_t waddr);
unsigned short mem_read16(memc*, unsigned short);
inline unsigned char mem_write(memc*, unsigned short, char);
inline waddr_t addr_to_waddr(memc*, uint16_t);

void set_break(memc *, waddr_t waddr);
void set_mem_write_break(memc *, waddr_t waddr);
void set_mem_read_break(memc *, waddr_t waddr);

void clear_break(memc *mem, waddr_t waddr);
void clear_mem_write_break(memc *, waddr_t waddr);
void clear_mem_read_break(memc *, waddr_t waddr);

void disable_break(memc *mem, waddr_t waddr);
void disable_mem_write_break(memc *, waddr_t waddr);
void disable_mem_read_break(memc *, waddr_t waddr);

BOOL check_break(memc *, waddr_t);
BOOL check_mem_read_break(memc *mem, waddr_t waddr);
BOOL check_mem_write_break(memc *mem, waddr_t waddr);

BOOL is_priveleged_page(CPU_t *cpu);
void change_page(memc *mem, int bank, char page, BOOL ram);
void update_bootmap_pages(memc *mem_c);

int tc_init(timerc*, int);
int CPU_init(CPU_t*, memc*, timerc*);
int CPU_step(CPU_t*);
int CPU_connected_step(CPU_t *cpu);
inline unsigned char CPU_mem_read(CPU_t *cpu, unsigned short addr);
inline unsigned char CPU_mem_write(CPU_t *cpu, unsigned short addr, unsigned char data);
CPU_t* CPU_clone(CPU_t *cpu);
#define HALT_SCALE	3

#ifdef WITH_REVERSE
int CPU_step_reverse(CPU_t* cpu);
#endif

#ifdef DEBUG
void displayreg(CPU_t *);
#endif


#ifdef NO_TIMER_ELAPSED
#define tc_add( timer_z , num ) \
	(timer_z)->tstates += (uint64_t) num;

#define tc_sub( timer_z , num ) \
	(timer_z)->tstates -= (uint64_t) num;

#define SEtc_add( timer_z , num ) \
	if (cpu->pio.model >= TI_83PSE) {\
		timer_z->tstates += num; \
	}
#define SEtc_sub( timer_z , num ) \
	if (cpu->pio.model >= TI_83PSE) {\
		timer_z->tstates -= num; \
	}

#else
#define tc_add( timer_z , num ) \
	(timer_z)->tstates += (uint64_t) num; \
	(timer_z)->elapsed += ((double)(num))/((double)(timer_z)->freq);

#define tc_sub( timer_z , num ) \
	(timer_z)->tstates -= (uint64_t) num; \
	(timer_z)->elapsed -= ((double)(num))/((double)(timer_z)->freq);

#define SEtc_add( timer_z , num ) \
	if (cpu->pio.model >= TI_83PSE) {\
		timer_z->tstates += num; \
		timer_z->elapsed += ((double)(num))/((double)(timer_z)->freq);\
	}
#define SEtc_sub( timer_z , num ) \
	if (cpu->pio.model >= TI_83PSE) {\
		timer_z->tstates -= num; \
		timer_z->elapsed -= ((double)(num))/((double)(timer_z)->freq);\
	}


#define tc_elapsed( timer_z ) \
	((timer_z)->elapsed)
#endif

#define tc_tstates( timer_z ) \
	((timer_z)->tstates)

#define endflash_break(cpu_v) cpu_v->mem_c->step = 0;\
		if (break_on_invalid_flash) {\
			cpu->mem_c->mem_write_break_callback(cpu);\
		}

#define endflash(cpu_v) cpu_v->mem_c->step = 0;

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
