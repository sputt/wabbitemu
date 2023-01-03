#include "stdafx.h"

#include "guioptions.h"

#include "gui.h"
#include "guiskin.h"
#include "gif.h"
#include "calc.h"
#include "displayoptionstest.h"
#include "lcd.h"
#include "guicutout.h"
#include "keys.h"
#include "registry.h"
#include "fileutilities.h"
#include "guiwizard.h"
#include "dbcommon.h"
#include "exportvar.h"

extern HINSTANCE g_hInst;

RECT PropRect = {0, 0, 0, 0};
HWND hwndProp = NULL;
int PropPageLast = -1;

// We have to save a slot for the ROM info
// and skin, it differs per calc
static LPCALC lpCalc;
static LPMAINWINDOW lpMainWindow;

void DoPropertySheet(HWND hwndOwner, LPMAINWINDOW mainWindow) {

	if (hwndProp != NULL) {
		SwitchToThisWindow(hwndProp, TRUE);
		return;
	}

	lpMainWindow = mainWindow;
	if (lpMainWindow == NULL) {
		return;
	}

	lpCalc = lpMainWindow->lpCalc;

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
	hwndProp = (HWND) PropertySheet(&psh);

	if (IsRectEmpty(&PropRect) == FALSE) {
		SetWindowPos(hwndProp, NULL, PropRect.left, PropRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	if (PropPageLast != -1) {
		PropSheet_SetCurSel(hwndProp, NULL, PropPageLast);
	}

	return;
}

static HWND imgDisplayPreview;
static double displayFPS = 48.0f;

LCD_t *DupLCDConfig(LCD_t *lcd_dest, const LCD_t *lcd_source, BOOL deep_copy) {
	if (lcd_dest == NULL || lcd_source == NULL || lcd_dest == lcd_source)
		return lcd_dest;

	if (deep_copy) {
		memcpy(lcd_dest, lcd_source, sizeof(LCD_t));
		ZeroMemory(lcd_dest->queue, sizeof(lcd_dest->queue));
	}
	else {
		lcd_dest->mode = lcd_source->mode;
		lcd_dest->steady_frame = lcd_source->steady_frame;
		lcd_dest->shades = lcd_source->shades;
	}
	return lcd_dest;
}

DWORD WINAPI ThreadDisplayPreview(LPVOID lpParam) {
	CPU_t *cpu = (CPU_t *) lpParam;
	double Time = 0.0f;
	int i;
	clock_t last_time = (clock_t) (clock() - (1000 / displayFPS));
	clock_t difference = 0;
	for (;;) {
		HDC hdc = GetDC(imgDisplayPreview);
		if (hdc == NULL) {
			continue;
		}
		uint8_t *screenImage;
		LCDBase_t *lcd = cpu->pio.lcd;
		switch (cpu->imode) {
		case 0: displayoptionstest_draw_bounce(4, displayFPS, Time); break;
		case 1: displayoptionstest_draw_scroll(4, displayFPS, Time); break;
		case 2: displayoptionstest_draw_gradient((int)(displayFPS / 10.0f), displayFPS, Time); break;
		case 3: lcd = lpCalc->cpu.pio.lcd;
		}

		if (cpu->imode != 3) {
			fastcopy(cpu);
		}

		if (cpu->imode == 2) {
			Time += 1 / 70.0f;
		} else {
			Time += 1 / displayFPS;
		}

		screenImage = lcd->image(lcd);
		
		int dpi = GetDpiForSystem();
		StretchDIBits(hdc, 0, 0, MulDiv(192, dpi, 96), MulDiv(128, dpi, 96),
			0, 0, 96, 64,
			screenImage,
			GetLCDColorPalette(TI_83P, lcd),
			DIB_RGB_COLORS,
			SRCCOPY);

		free(screenImage);
		ReleaseDC(imgDisplayPreview, hdc);

		for(i = 0; i < 16; i++) {
			if (cpu->imode == 2) {
				tc_add(cpu->timer_c, ((cpu->timer_c->freq / 70.0f)-(67 * 768)) / 16);
			} else {
				tc_add(cpu->timer_c, ((cpu->timer_c->freq / displayFPS)-(67 * 768)) / 16);
			}

			cpu->output = FALSE;
			if (cpu->pio.lcd != lpCalc->cpu.pio.lcd) {
				cpu->pio.lcd->data(cpu, &(cpu->pio.devices[0x11]));
			}
		}

		clock_t this_time = clock();
		clock_t displayTPF = (clock_t) (CLOCKS_PER_SEC / displayFPS);
		if (cpu->imode == 2) {
			displayTPF = (clock_t) (CLOCKS_PER_SEC / 70.0f);
		}
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

INT_PTR CALLBACK DisplayOptionsProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND cbMode,  cbSource;
	static HWND trbShades, trbSteady, trbFPS;
	static LCD_t *lcd;
	static CPU_t cpu;
	static HANDLE hdlThread = NULL;
	static int shades;
	static double freq;
	static LCD_MODE mode;
	switch (Message) {
		case WM_INITDIALOG: {
			static timer_context_t timer_c;

			tc_init(&timer_c, lpCalc->cpu.timer_c->freq);
			cpu.timer_c = &timer_c;
			cpu.imode = 0;

			LCD_t *current_lcd = (LCD_t *) lpCalc->cpu.pio.lcd;
			shades = current_lcd->shades;
			mode = current_lcd->mode;
			freq = current_lcd->steady_frame;

			lcd = LCD_init(&cpu, TI_83P);

			DupLCDConfig(lcd, current_lcd, TRUE);
			lcd->base.active = TRUE;
			lcd->word_len = 1;
			lcd->base.contrast = LCD_MID_CONTRAST;
			lcd->base.time = 0.0;

			cpu.pio.devices[0x11].active = TRUE;
			cpu.pio.devices[0x11].mem_c = NULL;
			cpu.pio.devices[0x11].aux = lcd;
			cpu.pio.devices[0x11].code = (devp) lcd->base.data;
			cpu.pio.lcd = (LCDBase_t *) lcd;

			cbMode = GetDlgItem(hwndDlg, IDC_CBODISPLAYMODE);
			imgDisplayPreview = GetDlgItem(hwndDlg, IDC_IMGDISPLAYPREVIEW);
			
			ComboBox_AddString(cbMode, _T("Perfect gray"));
			ComboBox_AddString(cbMode, _T("Steady freq"));
			ComboBox_AddString(cbMode, _T("Game gray"));
			ComboBox_SetCurSel(cbMode, lcd->mode);

			cbSource = GetDlgItem(hwndDlg, IDC_CBODISPLAYSOURCE);
			ComboBox_AddString(cbSource, _T("Bounce"));
			ComboBox_AddString(cbSource, _T("Scroll"));
			ComboBox_AddString(cbSource, _T("Gradient"));
			ComboBox_AddString(cbSource, _T("Live"));
			ComboBox_SetCurSel(cbSource, 0);

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

			int dpi = GetDpiForWindow(imgDisplayPreview);
			SetWindowPos(imgDisplayPreview, NULL, 0, 0, MulDiv(196, dpi, 96), MulDiv(132, dpi, 96), SWP_NOMOVE | SWP_NOZORDER);
			if (hdlThread != NULL) {
				TerminateThread(hdlThread, 0);
				hdlThread = NULL;
			}
			if (lpCalc->model >= TI_84PCSE) {
				EnableWindow(hwndDlg, FALSE);
				MessageBox(hwndDlg, _T("LCD Options are not available for the TI 84+C SE"), _T("Warning"), MB_OK);
			} else {
				EnableWindow(hwndDlg, TRUE);
				hdlThread = CreateThread(NULL, 0, ThreadDisplayPreview, &cpu, 0, NULL);
			}
			return FALSE;
		}
		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {
					if (lpCalc->model < TI_84PCSE) {
						DupLCDConfig((LCD_t *)lpCalc->cpu.pio.lcd, lcd, FALSE);
					}
					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_QUERYCANCEL: {
					LCD_t *current_lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
					current_lcd->shades = shades;
					current_lcd->mode = mode;
					current_lcd->steady_frame = freq;
					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, FALSE);
					return TRUE;
				}
				case PSN_KILLACTIVE: {
					LCD_t *current_lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
					current_lcd->shades = shades;
					current_lcd->mode = mode;
					current_lcd->steady_frame = freq;
					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, FALSE);
					return TRUE;
				}
			}
			break;
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:
					switch (LOWORD(wParam)) {
					case IDC_CBODISPLAYMODE: {
						LCD_t *current_lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
						current_lcd->mode = lcd->mode = (LCD_MODE)ComboBox_GetCurSel(cbMode);
						EnableWindow(trbSteady, lcd->mode == MODE_STEADY);
						PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
						return TRUE;
					}
					case IDC_CBODISPLAYSOURCE: {
						int index = ComboBox_GetCurSel(cbSource);
						BOOL bEnable = TRUE;
						HWND stcDisplayOption = GetDlgItem(hwndDlg, IDC_STCDISPLAYOPTION);
						switch (index) {
							case 0:
							case 1:
								Static_SetText(stcDisplayOption, _T("FPS"));
								break;
							case 2:
								Static_SetText(stcDisplayOption, _T("Levels"));
								break;
							case 3:
								bEnable = FALSE;
								break;
						}
						cpu.imode = index;
						EnableWindow(trbFPS, bEnable);
						return TRUE;
					}
					default:
						return FALSE;
					}
			}
			break;
		case WM_DESTROY: {
			TerminateThread(hdlThread, 0);
			CloseHandle(hdlThread);
			hdlThread = NULL;
			GetWindowRect(GetParent(hwndDlg), &PropRect);
			break;
		}
		case WM_HSCROLL:
		case WM_VSCROLL:
			LCD_t *current_lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
			if ((HWND) lParam == trbShades) {
				current_lcd->shades = lcd->shades = (u_int)SendMessage(trbShades, TBM_GETPOS, 0, 0) - 1;
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;
			}
			if ((HWND) lParam == trbSteady) {
				current_lcd->steady_frame = lcd->steady_frame = 1.0 / SendMessage(trbSteady, TBM_GETPOS, 0, 0);
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
		hKeyText = GetDlgItem(hwndDlg, IDC_KEYFILE),
		hFaceColor = GetDlgItem(hwndDlg, IDC_CHKFACECOLOR);
	EnableWindow(hBrowseKey, bEnable);
	EnableWindow(hBrowseSkin, bEnable);
	EnableWindow(hSkinText, bEnable);
	EnableWindow(hKeyText, bEnable);
	EnableWindow(hFaceColor, bEnable);
}

INT_PTR CALLBACK SkinOptionsProc(HWND hwndDlg, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND chkCutout, chkCustom, chkFaceColor, hColorSelect, hBrowseSkin, hBrowseKey, hSkinText, hKeyText, chkAlphaBlend;
	static COLORREF backupFaceplate;
	switch (Message) {
		case WM_INITDIALOG: {
			chkCutout = GetDlgItem(hwndDlg, IDC_CHKCUTOUT);
			chkCustom = GetDlgItem(hwndDlg, IDC_CHKCSTMSKIN);
			chkFaceColor = GetDlgItem(hwndDlg, IDC_CHKFACECOLOR);
			chkAlphaBlend = GetDlgItem(hwndDlg, IDC_CHKALPHABLEND);
			hColorSelect = GetDlgItem(hwndDlg, IDC_COLORPICK);
			hBrowseSkin = GetDlgItem(hwndDlg, IDC_BROWSESKIN);
			hBrowseKey = GetDlgItem(hwndDlg, IDC_BROWSEKEY);
			hSkinText = GetDlgItem(hwndDlg, IDC_SKNFILE);
			hKeyText = GetDlgItem(hwndDlg, IDC_KEYFILE);
			BOOL CustomSkinSetting = lpMainWindow->bCustomSkin;
			SkinOptionsToggleCustomSkin(hwndDlg, CustomSkinSetting);
			Button_SetCheck(chkCutout, lpMainWindow->bCutout);
			Button_SetCheck(chkAlphaBlend, lpMainWindow->bAlphaBlendLCD);
			Button_SetCheck(chkCustom, lpMainWindow->bCustomSkin);
			Button_SetCheck(chkFaceColor, lpMainWindow->bUseCustomFaceplateColor);
			EnableWindow(chkFaceColor, lpMainWindow->bCustomSkin);
			SendMessage(hColorSelect, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) LoadBitmap(g_hInst, _T("SkinPicker")));
			backupFaceplate = lpMainWindow->m_FaceplateColor;
			Edit_SetText(hSkinText, lpMainWindow->skin_path);
			Edit_SetText(hKeyText, lpMainWindow->keymap_path);
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
								_T("Browse for custom skin"), _T("png"), 0, 1)) {
									Edit_SetText(hSkinText, lpStrFile);
							}
							break;
						}
						case IDC_BROWSEKEY: {
							TCHAR lpStrFile[MAX_PATH];
							if (!BrowseFile(lpStrFile, _T("Image Files (*.bmp;*.jpg;*.png;*.tiff)\0*.bmp;*.jpg;*.png;*.tiff\0	All Files (*.*)\0*.*\0\0"),
								_T("Browse for custom keymap"), _T("png"), 0, 1)) {
									Edit_SetText(hKeyText, lpStrFile);
							}
							break;
						}
						case IDC_CHKALPHABLEND:
						case IDC_CHKCUTOUT:
							break;
						case IDC_CHKFACECOLOR:
							lpMainWindow->bUseCustomFaceplateColor = Button_GetCheck(chkFaceColor);
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
							ScreenToClient(hColorSelect, &ptCursor);
							COLORREF selectedColor = GetPixel(hColorPicker, ptCursor.x, ptCursor.y);
							lpMainWindow->m_FaceplateColor = selectedColor;
							gui_frame_update(lpMainWindow);
							SetFocus(hwndDlg);
							break;
						}
						default:
							return FALSE;
					}
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
					break;
			}
			return TRUE;
		}
		case WM_NOTIFY: {
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_RESET: {
					lpMainWindow->m_FaceplateColor = backupFaceplate;
					gui_frame_update(lpMainWindow);
					return TRUE;
				}
				case PSN_APPLY: {
					lpMainWindow->bCutout = Button_GetCheck(chkCutout);
					lpMainWindow->bAlphaBlendLCD = Button_GetCheck(chkAlphaBlend);
					lpMainWindow->bCustomSkin = Button_GetCheck(chkCustom);

					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);
					backupFaceplate = lpMainWindow->m_FaceplateColor;

					TCHAR lpStrFile[MAX_PATH];
					Edit_GetText(hSkinText, lpStrFile, sizeof(lpStrFile));
					StringCbCopy(lpMainWindow->skin_path, sizeof(lpMainWindow->skin_path), lpStrFile);
					Edit_GetText(hKeyText, lpStrFile, sizeof(lpStrFile));
					StringCbCopy(lpMainWindow->keymap_path, sizeof(lpMainWindow->skin_path), lpStrFile);
					if (gui_frame_update(lpMainWindow) == FALSE) {
						Button_SetCheck(chkCustom, lpMainWindow->bCustomSkin);
					}
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
	static HWND saveState_check, loadFiles_check, doBackups_check, alwaysTop_check, exeViolation_check,
		backupTime_edit, invalidFlash_check, turnOn_check, tiosDebug_check, checkUpdates_check,
		showWhatsNew_check, portableMode_check;
	switch (Message) {
		case WM_INITDIALOG: {
			saveState_check = GetDlgItem(hwnd, IDC_CHK_SAVE);
			loadFiles_check = GetDlgItem(hwnd, IDC_CHK_LOADFILES);
			doBackups_check = GetDlgItem(hwnd, IDC_CHK_REWINDING);
			alwaysTop_check = GetDlgItem(hwnd, IDC_CHK_ONTOP);
			portableMode_check = GetDlgItem(hwnd, IDC_CHK_PORTABLE);
			exeViolation_check = GetDlgItem(hwnd, IDC_CHK_BRK_EXE_VIOLATION);
			backupTime_edit = GetDlgItem(hwnd, IDC_EDT_BACKUPTIME);
			invalidFlash_check = GetDlgItem(hwnd, IDC_CHK_BRK_INVALID_FLASH);
			turnOn_check = GetDlgItem(hwnd, IDC_CHK_AUTOON);
			tiosDebug_check = GetDlgItem(hwnd, IDC_CHK_TIOS_DEBUG);
			checkUpdates_check = GetDlgItem(hwnd, IDC_CHK_UPDATES);
			showWhatsNew_check = GetDlgItem(hwnd, IDC_CHK_SHOWWHATSNEW);
			// disable option for now
			EnableWindow(doBackups_check, FALSE);
			return SendMessage(hwnd, WM_USER, 0, 0);
		}
		case WM_COMMAND: {
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:
					PropSheet_Changed(GetParent(hwnd), hwnd);
					return TRUE;
				case BN_CLICKED:
					switch(LOWORD(wParam)) {
						case IDC_CHK_SAVE:
						case IDC_CHK_LOADFILES:
						case IDC_CHK_REWINDING:
						case IDC_CHK_BRK_EXE_VIOLATION:
						case IDC_CHK_BRK_INVALID_FLASH:
						case IDC_CHK_AUTOON:
						case IDC_CHK_TIOS_DEBUG:
						case IDC_CHK_UPDATES:
						case IDC_CHK_SHOWWHATSNEW:
						case IDC_CHK_PORTABLE:
							break;
					}
					PropSheet_Changed(GetParent(hwnd), hwnd);
					return FALSE;
			}
			return TRUE;
		}

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {
					show_whats_new = Button_GetCheck(showWhatsNew_check);
					exit_save_state = Button_GetCheck(saveState_check);
					new_calc_on_load_files = Button_GetCheck(loadFiles_check);
					break_on_exe_violation = Button_GetCheck(exeViolation_check);
					break_on_invalid_flash = Button_GetCheck(invalidFlash_check);
					auto_turn_on = Button_GetCheck(turnOn_check);
					portable_mode = Button_GetCheck(portableMode_check);
					TCHAR buf[256];
					Edit_GetText(backupTime_edit, buf, ARRAYSIZE(buf));
					double persec = _ttof(buf);
					if (persec == 0.0)
						persec = 50.0;

					lpMainWindow->bAlwaysOnTop = Button_GetCheck(alwaysTop_check);
					lpMainWindow->bTIOSDebug = !Button_GetCheck(tiosDebug_check);
					check_updates = Button_GetCheck(checkUpdates_check);
					SendMessage(hwnd, WM_USER, 0, 0);

					//we need to persist this immediately
					if (portable_mode) {
						SaveRegistrySettings(lpMainWindow, lpCalc);
					} else {
						SaveWabbitKey(_T("load_files_first"), REG_DWORD, &new_calc_on_load_files);
					}
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			break;
		case WM_USER: {
			Button_SetCheck(showWhatsNew_check, show_whats_new);
			Button_SetCheck(saveState_check, exit_save_state);
			Button_SetCheck(loadFiles_check, new_calc_on_load_files);
			Button_SetCheck(alwaysTop_check, lpMainWindow->bAlwaysOnTop);
			Button_SetCheck(portableMode_check, portable_mode);
			Button_SetCheck(exeViolation_check, break_on_exe_violation);
			Button_SetCheck(invalidFlash_check, break_on_invalid_flash);
			Button_SetCheck(turnOn_check, auto_turn_on);
			Button_SetCheck(tiosDebug_check, !lpMainWindow->bTIOSDebug);
			Button_SetCheck(checkUpdates_check, check_updates);
			return TRUE;
		}
		case WM_DESTROY:
			GetWindowRect(GetParent(hwnd), &PropRect);
			break;
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

			int size = lpCalc->model >= TI_84PCSE ? screenshot_color_size : screenshot_size;

			int speedPos = 0;
			if (gif_base_delay_start != 0) {
				speedPos = ((100 / gif_base_delay_start) - 9) / TBRSTEP;
			}

			SendMessage(hwndSpeed, TBM_SETPOS, TRUE, speedPos);

			int delayMin = (100 / (9 + (TBRTICS * TBRSTEP)));
			int fpsMax = 100 / delayMin;
			HWND hwndMaxSpeed = GetDlgItem(hwndDlg, IDC_STCGIFMAX);
			TCHAR lpszMax[10];
			StringCbPrintf(lpszMax, sizeof(lpszMax), _T("%d"), fpsMax);
			SetWindowText(hwndMaxSpeed, lpszMax);

			chkAutosave = GetDlgItem(hwndDlg, IDC_CHKENABLEAUTOSAVE);
			Button_SetCheck(chkAutosave, screenshot_autosave);
			GIFOptionsToggleAutosave(hwndDlg, screenshot_autosave);

			edtGIFFilename = GetDlgItem(hwndDlg, IDC_EDTGIFFILENAME);
			Edit_SetText(edtGIFFilename, screenshot_file_name);

			chkUseIncreasing = GetDlgItem(hwndDlg, IDC_CHKUSEINCREASING);
			Button_SetCheck(chkUseIncreasing, screenshot_use_increasing);

			rbnScreen = GetDlgItem(hwndDlg, IDC_RBNSCREEN);
			rbnGray = GetDlgItem(hwndDlg, IDC_RBNGRAYSCALE);
			Button_SetCheck(gif_bw ? rbnGray : rbnScreen, BST_CHECKED);

			chkSize = GetDlgItem(hwndDlg, IDC_CHKGIF2X);
			Button_SetCheck(chkSize, (size == 2) ? BST_CHECKED : BST_UNCHECKED);
			return TRUE;
		}
		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {

					int speedPos = (int) SendMessage(hwndSpeed, TBM_GETPOS, 0, 0);

					if (gif_write_state == GIF_IDLE) {
						gif_base_delay_start = 	(100 / (9 + (speedPos * TBRSTEP)));
					}

					screenshot_autosave = Button_GetState(chkAutosave) & 0x0003 ? TRUE : FALSE;
					screenshot_use_increasing = Button_GetState(chkUseIncreasing) & 0x0003 ? TRUE : FALSE;
					gif_bw = Button_GetCheck(rbnGray);

					SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);

					int size = Button_GetState(chkSize) & 0x0003 ? 2 : 1;
					if (lpCalc->model >= TI_84PCSE) {
						screenshot_color_size = size;
					} else {
						screenshot_size = size;
					}

					Edit_GetText(edtGIFFilename, screenshot_file_name, ARRAYSIZE(screenshot_file_name));
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
							BrowseFile(screenshot_file_name, _T("Graphics Interchange Format  (*.gif)\0*.gif\0All Files (*.*)\0*.*\0\0"),
								_T("Wabbitemu GIF File Target"), _T("gif"), 0, 1);
							Edit_SetText(edtGIFFilename, screenshot_file_name);
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
		case WM_DESTROY:
			GetWindowRect(GetParent(hwndDlg), &PropRect);
			break;
	}

	return FALSE;
}

