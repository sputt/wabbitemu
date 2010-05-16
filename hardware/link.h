#ifndef LINK_H
#define LINK_H
#include "core.h"		// CPU_t

#ifdef WINVER
#include "sound.h"		// audio_t
#endif

#include "var.h"
#include "state.h"

// Link timing
#define LINK_DELAY 			100				/* Delay between commands */
#define LINK_GARBAGE_DELAY 	(14 * MHZ_6)	/* Delay for oncalc garbage collector */
#define LINK_TIMEOUT 		(2 * MHZ_6)		/* Maximum time given to respond per bit */
#define LINK_STEP			10				/* cycles between link status checks */

// Link errors
typedef enum {
	LERR_SUCCESS = 0,			/* No error */
	LERR_LINK = 1,				/* General link error */
	LERR_TIMEOUT,				/* Time out error */
	LERR_FORCELOAD,				/* Error force loading an application (TI-83+) */
	LERR_CHKSUM,				/* Packet with invalid checksum was received */
	LERR_NOTINIT,				/* Link was not initialized */
	LERR_MEM,					/* Not enough memory on calc */
	LERR_MODEL,					/* Not the correct model for file */
	LERR_FILE,					/* Invalid TIFILE in argument */
	LERR_SYSTEM					/* Something wrong in wabbitemu */
} LINK_ERR;

// Destination flags
typedef enum {
	SEND_CUR,					/* sends based on current flag settings */
	SEND_RAM,					/* sends to RAM, regardless of flag settings */
	SEND_ARC					/* sends to archive, regardless of flag settings */
} SEND_FLAG;

typedef enum {
	CID_VAR		= 0x06,			/* Request variable header */
	CID_CTS		= 0x09,			/* Clear to send */
	CID_DATA	= 0x15,
	CID_VER		= 0x2D,
	CID_EXIT	= 0x36,
	CID_ACK		= 0x56,
	CID_ERR		= 0x5A,
	CID_RDY		= 0x68,
	CID_SCR		= 0x6D,
	CID_DEL		= 0x88,
	CID_EOT		= 0x92,
	CID_REQ		= 0xA2,
	CID_RTS		= 0xC9
} LINK_COMMAND_ID;

/* Contains connections and current state
 * of a link port. */
typedef struct link {
	u_char host;					// what we wrote to the link port
	u_char *client;					// what they wrote to the link port
	volatile size_t vlink_send;		// amount already sent over vlink
	volatile size_t vlink_recv;		// amount already received over the link
	size_t vlink_size;			// Size of the var currently on the link (if known)
	#ifdef WINVER
	AUDIO_t audio;
	#endif
} link_t;

#pragma pack(push, 1)

/* Header for packets send over the link */
typedef struct _TI_PKTHDR {
	uint8_t machine_ID;			/* corresponds to a particular device */
	uint8_t command_ID;			/* specifies the format of the rest of the pkt */
	uint16_t data_len;			/* optional field */
} TI_PKTHDR;

typedef struct {
	uint8_t size;
	uint16_t address;
	uint8_t type;
	uint8_t data[256];
	uint8_t chksum;
} intelhex_t;

/* Header describing variable that is
 * to send or received over the link */
typedef struct _TI_VARHDR {
	uint16_t length;			// datalength not including the header
	uint8_t type_ID;			// specifies type of variable
	char name[8];
	uint8_t version;
	uint8_t type_ID2;
} TI_VARHDR;

/* Header describing the flash application
 * sent or received over the link */
typedef struct _TI_FLASHHDR {
	uint16_t sizeLSB;
	uint8_t type_ID;
	uint16_t sizeMSB;
	uint8_t flag;
	uint16_t offset;
	uint16_t page;
} TI_FLASHHDR;

typedef struct _TI_BACKUPHDR {
	uint16_t flags_size;
	uint8_t type_ID;
	uint16_t data_size;
	uint16_t symbol_size;
	uint16_t user_addr;
} TI_BACKUPHDR;

#pragma pack(pop)

typedef struct _TI_DATA {
	uint16_t length;
	void *data;
} TI_DATA;

#define RealObj         0x00
#define ListObj         0x01
#define MatObj          0x02
#define EquObj          0x03
#define StrngObj        0x04
#define ProgObj         0x05
#define ProtProgObj     0x06
#define PictObj         0x07
#define GDBObj          0x08
#define UnknownObj      0x09
#define UnknownEquObj   0x0A
#define NewEquObj       0x0B
#define CplxObj         0x0C
#define CListObj        0x0D
#define UndefObj        0x0E
#define WindowObj       0x0F
#define ZStoObj         0x10
#define TblRngObj       0x11
#define LCDObj          0x12
#define BackupObj       0x13
#define AppObj			0x14
#define AppVarObj		0x15
#define TempProgObj     0x16
#define GroupObj        0x17
#define EquObj_2        0x23
// used with linking
#define FlashObj		0x24
#define IDListObj		0x26
#define EquObj_3        0x63

LINK_ERR link_send_var(CPU_t*, TIFILE_t*, SEND_FLAG);
void Load_8xu(FILE*);
int ReadIntelHex(FILE*, intelhex_t *);
int link_connect(CPU_t *, CPU_t *);
#endif

