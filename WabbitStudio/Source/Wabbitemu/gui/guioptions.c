#include "stdafx.h"

#include "guioptions.h"

#include <commdlg.h>
#include "gui.h"
#include "gif.h"
#include "resource.h"
#include "calc.h"
#include "displayoptionstest.h"
#include "lcd.h"
#include "guicutout.h"
#include "keys.h"
#include "registry.h"
#include "fileutilities.h"


extern HINSTANCE g_hInst;
extern BITMAPINFO *bi;

RECT PropRect = {0, 0, 0, 0};
HWND hwndProp = NULL;
int PropPageLast = -1;

// We have to save a slot for the ROM info
// and skin, it differs per calc
static LPCALC lpCalc;

void DoPropertySheet(HWND hwndOwner) {

	if (hwndProp != NULL) {
		SwitchToThisWindow(hwndProp, TRUE);
		return;
	}

	lpCalc = (LPCALC) GetWindowLongPtr(hwndOwner, GWLP_USERDATA);
	PROPSHEETPAGE psp[6];
	PROPSHEETHEADER psh;

	psp[0].dwSize = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[0].hInstance = g_hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE(IDD_GENERAL);
	psp[0].pszIcon = NULL;
	psp[0].pfnDlgProc = GeneralOptionsProc;
	psp[0].pszTitle = _T("General");
	psp[0].lParam = 0;
	psp[0].pfnCallback = NULL;

	psp[1].dwSize = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[1].hInstance = g_hInst;
	psp[1].pszTemplate = MAKEINTRESOURCE(IDD_GIFOPTIONS);
	psp[1].pszIcon = NULL;
	psp[1].pfnDlgProc = GIFOptionsProc;
	psp[1].pszTitle = _T("Screen Capture");
	psp[1].lParam = 0;
	psp[1].pfnCallback = NULL;

	psp[2].dwSize = sizeof(PROPSHEETPAGE);
	psp[2].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[2].hInstance = g_hInst;
	psp[2].pszTemplate = MAKEINTRESOURCE(IDD_DISPLAYOPTIONS);
	psp[2].pszIcon = NULL;
	psp[2].pfnDlgProc = DisplayOptionsProc;
	psp[2].pszTitle = _T("Display");
	psp[2].lParam = 0;
	psp[2].pfnCallback = NULL;

	psp[3].dwSize = sizeof(PROPSHEETPAGE);
	psp[3].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[3].hInstance = g_hInst;
	psp[3].pszTemplate = MAKEINTRESOURCE(IDD_ROMOPTIONS);
	psp[3].pszIcon = NULL;
	psp[3].pfnDlgProc = ROMOptionsProc;
	psp[3].pszTitle = _T("ROM");
	psp[3].lParam = 0;
	psp[3].pfnCallback = NULL;

	psp[4].dwSize = sizeof(PROPSHEETPAGE);
	psp[4].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[4].hInstance = g_hInst;
	psp[4].pszTemplate = MAKEINTRESOURCE(IDD_SKINOPTIONS);
	psp[4].pszIcon = NULL;
	psp[4].pfnDlgProc = SkinOptionsProc;
	psp[4].pszTitle = _T("Skin");
	psp[4].lParam = 0;
	psp[4].pfnCallback = NULL;

	psp[5].dwSize = sizeof(PROPSHEETPAGE);
	psp[5].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[5].hInstance = g_hInst;
	psp[5].pszTemplate = MAKEINTRESOURCE(IDD_KEYSOPTIONS);
	psp[5].pszIcon = NULL;
	psp[5].pfnDlgProc = KeysOptionsProc;
	psp[5].pszTitle = _T("Keys");
	psp[5].lParam = 0;
	psp[5].pfnCallback = NULL;

	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP | PSH_MODELESS;
	psh.hwndParent = hwndOwner;
	psh.hInstance = g_hInst;
	psh.pszIcon = NULL;
	psh.pszCaption = (LPTSTR) _T("Wabbitemu Options");
	psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.ppsp = (LPCPROPSHEETPAGE) &psp;
	psh.pfnCallback = NULL;
	hwndProp = (HWND) PropertySheet(&psh);

	if (IsRectEmpty(&PropRect) == FALSE) {
		SetWindowPos(hwndProp, NULL,
				PropRect.left, PropRect.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}

	_tprintf_s(_T("PropPageLast: %d\n"), PropPageLast);
	if (PropPageLast != -1) {
		PropSheet_SetCurSel(hwndProp, NULL, PropPageLast);
	}

	return;
}


static HWND imgDisplayPreview;
static double displayFPS = 48.0f;

DWORD WINAPI ThreadDisplayPreview( LPVOID lpParam ) {
	CPU_t *cpu = (CPU_t *) lpParam;
	double Time = 0.0f;
	int i;
	clock_t last_time = (clock_t) (clock() - (1000 / displayFPS));
	clock_t difference = 0;
	for (;;) {
		if (cpu->pio.lcd != calcs[gslot].cpu.pio.lcd) {
			u_char *buffer = NULL;
			switch (cpu->imode) {
			case 0: buffer = displayoptionstest_draw_bounce(4,displayFPS,Time); break;
			case 1: buffer = displayoptionstest_draw_scroll(4,displayFPS,Time); break;
			case 2: buffer = displayoptionstest_draw_gradient((int) (displayFPS / 10.0f), displayFPS, Time);
			}
			fastcopy(buffer, cpu);
			if (cpu->imode == 2) Time += 1/70.0f;
			else Time += 1/displayFPS;
		}

		HDC hdc = GetDC(imgDisplayPreview);
		if (hdc == NULL) continue;


		StretchDIBits(hdc, 0, 0, 192, 128,
			0, 0, 96, 64,
			LCD_image(cpu->pio.lcd),
			bi,
			DIB_RGB_COLORS,
			SRCCOPY);

		ReleaseDC(imgDisplayPreview, hdc);

		for(i=0;i<16;i++) {
			if (cpu->imode == 2) {
				tc_add(cpu->timer_c,((MHZ_6/70.0f)-(67*768))/16);
			} else {
				tc_add(cpu->timer_c,((MHZ_6/displayFPS)-(67*768))/16);
			}

			cpu->output = FALSE;
			LCD_data(cpu,&(cpu->pio.devices[0x11]));
		}

		clock_t this_time = clock();
		clock_t displayTPF = (clock_t) (1000/displayFPS);
		if (cpu->imode == 2) displayTPF = (clock_t) (1000/70.0f);
		// where we should be minus where we are
		difference += ((last_time + displayTPF) - this_time);
		last_time = this_time;

		while (difference > displayTPF) {
			Sleep(displayTPF);
			difference -= displayTPF;
		}
		if (difference > -displayTPF) {
			Sleep(displayTPF);
		}
	}
}


LCD_t *DupLCDConfig(LCD_t *lcd_dest, const LCD_t *lcd_source) {
	if (lcd_dest == NULL || lcd_source == NULL) return lcd_dest;
	if (lcd_dest == lcd_source) return lcd_dest;

	lcd_dest->time = lcd_source->time;
	lcd_dest->write_last = lcd_source->write_last;
	lcd_dest->write_avg = lcd_source->write_avg;
	lcd_dest->mode = lcd_source->mode;
	lcd_dest->steady_frame = lcd_source->steady_frame;
	lcd_dest->shades = lcd_source->shades;
	lcd_dest->ufps_last = lcd_source->ufps_last;
	return lcd_dest;
}


INT_PTR CALLBACK DisplayOptionsProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND cbMode,  cbSource;
	static HWND trbShades, trbSteady, trbFPS;
	static LCD_t *lcd, *lcd_old;
	static CPU_t cpu;
	static HANDLE hdlThread = NULL;
	switch (Message) {
		case WM_INITDIALOG: {

			static timer_context_t timer_c;

			tc_init(&timer_c, MHZ_6);

			cpu.timer_c = &timer_c;
			cpu.imode = 0;

			lcd = LCD_init(&cpu, TI_83P);
			lcd_old = lcd;

			DupLCDConfig(lcd, calcs[gslot].cpu.pio.lcd);
			lcd->active = TRUE;
			lcd->word_len = 1;
			lcd->contrast = 52;
			lcd->time = 0.0;

			cpu.pio.devices[0x11].active = TRUE;
			cpu.pio.devices[0x11].mem_c = NULL;
			cpu.pio.devices[0x11].aux = lcd;
			cpu.pio.devices[0x11].code = (devp) LCD_data;

			cpu.pio.lcd = lcd;

			cbMode = GetDlgItem(hwndDlg, IDC_CBODISPLAYMODE);
			imgDisplayPreview = GetDlgItem(hwndDlg, IDC_IMGDISPLAYPREVIEW);

			SendMessage(cbMode, CB_ADDSTRING,  0, (LPARAM) _T("Perfect gray"));
			SendMessage(cbMode, CB_ADDSTRING,  0, (LPARAM) _T("Steady freq"));
			SendMessage(cbMode, CB_ADDSTRING,  0, (LPARAM) _T("Game gray"));
			if (lcd->mode == MODE_PERFECT_GRAY)
				SendMessage(cbMode, CB_SETCURSEL, 0, (LPARAM) lcd->mode);
			else if (lcd->mode == MODE_STEADY)
				SendMessage(cbMode, CB_SETCURSEL, 1, (LPARAM) lcd->mode);
			else
				SendMessage(cbMode, CB_SETCURSEL, 2, (LPARAM) lcd->mode);


			cbSource = GetDlgItem(hwndDlg, IDC_CBODISPLAYSOURCE);
			SendMessage(cbSource, CB_ADDSTRING, 0, (LPARAM) _T("Bounce"));
			SendMessage(cbSource, CB_ADDSTRING, 0, (LPARAM) _T("Scroll"));
			SendMessage(cbSource, CB_ADDSTRING, 0, (LPARAM) _T("Gradient"));
			SendMessage(cbSource, CB_ADDSTRING, 0 ,(LPARAM) _T("Live"));
			SendMessage(cbSource, CB_SETCURSEL, 0, (LPARAM) 0);

			trbShades = GetDlgItem(hwndDlg, IDC_TRBDISPLAYSHADES);
			SendMessage(trbShades, TBM_SETRANGE,
			    (WPARAM) TRUE,
			    (LPARAM) MAKELONG(2, LCD_MAX_SHADES));
			SendMessage(trbShades, TBM_SETTICFREQ, 1, 0);

			SendMessage(trbShades, TBM_SETPOS, TRUE, lcd->shades + 1);

			trbSteady = GetDlgItem(hwndDlg, IDC_TRBDISPLAYFREQ);
			SendMessage(trbSteady, TBM_SETRANGE,
				(WPARAM) TRUE,
				(LPARAM) MAKELONG(STEADY_FREQ_MIN, STEADY_FREQ_MAX));
			SendMessage(trbSteady, TBM_SETPOS, TRUE, MAKELPARAM(1.0 / lcd->steady_frame, 0));
			EnableWindow(trbSteady, lcd->mode == MODE_STEADY);

			trbFPS = GetDlgItem(hwndDlg, IDC_TRBDISPLAYFPS);
			SendMessage(trbFPS, TBM_SETRANGE,
						    (WPARAM) TRUE,
						    (LPARAM) MAKELONG(20, 80));
			SendMessage(trbFPS, TBM_SETTICFREQ, 10, 0);
			SendMessage(trbFPS, TBM_SETPOS, TRUE, MAKELPARAM(displayFPS, 0));

			SetWindowPos(imgDisplayPreview, NULL, 0, 0, 196, 132, SWP_NOMOVE | SWP_NOZORDER);
			if (hdlThread != NULL) TerminateThread(hdlThread, 0);
			hdlThread = CreateThread(NULL,0,ThreadDisplayPreview, &cpu, 0, NULL);
			return FALSE;
		}
		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {
					DupLCDConfig(calcs[gslot].cpu.pio.lcd, lcd);
					//calcs[gslot].cpu.pio.lcd->mode = lcd->mode;
					//calcs[gslot].cpu.pio.lcd->shades = lcd->shades;
					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			break;
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:
					switch (LOWORD(wParam)) {
					case  IDC_CBODISPLAYMODE:
						lcd->mode = (LCD_MODE) SendMessage(cbMode, CB_GETCURSEL, 0, 0);
						EnableWindow(trbSteady, lcd->mode == MODE_STEADY);
						PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
						return TRUE;
					case IDC_CBODISPLAYSOURCE: {
						static int last_index = 0;
						int index = (int) SendMessage(cbSource, CB_GETCURSEL, 0, 0);
						BOOL bEnable = TRUE;
						HWND stcDisplayOption = GetDlgItem(hwndDlg, IDC_STCDISPLAYOPTION);
						switch (index) {
						case 2:
							if (last_index == 3) DupLCDConfig(lcd_old, lcd);
							lcd = lcd_old;
							SendMessage(stcDisplayOption, WM_SETTEXT, 0, (LPARAM) _T("Levels"));
							break;
						case 0:
						case 1:
							SendMessage(stcDisplayOption, WM_SETTEXT, 0, (LPARAM) _T("FPS"));
							if (last_index == 3) DupLCDConfig(lcd_old, lcd);
							lcd = lcd_old;
							break;
						case 3:
							lcd = calcs[gslot].cpu.pio.lcd;
							if (last_index != 3) DupLCDConfig(lcd, lcd_old);
							bEnable = FALSE;
							break;
						}
						last_index = index;
						cpu.imode = index;
						cpu.pio.devices[0x11].aux = lcd;
						cpu.pio.lcd = lcd;
						if (last_index == 3) cpu.timer_c->elapsed = calcs[gslot].timer_c.elapsed;
						EnableWindow(trbFPS, bEnable);
						return TRUE;
					}
					default:
						return FALSE;
					}
			}
			break;
		case WM_DESTROY:
		{
			TerminateThread(hdlThread, 0);
			CloseHandle(hdlThread);
			GetWindowRect(hwndDlg, &PropRect);
			break;
		}
		case WM_HSCROLL:
		case WM_VSCROLL:
			if ((HWND) lParam == trbShades) {
				lcd->shades = (u_int) SendMessage(trbShades, TBM_GETPOS, 0, 0) - 1;
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;
			}
			if ((HWND) lParam == trbSteady) {
				lcd->steady_frame = 1.0 / SendMessage(trbSteady, TBM_GETPOS, 0, 0);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;
			}
			if ((HWND) lParam == trbFPS) {
				displayFPS = (double) SendMessage(trbFPS, TBM_GETPOS, 0, 0);
				return TRUE;
			}
			return FALSE;

	}
	return FALSE;
}



