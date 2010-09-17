#include "stdafx.h"

#include "guiwizard.h"
#include "guioptions.h"
#include "gui.h"
#include "calc.h"
#include "fileutilities.h"
#include <commdlg.h>
#include "resource.h"

INT_PTR CALLBACK HelpProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

extern HINSTANCE g_hInst;
static HWND hwndWiz = NULL;
static BOOL use_bootfree = FALSE;
static int model = -1;
char osPath[MAX_PATH];
char dumperPath[MAX_PATH];
static char TIConnectPath[MAX_PATH];
static BOOL error = FALSE;

BOOL DoWizardSheet(HWND hwndOwner) {

	HPROPSHEETPAGE hPropSheet[5];
	PROPSHEETPAGE psp;
	PROPSHEETHEADER psh;

	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.hInstance = g_hInst;
	psp.dwFlags = PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	psp.lParam = 0;
	psp.pszHeaderTitle = "Wabbitemu";
	psp.pszHeaderSubTitle = "Setup";
	psp.pszTemplate = MAKEINTRESOURCE(IDD_SETUP_START);
	psp.pfnDlgProc = SetupStartProc;
	hPropSheet[0] = CreatePropertySheetPage(&psp);

	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.hInstance = g_hInst;
	psp.dwFlags = PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	psp.lParam = 0;
	psp.pszHeaderTitle = "Wabbitemu";
	psp.pszHeaderSubTitle = "Calculator Type";
	psp.pszTemplate = MAKEINTRESOURCE(IDD_SETUP_TYPE);
	psp.pfnDlgProc = SetupTypeProc;
	hPropSheet[1] = CreatePropertySheetPage(&psp);

	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.hInstance = g_hInst;
	psp.dwFlags = PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	psp.lParam = 0;
	psp.pszHeaderTitle = "Wabbitemu";
	psp.pszHeaderSubTitle = "OS Selection";
	psp.pszTemplate = MAKEINTRESOURCE(IDD_SETUP_TIOS);
	psp.pfnDlgProc = SetupOSProc;
	hPropSheet[2] = CreatePropertySheetPage(&psp);

	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.hInstance = g_hInst;
	psp.dwFlags = PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	psp.lParam = 0;
	psp.pszHeaderTitle = "Wabbitemu";
	psp.pszHeaderSubTitle = "Send ROM Dumper";
	psp.pszTemplate = MAKEINTRESOURCE(IDD_SETUP_LOADFILE);
	psp.pfnDlgProc = SetupROMDumperProc;
	hPropSheet[3] = CreatePropertySheetPage(&psp);

	psp.dwSize = sizeof(PROPSHEETPAGE);
	psp.hInstance = g_hInst;
	psp.dwFlags = PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	psp.lParam = 0;
	psp.pszHeaderTitle = "Wabbitemu";
	psp.pszHeaderSubTitle = "Make ROM";
	psp.pszTemplate = MAKEINTRESOURCE(IDD_SETUP_GETFILE);
	psp.pfnDlgProc = SetupMakeROMProc;
	hPropSheet[4] = CreatePropertySheetPage(&psp);

	DWORD flags;
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion >= 6)
		flags = PSH_AEROWIZARD;
	else
		flags = PSH_WIZARD97;

	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.hInstance = g_hInst;
	psh.dwFlags = flags | PSH_WATERMARK;// | PSH_HEADER;
	psh.hwndParent = hwndOwner;
	psh.phpage = hPropSheet;
	psh.pszCaption = (LPSTR) "Wabbitemu Setup";
	psh.pszbmHeader = NULL;//MAKEINTRESOURCE("A");
	psh.pszbmWatermark = NULL;
	psh.nPages = ARRAYSIZE(hPropSheet);
	psh.nStartPage = 0;
	psh.pfnCallback = NULL;
	psh.pszIcon = NULL;
	PropertySheet(&psh);
	return error;
}

