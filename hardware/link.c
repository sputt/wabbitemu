#include "stdafx.h"

#include "link.h"
#include "calc.h"
#include "lcd.h"	//lcd->active
#include "keys.h"	//key_press

//#define DEBUG

static u_char vout, *vin; // Virtual Link data
#define vlink (((vout & 0x03)|(*vin & 0x03))^3)	// Virtual Link status
static jmp_buf exc_pkt, exc_byte; // Exceptions
/*
 * Byte Exception
 *  Thrown when individual bytes fail to get sent or received over the
 *  virtual link.  Error codes begin with LERR_
 *
 * Packet Exception
 *  Thrown when a packet is of an unexpected or incorrect type */

/* Macro to wrap 16-bit values so they will send
 * correctly on big-endian systems */
#ifdef __BIG_ENDIAN__
#define link_endian(z) ((((z) & 0xFF)<<8) | ((z) >> 8))
#else
#define link_endian(z) (z)
#endif

/* Prototypes of static functions*/
static LINK_ERR forceload_app(CPU_t *, TIFILE_t *);
#ifdef _DEBUG
static void print_command_ID(uint8_t);
#endif

/* Initialize the virtual link with the PC
 * No other initialization is performed */
int link_init(CPU_t *cpu) {
	link_t *link = cpu->pio.link;

	if (link == NULL)
		return 1;

	vout = 0;
	link->client = &vout;
	vin = &link->host;

	return 0;
}

int link_connect(CPU_t *cpu1, CPU_t *cpu2) {
	link_t *link1 = cpu1->pio.link;
	link_t *link2 = cpu2->pio.link;

	if (link1 == NULL || link2 == NULL)
		return 1;

	//link1->host = &link2->client;
	//link2->host = &link1->client;
	link1->client = &link2->host;
	link2->client = &link1->host;

	return 0;
}

/* Run a number of tstates */
static void link_wait(CPU_t *cpu, time_t tstates) {
	long long time_end = tc_tstates(cpu->timer_c) + tstates;

	while (tc_tstates(cpu->timer_c) < time_end)
		CPU_step(cpu);
}

/* Send a byte through the virtual link
 * On error: Throws a Byte Exception */
static void link_send(CPU_t *cpu, u_char byte) {
	CPU_t *cpu2;
	cpu2 = &calcs[1].cpu;
	u_int bit, i;
	link_t *link1, *link2;
	link1 = cpu->pio.link;
	link2 = cpu2->pio.link;
#ifdef WINVER
	if (calcs[1].hwndFrame != NULL) {
		link1->client = &link2->host;
		link2->client = &link1->host;
	}
#endif

	for (bit = 0; bit < 8; bit++, byte >>= 1) {
		vout = (byte & 1) + 1;

		for (i = 0; i < LINK_TIMEOUT && vlink != 0; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);
		if (vlink != 0)
			longjmp(exc_byte, LERR_TIMEOUT);

		vout = 0;
		for (i = 0; i < LINK_TIMEOUT && vlink != 3; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);
		if (vlink != 3)
			longjmp(exc_byte, LERR_TIMEOUT);
	}

	cpu->pio.link->vlink_send++;
}

/* Receive a byte through the virtual link
 * On error: Throws a Byte Exception */
static u_char link_recv(CPU_t *cpu) {
	u_int bit, i;
	u_char byte = 0;

	for (bit = 0; bit < 8; bit++) {
		byte >>= 1;

		for (i = 0; i < LINK_TIMEOUT && vlink == 3; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);

		if (vlink == 0)
			longjmp(exc_byte, LERR_LINK);
		if (vlink == 3)
			longjmp(exc_byte, LERR_TIMEOUT);

		vout = vlink;
		if (vout == 1)
			byte |= 0x80;

		for (i = 0; i < LINK_TIMEOUT && vlink == 0; i += LINK_STEP)
			link_wait(cpu, LINK_STEP);
		if (vlink == 0)
			longjmp(exc_byte, LERR_TIMEOUT);
		vout = 0;
	}

	cpu->pio.link->vlink_recv++;
	return byte;
}