void GIFOptionsToggleAutosave(HWND hwndDlg, BOOL bEnable) {
	HWND
	hwndEdt 	= GetDlgItem(hwndDlg, IDC_EDTGIFFILENAME),
	hwndChk 	= GetDlgItem(hwndDlg, IDC_CHKUSEINCREASING),
//	hwndStc 	= GetDlgItem(hwndDlg, IDC_STCAUTOSAVE),
	hwndBtn		= GetDlgItem(hwndDlg, IDC_BTNGIFBROWSE);

	EnableWindow(hwndEdt, bEnable);
	EnableWindow(hwndChk, bEnable);
//	EnableWindow(hwndStc, bEnable);
	EnableWindow(hwndBtn, bEnable);
}

void SkinOptionsToggleCustomSkin(HWND hwndDlg, BOOL bEnable){
	HWND hBrowseSkin = GetDlgItem(hwndDlg, IDC_BROWSESKIN),
		hBrowseKey = GetDlgItem(hwndDlg, IDC_BROWSEKEY),
		hSkinText = GetDlgItem(hwndDlg, IDC_SKNFILE),
		hKeyText = GetDlgItem(hwndDlg, IDC_KEYFILE);
	EnableWindow(hBrowseKey, bEnable);
	EnableWindow(hBrowseSkin, bEnable);
	EnableWindow(hSkinText, bEnable);
	EnableWindow(hKeyText, bEnable);
}

