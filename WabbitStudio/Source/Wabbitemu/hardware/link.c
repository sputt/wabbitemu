#include "stdafx.h"

#include "var.h"
#include "link.h"
#include "lcd.h"	//lcd->active
#include "colorlcd.h"
#include "keys.h"	//key_press

//#define DEBUG
#define vlink(zlink) ((((zlink)->vout & 0x03)|(*((zlink)->vin) & 0x03))^3)	// Virtual Link status
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
static LINK_ERR link_send_app(CPU_t *, TIFILE_t *);
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
static void link_wait(CPU_t *cpu, time_t tstates) {
	uint64_t time_end = tc_tstates(cpu->timer_c) + tstates;

	while (tc_tstates(cpu->timer_c) < time_end) {
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
static void link_RTS(CPU_t *cpu, TIVAR_t *var, int dest) {
	TI_VARHDR var_hdr;

	if (cpu->pio.model == TI_85 || cpu->pio.model == TI_86) {
		memset(&var_hdr, ' ', sizeof(TI_VARHDR));
		memset(var_hdr.name86, 0, sizeof(var_hdr.name86));
		memcpy_s(var_hdr.name86, 8, (char *) var->name, 8);
		var_hdr.name_length = var->name_length;
	} else {
		memset(var_hdr.name, 0, sizeof(var_hdr.name));
		memcpy_s(var_hdr.name, 8, (char *) var->name, 8);
		var_hdr.version = var->version;

		if (dest == SEND_RAM) {
			var_hdr.type_ID2 = 0x00;
		} else if (dest == SEND_ARC) {
			var_hdr.type_ID2 = 0x80;
		} else {
			var_hdr.type_ID2 = var->flag;
		}
	}

	var_hdr.length = link_endian(var->length);
	var_hdr.type_ID = var->vartype;

	//printf("Model: %d, length: %d\n", cpu->pio.model, link_endian(tifile->var->length));
	if (cpu->pio.model == TI_82 || cpu->pio.model == TI_85)
		link_send_pkt(cpu, CID_VAR, &var_hdr);
	else
		link_send_pkt(cpu, CID_RTS, &var_hdr);
}

LINK_ERR link_send_backup(CPU_t *cpu, TIFILE_t *tifile, SEND_FLAG dest) {
	if (link_init(cpu))
		return LERR_NOTINIT;

	// If the calculator's LCD is off, it likely is not in
	// the correct software state to receive link data.
	// Turn it on by simulating pressing the 'ON' button
	if (!cpu->pio.lcd->active) {
		link_wait(cpu, MHZ_6);
		cpu->pio.keypad->on_pressed |= KEY_FALSEPRESS;
		link_wait(cpu, MHZ_6 / 2);
		cpu->pio.keypad->on_pressed &= ~KEY_FALSEPRESS;
		link_wait(cpu, MHZ_6);

		if (!cpu->pio.lcd->active)
			return LERR_LINK;
	}

	int group = cpu->pio.model == TI_85 ? 6 : 1;
	int bit = cpu->pio.model == TI_85 ? 4 : 0;
	if (tifile->backup == NULL)
		return LERR_FILE;
	TIBACKUP_t *backup = tifile->backup;
	cpu->pio.link->vlink_size = backup->length1 + backup->length2 + backup->length3;

	int err;
	switch (err = setjmp(exc_pkt)) {
	case 0: {
		TI_BACKUPHDR bkhdr;
		TI_PKTHDR rpkt;
		u_char data[64];

		bkhdr.flags_size = backup->length1;
		bkhdr.type_ID = backup->vartype;
		bkhdr.data_size = backup->length2;
		bkhdr.symbol_size = backup->length3;
		bkhdr.user_addr = backup->address;

		// Send the VAR with Backup style header
		link_send_pkt(cpu, CID_VAR, &bkhdr);

		keypad_press(cpu, group, bit);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK) {
			keypad_release(cpu, group, bit);
			return LERR_LINK;
		}

		// Receive Clear To Send
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_CTS) {
			keypad_release(cpu, group, bit);
			if (rpkt.command_ID == CID_EXIT) {
				link_send_pkt(cpu, CID_ACK, NULL);
				return LERR_MEM;
			} else
				return LERR_LINK;
		}

		// Send the ACK
		link_send_pkt(cpu, CID_ACK, NULL);

		// Send the single data packet containing the first data section
		TI_DATA s_data = { backup->length1, backup->data1 };
		link_send_pkt(cpu, CID_DATA, &s_data);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK) {
			keypad_release(cpu, group, bit);
			return LERR_LINK;
		}

		// Send the single data packet containing the second data section
		s_data.length = backup->length2;
		s_data.data = backup->data2;
		link_send_pkt(cpu, CID_DATA, &s_data);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK) {
			keypad_release(cpu, group, bit);
			return LERR_LINK;
		}

		// Send the single data packet containing the final data section
		s_data.length = backup->length3;
		s_data.data = backup->data3;
		link_send_pkt(cpu, CID_DATA, &s_data);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK) {
			keypad_release(cpu, group, bit);
			return LERR_LINK;
		}

		// Send the End of Transmission
		if (cpu->pio.model != TI_82)
			link_send_pkt(cpu, CID_EOT, NULL);
		keypad_release(cpu, group, bit);
		break;
	}
	default:
			keypad_release(cpu, group, bit);
			return LERR_SYSTEM;
	}
	return LERR_SUCCESS;
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
	if (tifile->type == FLASH_TYPE) {
		switch (tifile->flash->type) {
			case FLASH_TYPE_OS:
				return forceload_os(cpu, tifile);
			case FLASH_TYPE_APP:
				return forceload_app(cpu, tifile);
		}
	}

	// Make sure the TIFILE is well formed
	if (tifile->var == NULL)
		return LERR_FILE;

	int i = 0;
	TIVAR_t *var = tifile->vars[i];
	while (var != NULL) {
		cpu->pio.link->vlink_size = var->length;

		int err;
		switch (err = setjmp(exc_pkt)) {
		case 0: {
			TI_PKTHDR rpkt;
			u_char data[64];

			// Request to send
			link_RTS(cpu, var, dest);

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
			TI_DATA s_data = { var->length, var->data };
			link_send_pkt(cpu, CID_DATA, &s_data);

			// Receive the ACK
			link_recv_pkt(cpu, &rpkt, data);
			if (rpkt.command_ID != CID_ACK)
				return LERR_LINK;

			// Send the End of Transmission
			if (cpu->pio.model != TI_82 && cpu->pio.model != TI_85)
				link_send_pkt(cpu, CID_EOT, NULL);
			break;
		}
		default:
				return LERR_SYSTEM;
		}
		var = tifile->vars[++i];
	}
	if (cpu->pio.model == TI_85 || cpu->pio.model == TI_82) {
		TI_PKTHDR rpkt;
		u_char data[64];
		link_send_pkt(cpu, CID_EOT, NULL);

		// Receive the ACK
		link_recv_pkt(cpu, &rpkt, data);
		if (rpkt.command_ID != CID_ACK)
			return LERR_LINK;
	}
	return LERR_SUCCESS;
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
			for (offset = 0; offset < PAGE_SIZE && offset
					< tifile->flash->pagesize[page]; offset += 0x80) {

				// Send the flash header
				TI_FLASHHDR flash_hdr;
				flash_hdr.sizeLSB = link_endian(0x0080);
				flash_hdr.type_ID = FlashObj;
				flash_hdr.sizeMSB = link_endian(0x0000);
				flash_hdr.offset = link_endian(offset + PAGE_SIZE);
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
		if (rpkt.command_ID != CID_ACK) {
			return LERR_LINK;
		}

		return LERR_SUCCESS;
	}
	default:
		return LERR_SYSTEM;
	}
}