BOOL link_connected()
{
	return calcs[gslot].cpu.pio.link->client != &vout;
}

/* Calculate a TI Link Protocol checksum
 *
 * checksum = 16-bit byte sum of all data bytes
 *  with the carries discarded */
static uint16_t link_chksum(void *data, size_t length) {
	uint16_t chksum = 0;
	size_t i;
	for (i = 0; i < length; i++)
		chksum += ((u_char *) data)[i];

	return chksum;
}

/* Returns a Machine ID for the calc attached to virtual link
 * On error: Returns -1 */
static u_char link_target_ID(CPU_t *cpu) {
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
		return 0x23;
	default:
		return ~0;
	}
}

/* Send a sequence of bytes over the virtual link
 * On error: Throws a Byte Exception */
static void link_send_bytes(CPU_t *cpu, void *data, size_t length) {
	size_t i;
	for (i = 0; i < length; i++)
		link_send(cpu, ((u_char*) data)[i]);
}

/*
 * Receive a sequence of bytes over the virtual link
 * On error: Throws a Byte Exception
 */
static void link_recv_bytes(CPU_t *cpu, void *data, size_t length) {
	size_t i;
	for (i = 0; i < length; i++)
		((u_char*) data)[i] = link_recv(cpu);
}

/* Send a TI packet over the virtual link
 * On error: Throws a Packet Exception */
static void link_send_pkt(CPU_t *cpu, u_char command_ID, void *data) {
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

		if (type_ID == BackupObj)
			data_len = sizeof(TI_BACKUPHDR);
		else if (type_ID >= 0x22 && type_ID <= 0x28)
			data_len = sizeof(TI_FLASHHDR);
		else {
			data_len = sizeof(TI_VARHDR);
			// Non flash calculators do not pass
			// version or 2nd flags
			if (cpu->pio.model < TI_83P)
				data_len -= 2;
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
static void link_recv_pkt(CPU_t *cpu, TI_PKTHDR *hdr, u_char *data) {
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

/* Send a Request To Send packet
 * On error: Throws Packet Exception */
static void link_RTS(CPU_t *cpu, TIFILE_t *tifile, int dest) {
	TI_VARHDR var_hdr;

	var_hdr.length = link_endian(tifile->var->length);
	var_hdr.type_ID = tifile->var->vartype;
	memset(var_hdr.name, 0, sizeof(var_hdr.name));
#ifdef WINVER
	strncpy_s(var_hdr.name, (char *) tifile->var->name, 8);
#else
	strncpy(var_hdr.name, (char *) tifile->var->name, 8);
#endif
	var_hdr.version = tifile->var->version;
	if (dest == SEND_RAM) {
		var_hdr.type_ID2 = 0x00;
	} else if (dest == SEND_ARC) {
		var_hdr.type_ID2 = 0x80;
	} else {
		var_hdr.type_ID2 = tifile->var->flag;
	}

	//printf("Model: %d, length: %d\n", cpu->pio.model, link_endian(tifile->var->length));
	if (cpu->pio.model == TI_82)
		link_send_pkt(cpu, CID_VAR, &var_hdr);
	else
		link_send_pkt(cpu, CID_RTS, &var_hdr);
}

// Order that VTI uses:
// Packet 1
//	8	ID =
//	8	CID_VAR
//	16	var header size
//	- var hdr

// Send ACK
// Send CTS

// GetDATA


/*
 * Send a variable over the virtual link
 * On error: Returns an error code
 */
LINK_ERR link_send_var(CPU_t *cpu, TIFILE_t *tifile, SEND_FLAG dest) {
	if (link_init(cpu))
		return LERR_NOTINIT;

	// If the calculator's LCD is off, it likely is not in
	// the correct software state to receive link data.
	// Turn it on by simulating pressing the 'ON' button
	if (!cpu->pio.lcd->active) {
		link_wait(cpu, MHZ_6);
		cpu->pio.keypad->on_pressed |= KEY_FALSEPRESS;
		link_wait(cpu, MHZ_6/2);
		cpu->pio.keypad->on_pressed &= ~KEY_FALSEPRESS;
		link_wait(cpu, MHZ_6);

		if (!cpu->pio.lcd->active)
			return LERR_LINK;
	}

	// Here's some code to temporarily pass off the apps
	// to a send app function
	if (tifile->type == FLASH_TYPE)
		return forceload_app(cpu, tifile);

	// Make sure the TIFILE is well formed
	if (tifile->var == NULL)
		return LERR_FILE;

	cpu->pio.link->vlink_size = tifile->var->length;

	int err;
	switch (err = setjmp(exc_pkt)) {
	case 0: {
		TI_PKTHDR rpkt;
		u_char data[64];

		// Request to send
		link_RTS(cpu, tifile, dest);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK)
			return LERR_LINK;

		// Receive Clear To Send
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_CTS) {
			if (rpkt.command_ID == CID_EXIT) {
				link_send_pkt(cpu, CID_ACK, NULL);
				return LERR_MEM;
			} else
				return LERR_LINK;
		}

		// Send the ACK
		link_send_pkt(cpu, CID_ACK, NULL);

		// Send the single data packet containing the whole file
		TI_DATA s_data = { tifile->var->length, tifile->var->data };
		link_send_pkt(cpu, CID_DATA, &s_data);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK)
			return LERR_LINK;

		// Send the End of Transmission
		if (cpu->pio.model != TI_82)
			link_send_pkt(cpu, CID_EOT, NULL);

			return LERR_SUCCESS;
	}
	default:
			return LERR_SYSTEM;
	}
}