INT_PTR CALLBACK SetupStartProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static BOOL inited = FALSE;
	static HWND hBootFree, hDumpRom, hOwnRom, hInfoText;
	switch (Message) {
		case WM_INITDIALOG: {
			hBootFree = GetDlgItem(hwnd, IDC_RADIO_BOOTFREE);
			hDumpRom = GetDlgItem(hwnd, IDC_RADIO_DUMP_ROM);
			hOwnRom = GetDlgItem(hwnd, IDC_RADIO_OWN_ROM);
			hInfoText = GetDlgItem(hwnd, IDC_INFO_TEXT);
			return FALSE;
		}
		case WM_COMMAND: {
			switch(HIWORD(wParam)) {
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case IDC_CHECK_NOSHOW:
							show_wizard = !Button_GetCheck(GetDlgItem(hwnd, IDC_CHECK_NOSHOW));
							break;
						case IDC_RADIO_OWN_ROM:
							PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_FINISH);
							break;
						case IDC_RADIO_BOOTFREE:
						case IDC_RADIO_DUMP_ROM:
							PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_NEXT);
							break;
					}
					break;
			}
			return TRUE;
		}
		case WM_NOTIFY :
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			switch(pnmh->code) {
				case PSN_SETACTIVE:
					if (inited)
						PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_NEXT);
					else {
						PropSheet_SetWizButtons(GetParent(hwnd), 0);
						inited = TRUE;
					}
					break;
				case PSN_WIZNEXT:
					use_bootfree = Button_GetCheck(hBootFree) == BST_CHECKED; 
					break;
				case PSN_WIZFINISH: {
					char* string = LoadRomIntialDialog();
					if (string) {
						int slot = calc_slot_new();
						slot = rom_load(slot, string);
						if (slot != -1) gui_frame(slot);
						break;
					} else error = TRUE;
					break;
				}
				case PSN_QUERYCANCEL:
					error = TRUE;
					break;
			}
			return TRUE;
		}
		case WM_DESTROY:
			return FALSE;
	}
	return FALSE;
}

void ModelInit(int slot)
{
	switch(model) {
		case TI_73:
			calc_init_83p(slot);
			break;
		case TI_83P:
			calc_init_83p(slot);
			break;
		case TI_83PSE:
			calc_init_83pse(slot);
			break;
		case TI_84P:
			calc_init_84p(slot);
			break;
		case TI_84PSE:
			calc_init_83pse(slot);
			break;
	}
}

INT_PTR CALLBACK SetupTypeProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static BOOL inited = FALSE;
	static HWND hQuestion, hTI73, hTI82, hTI83, hTI83P, hTI83PSE, hTI84P, hTI84PSE, hTI85, hTI86;
	switch (Message) {
		case WM_INITDIALOG:
			hQuestion = GetDlgItem(hwnd, IDC_STATIC_TYPE);
			hTI73 = GetDlgItem(hwnd, IDC_RADIO_TI73);
			hTI82 = GetDlgItem(hwnd,  IDC_RADIO_TI82);
			hTI83 = GetDlgItem(hwnd, IDC_RADIO_TI83);
			hTI83P = GetDlgItem(hwnd, IDC_RADIO_TI83P);
			hTI83PSE = GetDlgItem(hwnd, IDC_RADIO_TI83PSE);
			hTI84P = GetDlgItem(hwnd, IDC_RADIO_TI84P);
			hTI84PSE = GetDlgItem(hwnd, IDC_RADIO_TI84PSE);
			hTI85 = GetDlgItem(hwnd, IDC_RADIO_TI85);
			hTI86 = GetDlgItem(hwnd, IDC_RADIO_TI86);
			return FALSE;
		case WM_COMMAND: {
			switch(HIWORD(wParam)) {
				case BN_CLICKED:
					/*switch(LOWORD(wParam)) {
						case IDC_RADIO_TI73:
						case IDC_RADIO_TI82:
						case IDC_RADIO_TI83:
						case IDC_RADIO_TI83P:
						case IDC_RADIO_TI83PSE:
						case IDC_RADIO_TI84P:
						case IDC_RADIO_TI84PSE:
						case IDC_RADIO_TI85:
						case IDC_RADIO_TI86:

							break;
						default:*/
							PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_BACK | PSWIZB_NEXT);
							/*break;
					}*/
					break;
			}
			return TRUE;
		}
		case WM_NOTIFY :
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			switch(pnmh->code) {
				case PSN_SETACTIVE: {
					//TODO: make all the calcs rom dumping work
					if (use_bootfree || true) {
						Static_SetText(hQuestion, "What type of calculator would you like to emulate?");
						Button_Enable(hTI82, FALSE);
						Button_Enable(hTI83, FALSE);
						Button_Enable(hTI85, FALSE);
						Button_Enable(hTI86, FALSE);
					} else {
						Static_SetText(hQuestion, "What type of calculator are you going to dump?");
						Button_Enable(hTI82, TRUE);
						Button_Enable(hTI83, TRUE);
						Button_Enable(hTI85, TRUE);
						Button_Enable(hTI86, TRUE);
					}
					if (inited)
						PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_BACK | PSWIZB_NEXT);
					else {
						PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_BACK);
						inited = TRUE;
					}
					break;
				}
				case PSN_WIZNEXT: {
					if (Button_GetCheck(hTI73) == BST_CHECKED)
						model = TI_73;
					else if (Button_GetCheck(hTI82) == BST_CHECKED)
						model = TI_82;
					else if (Button_GetCheck(hTI83) == BST_CHECKED)
						model = TI_83;
					else if (Button_GetCheck(hTI83P) == BST_CHECKED)
						model = TI_83P;
					else if (Button_GetCheck(hTI83PSE) == BST_CHECKED)
						model = TI_83PSE;
					else if (Button_GetCheck(hTI84P) == BST_CHECKED)
						model = TI_84P;
					else if (Button_GetCheck(hTI84PSE) == BST_CHECKED)
						model = TI_84PSE;
					else if (Button_GetCheck(hTI85) == BST_CHECKED)
						model = TI_85;
					else if (Button_GetCheck(hTI86) == BST_CHECKED)
						model = TI_86;
					break;
				}
				case PSN_QUERYCANCEL:
					error = TRUE;
					break;
			}
			return TRUE;
		}
		case WM_DESTROY:
			return FALSE;
	}
	return FALSE;
}