INT_PTR CALLBACK ROMOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND edtRom_path, edtRom_version, edtRom_model, edtRom_size, stcRom_image, saveState_check,
			ramPages_check, edtLCD_delay, old83p_check, extraSpeed_check;
	static HBITMAP hbmTI83P = NULL;
	switch (Message) {
		case WM_INITDIALOG: {
			edtRom_path = GetDlgItem(hwnd, IDC_EDTROMPATH);
			edtRom_version = GetDlgItem(hwnd, IDC_EDTROMVERSION);
			edtRom_model = GetDlgItem(hwnd, IDC_EDTROMMODEL);
			edtRom_size = GetDlgItem(hwnd, IDC_EDTROMSIZE);
			stcRom_image = GetDlgItem(hwnd, IDC_STCROMIMAGE);
			saveState_check = GetDlgItem(hwnd, IDC_CHKSAVE);
			ramPages_check = GetDlgItem(hwnd, IDC_CHK_RAMPAGES);
			extraSpeed_check = GetDlgItem(hwnd, IDC_CHK_EXTRASPEED);
			edtLCD_delay = GetDlgItem(hwnd, IDC_EDT_LCDDELAY);
			old83p_check = GetDlgItem(hwnd, IDC_CHK_83P_OLD);

			return SendMessage(hwnd, WM_USER, 0, 0);
		}
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case IDC_BTNROMBROWSE: {
							TCHAR lpszFile[MAX_PATH] = _T("\0");
							const TCHAR lpstrFilter[] = _T("Calculator ROM  (*.rom, *.bin)\0*.rom;*.bin\0\
															All Files (*.*)\0*.*\0\0");
							const TCHAR lpstrTitle[] = _T("Wabbitemu Load ROM");
							const TCHAR lpstrDefExt[] = _T("rom");
							
							if (!BrowseFile(lpszFile, lpstrFilter, lpstrTitle, lpstrDefExt, OFN_PATHMUSTEXIST, 1)) {
								StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), lpszFile);
								SendMessage(hwnd, WM_USER, 0, 0);
							}
							break;
						}
						case IDC_BTN1: {
							TCHAR lpszFile[MAX_PATH];
							if (!SaveFile(lpszFile, _T("ROMs  (*.rom)\0*.rom\0Bins  (*.bin)\0*.bin\0All Files (*.*)\0*.*\0\0"),
											_T("Wabbitemu Export Rom"), _T("rom"), OFN_PATHMUSTEXIST, 1)) {
								MFILE *file = ExportRom(lpszFile, lpCalc);
								mclose(file);
							}
							break;
						}
						case IDC_EDT_LCDDELAY:
						case IDC_CHK_RAMPAGES:
						case IDC_CHKSAVE:
							PropSheet_Changed(GetParent(hwnd), hwnd);
							break;
					}
			}
			return TRUE;

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {
					TCHAR buf[64];
					u_int delay;
					exit_save_state = Button_GetCheck(saveState_check);
					lpCalc->cpu.mem_c->ram_version = Button_GetCheck(ramPages_check) ? 2 : 0;
					lpCalc->cpu.cpu_version = Button_GetCheck(old83p_check) ? 1 : 0;
					lpCalc->cpu.timer_c->timer_version = Button_GetCheck(extraSpeed_check) ? 1 : 0;
					Edit_GetText(edtLCD_delay,buf, ARRAYSIZE(buf));
					delay = _ttoi(buf);
					if (delay != 0) {
						if (lpCalc->model < TI_84PCSE) {
							LCD_t *lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
							lcd->lcd_delay = delay;
						}
					}
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			break;

		// Update all of the ROM attributes
		case WM_USER: {
			TCHAR buf[64];
			int delay = 0;
			if (lpCalc->model < TI_84PCSE) {
				LCD_t *lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
				delay = lcd->lcd_delay;
				EnableWindow(edtLCD_delay, TRUE);
			} else {
				EnableWindow(edtLCD_delay, FALSE);
			}
			StringCbPrintf(buf, sizeof(buf), _T("%i"), delay);
			Button_SetCheck(saveState_check, exit_save_state);
			Edit_SetText(edtRom_path, lpCalc->rom_path);
			Edit_SetText(edtLCD_delay, buf);
#ifdef _UNICODE
			TCHAR szRomVersion[256];
			MultiByteToWideChar(CP_ACP, 0, lpCalc->rom_version, -1, szRomVersion, ARRAYSIZE(szRomVersion));
			Edit_SetText(edtRom_version, szRomVersion);
#else
			Edit_SetText(edtRom_version, lpCalc->rom_version);
#endif
			Edit_SetText(edtRom_model, calc_get_model_string(lpCalc->model));
			Button_SetCheck(extraSpeed_check, lpCalc->cpu.timer_c->timer_version > 1);
			Button_SetCheck(ramPages_check, lpCalc->cpu.mem_c->ram_version == 2);
			Button_SetCheck(old83p_check, lpCalc->cpu.cpu_version);
			TCHAR szRomSize[16];
			StringCbPrintf(szRomSize, sizeof(szRomSize), _T("%0.1f KB"), (float) lpCalc->cpu.mem_c->flash_size / 1024.0f);
			Edit_SetText(edtRom_size, szRomSize);
			if (hbmTI83P) {
				DeleteObject(hbmTI83P);
				}
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
		case WM_DESTROY:
			GetWindowRect(GetParent(hwnd), &PropRect);
			break;
	}
	return FALSE;
}