BOOL check_flashpage_empty(u_char (*dest)[PAGE_SIZE], u_int page, u_int num_pages) {
	u_char *space = &dest[page][PAGE_SIZE - 1];
	u_int i;
	// Make sure the subsequent pages are empty
	for (i = 0; i < num_pages * PAGE_SIZE; i++, space--) {
		if (*space != 0xFF) {
			printf("Subsequent pages not empty\n");
			return FALSE;
		}
	}
	return TRUE;
}

#define TI_84PCSE_CERT_APP_TRIAL_OFFSET 0x1FC8
#define TI_83PSE_CERT_APP_TRIAL_OFFSET 0x1E50
#define TI_83P_CERT_APP_TRIAL_OFFSET 0x1F18
#define APP_BITMAP_VALID_OFFSET 0x1FE0
#define CERT_PAGE cpu->mem_c->flash_pages - 2

/* Fixes the certificate page so that the app is no longer marked as a trial
 * cpu: cpu for the core the application is on
 * page: the page the application you want to mark is on
 */
void fix_certificate(CPU_t *cpu, u_int page) {

	u_char (*dest)[PAGE_SIZE] = (u_char (*)[PAGE_SIZE]) cpu->mem_c->flash;
	upages_t upages;
	state_userpages(cpu, &upages);
	int offset;
	switch (cpu->pio.model) {
	case TI_83P:
		offset = TI_83P_CERT_APP_TRIAL_OFFSET;
		break;
	case TI_84PCSE:
		offset = TI_84PCSE_CERT_APP_TRIAL_OFFSET;
		break;
	default:
		offset = TI_83PSE_CERT_APP_TRIAL_OFFSET;
		break;
	}

	// erase the part of the certificate that marks it as a trial app
	dest[CERT_PAGE][offset + 2 * (upages.start - page)] = 0x80;
	dest[CERT_PAGE][offset+1 + 2 * (upages.start - page)] = 0x00;
	// don't know which certificate slot the os expects out certificate to be in
	// so we'll write to both
	dest[CERT_PAGE][offset + PAGE_SIZE / 2 +  2 * (upages.start - page)] = 0x80;
	dest[CERT_PAGE][offset + PAGE_SIZE / 2 + 1 + 2 * (upages.start - page)] = 0x00;

	// mark all bitmaps as valid. each app has a bit, so 8 apps per byte
	memset(dest[CERT_PAGE] + APP_BITMAP_VALID_OFFSET, 0, (upages.start - upages.end) / 8);
	// same as above write to both possible certificates
	memset(dest[CERT_PAGE] + APP_BITMAP_VALID_OFFSET + PAGE_SIZE / 2, 0, (upages.start - upages.end) / 8);
}