/* 	Set bSave = TRUE if you are prompting for a file name to save to
 *	If you're fetching a filename otherwise, bSave = FALSE */
int SetGifName(BOOL bSave) {
	OPENFILENAME ofn;

	TCHAR lpstrFilter[] 	= _T("\
Graphics Interchange Format  (*.gif)\0*.gif\0\
All Files (*.*)\0*.*\0\0");
	TCHAR lpstrFile[MAX_PATH];
	unsigned int Flags = 0;

	if (bSave) Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

	int i;
	for (i = (int) _tcslen(gif_file_name)-1; i && gif_file_name[i] != '\\'; i--);

	if (i) {
#ifdef WINVER
		StringCbCopy(lpstrFile, sizeof(lpstrFile), gif_file_name + i + 1);
#else
		strcpy(lpstrFile, gif_file_name + i + 1);
#endif
	} else {
		lpstrFile[0] = '\0';
	}

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= GetForegroundWindow();
	ofn.hInstance			= NULL;
	ofn.lpstrFilter			= (LPCTSTR) lpstrFilter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= lpstrFile;
	ofn.nMaxFile			= sizeof(lpstrFile);
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= _T("Wabbitemu GIF File Target");
	ofn.Flags				= Flags | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES;
	ofn.lpstrDefExt			= _T("gif");
	ofn.lCustData			= 0;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;
	ofn.pvReserved			= NULL;
	ofn.dwReserved			= 0;
	ofn.FlagsEx				= 0;

	if (!GetSaveFileName(&ofn)) {
		return 1;
	}
#ifdef WINVER
	StringCbCopy(gif_file_name, sizeof(gif_file_name), lpstrFile);
#else
	strcpy(gif_file_name, lpstrFile);
#endif
	return 0;
}

int BrowseBMPFile(TCHAR *lpstrFile[]) {
	OPENFILENAME ofn;
	TCHAR lpstrFilter[] 	= _T("	BMP  (*.bmp)\0*.bmp\0	All Files (*.*)\0*.*\0\0");
	unsigned int Flags = 0;
	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= GetForegroundWindow();
	ofn.hInstance			= NULL;
	ofn.lpstrFilter			= (LPCTSTR) lpstrFilter;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= (LPTSTR) lpstrFile;
	ofn.nMaxFile			= sizeof(lpstrFile);
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= _T("Wabbitemu Open Bitmap");
	ofn.Flags				= Flags | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES;
	ofn.lpstrDefExt			= _T("bmp");
	ofn.lCustData			= 0;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;
	ofn.pvReserved			= NULL;
	ofn.dwReserved			= 0;
	ofn.FlagsEx				= 0;
	if (!GetOpenFileName(&ofn)) {
		return 1;
	}
	return 0;
}