/* Send a flash application over the virtual link
 * On error: Returns an error code */
LINK_ERR link_send_app(CPU_t *cpu, TIFILE_t *tifile) {
	if (link_init(cpu) != 0)
		return LERR_NOTINIT;

	if (tifile->flash == NULL)
		return LERR_FILE;

	// Get the size of the whole APP
	size_t i;
	for (i = 0, cpu->pio.link->vlink_size = 0; i < tifile->flash->pages; i++)
		cpu->pio.link->vlink_size += tifile->flash->pagesize[i];

	//printf("App total size: %d\n", cpu->pio.link->vlink_size);
	int err;
	switch (err = setjmp(exc_pkt)) {
	case 0: {
		TI_PKTHDR rpkt;
		u_char data[64];

		// Send the version request
		link_send_pkt(cpu, CID_VER, NULL);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK)
			return LERR_LINK;

		// Send the CTS
		link_send_pkt(cpu, CID_CTS, NULL);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK)
			return LERR_LINK;

		// Receive the version
		link_recv_pkt(cpu, &rpkt, data);

		// Send the ACK
		link_send_pkt(cpu, CID_ACK, NULL);

		// Send the ready request
		link_send_pkt(cpu, CID_RDY, NULL);

		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK)
			return LERR_LINK;
		uint16_t page, offset;
		for (page = 0; page < tifile->flash->pages; page++) {
			for (offset = 0; offset < 0x4000 && offset
					< tifile->flash->pagesize[page]; offset += 0x80) {

				// Send the flash header
				TI_FLASHHDR flash_hdr;
				flash_hdr.sizeLSB = link_endian(0x0080);
				flash_hdr.type_ID = FlashObj;
				flash_hdr.sizeMSB = link_endian(0x0000);
				flash_hdr.offset = link_endian(offset + 0x4000);
				flash_hdr.page = link_endian(page);
				link_send_pkt(cpu, CID_VAR, &flash_hdr);

				// Receive the ACK
				link_recv_pkt(cpu, &rpkt, data);
				if (rpkt.command_ID != CID_ACK)
					return LERR_LINK;

				// Receive the CTS
				link_recv_pkt(cpu, &rpkt, data);
				if (rpkt.command_ID != CID_CTS) {
					if (rpkt.command_ID == CID_EXIT)
						return LERR_MEM;
					return LERR_LINK;
				}

				// Send the ACK
				link_send_pkt(cpu, CID_ACK, NULL);

				// Send the data packet
				TI_DATA s_data = { 0x0080, &tifile->flash->data[page][offset] };
				link_send_pkt(cpu, CID_DATA, &s_data);

				// Receive the ACK
				link_recv_pkt(cpu, &rpkt, data);
				if (rpkt.command_ID != CID_ACK)
					return LERR_LINK;
			}
		}

		link_send_pkt(cpu, CID_EOT, NULL);

		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK)
			return LERR_LINK;
			return LERR_SUCCESS;
	}
	default:
			return LERR_SYSTEM;
	}
}