LINK_ERR forceload_os(CPU_t *cpu, TIFILE_t *tifile) {
	u_int i, page;
	u_char (*dest)[PAGE_SIZE] = (u_char (*)[PAGE_SIZE]) cpu->mem_c->flash;
	if (dest == NULL)
		return LERR_MODEL;

	if (tifile->flash == NULL) {
		return LERR_FILE;
	}

	for (i = 0; i < ARRAYSIZE(tifile->flash->data); i++) {
		if (tifile->flash->data[i] == NULL) {
			continue;
		}

		if (i > 0x10) {
			page = i + cpu->mem_c->flash_pages - 0x20;
		} else {
			page = i;
		}

		int sector = (page / 4) * 4;
		int size;
		if (sector >= cpu->mem_c->flash_pages - 4) {
			size = PAGE_SIZE * 2;
		} else {
			size = PAGE_SIZE * 4;
		}

		memset(dest[sector], 0xFF, size);
	}

	for (i = 0; i < ARRAYSIZE(tifile->flash->data); i++) {
		if (tifile->flash->data[i] == NULL) {
			continue;
		}

		if (i > 0x10) {
			page = i + cpu->mem_c->flash_pages - 0x20;
		} else {
			page = i;
		}

		memcpy(dest[page], tifile->flash->data[i], PAGE_SIZE);
	}
	
	// valid OS
	dest[0][0x56] = 0x5A;
	dest[0][0x57] = 0xA5;

	return LERR_SUCCESS;
}

u_char *find_field(u_char *dest, u_char id1, u_char id2) {
	int i;
	//apparently non user apps have a slightly different header
	//therefore we have to actually find the identifier
	for (i = 0; i < PAGE_SIZE; i++) {
		if (dest[i] == id1 && dest[i + 1] == id2) {
			return dest + i + 2;
		}
	}

	return NULL;
}

/*
 * Finds the page size identifier and returns the number of pages specified.
 * If the identifiers cannot be found, 0 is returned.
 */
int get_page_size(u_char *dest) {
	int i;
	//apparently non user apps have a slightly different header
	//therefore we have to actually find the identifier
	dest = find_field(dest, 0x80, 0x81);
	if (dest == NULL) {
		return 0;
	}

	return *dest;
}