TCHAR* NameFromVKey(UINT nVK);

static TCHAR keyPressBuf[64];
static UINT editKeyPressed;
LRESULT CALLBACK EmuKeyHandleProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
	switch (uMsg) {
		case WM_KEYDOWN: {
			editKeyPressed = (int) wParam;
			if (editKeyPressed == VK_SHIFT) {
				if (GetKeyState(VK_LSHIFT) & 0xFF00) {
					editKeyPressed = VK_LSHIFT;
				} else {
					editKeyPressed = VK_RSHIFT;
				}
			}
			if (editKeyPressed == VK_CONTROL) {
				if (GetKeyState(VK_LCONTROL) & 0xFF00) {
					editKeyPressed = VK_LCONTROL;
				} else {
					editKeyPressed = VK_RCONTROL;
				}
			}
			TCHAR *string = NameFromVKey(editKeyPressed);
			StringCbCopy(keyPressBuf, sizeof(keyPressBuf), string);
			SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(0, EN_CHANGE), (LPARAM) hwnd);
			return TRUE;
		}
	}
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void ChangeCommand(keyprog_t *, int);
TCHAR* NameFromAccel(ACCEL);
int GetNumKeyEntries(keyprog_t *);
void AssignAccel(HWND);
void RemoveAccel();
void AssignEmuKey(HWND, keyprog_t *);
void RemoveEmuKey(keyprog_t *);
void AddNormalKeys(key_string_t keystrings[KEY_STRING_SIZE]);