INT_PTR CALLBACK SetupOSProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hComboOS, hBrowseOS, hEditOSPath, hStaticProgress, hProgressBar, hRadioBrowse, hRadioDownload;
	switch (Message) {
		case WM_INITDIALOG: {
			hComboOS = GetDlgItem(hwnd, IDC_COMBO_OS);
			hBrowseOS = GetDlgItem(hwnd, IDC_BROWSE_OS);
			hEditOSPath = GetDlgItem(hwnd, IDC_EDIT_OS_PATH);
			hStaticProgress = GetDlgItem(hwnd, IDC_STATIC_PROGRESS);
			hProgressBar = GetDlgItem(hwnd, IDC_PROGRESS);
			hRadioBrowse = GetDlgItem(hwnd, IDC_RADIO_BROWSE_OS);
			hRadioDownload = GetDlgItem(hwnd, IDC_RADIO_DOWNLOAD_OS);
			return FALSE;
		}
		case WM_COMMAND: {
			switch(HIWORD(wParam)) {
				case BN_CLICKED: {
					switch(LOWORD(wParam)) {
						case IDC_RADIO_BROWSE_OS: {
							ShowWindow(hEditOSPath, SW_SHOWNORMAL);
							ShowWindow(hBrowseOS, SW_SHOWNORMAL);
							ShowWindow(hComboOS, SW_HIDE);
							break;
						}
						case IDC_RADIO_DOWNLOAD_OS: {
							ShowWindow(hEditOSPath, SW_HIDE);
							ShowWindow(hBrowseOS, SW_HIDE);
							ShowWindow(hComboOS, SW_SHOWNORMAL);
							ComboBox_ResetContent(hComboOS);
							switch(model) {
								case TI_73:
									ComboBox_AddString(hComboOS, "OS 1.91");
									break;
								case TI_83P:
								case TI_83PSE:
									ComboBox_AddString(hComboOS, "OS 1.19");
									break;
								case TI_84P:
								case TI_84PSE: {
									ComboBox_AddString(hComboOS, "OS 2.43");
									ComboBox_AddString(hComboOS, "OS 2.53 MP");
									break;
								}
							}
							ComboBox_SetCurSel(hComboOS, 0);
							break;
						}
						case IDC_BROWSE_OS: {
							char buf[512];
							if (!BrowseFile(buf,			//output
								"	83 Plus Series OS  (*.8xu)\0*.8xu\0	73 OS  (*.73u)\0*.73u\0	All Files (*.*)\0*.*\0\0", //filter
								"Wabbitemu Open OS",		//title
								"8xu"))						//def ext
								Edit_SetText(hEditOSPath, buf);
							break;
						}
						break;
					}
				}
			}
			return TRUE;
		}
		case WM_NOTIFY: {
			LPNMHDR pnmh = (LPNMHDR) lParam;
			switch(pnmh->code) {
				case NM_CLICK:
				case NM_RETURN: {
						PNMLINK pNMLink = (PNMLINK)lParam;
						LITEM item = pNMLink->item;
						char buffer[1024];
						memset(buffer, 0, ARRAYSIZE(buffer));
						int length = (int) wcslen(item.szUrl);
						WideCharToMultiByte(CP_ACP, 0, item.szUrl, length, buffer, length, NULL, NULL);
						ShellExecute(NULL, _T("open"), buffer, NULL, NULL, SW_SHOWNORMAL);
						break;
				}
				case PSN_SETACTIVE: {
					DWORD flags = PSWIZB_BACK | PSWIZB_NEXT;
					if (use_bootfree)
						flags = PSWIZB_BACK | PSWIZB_FINISH;
					PropSheet_SetWizButtons(GetParent(hwnd), flags);
					switch(model) {
						case TI_73:
						case TI_83P:
						case TI_83PSE:
						case TI_84P:
						case TI_84PSE: {
							Button_Enable(hRadioDownload, TRUE);
							Button_SetCheck(hRadioDownload, BST_CHECKED);
							SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_RADIO_DOWNLOAD_OS ,BN_CLICKED), 0);
							break;
						}
						default: {
							Button_Enable(hRadioDownload, FALSE);
							Button_SetCheck(hRadioBrowse, BST_CHECKED);
							SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_RADIO_BROWSE_OS ,BN_CLICKED), 0);
							break;
						}
					}
					break;
				}
				case PSN_WIZNEXT: {
					if (Button_GetCheck(hRadioDownload) == BST_CHECKED) {
						Static_SetText(hStaticProgress, _T("Downloading OS..."));
						DownloadOS(ComboBox_GetCurSel(hComboOS) == 0);
					} else {
						Edit_GetText(hEditOSPath, osPath, MAX_PATH);
					}
					break;
				}
				case PSN_WIZFINISH: {
					Static_SetText(hStaticProgress, _T("Creating ROM file..."));
					ShowWindow(hProgressBar, SW_SHOW);
					char buffer[MAX_PATH];
					GetExportROMName(buffer);
					SendMessage(hProgressBar, PBM_SETSTEP, (WPARAM) 25, 0);
					SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
					if (Button_GetCheck(hRadioDownload) == BST_CHECKED) {
						Static_SetText(hStaticProgress, _T("Downloading OS..."));
						DownloadOS(ComboBox_GetCurSel(hComboOS) == 0);
					} else {
						Edit_GetText(hEditOSPath, osPath, MAX_PATH);
					}
					SendMessage(hProgressBar, PBM_STEPIT, 0, 0);
					int slot = calc_slot_new();
					//ok yes i know this is retarded...but this way we can use Load_8xu
					//outside this function...
					HMODULE hModule = GetModuleHandle(NULL);
					HRSRC resource;
					switch(model) {
						case TI_73:
							calc_init_83p(slot);
							resource = FindResource(hModule, MAKEINTRESOURCE(HEX_BOOT73), _T("HEX"));
							break;
						case TI_83P:
							calc_init_83p(slot);
							resource = FindResource(hModule, MAKEINTRESOURCE(HEX_BOOT83P), _T("HEX"));
							break;
						case TI_83PSE:
							calc_init_83pse(slot);
							resource = FindResource(hModule, MAKEINTRESOURCE(HEX_BOOT83PSE), _T("HEX"));
							break;
						case TI_84P:
							calc_init_84p(slot);
							resource = FindResource(hModule, MAKEINTRESOURCE(HEX_BOOT84P), _T("HEX"));
							break;
						case TI_84PSE:
							calc_init_83pse(slot);
							resource = FindResource(hModule, MAKEINTRESOURCE(HEX_BOOT84PSE), _T("HEX"));
							break;
					}
					gslot = slot;
					//slot stuff
					calcs[slot].active = TRUE;
#ifdef WINVER
					strcpy_s(calcs[slot].rom_path, buffer);
#else
					strcpy(calcs[slot].rom_path, buffer);
#endif
					calcs[slot].model = model;
					calcs[slot].cpu.pio.model = model;

					SendMessage(hProgressBar, PBM_STEPIT, 0, 0);
					char hexFile[MAX_PATH];
#ifdef WINVER
					char *env;
					size_t envLen;
					_dupenv_s(&env, &envLen, "appdata");
					strcpy_s(hexFile, envLen, env);
					free(env);
					strcat_s(hexFile, "\\boot.hex");
#else
					strcpy(hexFile, getenv("appdata"));
					strcat(hexFile, "\\boot.hex");
#endif
					ExtractResource(hexFile, resource);
#ifdef WINVER
					FILE *file;
					fopen_s(&file, hexFile, "rb");
#else
					FILE *file = fopen(hexFile, "rb");
#endif
					writeboot(file);
					fclose(file);
					remove(hexFile);
					//if you dont want to load an OS, fine...
					if (strlen(osPath) > 0) {
#ifdef WINVER
						fopen_s(&file, osPath, "rb");
#else
						file = fopen(osPath, "rb");
#endif
						Load_8xu(file);
						fclose(file);
						calcs[slot].mem_c.flash[0x56] = 0x5A;
						calcs[slot].mem_c.flash[0x57] = 0xA5;
						if (Button_GetCheck(hRadioDownload) == BST_CHECKED)
							remove(osPath);
					}
					calc_erase_certificate(calcs[slot].cpu.mem_c->flash,calcs[slot].cpu.mem_c->flash_size);
					calc_reset(slot);
					calc_run_timed(gslot, 200);
					calcs[slot].cpu.pio.keypad->on_pressed |= KEY_FALSEPRESS;
					calc_run_timed(gslot, 300);
					calcs[slot].cpu.pio.keypad->on_pressed &= ~KEY_FALSEPRESS;
					SendMessage(hProgressBar, PBM_STEPIT, 0, 0);
					gui_frame(slot);
					//write the output from file
#ifdef WINVER
					fopen_s(&file, buffer, "wb");
#else
					file = fopen(buffer, "wb");
#endif
					char* rom = (char *) calcs[slot].mem_c.flash;
					int size = calcs[slot].mem_c.flash_size;
					if (size != 0 && rom != NULL && file !=NULL) {
						u_int i;
						for(i = 0; i < size; i++) {
							fputc(rom[i], file);
						}
						fclose(file);
					}
					SendMessage(hProgressBar, PBM_STEPIT, 0, 0);
					break;
				}
				case PSN_QUERYCANCEL:
					error = TRUE;
					break;
			}
			return TRUE;
		}
		case WM_DESTROY:
			return FALSE;
	}
	return FALSE;
}

