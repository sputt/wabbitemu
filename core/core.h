#ifndef CORE_H
#define CORE_H

#include "coretypes.h"
#include "modeltypes.h"

/*defines the start of the app page*/
/*this page starts in HIGH mem and grows to LOW */
#define TI_73_APPPAGE		0x15
#define TI_83P_APPPAGE		0x15
#define TI_83PSE_APPPAGE	0x69
#define TI_84P_APPPAGE		0x29
#define TI_84PSE_APPPAGE	0x69
#define TI_84PCSE_APPPAGE	0xE3


/*defines the Number of user archive pages*/
#define TI_73_USERPAGES		0x0A
#define TI_83P_USERPAGES	0x0A
#define TI_83PSE_USERPAGES	0x60
#define TI_84P_USERPAGES	0x1E
#define TI_84PSE_USERPAGES	0x60
#define TI_84PCSE_USERPAGES	0xDB

#ifndef PAGE_SIZE
#define PAGE_SIZE 16384
#endif

#ifndef BIT
#define BIT(bit) (1 << (bit))
#endif

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
#define NUM_BANKS 5

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

typedef enum {
	FLASH_READ,
	FLASH_AA,
	FLASH_55,
	FLASH_PROGRAM,
	FLASH_ERASE,
	FLASH_ERASE_AA,
	FLASH_ERASE_55,
	FLASH_FASTMODE,
	FLASH_FASTMODE_PROG,
	FLASH_FASTMODE_EXIT,
	FLASH_AUTOSELECT,
	FLASH_ERROR,
} FLASH_COMMAND;

typedef struct memory_context {
	/* to be defined */
	u_char *flash;
	u_char *ram;
	union {
		struct {
			u_char *flash_break;
			u_char *ram_break;
		};
		u_char *breaks[2];
	};

	BOOL (*breakpoint_manager_callback)(struct memory_context *, BREAK_TYPE, waddr_t);

	int flash_size;
	int flash_pages;
	int ram_size;
	int ram_pages;

	FLASH_COMMAND step;				// the current flash command
	uint64_t flash_write_delay;		// number of tstates to delay before allowing flash read/write
	BOOL flash_locked;				// Whether flash is writeable or not.
	unsigned char flash_write_byte;	// the last value written to flash
	BOOL flash_error;				// whether there was an error programming the byte
	unsigned char flash_toggles;	// flash toggles
	bank_state_t *banks;			//pointer to the correct bank state currently
	bank_state_t normal_banks[NUM_BANKS];	//Current state of each bank
									// structure 5 is used to preserve the 4th in boot map
	bank_state_t bootmap_banks[NUM_BANKS];	//used to hold a backup of the banks when this is boot mapped
	BOOL boot_mapped;				//Special mapping used in boot that changes how paging works
	BOOL hasChangedPage0;			//Check if boot code is still mapped to page 0 or not
	int protected_page_set;			//Special for the 83p, used to determine which group of pages you are referring to
	int protected_page[4];			//Special for the 83p, used to determine which page of a set to protect
	RAM_PROT_MODE prot_mode;

	int flash_version;
	int ram_version;

	int read_OP_flash_tstates;	//These are for delays on SEs, typically they should be 0.
	int read_NOP_flash_tstates;
	int write_flash_tstates;
	int read_OP_ram_tstates;
	int read_NOP_ram_tstates;
	int write_ram_tstates;

	unsigned short flash_upper;
	unsigned short flash_lower;

	unsigned short ram_upper;
	unsigned short ram_lower;

	unsigned char port06;
	unsigned char port07;
	// upper bits of port 6
	unsigned char port0E;
	// upper bits of port 7
	unsigned char port0F;
	// upper bits of ports 22 and 23
	unsigned char port24;

	int port27_remap_count;		// amount of 64 byte chunks remapped from RAM page 0 to bank 3
	int port28_remap_count;		// amount of 64 byte chunks remapped from RAM page 1 to bank 1
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
	unsigned char skip_count;
	unsigned char skip_factor;
	device_t *device;
} interrupt_t;

typedef struct pio_context {
	int model;
	struct LCDBase *lcd;
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

#define MIN_BLOCK_SIZE 16
#define PROFILER_NUM_BLOCKS (PAGE_SIZE / MIN_BLOCK_SIZE)

typedef struct profiler {
	BOOL running;
	int blockSize;
	uint64_t totalTime;
	uint64_t **flash_data;
	uint64_t **ram_data;
	BOOL show_disassembly;
	BOOL sort_output;
} profiler_t;

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
	int cpu_version;
	int model_bits;
	reverse_time_t prev_instruction_list[512];
	reverse_time_t *prev_instruction;
	int reverse_instr;
	BOOL reverse_wrap;
	BOOL do_opcode_callback;
	BOOL is_link_instruction;
	unsigned long long linking_time;
	unsigned long long hasHitEnter;

	profiler_t profiler;
	unsigned short old_pc;

	void(*exe_violation_callback)(struct CPU *);
	void(*invalid_flash_callback)(struct CPU *);
	void(*mem_read_break_callback)(struct CPU *);
	void(*mem_write_break_callback)(struct CPU *);
	void(*lcd_enqueue_callback)(struct CPU *);
} CPU_t;

typedef int (*opcodep)(CPU_t*);
typedef int (*index_opcodep)(CPU_t*, char);

unsigned char mem_read(memc*, unsigned short);
uint8_t wmem_read(memc*, waddr_t);
uint16_t wmem_read16(memc *mem, waddr_t waddr);
unsigned short mem_read16(memc*, unsigned short);
unsigned char mem_write(memc*, unsigned short, char);
uint8_t wmem_write(memc *mem, waddr_t waddr, uint8_t data);
waddr_t addr16_to_waddr(memc*, uint16_t);
waddr_t addr32_to_waddr(unsigned int addr, BOOL is_ram);

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
void change_page(memc *mem, int bank, u_char page, BOOL ram);
void update_bootmap_pages(memc *mem_c);

int tc_init(timerc*, int);
int CPU_init(CPU_t*, memc*, timerc*);
int CPU_reset(CPU_t *);
int CPU_step(CPU_t*);
int CPU_connected_step(CPU_t *cpu);
unsigned char CPU_mem_read(CPU_t *cpu, unsigned short addr);
void CPU_mem_write(CPU_t *cpu, unsigned short addr, unsigned char data);
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

#endif

#define tc_tstates( timer_z ) \
	((timer_z)->tstates)

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