INT_PTR CALLBACK SkinOptionsProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkCutout, chkCustom, hColorSelect, hBrowseSkin, hBrowseKey, hSkinText, hKeyText, chkAlphaBlend;
	static COLORREF backupFaceplate;
	switch (Message) {
		case WM_INITDIALOG: {
			chkCutout = GetDlgItem(hwndDlg, IDC_CHKCUTOUT);
			chkCustom = GetDlgItem(hwndDlg, IDC_CHKCSTMSKIN);
			chkAlphaBlend = GetDlgItem(hwndDlg, IDC_CHKALPHABLEND);
			hColorSelect = GetDlgItem(hwndDlg, IDC_COLORPICK);
			hBrowseSkin = GetDlgItem(hwndDlg, IDC_BROWSESKIN);
			hBrowseKey = GetDlgItem(hwndDlg, IDC_BROWSEKEY);
			hSkinText = GetDlgItem(hwndDlg, IDC_SKNFILE);
			hKeyText = GetDlgItem(hwndDlg, IDC_KEYFILE);
			BOOL CustomSkinSetting = lpCalc->bCustomSkin;
			SkinOptionsToggleCustomSkin(hwndDlg, CustomSkinSetting);
			Button_SetCheck(chkCutout, lpCalc->bCutout);
			Button_SetCheck(chkAlphaBlend, lpCalc->bAlphaBlendLCD);
			Button_SetCheck(chkCustom, lpCalc->bCustomSkin);
			SendMessage(hColorSelect, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) LoadBitmap(g_hInst, _T("SkinPicker")));
			backupFaceplate = lpCalc->FaceplateColor;
#ifndef _UNICODE
			CW2A skinPath(lpCalc->skin_path);
			CW2A keyPath(lpCalc->keymap_path);
			Edit_SetText(hSkinText, skinPath);
			Edit_SetText(hKeyText, keyPath);
#else
			Edit_SetText(hSkinText, lpCalc->skin_path);
			Edit_SetText(hKeyText, lpCalc->keymap_path);
#endif
			return 0;
		}
		case WM_COMMAND: {
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:	
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);	
					return TRUE;
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case IDC_BROWSESKIN: {
							TCHAR lpStrFile[MAX_PATH];
							if (!BrowseFile(lpStrFile, _T("Image Files (*.bmp;*.jpg;*.png;*.tiff)\0*.bmp;*.jpg;*.png;*.tiff\0	All Files (*.*)\0*.*\0\0"),
								_T("Browse for custom skin"), _T("png"))) {
									Edit_SetText(hSkinText, lpStrFile);
							}
							break;
						}
						case IDC_BROWSEKEY: {
							TCHAR lpStrFile[MAX_PATH];
							if (!BrowseFile(lpStrFile, _T("Image Files (*.bmp;*.jpg;*.png;*.tiff)\0*.bmp;*.jpg;*.png;*.tiff\0	All Files (*.*)\0*.*\0\0"),
								_T("Browse for custom keymap"), _T("png"))) {
									Edit_SetText(hKeyText, lpStrFile);
							}
							break;
						}
						case IDC_CHKALPHABLEND:
						case IDC_CHKCUTOUT:
							break;
						case IDC_CHKCSTMSKIN: {
							BOOL customSkinSetting = Button_GetCheck(chkCustom);
							SkinOptionsToggleCustomSkin(hwndDlg, customSkinSetting);
							break;
						}
						case IDC_COLORPICK: {
							HDC hColorPicker = GetDC(hColorSelect);
							POINT ptCursor;
							GetCursorPos(&ptCursor);
							RECT rc;
							GetWindowRect(hColorSelect, &rc);
							ptCursor.x -= rc.left;
							ptCursor.y -= rc.top;
							COLORREF selectedColor = GetPixel(hColorPicker, ptCursor.x, ptCursor.y);
							lpCalc->FaceplateColor = selectedColor;
							gui_frame_update(lpCalc);
							break;
						}
						default:
							return FALSE;
					}
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
					break;
			}
			//PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			return TRUE;
		}
		case WM_NOTIFY: {
    		switch (((NMHDR FAR *) lParam)->code) {
				case PSN_RESET: {
					lpCalc->FaceplateColor = backupFaceplate;
					gui_frame_update(lpCalc);
				}
				case PSN_APPLY: {
					lpCalc->bCutout = Button_GetCheck(chkCutout);
					lpCalc->bAlphaBlendLCD = Button_GetCheck(chkAlphaBlend);
					lpCalc->bCustomSkin = Button_GetCheck(chkCustom);

					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);
					backupFaceplate = lpCalc->FaceplateColor;

					TCHAR lpStrFile[MAX_PATH];
					Edit_GetText(hSkinText, lpStrFile, sizeof(lpStrFile));
#ifndef _UNICODE
					CA2W lpWStrFile(lpStrFile);
					StringCbCopyW(lpCalc->skin_path, sizeof(lpCalc->skin_path), lpWStrFile);
#else
					StringCbCopy(lpCalc->skin_path, sizeof(lpCalc->skin_path), lpStrFile);
#endif
					Edit_GetText(hKeyText, lpStrFile, sizeof(lpStrFile));
#ifndef _UNICODE
					CA2W lpWStrFile2(lpStrFile);
					StringCbCopyW(lpCalc->keymap_path, sizeof(lpCalc->skin_path), lpWStrFile2);
#else
					StringCbCopy(lpCalc->keymap_path, sizeof(lpCalc->skin_path), lpStrFile);
#endif
					gui_frame_update(lpCalc);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

INT_PTR CALLBACK GeneralOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND saveState_check, loadFiles_check, doBackups_check, wizard_check, alwaysTop_check;
	switch (Message) {
		case WM_INITDIALOG: {
			saveState_check = GetDlgItem(hwnd, IDC_CHKSAVE);
			loadFiles_check = GetDlgItem(hwnd, IDC_CHKLOADFILES);
			doBackups_check = GetDlgItem(hwnd, IDC_CHKREWINDING);
			wizard_check = GetDlgItem(hwnd, IDC_CHKSHOWWIZARD);
			alwaysTop_check = GetDlgItem(hwnd, IDC_CHKONTOP);
			return SendMessage(hwnd, WM_USER, 0, 0);
		}
		case WM_COMMAND: {
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:
					PropSheet_Changed(GetParent(hwnd), hwnd);
					return TRUE;
				case BN_CLICKED:
					switch(LOWORD(wParam)) {
						case IDC_CHKSAVE:
							break;
						case IDC_CHKLOADFILES:
							break;
						case IDC_CHKREWINDING:
							break;
						case IDC_CHKSHOWWIZARD:
							break;
					}
					PropSheet_Changed(GetParent(hwnd), hwnd);
					break;
			}
			return TRUE;
		}

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {
					int i;
					exit_save_state = Button_GetCheck(saveState_check);
					load_files_first = Button_GetCheck(loadFiles_check);
					show_wizard = Button_GetCheck(wizard_check);
					//we need to persist this immediately
					SaveWabbitKey(_T("load_files_first"), REG_DWORD, &load_files_first);
#ifdef WITH_BACKUPS
					do_backups = Button_GetCheck(doBackups_check);
					if (!do_backups) {
						for (i = 0; i < MAX_CALCS; i++)
							free_backups(&calcs[i]);
					}
#endif
					lpCalc->bAlwaysOnTop = Button_GetCheck(alwaysTop_check);
					gui_frame_update(lpCalc);
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			break;
		case WM_USER: {
			Button_SetCheck(saveState_check, exit_save_state);
			Button_SetCheck(loadFiles_check, load_files_first);
#ifdef WITH_BACKUPS
			Button_SetCheck(doBackups_check, do_backups);
#endif
			Button_SetCheck(wizard_check, show_wizard);
			Button_SetCheck(alwaysTop_check, lpCalc->bAlwaysOnTop);
			return TRUE;
		}
	}
	return FALSE;
}

INT_PTR CALLBACK GIFOptionsProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndSpeed, chkAutosave, edtGIFFilename, chkUseIncreasing, chkSize;
	static HWND rbnScreen, rbnGray;
	switch (Message) {
		case WM_INITDIALOG: {

			hwndSpeed = GetDlgItem(hwndDlg, IDC_TRBGIFFPS);
			SendMessage(hwndSpeed, TBM_SETRANGE,
			    (WPARAM) TRUE,
			    (LPARAM) MAKELONG(0, TBRTICS));
			SendMessage(hwndSpeed, TBM_SETTICFREQ, 1, 0);

			int speedPos = 0;
			if (gif_base_delay_start != 0) {
			 	speedPos = ((100 / gif_base_delay_start) - 9) / TBRSTEP;
			}

			SendMessage(hwndSpeed, TBM_SETPOS, TRUE, speedPos);

			int delayMin = (100 / (9 + (TBRTICS * TBRSTEP)));
			int fpsMax = 100 / delayMin;
			HWND hwndMaxSpeed = GetDlgItem(hwndDlg, IDC_STCGIFMAX);
			TCHAR lpszMax[10];
#ifdef WINVER
			StringCbPrintf(lpszMax, sizeof(lpszMax), _T("%d"), fpsMax);
#else
			sprintf(lpszMax, "%d", fpsMax);
#endif
			SendMessage(hwndMaxSpeed, WM_SETTEXT, 0, (LPARAM) lpszMax);


			chkAutosave = GetDlgItem(hwndDlg, IDC_CHKENABLEAUTOSAVE);
			Button_SetCheck(chkAutosave, gif_autosave);
			GIFOptionsToggleAutosave(hwndDlg, gif_autosave);

			edtGIFFilename = GetDlgItem(hwndDlg, IDC_EDTGIFFILENAME);
			Edit_SetText(edtGIFFilename, gif_file_name);

			chkUseIncreasing = GetDlgItem(hwndDlg, IDC_CHKUSEINCREASING);
			Button_SetCheck(chkUseIncreasing, gif_use_increasing);

			rbnScreen = GetDlgItem(hwndDlg, IDC_RBNSCREEN);
			rbnGray = GetDlgItem(hwndDlg, IDC_RBNGRAYSCALE);
			Button_SetCheck(gif_bw ? rbnGray : rbnScreen, BST_CHECKED);

			chkSize = GetDlgItem(hwndDlg, IDC_CHKGIF2X);
			Button_SetCheck(chkSize, (gif_size == 2) ? BST_CHECKED : BST_UNCHECKED);
			return TRUE;
		}
		case WM_NOTIFY:
    		switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {

					int speedPos = (int) SendMessage(hwndSpeed, TBM_GETPOS, 0, 0);
					//printf("spedpos: %d\n",speedPos);

					if (gif_write_state == GIF_IDLE) {
						gif_base_delay_start = 	//GIF_MINIMUM + (GIF_MAXIMUM=GIF_MINIMUM)/6
												(100 / (9 + (speedPos * TBRSTEP)));
					}
					//printf("gifbasedelay: %d\n",gif_base_delay_start);

					gif_autosave = Button_GetState(chkAutosave) & 0x0003 ? TRUE : FALSE;

					gif_use_increasing = Button_GetState(chkUseIncreasing) & 0x0003 ? TRUE : FALSE;

					gif_bw = Button_GetCheck(rbnGray);

					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);

					gif_size = Button_GetState(chkSize) & 0x0003 ? 2 : 1;

					Edit_GetText(edtGIFFilename, gif_file_name, ARRAYSIZE(gif_file_name));
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			break;
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
					return TRUE;
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case IDC_CHKENABLEAUTOSAVE: {
							BOOL bEnable = FALSE;
							if (Button_GetState(chkAutosave) & 0x0003) {
								bEnable = TRUE;
							}
							GIFOptionsToggleAutosave(hwndDlg, bEnable);
							break;
						}
						case IDC_BTNGIFBROWSE:
							SetGifName(FALSE);
							Edit_SetText(edtGIFFilename, gif_file_name);
							break;
						case IDC_CHKUSEINCREASING:
						case IDC_CHKGIF2X:
						case IDC_RBNGRAYSCALE:
						case IDC_RBNSCREEN:
							break;
						default:
							return FALSE;
					}
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
					break;
			}
			break;
		case WM_HSCROLL:
		case WM_VSCROLL:
			PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
			return TRUE;
	}

	return FALSE;
}