BOOL DownloadOS(BOOL version = 0)
{
	char downloaded_file[MAX_PATH];
#ifdef WINVER
	char *env;
	size_t envLen;
	_dupenv_s(&env, &envLen, "appdata");
	strcpy_s(downloaded_file, env);
	free(env);
	strcat_s(downloaded_file, _T("\\OS.8xu"));
	strcpy_s(osPath, downloaded_file);
#else
	strcpy(downloaded_file, getenv("appdata"));
	strcat(downloaded_file, _T("\\OS.8xu"));
	strcpy(osPath, downloaded_file);
#endif
	char *url;
	switch (model) {
		case TI_73:
			url = "http://education.ti.com/downloads/files/73/TI73_OS.73u";
			break;
		case TI_83P:
		case TI_83PSE:
			url = "http://education.ti.com/downloads/files/83plus/TI83Plus_OS.8Xu";
			break;
		case TI_84P:
		case TI_84PSE:
			if (version)
				url = "http://education.ti.com/downloads/files/83plus/TI84Plus_OS243.8Xu";
			else
				url = "http://education.ti.com/downloads/files/83plus/TI84Plus_OS.8Xu";
			break;
	}
	HRESULT hr = URLDownloadToFile(NULL, url, downloaded_file, 0, NULL);
	if (!SUCCEEDED(hr))
		MessageBox(NULL, "Unable to download file", "Download failed", MB_OK);
	return SUCCEEDED(hr);
}