static LINK_ERR replace_app(CPU_t *cpu, TIFILE_t *tifile, upages_t upages, u_char (*dest)[PAGE_SIZE], int page) {
	int pageDiff = tifile->flash->pages - get_page_size(dest[page]);
	u_int currentPage = page - tifile->flash->pages;
	u_int end_page = pageDiff > 0 ? currentPage : currentPage + pageDiff;
	while (!check_flashpage_empty(dest, end_page, 1) && end_page >= upages.end) {
		end_page -= get_page_size(dest[end_page]);
	}

	if (end_page != currentPage) {
		if (pageDiff > 0) {
			if (end_page - pageDiff < upages.end) {
				return LERR_MEM;
			}

			memmove(dest[currentPage-pageDiff], dest[currentPage], PAGE_SIZE * (currentPage - end_page));
			if (cpu->pio.model == TI_83P) {
				// mark pages unprotected
				for (u_int i = end_page - 7; i <= end_page + pageDiff - 8; i++) {
					cpu->mem_c->protected_page[i / 8] &= ~(1 << (i % 8));
				}
			}
		} else {
			// 0xFF all extra pages
			for (u_int i = tifile->flash->pages; i < tifile->flash->pages - pageDiff; i++, currentPage--) {
				memset(dest[currentPage], 0xFF, PAGE_SIZE);
			}

			if (cpu->pio.model == TI_83P) {
				// mark pages as protected
				for (u_int i = end_page - 7; i <= end_page - pageDiff - 8; i++) {
					cpu->mem_c->protected_page[i / 8] |= 1 << (i % 8);
				}
			}
		}
	}
	// fix page execution permissions
	cpu->mem_c->flash_upper -= pageDiff;
	return LERR_SUCCESS;
}

/* Force load a TI-83+ series APP
 * On error: Returns an error code */
static LINK_ERR forceload_app(CPU_t *cpu, TIFILE_t *tifile) {
	u_char (*dest)[PAGE_SIZE] = (u_char (*)[PAGE_SIZE]) cpu->mem_c->flash;
	if (dest == NULL) {
		return LERR_MODEL;
	}

	if (tifile->flash == NULL) {
		return LERR_FILE;
	}

	upages_t upages;
	state_userpages(cpu, &upages);
	if (upages.start == -1) {
		return LERR_MODEL;
	}
	
	u_int page;
	for (page = upages.start; page >= upages.end + tifile->flash->pages; ) {
		if (check_flashpage_empty(dest, page, 1)) {
			break;
		}

		int page_size;
		// different size app need to send the long way
		u_char *loadedAppName = find_field(dest[page], 0x80, 0x48);
		if (loadedAppName == NULL) {
			// not sure what 0x45 is, but i've seen multiple apps use it
			loadedAppName = find_field(dest[page], 0x80, 0x45);
			if (loadedAppName == NULL) {
				return LERR_FILE;
			}
		}

		if (!memcmp(loadedAppName, &tifile->flash->name, 8)) {
			if (get_page_size(dest[page]) != tifile->flash->pages) {
				LINK_ERR err = replace_app(cpu, tifile, upages, dest, page);
				if (err == LERR_SUCCESS) {
					break;
				}
				return err;
			}

			u_int i;
			for (i = 0; i < tifile->flash->pages; i++, page--) {
				memcpy(dest[page], tifile->flash->data[i], PAGE_SIZE);
			}
			// note that this does not fix the old marks, only ensures that
			// the new order of apps has the correct parts marked
			applist_t applist;
			state_build_applist(cpu, &applist);
			for (i = 0; i < applist.count; i++) {
				fix_certificate(cpu, applist.apps[i].page);
			}

			printf("Found already\n");
			return LERR_SUCCESS;
		}
		page_size = get_page_size(dest[page]);
		page -= page_size;
	}

	if (page - tifile->flash->pages < upages.end) {
		return LERR_MEM;
	}

	//mark the app as non trial
	fix_certificate(cpu, page);
	//force reset the app list says BrandonW. seems to work, apps show up (sometimes)
	if (cpu->pio.model >= TI_84PCSE) {
		mem_write(cpu->mem_c, 0x9F68, 0x00);
	} else {
		mem_write(cpu->mem_c, 0x9C87, 0x00);
	}
	

	//u_char *space = &dest[page][PAGE_SIZE - 1];
	u_int i;
	// Make sure the subsequent pages are empty
	if (!check_flashpage_empty(dest, page, tifile->flash->pages)) {
		return LERR_MEM;
	}

	for (i = 0; i < tifile->flash->pages; i++, page--) {
		memcpy(dest[page], tifile->flash->data[i], PAGE_SIZE);
	}

	cpu->mem_c->flash_upper -= tifile->flash->pages;
	for (i = page - 7; i <= page + tifile->flash->pages - 8; i++) {
		//-8 is for the start of user mem
		cpu->mem_c->protected_page[i / 8] &= ~(1 << (i % 8));
	}

	return LERR_SUCCESS;
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
		page += mem_c->flash_pages;			//last page is boot page
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