extern keyprog_t keygrps[256];
extern keyprog_t keysti83[256];
extern keyprog_t keysti86[256];
extern keyprog_t defaultkeysti83[256];
extern keyprog_t defaultkeysti86[256];
keyprog_t *defaultkeys, *userkeys;
ACCEL hNewAccels[256];		// working copy
HWND hListMenu, hHotKey, hEmuKey, hListKeys, hAssignButton, hRemoveButton, hResetButton, hComboBox;
HMENU hMenu;
int m_nCommands; 			// total commands listed (all categories)
int nAccelStore, nAccelUsed;		// how many ACCELs allocated and how many used already
int nEmuStore, nEmuUsed; 			// how many ACCELs allocated and how many used already
TCHAR *m_sCtrl, *m_sAlt, *m_sShift;
int m_nInitialLen;
DWORD m_dwCheckSum; 		// trivia for initial table
static int cur_sel;
BOOL accelerators = TRUE;	//true if were editing the accelerators
TCHAR menu_text_buf[256];
key_string_t **emu_strings;
INT_PTR CALLBACK KeysOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static keyprog_t newkeys[256];
	switch(Message) {
		case WM_INITDIALOG: {

			


			if (lpCalc->model == TI_85 || lpCalc->model == TI_86) {
				userkeys = keysti86;
				defaultkeys = defaultkeysti86;
			} else {
				userkeys = keysti83;
				defaultkeys = defaultkeysti83;
			}

			memcpy(newkeys, userkeys, 256 *sizeof(keyprog_t));
			int i;
			hComboBox = GetDlgItem(hwnd, IDC_COMBO_MENU);
			hMenu = LoadMenu(g_hInst, (LPCTSTR) IDR_MAIN_MENU);
			hListMenu = GetDlgItem(hwnd, IDC_LIST_MENU);
			hListKeys = GetDlgItem(hwnd, IDC_LIST_KEYS);
			hHotKey = GetDlgItem(hwnd, IDC_HOTKEY);
			hEmuKey = GetDlgItem(hwnd, IDC_EDT_KEY);
			hAssignButton = GetDlgItem(hwnd, IDC_ASSIGN_ACCEL);
			hRemoveButton = GetDlgItem(hwnd, IDC_REMOVE_ACCEL);
			hResetButton = GetDlgItem(hwnd, IDC_RESET_ACCEL);
			int numEntries = CopyAcceleratorTable(haccelmain, NULL, 0);
			nAccelUsed = CopyAcceleratorTable(haccelmain, hNewAccels, numEntries);
			nEmuUsed = GetNumKeyEntries(newkeys);
			nAccelStore = 256;
			nEmuStore = 256;

			int count = GetMenuItemCount(hMenu);
			for (i = 0; i < count; i++) {
				TCHAR *string = (TCHAR *) GetFriendlyMenuText(hMenu, i, 0);
				ComboBox_AddString(hComboBox, string);
			}
			ComboBox_AddString(hComboBox, _T("Emulator"));
			ComboBox_SetCurSel(hComboBox, 0);
			SendMessage(hwnd, WM_COMMAND, CBN_SELCHANGE << 16, 0);
			SetWindowSubclass(hEmuKey, EmuKeyHandleProc, 0, 0);
			return TRUE;
		}
		case WM_COMMAND: {
			switch(HIWORD(wParam)) {
				case EN_CHANGE: {
					static BOOL changingKey = FALSE;
					BOOL enable;
					if (hEmuKey == ((HWND) lParam)) {
						enable = _tcslen(keyPressBuf) > 0;
						if (!changingKey) {
							changingKey = TRUE;
							Edit_SetText(hEmuKey, keyPressBuf);
							changingKey = FALSE;
						}
					} else {
						enable = (BOOL) SendMessage(hHotKey, HKM_GETHOTKEY, 0 , 0);
					}
					EnableWindow(hAssignButton, enable);
					break;
				}
				case CBN_SELCHANGE: {
					HWND hSender = (HWND) lParam;
					if (hSender == hListKeys) {
						int sel = ListBox_GetCurSel(hListKeys);
						EnableWindow(hRemoveButton, sel >= 0);
					} else {
						ChangeMenuCommands(hSender);
					}
					break;
				}
				case BN_CLICKED: {
					HWND hSender = (HWND) lParam;
					if (hSender == hAssignButton) {
						if (accelerators) {
							AssignAccel(hwnd);
						} else {
							AssignEmuKey(hwnd, newkeys);
						}
					} else if (hSender == hRemoveButton) {
						if (accelerators) {
							RemoveAccel();
						} else {
							RemoveEmuKey(newkeys);
						}
					} else if (hSender == hResetButton) {
						//reload whatever is packed in with us
						HACCEL hAccelNew = LoadAccelerators(g_hInst, _T("Z80Accel"));
						int numEntries = CopyAcceleratorTable(hAccelNew, NULL, 0);
						nAccelUsed = CopyAcceleratorTable(hAccelNew, hNewAccels, numEntries);
						DestroyAcceleratorTable(hAccelNew);
						//reload normal emu keys
						memcpy(newkeys, defaultkeys, sizeof(keyprog_t) * 256);
						ChangeCommand(newkeys, 256);
					}
					PropSheet_Changed(GetParent(hwnd), hwnd);
					break;
				}
			}
			break;
		}
		case WM_NOTIFY: {
			switch(((LPNMHDR)lParam)->code) {
				case PSN_RESET:
					break;
				case LVN_ITEMCHANGED: {
					ChangeCommand(newkeys, 256);
					break;
				}
				case PSN_APPLY: {
					DestroyAcceleratorTable(haccelmain);
					haccelmain = CreateAcceleratorTable(hNewAccels, nAccelUsed);
					memcpy(userkeys, newkeys, sizeof(keyprog_t) * 256);
					memcpy(keygrps, newkeys, sizeof(keyprog_t) * 256);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLongPtr(hwnd, DWLP_MSGRESULT, FALSE);
					return TRUE;
			}
			break;
		}
		case WM_DESTROY:
			GetWindowRect(GetParent(hwnd), &PropRect);
			break;
	}
	return DefWindowProc(hwnd, Message, wParam, lParam);
}