DWORD WINAPI StartTIConnect( LPVOID lpParam ) {
	char *path = (char *) lpParam;
	char argBuf[MAX_PATH];
	sprintf_s(argBuf, "\"%s\" \"%s\"", TIConnectPath, path);
	STARTUPINFO si; 
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si)); 
	memset(&pi, 0, sizeof(pi)); 
	si.cb = sizeof(si);
	if (!CreateProcess(NULL, argBuf,
		NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, 
		NULL, NULL, &si, &pi)) {
		MessageBox(NULL, "Unable to start the process. Try manually sending the file.", "Error", MB_OK);
		return FALSE;
	}
	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);
						
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return TRUE;
}


INT_PTR CALLBACK SetupROMDumperProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static BOOL inited = FALSE;
	static HWND hButtonAuto, hButtonManual, hStaticError;
	switch (Message) {
		case WM_INITDIALOG: {
			hButtonAuto = GetDlgItem(hwnd, IDC_BUTTON_AUTO);
			hButtonManual = GetDlgItem(hwnd, IDC_BUTTON_MANUAL);
			hStaticError = GetDlgItem(hwnd, IDC_STATIC_ERROR);
			char *env;
			size_t envLen;
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				_dupenv_s(&env, &envLen, "programfiles(x86)");
			else
				_dupenv_s(&env, &envLen, "programfiles");
			strcat_s(TIConnectPath, env);
			free(env);
			//yes i know hard coding is bad :/
			strcat_s(TIConnectPath, "\\TI Education\\TI Connect\\TISendTo.exe");
			ExtractDumperProg();
			FILE *connectProg;
			if (fopen_s(&connectProg, TIConnectPath, "r") != 0) {
				Static_SetText(hStaticError, "Unable to find TI Connect, please manually send the dumper program.");
				Button_Enable(hButtonAuto, FALSE);
				ShowWindow(hStaticError, SW_SHOW);
			} else
				fclose(connectProg);
			return FALSE;
		}
		case WM_COMMAND: {
			switch(HIWORD(wParam)) {
				case BN_CLICKED:
					if (LOWORD(wParam) == IDC_BUTTON_AUTO) {
						CreateThread( 
							NULL,					// default security attributes
							0,						// use default stack size  
							StartTIConnect,			// thread function name
							&dumperPath,			// argument to thread function 
							0,						// use default creation flags 
							NULL);					// returns the thread identifier
					} else {
						char buf[MAX_PATH], ch;
						if (!SaveFile(buf, "	83 Plus Program  (*.8xp)\0*.8xp\0	All Files (*.*)\0*.*\0\0",
							"Wabbitemu Save ROM Dumper", "8xp")) {
							FILE *start, *end;
							fopen_s(&start, dumperPath, "rb");
							fopen_s(&end, buf, "wb");
							while(!feof(start)) {
								ch = fgetc(start);
								fputc(ch, end);
							}
							fclose(start);
							fclose(end);
							remove(dumperPath);
						}

					}
					PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_NEXT | PSWIZB_BACK);
					break;
			}
			return TRUE;
		}
		case WM_NOTIFY :
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			switch(pnmh->code) {
				case PSN_SETACTIVE:
					if (inited)
						PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_NEXT | PSWIZB_BACK);
					else {
						PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_BACK);
						inited = TRUE;
					}
					break;
				case PSN_WIZNEXT:
					break;
				case PSN_QUERYCANCEL:
					error = TRUE;
					break;
			}
			return TRUE;
		}
		case WM_DESTROY:
			return FALSE;
	}
	return FALSE;
}

