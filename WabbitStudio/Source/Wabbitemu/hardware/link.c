#include "stdafx.h"

#include "var.h"
#include "link.h"
#include "lcd.h"	//lcd->active
#include "colorlcd.h"
#include "keys.h"	//key_press

//#define DEBUG
#define vlink(zlink) ((((zlink)->vout & 0x03)|(*((zlink)->vin) & 0x03))^3)	// Virtual Link status
jmp_buf exc_pkt, exc_byte; // Exceptions
/*
 * Byte Exception
 *  Thrown when individual bytes fail to get sent or received over the
 *  virtual link.  Error codes begin with LERR_
 *
 * Packet Exception
 *  Thrown when a packet is of an unexpected or incorrect type */

#ifdef _DEBUG
static void print_command_ID(uint8_t);
#endif

/* Initialize the virtual link with the PC
 * No other initialization is performed */
int link_init(CPU_t *cpu) {
	link_t *link = cpu->pio.link;

	if (link == NULL)
		return 1;

	link->vout = 0;
	link->client = &link->vout;
	link->vin = &link->host;

	return 0;
}

int link_connect(CPU_t *cpu1, CPU_t *cpu2) {
	link_t *link1 = cpu1->pio.link;
	link_t *link2 = cpu2->pio.link;

	if (link1 == NULL || link2 == NULL)
		return 1;

	link1->client = &link2->host;
	link2->client = &link1->host;

	return 0;
}

int link_disconnect(CPU_t *cpu) {
	cpu->pio.link->client = NULL;
	return 0;
}

/* Run a number of tstates */
void link_wait(CPU_t *cpu, time_t tstates) {
	uint64_t time_end = cpu->timer_c->tstates + tstates;

	while (cpu->timer_c->tstates < time_end) {
		CPU_step(cpu);
	}
}

/* Send a byte through the virtual link
 * On error: Throws a Byte Exception */
static void link_send(CPU_t *cpu, u_char byte) {
	link_t *link = cpu->pio.link;
	u_int i;

	for (u_int bit = 0; bit < 8; bit++, byte >>= 1) {
		link->vout = (byte & 1) + 1;

		for (i = 0; i < LINK_TIMEOUT && vlink(link) != 0; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);
		if (i >= LINK_TIMEOUT)
			longjmp(exc_byte, LERR_TIMEOUT);

		link->vout = 0;
		for (i = 0; i < LINK_TIMEOUT && vlink(link) != 3; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);
		if (i >= LINK_TIMEOUT)
			longjmp(exc_byte, LERR_TIMEOUT);
	}

	cpu->pio.link->vlink_send++;
}

/* Receive a byte through the virtual link
* On error: Throws a Byte Exception */
static u_char link_recv(CPU_t *cpu) {
	link_t *link = cpu->pio.link;
	u_char byte = 0;
	u_int i;

	for (u_int bit = 0; bit < 8; bit++) {
		byte >>= 1;

		for (i = 0; i < LINK_TIMEOUT && vlink(link) == 3; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);

		if (vlink(link) == 0)
			longjmp(exc_byte, LERR_LINK);
		if (i >= LINK_TIMEOUT)
			longjmp(exc_byte, LERR_TIMEOUT);

		link->vout = vlink(link);
		if (link->vout == 1)
			byte |= 0x80;

		for (i = 0; i < LINK_TIMEOUT && vlink(link) == 0; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);
		if (i >= LINK_TIMEOUT)
			longjmp(exc_byte, LERR_TIMEOUT);
		link->vout = 0;
	}

	cpu->pio.link->vlink_recv++;
	return byte;
}

/* Calculate a TI Link Protocol checksum
 *
 * checksum = 16-bit byte sum of all data bytes
 *  with the carries discarded */
static uint16_t link_chksum(const void *data, size_t length) {
	uint16_t chksum = 0;
	for (size_t i = 0; i < length; i++)
		chksum += ((u_char *) data)[i];

	return chksum;
}