int GetNumKeyEntries(keyprog_t *keys) {
	int num_entries = 0;
	for (int i = 0; i < 256 && keys[i].vk != -1; i++) {
		if (keys[i].vk != 0) {
			num_entries++;
		}
	}
	return num_entries;
}

void AssignEmuKey(HWND hwnd, keyprog_t *keys) {
	int i;
	UINT key = editKeyPressed;
	int active = ListView_GetNextItem(hListMenu, -1, LVNI_SELECTED);
	if(active >= 0) {
		LVITEM lvi;
		lvi.iItem = active;
		lvi.mask = LVIF_PARAM;
		ListView_GetItem(hListMenu, &lvi);
		int newCmd = (int) lvi.lParam;
		for (i = 0; i < nEmuUsed; i++) {
			if(keys[i].vk == key) {
				break;
			}
		}
		if (i < nEmuUsed) {								// cheeky: could this be the same item being assigned a double key?
			if (LOWORD(newCmd) == keys[i].bit && HIWORD(newCmd) == keys[i].group && keys[i].vk == key)
				return; // no actions required
			// ask for shortcut overwrite confirmation
			if(MessageBox(hwnd, _T("This key is already in use.\nErase the old command assignment?"),
				_T("Overwrite?"), MB_ICONQUESTION | MB_YESNO) == IDNO)
				return;
			keys[i].bit = LOWORD(newCmd);
			keys[i].group = HIWORD(newCmd);
		} else if(nEmuUsed < nEmuStore) { // I'm sure capacity will never be reached
			keys[nEmuUsed].vk = key;
			keys[nEmuUsed].bit = LOWORD(newCmd);
			keys[nEmuUsed].group = HIWORD(newCmd);
			nEmuUsed++;
			keys[nEmuUsed].vk = 0;
		}
		ChangeCommand(keys, 256); // reflect changes
		// this key can only be clicked, and if it is default & disabled, the key navigation gets screwy
		// so make something else default item
		SetFocus(hHotKey);
	}
}