int GetROMName(TCHAR *lpstrFile) {
	OPENFILENAME ofn;

	TCHAR lpstrFilter[] 	= _T("\
Calculator ROM  (*.rom, *.bin)\0*.rom;*.bin\0\
All Files (*.*)\0*.*\0\0");

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= GetForegroundWindow();
	ofn.lpstrFilter			= (LPCTSTR) lpstrFilter;
	ofn.lpstrFile			= lpstrFile;
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrTitle			= _T("Wabbitemu Load ROM");
	ofn.Flags				= OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES;

	if (!GetOpenFileName(&ofn)) {
		return 1;
	}
	return 0;
}

int GetExportROMName(TCHAR *lpstrFile) {
	OPENFILENAME ofn;
	TCHAR lpstrFilter[] 	= _T("\
ROMS  (*.rom)\0*.rom\0\
BINS  (*.bin)\0*.bin\0\
All Files (*.*)\0*.*\0\0");

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(lpstrFile, MAX_PATH);

	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= GetForegroundWindow();
	ofn.lpstrFilter		= (LPCTSTR) lpstrFilter;
	ofn.lpstrFile		= lpstrFile;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrTitle		= _T("Wabbitemu Export Rom");
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_EXPLORER |
						  OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt		= _T("rom");
	if (!GetSaveFileName(&ofn)) return 1;
	return 0;
}


