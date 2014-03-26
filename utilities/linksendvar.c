#include "stdafx.h"

#include "linksendvar.h"
#include "keys.h"
#include "state.h"

extern jmp_buf exc_pkt;

/* Prototypes of static functions*/
static LINK_ERR forceload_app(CPU_t *, TIFILE_t *);
static LINK_ERR link_send_app(CPU_t *, TIFILE_t *);

LINK_ERR link_send_backup(CPU_t *cpu, TIFILE_t *tifile) {
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
					}
					else
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


/* Send a Request To Send packet
* On error: Throws Packet Exception */
void link_RTS(CPU_t *cpu, TIVAR_t *var, int dest) {
	TI_VARHDR var_hdr;

	if (cpu->pio.model == TI_85 || cpu->pio.model == TI_86) {
		memset(&var_hdr, 0, sizeof(TI_VARHDR));
		memset(var_hdr.name86, 0, sizeof(var_hdr.name86));
		memcpy_s(var_hdr.name86, 8, (char *)var->name, 8);
		var_hdr.name_length = var->name_length;
	}
	else {
		memset(var_hdr.name, 0, sizeof(var_hdr.name));
		memcpy_s(var_hdr.name, 8, (char *)var->name, 8);
		var_hdr.version = var->version;

		if (dest == SEND_RAM) {
			var_hdr.type_ID2 = 0x00;
		}
		else if (dest == SEND_ARC) {
			var_hdr.type_ID2 = 0x80;
		}
		else {
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
		link_wait(cpu, cpu->timer_c->freq);
		cpu->pio.keypad->on_pressed |= KEY_FALSEPRESS;
		link_wait(cpu, cpu->timer_c->freq / 2);
		cpu->pio.keypad->on_pressed &= ~KEY_FALSEPRESS;
		link_wait(cpu, cpu->timer_c->freq);

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
					if (cpu->pio.model < TI_84PCSE) {
						link_send_pkt(cpu, CID_ACK, NULL);
					}
					return LERR_MEM;
				}
				else
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

BOOL check_flashpage_empty(u_char(*dest)[PAGE_SIZE], u_int page, u_int num_pages) {
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

#define TI_83PSE_CERT_APP_TRIAL_OFFSET 0x1E50
#define TI_83P_CERT_APP_TRIAL_OFFSET 0x1F18

#define TI_83P_APP_BITMAP_VALID_OFFSET 0x1FE0
#define TI_84PCSE_APP_BITMAP_VALID_OFFSET 0x1FC8
#define CERT_PAGE cpu->mem_c->flash_pages - 2

/* Fixes the certificate page so that the app is no longer marked as a trial
* cpu: cpu for the core the application is on
* page: the page the application you want to mark is on
*/
void fix_certificate(CPU_t *cpu, u_int page) {

	u_char(*dest)[PAGE_SIZE] = (u_char(*)[PAGE_SIZE]) cpu->mem_c->flash;
	upages_t upages;
	state_userpages(cpu, &upages);
	int offset;
	switch (cpu->pio.model) {
	case TI_83P:
		offset = TI_83P_CERT_APP_TRIAL_OFFSET;
		break;
	default:
		offset = TI_83PSE_CERT_APP_TRIAL_OFFSET;
		break;
	}

	if (cpu->pio.model < TI_84PCSE) {
		// erase the part of the certificate that marks it as a trial app
		dest[CERT_PAGE][offset + 2 * (upages.start - page)] = 0x80;
		dest[CERT_PAGE][offset + 1 + 2 * (upages.start - page)] = 0x00;
		// don't know which certificate slot the os expects out certificate to be in
		// so we'll write to both
		dest[CERT_PAGE][offset + (PAGE_SIZE / 2) + 2 * (upages.start - page)] = 0x80;
		dest[CERT_PAGE][offset + (PAGE_SIZE / 2) + 1 + 2 * (upages.start - page)] = 0x00;
	}

	offset = cpu->pio.model >= TI_84PCSE ? TI_84PCSE_APP_BITMAP_VALID_OFFSET : TI_83P_APP_BITMAP_VALID_OFFSET;
	// mark all bitmaps as valid. each app has a bit, so 8 apps per byte
	memset(dest[CERT_PAGE] + offset, 0, (upages.start - upages.end) / 8);
	// same as above write to both possible certificates
	memset(dest[CERT_PAGE] + offset + (PAGE_SIZE / 2), 0, (upages.start - upages.end) / 8);
}

LINK_ERR forceload_os(CPU_t *cpu, TIFILE_t *tifile) {
	u_int i, page;
	u_char(*dest)[PAGE_SIZE] = (u_char(*)[PAGE_SIZE]) cpu->mem_c->flash;
	if (dest == NULL)
		return LERR_MODEL;

	if (tifile->flash == NULL) {
		return LERR_FILE;
	}

	int start_page;
	switch (cpu->pio.model) {
	case TI_84P:
		start_page = 0x20;
		break;
	case TI_83PSE:
	case TI_84PSE:
		start_page = 0x60;
		break;
	case TI_83P:
	case TI_84PCSE:
		start_page = 0x00;
		break;
	default:
		return LERR_MODEL;
	}

	for (i = 0; i < ARRAYSIZE(tifile->flash->data); i++) {
		if (tifile->flash->data[i] == NULL) {
			continue;
		}

		if (i > 0x10) {
			page = start_page + i;
		} else {
			page = i;
		}

		if (i >= (u_int)cpu->mem_c->flash_pages) {
			break;
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
			page = start_page + i;
		}
		else {
			page = i;
		}

		if (i >= (u_int)cpu->mem_c->flash_pages) {
			break;
		}

		memcpy(dest[page], tifile->flash->data[i], PAGE_SIZE);
	}

	// valid OS
	dest[0][0x56] = 0x5A;
	dest[0][0x57] = 0xA5;

	return LERR_SUCCESS;
}

static LINK_ERR replace_app(CPU_t *cpu, TIFILE_t *tifile, upages_t upages, u_char(*dest)[PAGE_SIZE], int page) {
	unsigned short pageDiff = (unsigned short)(tifile->flash->pages - get_page_size(dest[page]));
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

			memmove(dest[currentPage - pageDiff], dest[currentPage], PAGE_SIZE * (currentPage - end_page));
			if (cpu->pio.model == TI_83P) {
				// mark pages unprotected
				for (u_int i = end_page - 7; i <= end_page + pageDiff - 8; i++) {
					cpu->mem_c->protected_page[i / 8] &= ~(1 << (i % 8));
				}
			}
		}
		else {
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
	u_char(*dest)[PAGE_SIZE] = (u_char(*)[PAGE_SIZE]) cpu->mem_c->flash;
	if (dest == NULL) {
		return LERR_MODEL;
	}

	if (tifile->flash == NULL) {
		return LERR_FILE;
	}

	upages_t upages;
	state_userpages(cpu, &upages);
	if (upages.start == 0) {
		return LERR_MODEL;
	}

	u_int page;
	for (page = upages.start; page >= upages.end + tifile->flash->pages;) {
		if (check_flashpage_empty(dest, page, 1)) {
			break;
		}

		int page_size;
		// different size app need to send the long way
		u_char *loadedAppName;
		int name_len = find_field(dest[page], 0x80, 0x40, &loadedAppName);
		if (loadedAppName == NULL) {
			return LERR_FILE;
		}

		if (!memcmp(loadedAppName, &tifile->flash->name, name_len)) {
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

	// mark the app as non trial
	fix_certificate(cpu, page);
	// force reset the app list says BrandonW. seems to work, apps show up (sometimes)
	if (cpu->pio.model >= TI_84PCSE) {
		mem_write(cpu->mem_c, 0x9F68, 0x00);
	}
	else {
		mem_write(cpu->mem_c, 0x9C87, 0x00);
	}


	u_int i;
	// Make sure the subsequent pages are empty
	if (!check_flashpage_empty(dest, page, tifile->flash->pages)) {
		return LERR_MEM;
	}

	for (i = 0; i < tifile->flash->pages; i++, page--) {
		memcpy(dest[page], tifile->flash->data[i], PAGE_SIZE);
	}

	cpu->mem_c->flash_upper = (unsigned short)page;
	for (i = page - 7; i <= page + tifile->flash->pages - 8; i++) {
		// -8 is for the start of user mem
		cpu->mem_c->protected_page[i / 8] &= ~(1 << (i % 8));
	}

	return LERR_SUCCESS;
}