void RemoveEmuKey(keyprog_t *keys) {
	int idx = ListBox_GetCurSel(hListKeys);
	LRESULT lr = ListBox_GetItemData(hListKeys, idx);
	if (lr != LB_ERR) {
		int i;
		BYTE bit = (BYTE) LOWORD(lr);
		BYTE group = (BYTE) HIWORD(lr);
		TCHAR buf[64];
		ListBox_GetText(hListKeys, idx, buf);
		for(i = 0; i < nEmuUsed; i++)
			if (keys[i].bit == bit && keys[i].group == group) {
				TCHAR *name = NameFromVKey(keys[i].vk);
				BOOL sameKey = _tcscmp(buf, name);
				free(name);
				if (sameKey) {
					continue;
				}

				ListBox_DeleteString(hListKeys, idx);
				int last = --nEmuUsed;
				if (i < last) {
					keys[i] = keys[last];
				}

				keys[last].vk = 0;
				EnableWindow(hRemoveButton, FALSE);
				break;
			}
	}
}

void AssignAccel(HWND hwnd) {
	int i;
	WORD wVirtualKeyCode, wModifiers;
	DWORD key = (DWORD) SendMessage(hHotKey, HKM_GETHOTKEY, 0, 0);
	wVirtualKeyCode = LOBYTE(LOWORD(key));
	wModifiers = HIBYTE(LOWORD(key));
	int active = ListView_GetNextItem(hListMenu, -1, LVNI_SELECTED);
	if(wVirtualKeyCode && active >= 0) {
		//I don't really understand that: is this ALWAYS a virtual key? Looks like it!
		BYTE fVirt = (wModifiers & (HOTKEYF_SHIFT | HOTKEYF_CONTROL | HOTKEYF_ALT)) << 2;
		fVirt |= FVIRTKEY | FNOINVERT;							// various checks: valid range, not already assigned et cetera
		LVITEM lvi;
		lvi.iItem = active;
		lvi.mask = LVIF_PARAM;
		ListView_GetItem(hListMenu, &lvi);
		WORD newCmd = (WORD) lvi.lParam;
		for (i = 0; i < nAccelUsed; i++) {
			if (hNewAccels[i].fVirt == fVirt && hNewAccels[i].key == wVirtualKeyCode) {
				break;
			}
		}
		if (i < nAccelUsed) {								// cheeky: could this be the same item being assigned a double key?
			if (newCmd == hNewAccels[i].cmd) {
				return; // no actions required
			}
			// ask for shortcut overwrite confirmation
			if(MessageBox(hwnd, _T("This key combination is already in use.\nErase the old command assignment?"),
				_T("Overwrite?"), MB_ICONQUESTION | MB_YESNO) == IDNO) {
				return;
			}
			hNewAccels[i].cmd = newCmd;
		} else if (nAccelUsed < nAccelStore) { // I'm sure capacity will never be reached
			hNewAccels[nAccelUsed].fVirt = fVirt;
			hNewAccels[nAccelUsed].key = wVirtualKeyCode;
			hNewAccels[nAccelUsed].cmd = newCmd;
			nAccelUsed++;
		}

		HMENU hMenu = lpMainWindow->hMenu;
		int menuItem = ComboBox_GetCurSel(hComboBox);
		HMENU hSubMenu = GetSubMenu(hMenu, menuItem);
		TCHAR text[256];
		StringCbCopy(text, sizeof(text), GetFriendlyMenuText(hSubMenu, active, 0));
		StringCbCat(text, sizeof(text), _T("\t"));
		StringCbCat(text, sizeof(text), NameFromAccel(hNewAccels[i]));
		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = text;
		SetMenuItemInfo(hSubMenu, active, TRUE, &mii);
		ChangeCommand(NULL, 0); // reflect changes
		// this key can only be clicked, and if it is default & disabled, the key navigation gets screwy
		// so make something else default item
		SetFocus(hHotKey);
	}
}