INT_PTR CALLBACK ROMOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND edtRom_path, edtRom_version, edtRom_model, edtRom_size, stcRom_image, saveState_check,
			ramPages_check;
	static HBITMAP hbmTI83P = NULL;
	switch (Message) {
		case WM_INITDIALOG: {
			edtRom_path = GetDlgItem(hwnd, IDC_EDTROMPATH);
			edtRom_version = GetDlgItem(hwnd, IDC_EDTROMVERSION);
			edtRom_model = GetDlgItem(hwnd, IDC_EDTROMMODEL);
			edtRom_size = GetDlgItem(hwnd, IDC_EDTROMSIZE);
			stcRom_image = GetDlgItem(hwnd, IDC_STCROMIMAGE);
			saveState_check = GetDlgItem(hwnd, IDC_CHKSAVE);
			ramPages_check = GetDlgItem(hwnd, IDC_CHECK_RAMPAGES);

			return SendMessage(hwnd, WM_USER, 0, 0);
		}
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case IDC_BTNROMBROWSE: {
							TCHAR lpszFile[MAX_PATH] = _T("\0");
							if (!GetROMName(lpszFile)) {
#ifdef WINVER
								StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), lpszFile);
#else
								strcpy(lpCalc->rom_path, lpszFile);
#endif
								//SendMessage(calcs[gslot].hwndLCD, WM_COMMAND, ECM_CALCRELOAD, 0);
								SendMessage(hwnd, WM_USER, 0, 0);
							}
							break;
						}
						case IDC_BTN1: {
							TCHAR lpszFile[MAX_PATH] = _T("\0");
							if (!GetExportROMName(lpszFile)) {
#ifdef WINVER
								FILE* outfile;
								_tfopen_s(&outfile, lpszFile, _T("wb"));
#else
								FILE* outfile = fopen(lpszFile,"wb");
#endif
								char* rom = (char *) lpCalc->mem_c.flash;
								int size = lpCalc->mem_c.flash_size;
								if (size != 0 && rom!=NULL && outfile!=NULL) {
									int i;
									for(i = 0; i < size; i++) {
										fputc(rom[i], outfile);
									}
									fclose(outfile);
								}
							}
							break;
						}
						case IDC_CHKSAVE:
							PropSheet_Changed(GetParent(hwnd), hwnd);
							break;
					}
			}
			return TRUE;

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {
					exit_save_state = Button_GetCheck(saveState_check);
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			break;

		// Update all of the ROM attributes
		case WM_USER:
			Button_SetCheck(saveState_check, exit_save_state);
			Edit_SetText(edtRom_path, lpCalc->rom_path);
#ifdef _UNICODE
			TCHAR szRomVersion[256];
			MultiByteToWideChar(CP_ACP, 0, lpCalc->rom_version, -1, szRomVersion, ARRAYSIZE(szRomVersion));
			Edit_SetText(edtRom_version, szRomVersion);
#else
			Edit_SetText(edtRom_version, lpCalc->rom_version);
#endif
			Edit_SetText(edtRom_model, CalcModelTxt[lpCalc->model]);
//			Button_SetCheck(ramPages_check, lpCalc->mem_c.ram_pages_missing);
			TCHAR szRomSize[16];
#ifdef WINVER
			StringCbPrintf(szRomSize, sizeof(szRomSize), _T("%0.1f KB"), (float) lpCalc->cpu.mem_c->flash_size / 1024.0f);
#else
			sprintf(szRomSize, "%0.1f KB", (float) lpCalc->cpu.mem_c->flash_size/1024.0f);
#endif
			Edit_SetText(edtRom_size, szRomSize);
			if (hbmTI83P)
				DeleteObject(hbmTI83P);
			switch (lpCalc->model) {
				case TI_83PSE:
					hbmTI83P = LoadBitmap(g_hInst, _T("CalcTI83PSE"));
					break;
				default:
					hbmTI83P = LoadBitmap(g_hInst, _T("CalcTI83P"));
					break;
			}
			SendMessage(stcRom_image, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hbmTI83P);
			return TRUE;
	}
	return FALSE;
}
//static ACCEL hInitialAccels[256];	// original tables not touched
ACCEL hNewAccels[256];		// working copy
HWND hListMenu;
// all submenus of current category
HWND hHotKey;
HWND hListKeys; 			// shortcuts for currently selected command
HMENU hMenu;
int m_nCommands; 			// total commands listed (all categories)
int nStore, nUsed; 			// how many ACCELs allocated and how many used already
TCHAR *m_sCtrl, *m_sAlt, *m_sShift;
int m_nInitialLen;
DWORD m_dwCheckSum; 		// trivia for initial table
static int cur_sel;
bool accelerators = true;
INT_PTR CALLBACK KeysOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_INITDIALOG: {
			int i;			
			HWND hComboBox = GetDlgItem(hwnd, IDC_COMBO_MENU);
			hMenu = LoadMenu(g_hInst, (LPCTSTR)IDR_MAIN_MENU);
			hListMenu = GetDlgItem(hwnd, IDC_LIST_MENU);
			hListKeys = GetDlgItem(hwnd, IDC_LIST_KEYS);
			hHotKey = GetDlgItem(hwnd, IDC_HOTKEY);
			int numEntries = CopyAcceleratorTable(haccelmain, NULL, 0);
			nUsed = CopyAcceleratorTable(haccelmain, hNewAccels, numEntries);
			nStore = 256;
			int count = GetMenuItemCount(hMenu);
			for (i = 0; i < count; i++) {
				TCHAR *string = (TCHAR *) GetFriendlyMenuText(hMenu, i, 0);
				ComboBox_AddString(hComboBox, string);
				free(string);
			}
			ComboBox_AddString(hComboBox, _T("Emulator"));
			ComboBox_SetCurSel(hComboBox, 0);
			SendMessage(hwnd, WM_COMMAND, CBN_SELCHANGE << 16, 0);
			return TRUE;
		}
		case WM_COMMAND: {
			switch(HIWORD(wParam)) {
				case EN_CHANGE: {
					BOOL enable = (BOOL) SendMessage(hHotKey, HKM_GETHOTKEY, 0 , 0);
					EnableWindow(GetDlgItem(hwnd, IDC_ASSIGN_ACCEL), enable);
					break;
				}
				case CBN_SELCHANGE: {
					HWND hSender = (HWND)lParam;
					if (hSender == hListKeys) {
						int sel = ListBox_GetCurSel(hListKeys);
						EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_ACCEL), sel >= 0);
					} else {
						ChangeMenuCommands(hSender);
					}
					break;
				}
				case BN_CLICKED: {
					HWND hSender = (HWND)lParam;
					if (hSender == GetDlgItem(hwnd, IDC_ASSIGN_ACCEL)) {
						int i;
						WORD wVirtualKeyCode, wModifiers;
						DWORD key = (DWORD) SendMessage(hHotKey, HKM_GETHOTKEY, 0, 0);
						wVirtualKeyCode = LOBYTE(LOWORD(key));
						wModifiers = HIBYTE(LOWORD(key));
						int active = ListView_GetNextItem(hListMenu, -1, LVNI_SELECTED);
						if(wVirtualKeyCode && active >= 0) {
							// i don't really understand that: is this ALWAYS a virt key? Looks like it!
							BYTE fVirt = (wModifiers & (HOTKEYF_SHIFT| HOTKEYF_CONTROL| HOTKEYF_ALT)) << 2;
							fVirt |= FVIRTKEY | FNOINVERT;							// various checks: valid range, not already assigned et cetera
							LVITEM lvi;
							lvi.iItem = active;
							lvi.mask = LVIF_PARAM;
							ListView_GetItem(hListMenu, &lvi);
							int newCmd = (int) lvi.lParam;
							for (i=0; i < nUsed; i++)
								if(hNewAccels[i].fVirt == fVirt && hNewAccels[i].key == wVirtualKeyCode)
									break;
							if (i < nUsed) {								// cheeky: could this be the same item being assigned a double key?
								if (newCmd == hNewAccels[i].cmd)
									return 0; // no actions required
								// ask for shortcut overwrite confirmation
								if(MessageBox(hwnd, _T("This key combination is already in use.\nErase the old command assignment?"),
									_T("Overwrite?"), MB_ICONQUESTION | MB_YESNO) == IDNO)
									return 0;
								hNewAccels[i].cmd = newCmd;
							} else if(nUsed < nStore) { // i'm sure capacity will never be reached
								hNewAccels[nUsed].fVirt = fVirt;
								hNewAccels[nUsed].key = wVirtualKeyCode;
								hNewAccels[nUsed].cmd = newCmd;
								nUsed++;
							}
							ChangeCommand(hwnd); // reflect changes
							// this key can only be clicked, and if it is default & disabled, the key navigation gets screwy
							// so make something else default item
							//SendMessage(DM_SETDEFID, IDOK);
							SetFocus(hHotKey);
						}
					} else if (hSender == GetDlgItem(hwnd, IDC_REMOVE_ACCEL)) {
						int idx = ListBox_GetCurSel(hListKeys);
						LRESULT lr = ListBox_GetItemData(hListKeys, idx);
						if(lr != LB_ERR) {
							int i;
							BYTE fVirt = (BYTE)LOWORD(lr);
							WORD key = HIWORD(lr);
							for(i = 0; i < nUsed; i++)
								if(hNewAccels[i].fVirt == fVirt && hNewAccels[i].key == key) {
									ListBox_DeleteString(hListKeys, idx);
									// pack working accelerator table too
									int last = nUsed-1;
									if(i < last)
										hNewAccels[i] = hNewAccels[last];
									nUsed--;
									EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_ACCEL), FALSE);
									break;
								}
						}
					} else if (hSender == GetDlgItem(hwnd, IDC_RESET_ACCEL)) {
						//reload whatever is packed in with us
						HACCEL hAccelNew = LoadAccelerators(g_hInst, _T("Z80Accel"));
						int numEntries = CopyAcceleratorTable(hAccelNew, NULL, 0);
						nUsed = CopyAcceleratorTable(hAccelNew, hNewAccels, numEntries);
						//need to clean up :D
						DestroyAcceleratorTable(hAccelNew);
						ChangeCommand(hwnd);
					}
					PropSheet_Changed(GetParent(hwnd), hwnd);
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			switch(((LPNMHDR)lParam)->code) {
				case LVN_ITEMCHANGED: {
					ChangeCommand(hwnd);
					break;
				}
				case PSN_APPLY: {
					DestroyAcceleratorTable(haccelmain);
					haccelmain = CreateAcceleratorTable(hNewAccels, nUsed);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
		}
	}
	return FALSE;
}

