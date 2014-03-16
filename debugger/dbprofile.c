#include "stdafx.h"
#include "dbprofile.h"
#include "dbcommon.h"
#include "fileutilities.h"
#include "disassemble.h"
#include "print.h"

extern HINSTANCE g_hInst;

LRESULT CALLBACK ProfileDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	profiler_t *profiler;
	switch(Message) {
	case WM_INITDIALOG: {
		profiler = (profiler_t *)lParam;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)profiler);
		HWND hCombo = GetDlgItem(hwnd, IDC_BLOCK_COMBO);
		ComboBox_AddItemData(hCombo, _T("16"));
		ComboBox_AddItemData(hCombo, _T("64"));
		ComboBox_AddItemData(hCombo, _T("256"));
		ComboBox_AddItemData(hCombo, _T("1024"));
		ComboBox_AddItemData(hCombo, _T("4096"));
		ComboBox_AddItemData(hCombo, _T("16384"));
		ComboBox_SetCurSel(hCombo, 0);

		return TRUE;
	}
	case WM_COMMAND: {
		switch(LOWORD(wParam)) {
			case IDOK: {
				profiler = (profiler_t *) GetWindowLongPtr(hwnd, GWLP_USERDATA);
				TCHAR string[9];
				int output;
				GetDlgItemText(hwnd, IDC_BLOCK_COMBO, string, 8);
				output = _tstoi(string);
				if (output < MIN_BLOCK_SIZE) {
					MessageBox(hwnd, _T("Block size must be at least 16 bytes"), _T("Error"), MB_OK);
					break;
				}

				profiler->blockSize = min(output, PAGE_SIZE);
				HWND hShowDisasm = GetDlgItem(hwnd, IDC_SHOWDISASM);
				profiler->show_disassembly = Button_GetCheck(hShowDisasm);
				HWND hSortOutput = GetDlgItem(hwnd, IDC_SORT_OUTPUT);
				profiler->sort_output = Button_GetCheck(hSortOutput);

				EndDialog(hwnd, IDOK);
				break;
			}
			case IDC_PROFILE_CANCEL:
				EndDialog(hwnd, IDCANCEL);
				break;
		}
		break;
	}
	case WM_CLOSE:
		EndDialog(hwnd, IDCANCEL);
		return FALSE;
	}
	return FALSE;
}

BOOL GetProfileOptions(HWND hwnd, profiler_t *profiler, int flash_pages, int ram_pages) {
	int result = (int)DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DLGPROFILE), hwnd,
		(DLGPROC)ProfileDialogProc, (LPARAM)profiler);
	if (result == IDCANCEL) {
		return FALSE;
	} 

	profiler->flash_data = (uint64_t **)calloc(flash_pages, PROFILER_NUM_BLOCKS * sizeof(uint64_t));
	profiler->ram_data = (uint64_t **)calloc(ram_pages, PROFILER_NUM_BLOCKS * sizeof(uint64_t));
	profiler->totalTime = 0;
	return TRUE;
}

extern Z80_com_t da_opcode[256];

typedef struct {
	uint8_t page;
	uint16_t block;
	double time_data;
} profiler_data_t;

static bool time_sort(profiler_data_t i, profiler_data_t j) {
	return i.time_data > j.time_data;
}

static bool page_sort(profiler_data_t i, profiler_data_t j) {
	if (i.page != j.page) {
		return i.page < j.page;
	}

	return i.block < j.block;
}

void ExportProfileData(FILE *file, LPCALC lpCalc,
	profiler_t *profiler, uint64_t(*data)[PROFILER_NUM_BLOCKS],
	int num_pages, BOOL is_ram)
{
	vector<profiler_data_t> data_list;

	for (uint8_t page = 0; page < num_pages; page++) {
		for (uint16_t block = 0; block < PAGE_SIZE / profiler->blockSize; block++) {
			double block_data = (double)data[page][block] / (double)profiler->totalTime;
			if ((block_data - 0.0) < DBL_EPSILON) {
				continue;
			}

			profiler_data_t new_data;
			new_data.page = page;
			new_data.block = block;
			new_data.time_data = block_data;
			data_list.push_back(new_data);
		}
	}

	waddr_t waddr;
	waddr.is_ram = is_ram;

	TCHAR commandString[255] = { 0 };
	if (profiler->totalTime == 0) {
		return;
	}

	if (profiler->sort_output) {
		sort(data_list.begin(), data_list.end(), time_sort);
	} else {
		sort(data_list.begin(), data_list.end(), page_sort);
	}

	for (auto it = data_list.begin(); it < data_list.end(); it++) {
		profiler_data_t new_data = *it;
		_ftprintf_s(file,
			_T("%02X: $%04X - $%04X:\t%f%%\t%d tstates\r\n"),
			new_data.page,
			new_data.block * profiler->blockSize,
			((new_data.block + 1) * profiler->blockSize) - 1,
			new_data.time_data * 100,
			data[new_data.page][new_data.block]);

		waddr.page = new_data.page;
		waddr.addr = (uint16_t)(new_data.block * profiler->blockSize);

		if (!profiler->show_disassembly) {
			continue;
		}

		Z80_info_t *info = (Z80_info_t *)malloc(sizeof(Z80_info_t)* profiler->blockSize);
		disassemble(lpCalc, FLASH, waddr, profiler->blockSize, TRUE, info);
		for (int i = 0; i < profiler->blockSize; i++) {
			mysprintf(lpCalc, commandString, sizeof(commandString), info, REGULAR,
				da_opcode[info[i].index].format, info[i].a1, info[i].a2, info[i].a3, info[i].a4);
			_ftprintf_s(file, _T("%s\r\n"), commandString);
		}

		free(info);
	}
}

void ExportProfile(LPCALC lpCalc, profiler_t *profiler) {
	int flash_pages = lpCalc->mem_c.flash_pages;
	int ram_pages = lpCalc->mem_c.ram_pages;

	FILE* file;
	TCHAR buffer[MAX_PATH];
	if (BrowseFile(buffer, _T("	Text file  (*.txt)\0*.txt\0	All Files (*.*)\0*.*\0\0"),
		_T("Wabbitemu Save Profile"), _T("txt"), 0, 1)) {
		// make the profiler running again
		profiler->running = TRUE;
		return;
	}

	uint64_t(*flash_data)[PROFILER_NUM_BLOCKS] = (uint64_t(*)[PROFILER_NUM_BLOCKS]) profiler->flash_data;
	_tfopen_s(&file, buffer, _T("wb"));
	_ftprintf_s(file, _T("Total Tstates: %i\r\n"), profiler->totalTime);
	_ftprintf_s(file, _T("Flash Memory:\r\n"));

	ExportProfileData(file, lpCalc, profiler, flash_data, flash_pages, FALSE);

	free(profiler->flash_data);
	profiler->flash_data = NULL;

	uint64_t(*ram_data)[PROFILER_NUM_BLOCKS] = (uint64_t(*)[PROFILER_NUM_BLOCKS]) profiler->ram_data;
	_ftprintf_s(file, _T("\r\nRAM:\r\n"));

	ExportProfileData(file, lpCalc, profiler, ram_data, ram_pages, TRUE);

	free(profiler->ram_data);
	profiler->ram_data = NULL;
	fclose(file);
}