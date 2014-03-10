#include "stdafx.h"

#include "dbfinddialog.h"
#include "dbcommon.h"
#include "calc.h"

typedef struct {
	int find_value;
	BOOL big_endian;
	BOOL search_backwards;

	HWND edtAddr;
	HWND forwardsCheck;
	HWND backwardsCheck;
	HWND littleEndianCheck;
	HWND bigEndianCheck;
	
	find_dialog_params_t params;
} find_dialog_t;

static void GetNextWaddr(LPCALC lpCalc, ViewType type, int *bank_num, waddr_t *waddr) {
	waddr->addr++;
	if (waddr->addr % PAGE_SIZE == 0) {
		waddr->addr = 0;
		if (type == REGULAR) {
			(*bank_num)++;
			if (*bank_num < 4) {
				waddr->page = (uint8_t)lpCalc->mem_c.banks[*bank_num].page;
				waddr->is_ram = (uint16_t)lpCalc->mem_c.banks[*bank_num].ram;
			} else {
				return;
			}
		} else {
			int pages = waddr->is_ram ? lpCalc->mem_c.ram_pages : lpCalc->mem_c.flash_pages;
			if (waddr->page < pages) {
				waddr->page++;
			} else {
				return;
			}
		}
	}
}

static waddr_t * FindValue(find_dialog_params_t params, int valueToFind, BOOL search_backwards, BOOL big_endian) {
	LPCALC lpCalc = params.lpCalc;
	waddr_t waddr;
	if (search_backwards) {
		if (params.type == REGULAR) {
			waddr = addr16_to_waddr(&lpCalc->mem_c, (uint16_t)(params.start_addr - 1));
		} else {
			waddr = addr32_to_waddr(params.start_addr - 1, params.type == RAM);
		}
	} else {
		if (params.type == REGULAR) {
			waddr = addr16_to_waddr(&lpCalc->mem_c, (uint16_t)(params.start_addr + 1));
		} else {
			waddr = addr32_to_waddr(params.start_addr + 1, params.type == RAM);
		}
	}

	int bank_num = mc_bank(waddr.addr);
	int valSize = 1;
	int value = 0;
	int tempVal = valueToFind;
	while (tempVal > 0) {
		tempVal /= 0xFF;
		valSize++;
	}
	valSize--;
	do {
		if (search_backwards) {
			waddr.addr--;
			if ((waddr.addr + 1) % PAGE_SIZE == 0) {
				if (params.type == REGULAR) {
					bank_num--;
					if (bank_num >= 0) {
						waddr.addr = (uint16_t)((bank_num + 1) * PAGE_SIZE - 1);
						waddr.page = (uint8_t)lpCalc->mem_c.banks[bank_num].page;
						waddr.is_ram = lpCalc->mem_c.banks[bank_num].ram;
					}
					else {
						break;
					}
				}
				else {
					waddr.addr = PAGE_SIZE - 1;
					waddr.page--;
					if ((int8_t)waddr.page < 0) {
						break;
					}
				}
			}
		}
		else {
			GetNextWaddr(lpCalc, params.type, &bank_num, &waddr);
		}

		if (params.type == REGULAR) {
			if (bank_num == 4) {
				break;
			}
		}
		else {
			if (waddr.page == (waddr.is_ram ? lpCalc->mem_c.ram_pages : lpCalc->mem_c.flash_pages)) {
				break;
			}
		}
		value = 0;
		waddr_t tempWaddr;
		memcpy(&tempWaddr, &waddr, sizeof(waddr_t));
		for (int b = 0;;) {
			int shift = big_endian ? (valSize - 1 - b) * 8 : b * 8;
			value += wmem_read(&lpCalc->mem_c, tempWaddr) << shift;
			b++;
			if (b >= valSize) {
				break;
			}
			GetNextWaddr(lpCalc, params.type, &bank_num, &tempWaddr);
		}
	} while (value != valueToFind);

	if (value != valueToFind) {
		return NULL;
	}

	waddr_t *returnWaddr = (waddr_t *)malloc(sizeof(waddr_t));
	memcpy(returnWaddr, &waddr, sizeof(waddr_t));
	return returnWaddr;
}

INT_PTR CALLBACK FindDialogProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	find_dialog_t *dialog = (find_dialog_t *)GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
	switch (Message) {
	case WM_INITDIALOG: {
		find_dialog_params_t *params = (find_dialog_params_t *)lParam;
		dialog = (find_dialog_t *)malloc(sizeof(find_dialog_t));

		dialog->edtAddr = GetDlgItem(hwndDlg, IDC_EDT_FIND);
		dialog->forwardsCheck = GetDlgItem(hwndDlg, IDC_RADIO_FORWARDS);
		dialog->backwardsCheck = GetDlgItem(hwndDlg, IDC_RADIO_BACKWARDS);

		dialog->littleEndianCheck = GetDlgItem(hwndDlg, IDC_RADIO_LITTLEENDIAN);
		dialog->bigEndianCheck = GetDlgItem(hwndDlg, IDC_RADIO_BIGENDIAN);

		SetFocus(GetDlgItem(hwndDlg, IDC_EDT_FIND));
		Button_SetCheck(dialog->forwardsCheck, TRUE);
		Button_SetCheck(dialog->bigEndianCheck, TRUE);
		dialog->params = *params;
		dialog->big_endian = TRUE;
		dialog->search_backwards = FALSE;
		SetWindowLongPtr(hwndDlg, GWLP_USERDATA, (LPARAM) dialog);
		return FALSE;
	}
	case WM_ACTIVATE: {
		if (0 == wParam) {
			hwndCurrentDlg = NULL;
		} else {
			hwndCurrentDlg = hwndDlg;
		}

		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FIND_NEXT: {
			TCHAR result[32];
			GetDlgItemText(hwndDlg, IDC_EDT_FIND, result, 32);
			_stscanf_s(result, _T("%x"), &dialog->find_value);

			waddr_t *waddr = FindValue(dialog->params, dialog->find_value, dialog->search_backwards, dialog->big_endian);

			if (waddr == NULL) {
				MessageBox(hwndDlg, _T("Unable to find value"), _T("Find"), MB_OK);
			} else {
				int global_addr;
				if (dialog->params.type == REGULAR) {
					int i;
					for (i = 0; i < 4; i++) {
						if (dialog->params.lpCalc->mem_c.banks[i].page == waddr->page) {
							break;
						}
					}
					global_addr = i * PAGE_SIZE + mc_base(waddr->addr);
				} else {
					global_addr = waddr->addr + waddr->page * PAGE_SIZE;
				}

				dialog->params.start_addr = global_addr;
				SendMessage(dialog->params.hwndParent, WM_COMMAND, DB_FIND_NEXT, global_addr);
				break;
			}
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			SendMessage(dialog->params.hwndParent, WM_COMMAND, DB_FIND_CLOSE, 0);
			free(dialog);
			break;
		}
		switch (HIWORD(wParam)) {
		case BN_CLICKED: {
			switch (LOWORD(wParam)) {
			case IDC_RADIO_BACKWARDS:
				dialog->search_backwards = TRUE;
				return FALSE;
			case IDC_RADIO_FORWARDS:
				dialog->search_backwards = FALSE;
				return FALSE;
			case IDC_RADIO_LITTLEENDIAN:
				dialog->big_endian = FALSE;
				return FALSE;
			case IDC_RADIO_BIGENDIAN:
				dialog->big_endian = TRUE;
				return FALSE;
			}
		}
			break;
		}
	}

	return FALSE;
}