void RemoveAccel() {
	int idx = ListBox_GetCurSel(hListKeys);
	LRESULT lr = ListBox_GetItemData(hListKeys, idx);
	int active = ListView_GetNextItem(hListMenu, -1, LVNI_SELECTED);
	if (lr != LB_ERR) {
		int i;
		BYTE fVirt = (BYTE) LOWORD(lr);
		WORD key = HIWORD(lr);
		for(i = 0; i < nAccelUsed; i++) {
			if (hNewAccels[i].fVirt == fVirt && hNewAccels[i].key == key) {
				ListBox_DeleteString(hListKeys, idx);
				// pack working accelerator table too
				int last = nAccelUsed-1;
				if (i < last) {
					hNewAccels[i] = hNewAccels[last];
				}
				nAccelUsed--;
				EnableWindow(hRemoveButton, FALSE);
				break;
			}
		}
	}

	HMENU hMenu = lpMainWindow->hMenu;
	int menuItem = ComboBox_GetCurSel(hComboBox);
	HMENU hSubMenu = GetSubMenu(hMenu, menuItem);
	TCHAR text[256];
	StringCbCopy(text, sizeof(text), GetFriendlyMenuText(hSubMenu, active, 0));
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;
	mii.dwTypeData = text;
	SetMenuItemInfo(hSubMenu, active, TRUE, &mii);
}