INT_PTR CALLBACK SetupMakeROMProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static BOOL inited = FALSE;
	static HWND hStaticAppVar, hBrowseButton1, hBrowseButton2, hEditVar1, hEditVar2;
	switch (Message) {
		case WM_INITDIALOG: {
			hStaticAppVar = GetDlgItem(hwnd, IDC_STATIC_APPVAR);
			hBrowseButton1 = GetDlgItem(hwnd, IDC_BUTTON_BROWSE1);
			hBrowseButton2 = GetDlgItem(hwnd, IDC_BUTTON_BROWSE2);
			hEditVar1 = GetDlgItem(hwnd, IDC_EDT_APPVAR1);
			hEditVar2 = GetDlgItem(hwnd, IDC_EDT_APPVAR2);
			return FALSE;
		}
		case WM_COMMAND: {
			switch(HIWORD(wParam)) {
				case BN_CLICKED: {
					PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_FINISH | PSWIZB_BACK);
					char buf[MAX_PATH], ch;
					if (BrowseFile(buf, "	83 Plus Application Variables (*.8xv)\0*.8xv\0	All Files (*.*)\0*.*\0\0",
							"Wabbitemu Open ROM Dump", "8xp"))
						break;
					if (LOWORD(wParam) == IDC_BUTTON_BROWSE1) {
						Edit_SetText(hEditVar1, buf);
						ch = '2';
					} else {
						Edit_SetText(hEditVar2, buf);
						ch = '1';
					}
					int len = (int) strlen(buf);
					buf[len - 5] = ch;
					FILE *file;
					errno_t error = fopen_s(&file, buf, "r");
					if (error)
						break;
					fclose(file);
					if (LOWORD(wParam) == IDC_BUTTON_BROWSE2)
						Edit_SetText(hEditVar1, buf);
					else
						Edit_SetText(hEditVar2, buf);
					break;
				}
			}
			return TRUE;
		}
		case WM_NOTIFY :
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			switch(pnmh->code) {
				case NM_CLICK:
				case NM_RETURN: {
					PNMLINK pNMLink = (PNMLINK) lParam;
					LITEM item = pNMLink->item;
					char buffer[1024];
					memset(buffer, 0, ARRAYSIZE(buffer));
					int length = (int) wcslen(item.szUrl);
					WideCharToMultiByte(CP_ACP, 0, item.szID, length, buffer, length, NULL, NULL);
					if (strcmp(buffer, "RunHelp"))
						DialogBox(NULL, MAKEINTRESOURCE(DLG_RUNHELP), hwnd, (DLGPROC) HelpProc);
					break;
				}
				//The program will create two application variables D83PBE1.8xv and D84PBE2.8xv. You need to send these back to the computer, and browse them with the buttons below
				case PSN_SETACTIVE:
					if (inited)
						PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_FINISH | PSWIZB_BACK);
					else {
						PropSheet_SetWizButtons(GetParent(hwnd), PSWIZB_DISABLEDFINISH | PSWIZB_BACK);
						inited = TRUE;
					}
					char buf[1024];
					char name[12];
					strcpy_s(name, "D83PBE1.8xv");
					switch(model) {
						case TI_83P:
						case TI_83PSE:
							if (model != TI_83P)
								name[4] = 'S';
							sprintf_s(buf, "%s %s%s", "The program will create an application variable", name, ". You need to send this back to the computer, and locate it with the button below");
							break;
						case TI_84P:
						case TI_84PSE:
							name[2] = '4';
							if (model != TI_84P)
								name[4] = 'S';
							char name2[12];
							strcpy_s(name2, name);
							name2[6] = '2';
							sprintf_s(buf, "%s %s %s %s%s", "The program will create two application variables", name, "and", name2,". You need to send these back to the computer, and locate them with the buttons below");
							break;
					}
					Static_SetText(hStaticAppVar, buf);
					if (model < TI_84P) {
						ShowWindow(hBrowseButton2, SW_HIDE);
						ShowWindow(hEditVar2, SW_HIDE);
					}
					break;
				case PSN_WIZFINISH: {
					char browse[MAX_PATH];
					Edit_GetText(hEditVar1, browse, MAX_PATH);
					char buffer[MAX_PATH];
					GetExportROMName(buffer);
					int slot = calc_slot_new();
					ModelInit(slot);
					gslot = slot;
					//slot stuff
					calcs[slot].active = TRUE;
					strcpy_s(calcs[slot].rom_path, buffer);
					calcs[slot].model = model;
					calcs[slot].cpu.pio.model = model;
					FILE *file;
					fopen_s(&file, browse, "rb");
					//goto the name to see whcih one we have
					fseek(file, 66, SEEK_SET);
					char ch = fgetc(file);
					//goto the first byte of data
					fseek(file, 74, SEEK_SET);
					int page;
					unsigned char (*flash)[16384] = (uint8_t(*)[16384]) calcs[gslot].mem_c.flash;
					if (ch == '1') {
						int i;
						page = calcs[slot].mem_c.flash_pages - 1;
						for (i = 0; i < PAGE_SIZE; i++)
							flash[page][i] = fgetc(file);
					}
					else {
						int i;
						page = calcs[slot].mem_c.flash_pages - 0x11;;
						for (i = 0; i < PAGE_SIZE; i++)
							flash[page][i] = fgetc(file);
					}
					fclose(file);
					//second boot page
					if (model >= TI_84P) {
						Edit_GetText(hEditVar2, browse, MAX_PATH);
						fopen_s(&file, browse, "rb");
						//goto the name to see whcih one we have
						fseek(file, 66, SEEK_SET);
						char ch = fgetc(file);
						//goto the first byte of data
						fseek(file, 74, SEEK_SET);
						if (ch == '1') {
							int i;
							page = calcs[slot].mem_c.flash_pages - 1;
							for (i = 0; i < PAGE_SIZE; i++)
								flash[page][i] = fgetc(file);
						}
						else {
							int i;
							page = calcs[slot].mem_c.flash_pages - 0x11;;
							for (i = 0; i < PAGE_SIZE; i++)
								flash[page][i] = fgetc(file);
						}
					}

					//if you dont want to load an OS, fine...
					if (strlen(osPath) > 0) {
						fopen_s(&file, osPath, "rb");
						Load_8xu(file);
						fclose(file);
						calcs[slot].mem_c.flash[0x56] = 0x5A;
						calcs[slot].mem_c.flash[0x57] = 0xA5;
						//if (Button_GetCheck(hRadioDownload) == BST_CHECKED)
						//	remove(osPath);
					}
					calc_erase_certificate(calcs[slot].cpu.mem_c->flash,calcs[slot].cpu.mem_c->flash_size);
					calc_reset(slot);
					calc_run_timed(gslot, 200);
					calcs[slot].cpu.pio.keypad->on_pressed |= KEY_FALSEPRESS;
					calc_run_timed(gslot, 300);
					calcs[slot].cpu.pio.keypad->on_pressed &= ~KEY_FALSEPRESS;
					gui_frame(slot);
					//write the output from file
					fopen_s(&file, buffer, "wb");
					char* rom = (char *) calcs[slot].mem_c.flash;
					int size = calcs[slot].mem_c.flash_size;
					if (size != 0 && rom != NULL && file !=NULL) {
						u_int i;
						for(i = 0; i < size; i++) {
							fputc(rom[i], file);
						}
						fclose(file);
					}
					break;
				}
				case PSN_QUERYCANCEL:
					error = TRUE;
					break;
			}
			return TRUE;
		}
		case WM_DESTROY:
			return FALSE;
	}
	return FALSE;
}