void ChangeMenuCommands(HWND hSender) {
	TCHAR buffer[256];
	LVITEM li;
	li.mask = LVIF_TEXT;
	li.iSubItem = 0;
	li.cchTextMax = 64;
	cur_sel = (int) SendMessage(hSender, CB_GETCURSEL, 0, 0);
	HMENU hSubMenu = GetSubMenu(hMenu, cur_sel);
	ListView_DeleteAllItems(hListMenu);
	memset(buffer, 0, ARRAYSIZE(buffer));
	if (hSubMenu == NULL) {
		//assume that if we cant find the menu, then its the emulator keys
		AddNormalKeys((TCHAR *) &buffer);
		accelerators = false;
	} else {
		RecurseAddItems(hSubMenu, (TCHAR *) &buffer);
		accelerators = true;
	}
}

TCHAR* NameFromVKey(UINT nVK) {
	UINT nScanCode = MapVirtualKeyEx(nVK, 0, GetKeyboardLayout(0));
	switch(nVK) {		// Keys which are "extended" (except for Return which is Numeric Enter as extended)	
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_NEXT:  // Page down
		case VK_PRIOR: // Page up
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			nScanCode |= 0x100; // Add extended bit	
	}
	// GetKeyNameText() expects the scan code to be on the same format as WM_KEYDOWN
	// Hence the left shift
	TCHAR *str = (TCHAR *) malloc(80);
	memset(str, 0, 80);
	GetKeyNameText(nScanCode << 16, str, 79);
	// these key names are capitalized and look a bit daft
	int len = (int) _tcslen(str);
	if(len > 1) {
		LPTSTR p2 = CharNext(str);
		CharLowerBuff(p2, (DWORD) (len - (p2 - str)));
	}
	return str; // internationalization ready, sweet!
}