void ChangeMenuCommands(HWND hSender) {
	TCHAR buffer[256];
	LVITEM li;
	li.mask = LVIF_TEXT;
	li.iSubItem = 0;
	li.cchTextMax = 64;
	cur_sel = ComboBox_GetCurSel(hSender);
	HMENU hSubMenu = GetSubMenu(hMenu, cur_sel);
	ListView_DeleteAllItems(hListMenu);
	memset(buffer, 0, ARRAYSIZE(buffer));
	if (hSubMenu == NULL) {
		//assume that if we cant find the menu, then its the emulator keys
		AddNormalKeys((lpCalc->cpu.pio.model == TI_86 || lpCalc->cpu.pio.model == TI_85) ? ti86keystrings : ti83pkeystrings);
		accelerators = false;
	} else {
		RecurseAddItems(hSubMenu, (TCHAR *) &buffer);
		accelerators = true;
	}
	ShowWindow(hHotKey, accelerators);
	ShowWindow(hEmuKey, !accelerators);
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
		case VK_RCONTROL:
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

void ChangeCommand(keyprog_t *keys, int keysSize) {		// new command selected
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
		if (!accelerators) {
			for(i = 0; i < keysSize; i++) {
				if (keys[i].bit == LOWORD(cmd) && keys[i].group == HIWORD(cmd)) {
					TCHAR *name = NameFromVKey(keys[i].vk);
					idx = ListBox_AddString(hListKeys, name);
					free(name);
					lvi.lParam = cmd;
					ListView_SetItem(hListKeys, &lvi);
					ListBox_SetItemData(hListKeys, idx, cmd);
				}
			}
		} else {
			for(i = 0; i < nAccelUsed; i++) {
				if(hNewAccels[i].cmd == cmd) {
					name = (TCHAR *) NameFromAccel(hNewAccels[i]);
					idx = ListBox_AddString(hListKeys, name);
					free(name);
					lvi.lParam = MAKELPARAM(hNewAccels[i].fVirt, hNewAccels[i].key);
					ListView_SetItem(hListKeys, &lvi);
					ListBox_SetItemData(hListKeys, idx, MAKELPARAM(hNewAccels[i].fVirt, hNewAccels[i].key));
				}
			}
			EnableWindow(hAssignButton, FALSE);
		}
		ListBox_SetCurSel(hListKeys, LB_ERR);
	}
	SendMessage(hHotKey, HKM_SETHOTKEY, 0, 0);
	EnableWindow(hRemoveButton, FALSE);
}

TCHAR* NameFromAccel(ACCEL key) {
	TCHAR *name = (TCHAR *) malloc(80);
	memset(name, 0, 80);
	if(key.fVirt & FCONTROL)
		StringCbCat(name, 80, _T("Ctrl"));
	if(key.fVirt & FALT)
		StringCbCat(name, 80, _T("Alt"));
	if(key.fVirt & FSHIFT)
		StringCbCat(name, 80, _T("Shift"));
	// FNOINVERT is useless, backward compatibility
	if(key.fVirt & FVIRTKEY) {
		if (key.fVirt & (FSHIFT | FALT | FCONTROL))
			StringCbCat(name, 80, _T(" + "));
		TCHAR *temp = (TCHAR *) NameFromVKey(key.key);
		StringCbCat(name, 80, temp);
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

void AddNormalKeys(key_string_t keystrings[KEY_STRING_SIZE]) {
	LVITEM li; // lparam is command ID
	li.mask = LVIF_TEXT | LVIF_PARAM;
	li.iSubItem = 0;	// browse this menu checking for submenus
	int i;
	for (i = 0; i < KEY_STRING_SIZE; i++) {
		if (_tcscmp(keystrings[i].text, _T(""))) {
			li.pszText = (LPTSTR)(LPCTSTR) keystrings[i].text;
			li.iItem = ListView_GetItemCount(hListMenu);
			li.lParam = MAKELPARAM(keystrings[i].bit, keystrings[i].group);
			ListView_InsertItem(hListMenu, &li);
		}
	}
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
		GetMenuItemInfo(hMenu, i, TRUE, &mi); 		// by position
		if(!mi.wID) 								// separators excluded
			continue;
		name = (TCHAR *) GetFriendlyMenuText(hMenu, i, 0);
		StringCbCopy(temp, sizeof(temp), base);
		StringCbCat(temp, sizeof(temp), name);
		if(mi.hSubMenu) {
			StringCbCat(temp, sizeof(temp), _T(" > "));
			RecurseAddItems(mi.hSubMenu, temp);
		} else {
			li.pszText = (LPTSTR)(LPCTSTR)temp;
			li.iItem = ListView_GetItemCount(hListMenu);
			li.lParam = mi.wID;			// is this mixed icon/nie mode going to make list view funny?
			ListView_InsertItem(hListMenu, &li);
		}
	}
}

TCHAR* GetFriendlyMenuText(HMENU hMenu, int nItem, UINT uFlag) {
	TCHAR buf[256];
	uFlag = MF_BYPOSITION;
	GetMenuString(hMenu, nItem, buf, ARRAYSIZE(buf), uFlag);	// strip ampersands and tab characters
	TCHAR *str = menu_text_buf;
	TCHAR *start = str;
	int i = 0;
	for (;;) {
		TCHAR ch = buf[i++];
		if (ch == '&') {
			continue;
		} else if (ch == '\t') {
			*str++ = 0;
			break;
		} else {
			*str++ = ch; // multi-bytes not an issue
		}
		if (!ch) {
			break;
		}
	}	// finally remove any trailing dots (usually "...")
	str -= 2;
	while (*str == '.') {
		*str-- = 0;
		}
	return start;
}