/* Returns a Machine ID for the calc attached to virtual link
 * On error: Returns -1 */
static u_char link_target_ID(const CPU_t *cpu) {
	switch (cpu->pio.model) {
	case TI_73:
		return 0x07;
	case TI_82:
		return 0x02;
	case TI_83:
		return 0x03;
	case TI_85:
		return 0x05;
	case TI_86:
		return 0x06;
	case TI_83P:
	case TI_83PSE:
	case TI_84P:
	case TI_84PSE:
	case TI_84PCSE:
		return 0x23;
	default:
		return 0xFF;
	}
}

/* Send a sequence of bytes over the virtual link
 * On error: Throws a Byte Exception */
void link_send_bytes(CPU_t *cpu, void *data, size_t length) {
	size_t i;
	for (i = 0; i < length; i++)
		link_send(cpu, ((u_char*) data)[i]);
}

/*
 * Receive a sequence of bytes over the virtual link
 * On error: Throws a Byte Exception
 */
void link_recv_bytes(CPU_t *cpu, void *data, size_t length) {
	size_t i;
	for (i = 0; i < length; i++)
		((u_char*) data)[i] = link_recv(cpu);
}

/* Send a TI packet over the virtual link
 * On error: Throws a Packet Exception */
void link_send_pkt(CPU_t *cpu, u_char command_ID, void *data) {
	TI_PKTHDR hdr;
	uint16_t data_len;

	hdr.machine_ID = link_target_ID(cpu);
	hdr.command_ID = command_ID;
#ifdef _DEBUG
	printf("SEND: ");
	print_command_ID(command_ID);
	putchar(' ');
#endif
	switch (command_ID) {
	case CID_VAR:
	case CID_DEL:
	case CID_REQ:
	case CID_RTS: {
		uint8_t type_ID = ((TI_VARHDR *) data)->type_ID;

		if (type_ID == BackupObj || (type_ID == BackupObj_82 && cpu->pio.model == TI_82))
			data_len = sizeof(TI_BACKUPHDR);
		else if (type_ID >= 0x22 && type_ID <= 0x28)
			data_len = sizeof(TI_FLASHHDR);
		else {
			data_len = sizeof(TI_VARHDR);
			// Non flash calculators do not pass
			// version or 2nd flags
			if (cpu->pio.model < TI_83P) {
				//85/86 does have name length 
				if (cpu->pio.model == TI_85 || cpu->pio.model == TI_86) {
					data_len--;
				} else {
					data_len -= 2;
				}
			}
		}
		cpu->pio.link->vlink_send -= (sizeof(TI_PKTHDR) + data_len);
		break;
	}
	case CID_DATA:
		data_len = ((TI_DATA*) data)->length;
		data = ((TI_DATA*) data)->data;
		cpu->pio.link->vlink_send -= (sizeof(TI_PKTHDR));
		break;
	case CID_EXIT:
		data_len = 5;
		cpu->pio.link->vlink_send -= (sizeof(TI_PKTHDR) + data_len);
		break;
	default:
		data_len = 0;
		cpu->pio.link->vlink_send -= (sizeof(TI_PKTHDR) + data_len);
		break;
	}

#ifdef _DEBUG
	printf("(%d) ", data_len);
	if (command_ID != CID_DATA) {
		int i;
		for (i = 0; i < data_len; i++) {
			printf("%02x ", ((u_char *) data)[i]);
		}
	}
	putchar('\n');
#endif

	int err;
	switch (err = setjmp(exc_byte)) {
	case 0:
		link_wait(cpu, LINK_DELAY);
		//if (command_ID == CID_DATA)
		//	data_len--;
		hdr.data_len = link_endian(data_len);
		link_send_bytes(cpu, &hdr, sizeof(TI_PKTHDR));
		if (data_len != 0) {
			uint16_t chksum = link_endian(link_chksum(data, data_len));
			link_send_bytes(cpu, data, data_len);
			link_send_bytes(cpu, &chksum, sizeof(chksum));
			cpu->pio.link->vlink_send -= sizeof(chksum);
		}
		return;
	default:
		return longjmp(exc_pkt, err);
	}
}