void ChangeCommand(HWND hwnd) {		// new command selected
	int active = ListView_GetNextItem(hListMenu, -1, LVNI_SELECTED), idx;
	TCHAR *name;
	ListBox_ResetContent(hListKeys);
	if(active != -1) {
		int i;
		LVITEM lvi;
		lvi.mask = LVIF_PARAM;
		lvi.iItem = active;
		ListView_GetItem(hListMenu, &lvi);
		u_int cmd = (u_int) lvi.lParam;		// add all accelerators registered for this command
		for(i = 0; i < nUsed; i++) {
			if(hNewAccels[i].cmd == cmd) {
				name = (TCHAR *) NameFromAccel(hNewAccels[i]);
				idx = ListBox_AddString(hListKeys, name);
				free(name);
				lvi.lParam = MAKELPARAM(hNewAccels[i].fVirt, hNewAccels[i].key);
				ListView_SetItem(hListKeys, &lvi);
				ListBox_SetItemData(hListKeys, idx, MAKELPARAM(hNewAccels[i].fVirt, hNewAccels[i].key));
			}
		}
		ListBox_SetCurSel(hListKeys, LB_ERR);
	}
	SendMessage(hHotKey, HKM_SETHOTKEY, 0, 0);
	EnableWindow(GetDlgItem(hwnd, IDC_ASSIGN_ACCEL), FALSE);
	EnableWindow(GetDlgItem(hwnd, IDC_REMOVE_ACCEL), FALSE);
}

TCHAR* NameFromAccel(ACCEL key) {
	TCHAR *name = (TCHAR *) malloc(80);
	memset(name, 0, 80);
	if(key.fVirt & FCONTROL)
#ifdef WINVER
		StringCbCat(name, _tcslen(name), _T("Ctrl + "));
#else
		strcat(name, "Ctrl + ");
#endif
	if(key.fVirt & FALT)
#ifdef WINVER
		StringCbCat(name, _tcslen(name), _T("Alt + "));
#else
		strcat(name, "Alt + ");
#endif
	if(key.fVirt & FSHIFT)
#ifdef WINVER
		StringCbCat(name, _tcslen(name), _T("Shift + "));
#else
		strcat(name, "Shift + ");
#endif
	// FNOINVERT is useless, backward compatibility
	if(key.fVirt & FVIRTKEY) {
		TCHAR *temp = (TCHAR *) NameFromVKey(key.key);
#ifdef WINVER
		StringCbCat(name, _tcslen(name), temp);
#else
		strcat(name, temp);
#endif
		free(temp);
	} else {
		// key field is an ASCII key code. (i never saw one of these)
#ifdef _UNICODE
		char ca = (char)key.key;
		wchar_t cu;
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &ca, 1, &cu, 1);
		name += cu;
#else
		name += key.key;
#endif
	}
	return name;
}

void AddNormalKeys(TCHAR *base) {
	LVITEM li; // lparam is command ID
	li.mask = LVIF_TEXT | LVIF_PARAM;
	li.iSubItem = 0;	// browse this menu checking for submenus
	//int nItems = ARRAYSIZE(keygrps);
}

void RecurseAddItems(HMENU hMenu, TCHAR *base) {
	int i;
	LVITEM li; // lparam is command ID
	li.mask = LVIF_TEXT | LVIF_PARAM;
	li.iSubItem = 0;	// browse this menu checking for submenus
	int nItems = GetMenuItemCount(hMenu);
	MENUITEMINFO mi;
	mi.cbSize = sizeof(MENUITEMINFO);
	mi.fMask = MIIM_ID | MIIM_SUBMENU;
	TCHAR *name, temp[64];
	for(i = 0; i < nItems; i++)	{
		//temp = (char*)malloc(64);
		GetMenuItemInfo(hMenu, i, TRUE, &mi); 		// by position
		if(!mi.wID) 								// separators excluded
			continue;
		name = (TCHAR *) GetFriendlyMenuText(hMenu, i, 0);
#ifdef WINVER
		StringCbCopy(temp, sizeof(temp), base);
		StringCbCat(temp, sizeof(temp), name);
#else
		strcpy(temp, base);
		strcat(temp, name);
#endif
		free(name);
		if(mi.hSubMenu) {
#ifdef WINVER
			StringCbCat(temp, sizeof(temp), _T(" > "));
#else
			strcat(temp, " > ");
#endif
			RecurseAddItems(mi.hSubMenu, temp);
			//free(temp);
		} else {
			if(!IsValidCmdRange(mi.wID))
				continue;
			li.pszText = (LPTSTR)(LPCTSTR)temp;
			li.iItem = ListView_GetItemCount(hListMenu);
			li.lParam = mi.wID;			// is this mixed icon/nie mode going to make listview funny?
			ListView_InsertItem(hListMenu, &li);
			//free(temp);
		}
	}
}

BOOL IsValidCmdRange(WORD cmdid) {	// return FALSE to exclude certain commands from customization
	return TRUE;
}

TCHAR * GetFriendlyMenuText(HMENU hMenu, int nItem, UINT uFlag) {
	TCHAR buf[256];
	uFlag = MF_BYPOSITION;
	GetMenuString(hMenu, nItem, buf, ARRAYSIZE(buf), uFlag);	// strip ampersands and tab characters
	TCHAR *str = (TCHAR *) malloc(_tcslen(buf) + 1);
	TCHAR *start = str;
	int i = 0;
	while(1) {
		TCHAR ch = buf[i++];
		if (ch == '&')
			continue;
		else if (ch == '\t') {
			*str++ = 0;
			break;
		} else
			*str++ = ch; // multibytes not an issue
		if(!ch)
			break;
	}	// finally remove any trailing dots (usually "...")
	str -= 2;
	while(*str == '.')
		*str-- = 0;
	return start;
}