/* Forceload a TI-83+ series APP
 * On error: Returns an error code */
static LINK_ERR forceload_app(CPU_t *cpu, TIFILE_t *tifile) {
	u_char (*dest)[PAGE_SIZE] = (u_char (*)[PAGE_SIZE]) cpu->mem_c->flash;
	if (dest == NULL)
		return LERR_MODEL;

	if (tifile->flash == NULL)
		return LERR_FILE;

	upages_t upages;
	state_userpages(cpu, &upages);
	if (upages.start == -1)
		return LERR_MODEL;
	
	u_int page;
	for (page = upages.start; page >= upages.end + tifile->flash->pages
			&& dest[page][0x00] == 0x80 && dest[page][0x01] == 0x0F; page
			-= dest[page][0x1C]) {

		if (!memcmp(&dest[page][0x12], &tifile->flash->data[0][0x12], 8)) {
			if (dest[page][0x1C] != tifile->flash->pages)
				return link_send_app(cpu, tifile);
			u_int i;
			for (i = 0; i < tifile->flash->pages; i++, page--) {
				memcpy(dest[page], tifile->flash->data[i], PAGE_SIZE);
			}
			printf("Found already\n");
			return LERR_SUCCESS;
		}
	}

	if (page < upages.end)
		return LERR_MEM;

	//there is probably some logic here that im missing...
	//the 83p wtf is up with that offset
	int offset = 0x1E50;
	if (cpu->pio.model == TI_83P)
		offset = 0x1F18;
	//erase the part of the certifcate that marks it as a trial app
	dest[cpu->mem_c->flash_pages-2][offset + 2 * (upages.start - page)] = 0x80;
	dest[cpu->mem_c->flash_pages-2][offset+1 + 2 * (upages.start - page)] = 0x00;
	//force reset the applist says BrandonW. seems to work, apps show up :P
	mem_write(cpu->mem_c, 0x9C87, 0x00);

	u_char *space = &dest[page][PAGE_SIZE - 1];
	u_int i;
	// Make sure the subsequent pages are empty
	for (i = 0; i < tifile->flash->pages * PAGE_SIZE; i++, space--) {
		if (*space != 0xFF) {
			printf("Subsequent pages not empty\n");
			return LERR_MEM;
		}
	}
	for (i = 0; i < tifile->flash->pages; i++, page--) {
		memcpy(dest[page], tifile->flash->data[i], PAGE_SIZE);
	}

	// Discard any error link_send_app returns
	link_send_app(cpu, tifile);
	// Delay for a few seconds so the calc will be responsive
	cpu->pio.link->vlink_size = 100;
	for (cpu->pio.link->vlink_send = 0; cpu->pio.link->vlink_send < 100; cpu->pio.link->vlink_send += 20) {
		link_wait(cpu, MHZ_6*1);
	}
	return LERR_SUCCESS;
}

#ifdef _DEBUG
static void print_command_ID(uint8_t command_ID) {
	char buffer[256];
#ifdef WINVER
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
#else
	switch (command_ID) {
		case CID_ACK:
		strcpy(buffer, "ACK");
		break;
		case CID_CTS:
		strcpy(buffer, "CTS");
		break;
		case CID_DATA:
		strcpy(buffer, "DATA");
		break;
		case CID_DEL:
		strcpy(buffer, "DEL");
		break;
		case CID_EOT:
		strcpy(buffer, "EOT");
		break;
		case CID_ERR:
		strcpy(buffer, "ERR");
		break;
		case CID_EXIT:
		strcpy(buffer, "SKIP/EXIT");
		break;
		case CID_RDY:
		strcpy(buffer, "RDY");
		break;
		case CID_REQ:
		strcpy(buffer, "REQ");
		break;
		case CID_RTS:
		strcpy(buffer, "RTS");
		break;
		case CID_SCR:
		strcpy(buffer, "SCR");
		break;
		case CID_VAR:
		strcpy(buffer, "VAR");
		break;
		case CID_VER:
		strcpy(buffer, "VER");
		break;
		default:
		strcpy(buffer, "error");
		break;
	}
#endif
	printf(buffer);
}
#endif