/* Receive a TI packet over the virtual link (blocking)
 * On error: Throws a Packet Exception */
void link_recv_pkt(CPU_t *cpu, TI_PKTHDR *hdr, u_char *data) {
	int err;
	switch (err = setjmp(exc_byte)) {
	case 0:
		memset(hdr, 0, sizeof(TI_PKTHDR));
		// Receive the packet header
		hdr->machine_ID = link_recv(cpu);
		hdr->command_ID = link_recv(cpu);
		hdr->data_len = link_recv(cpu) + (link_recv(cpu) << 8);
#ifdef _DEBUG
		printf("RECV %02x: ", hdr->machine_ID);
		print_command_ID(hdr->command_ID);
		putchar('\n');
#endif
		switch (hdr->command_ID) {
		case CID_VAR:
		case CID_DEL:
		case CID_REQ:
		case CID_RTS: {
			link_recv_bytes(cpu, data, 3);
			uint8_t type_ID = data[2];

			if (type_ID == BackupObj)
				hdr->data_len = sizeof(TI_BACKUPHDR);
			else if (type_ID >= 0x22 && type_ID <= 0x28)
				hdr->data_len = sizeof(TI_FLASHHDR);
			else
				hdr->data_len = sizeof(TI_VARHDR);

			link_recv_bytes(cpu, &data[3], hdr->data_len - 3);
			break; // Do checksum
		}
		case CID_DATA:
		case 0:
			// Receive the data
			link_recv_bytes(cpu, data, hdr->data_len);
			break; // Do checksum
		default:
			return;
		}
		break;
	default:
		return longjmp(exc_pkt, err);
	}
	uint16_t chksum = link_recv(cpu) + (link_recv(cpu) << 8);

	if (chksum != link_chksum(data, hdr->data_len))
		longjmp(exc_pkt, LERR_CHKSUM);
}

#ifdef _DEBUG
static void print_command_ID(uint8_t command_ID) {
	char buffer[256];
	switch (command_ID) {
		case CID_ACK:
		strcpy_s(buffer, "ACK");
		break;
		case CID_CTS:
		strcpy_s(buffer, "CTS");
		break;
		case CID_DATA:
		strcpy_s(buffer, "DATA");
		break;
		case CID_DEL:
		strcpy_s(buffer, "DEL");
		break;
		case CID_EOT:
		strcpy_s(buffer, "EOT");
		break;
		case CID_ERR:
		strcpy_s(buffer, "ERR");
		break;
		case CID_EXIT:
		strcpy_s(buffer, "SKIP/EXIT");
		break;
		case CID_RDY:
		strcpy_s(buffer, "RDY");
		break;
		case CID_REQ:
		strcpy_s(buffer, "REQ");
		break;
		case CID_RTS:
		strcpy_s(buffer, "RTS");
		break;
		case CID_SCR:
		strcpy_s(buffer, "SCR");
		break;
		case CID_VAR:
		strcpy_s(buffer, "VAR");
		break;
		case CID_VER:
		strcpy_s(buffer, "VER");
		break;
		default:
		strcpy_s(buffer, "error");
		break;
	}
	printf(buffer);
}
#endif

void writeboot(FILE* infile, memory_context_t *mem_c, int page) {
	INTELHEX_t ihex;
	if (!infile) return;
	if (page == -1)
		page += mem_c->flash_pages;			// last page is boot page
	unsigned char (*flash)[PAGE_SIZE] = (uint8_t(*)[PAGE_SIZE]) mem_c->flash;
	while(1) {
		if (!ReadIntelHex(infile, &ihex)) {
			return;
		}
		switch(ihex.Type) {
			case 0x00:
				memcpy(flash[page] + (ihex.Address & 0x3FFF), ihex.Data, ihex.DataSize);
				break;
			case 0x02:
				break;
			default:
				return;
		}
	}
}