INT_PTR CALLBACK HelpProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static int pic_num = 0;
	static HWND hwndBitmap;
	switch (Message) {
		case WM_INITDIALOG: {
			hwndBitmap = GetDlgItem(hwnd, IDC_PIC_HELP);
			SetTimer(hwnd, 0, 1, NULL);
			return 0;
		}
		case WM_TIMER: {
			HBITMAP hbmHelp = NULL;
			if (pic_num % 2)
				hbmHelp = LoadBitmap(g_hInst, "HOMESCREEN");
			else
				hbmHelp = LoadBitmap(g_hInst, "CATALOG");
			SendMessage(hwndBitmap, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hbmHelp);
			DeleteObject(hbmHelp);
			pic_num++;
			SetTimer(hwnd, 0, 5000, NULL);
			return 0;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: {
					EndDialog(hwnd, IDOK);
					return TRUE;
				}
			}
			break;
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}

void ExtractDumperProg() {
#ifdef WINVER
	char *env;
	size_t envLen;
	_dupenv_s(&env, &envLen, "appdata");
	strcpy_s(dumperPath, env);
	strcat_s(dumperPath, "\\dumper");
#else
	strcpy(dumperPath, getenv("appdata"));
	strcat(dumperPath, "\\dumper");
#endif
	HMODULE hModule = GetModuleHandle(NULL);
	HRSRC hrDumpProg;
	switch (model) {
		case TI_83P:
		case TI_83PSE:
		case TI_84P:
		case TI_84PSE:
			hrDumpProg = FindResource(hModule, MAKEINTRESOURCE(ROM8X), _T("CALCPROG"));
			strcat_s(dumperPath, ".8xp");
			break;
	}
	ExtractResource(dumperPath, hrDumpProg);
}

void ExtractResource(char *path, HRSRC resource) {
	HMODULE hModule = GetModuleHandle(NULL);
	HGLOBAL hGlobal = LoadResource(hModule, resource);
	DWORD size = SizeofResource(hModule, resource);
	void *data = LockResource(hGlobal);
	HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD writtenBytes;
	WriteFile(hFile, data, size, &writtenBytes, NULL);
	CloseHandle(hFile);
}