void Load_8xu(FILE* infile) {
	intelhex_t ihex;
	if (!infile)
		return;

	//0x31 says if its an OS or App
	fseek(infile, 0x31, SEEK_SET);
	if (fgetc(infile) != 0x23) {
		puts("[0x31] != 0x23");
		return;
	}

	fseek(infile, TI_FLASH_HEADER_SIZE,SEEK_SET);

	// Find the first page, usually after the first line
	do {
		if (!ReadIntelHex(infile, &ihex))
			return;
	} while (ihex.type != 0x02 || ihex.size != 2);

	int curpage = ((ihex.data[0] << 8) | ihex.data[1]) & 0x7F;
	uint8_t (*flash)[16384] = (uint8_t(*)[16384]) calcs[gslot].mem_c.flash;

	printf("Loading IntelHex OS\n");
	printf("Loading page %02Xh\n", curpage);
	while (1) {
		if (!ReadIntelHex(infile, &ihex)) {
			return;
		}
		switch (ihex.type) {
		case 0x00:
			memcpy(flash[curpage] + (ihex.address & 0x3FFF), ihex.data, ihex.size);
			break;
		case 0x02:
			if (ihex.size == 2) {
				curpage = ((ihex.data[0] << 8) + ihex.data[1]) & 0x7F;
				//HACK: umm yeah but i dont have a better way to do this
				if (curpage > 10)
					curpage += calcs[gslot].mem_c.flash_pages - 0x20;
			}
			printf("Loading page %02Xh\n", curpage);
			break;
		default:
			return;
		}
	}
}

int ReadIntelHex(FILE* ifile, intelhex_t *ihex) {
	uint8_t str[600];
	uint32_t i, size, addr, type, byte;
	memset(str, 0x00, 600);
	if (!fgets((char*)str, 580, ifile))
		return 0;
	if (str[0] == 0) memcpy(str, str+1, 579);
#ifdef WINVER
	if (sscanf_s((const char*)str, ":%02X%04X%02X%*s", &size, &addr, &type) != 3)
#else
	if (sscanf((const char*)str, ":%02X%04X%02X%*s", &size, &addr, &type) != 3)
#endif
		return 0;
	ihex->size = size;
	ihex->address = addr;
	ihex->type = type;
	memset(ihex->data, 0x00, 256);
	for (i = 0; i < size; i++) {
#ifdef WINVER
		if (sscanf_s((const char*)str + 9 + (i * 2), "%02X", &byte) != 1)
#else
		if (sscanf((const char*)str + 9 + (i * 2), "%02X", &byte) != 1)
#endif
			return 0;
		ihex->data[i] = byte;
	}
#ifdef WINVER
	if (sscanf_s((const char*)str + 9 + (i * 2), "%02X", &byte) != 1)
#else
	if (sscanf((const char*)str + 9 + (i * 2), "%02X", &byte) != 1)
#endif
		return 0;
	ihex->chksum = byte;
	return 1;
}

void writeboot(FILE* infile) {
	intelhex_t ihex;
	if (!infile) return;
	int curpage = calcs[gslot].mem_c.flash_pages - 1;			//last page is boot page
	unsigned char (*flash)[16384] = (uint8_t(*)[16384]) calcs[gslot].mem_c.flash;
	while(1) {
		if (!ReadIntelHex(infile,&ihex)) {
			return;
		}
		switch(ihex.type) {
			case 0x00:
				memcpy(flash[curpage]+(ihex.address&0x3FFF),ihex.data,ihex.size);
				break;
			case 0x02:
				break;
			default:
				return;
		}
	}
}

int FindField(unsigned char *data,unsigned char Field) {
	int i;
	for(i=6;i<128;) {
		if (data[i++]!=0x80) return 0;
		if ((data[i]&0xF0)==Field) return i+1;
		i+=1+(data[i]&0x0F);
	}
	return 0;
}



