#include <windows.h>
#include <windowsx.h>
#include "guioptions.h"

#include "gif.h"
#include "rsrc.h"
#include "calc.h"
#include "displayoptionstest.h"
#include "lcd.h"
#include "guicutout.h"
#include "extcalc.h"

extern HINSTANCE g_hInst;
extern BITMAPINFO *bi;

RECT PropRect = {0, 0, 0, 0};
HWND hwndProp = NULL;
int PropPageLast = -1;

// We have to save a slot for the ROM info
// and skin, it differs per calc
static int SlotSave;

void DoPropertySheet(HWND hwndOwner) {

	if (hwndProp != NULL) {
		SwitchToThisWindow(hwndProp, TRUE);
		return;
	}

	PROPSHEETPAGE psp[4];
	PROPSHEETHEADER psh;

	psp[0].dwSize = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[0].hInstance = g_hInst;
	psp[0].pszTemplate = MAKEINTRESOURCE(IDD_GIFOPTIONS);
	psp[0].pszIcon = NULL;
	psp[0].pfnDlgProc = GIFOptionsProc;
	psp[0].pszTitle = "Screen Capture";
	psp[0].lParam = 0;
	psp[0].pfnCallback = NULL;

	psp[1].dwSize = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[1].hInstance = g_hInst;
	psp[1].pszTemplate = MAKEINTRESOURCE(IDD_DISPLAYOPTIONS);
	psp[1].pszIcon = NULL;
	psp[1].pfnDlgProc = DisplayOptionsProc;
	psp[1].pszTitle = "Display";
	psp[1].lParam = 0;
	psp[1].pfnCallback = NULL;

	psp[2].dwSize = sizeof(PROPSHEETPAGE);
	psp[2].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[2].hInstance = g_hInst;
	psp[2].pszTemplate = MAKEINTRESOURCE(IDD_ROMOPTIONS);
	psp[2].pszIcon = NULL;
	psp[2].pfnDlgProc = ROMOptionsProc;
	psp[2].pszTitle = "ROM";
	psp[2].lParam = 0;
	psp[2].pfnCallback = NULL;

	psp[3].dwSize = sizeof(PROPSHEETPAGE);
	psp[3].dwFlags = PSP_USEICONID | PSP_USETITLE;
	psp[3].hInstance = g_hInst;
	psp[3].pszTemplate = MAKEINTRESOURCE(IDD_SKINOPTIONS);
	psp[3].pszIcon = NULL;
	psp[3].pfnDlgProc = SkinOptionsProc;
	psp[3].pszTitle = "Skin";
	psp[3].lParam = 0;
	psp[3].pfnCallback = NULL;

	psh.dwSize = sizeof(PROPSHEETHEADER);
	psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP | PSH_MODELESS;
	psh.hwndParent = hwndOwner;
	psh.hInstance = g_hInst;
	psh.pszIcon = NULL;
	psh.pszCaption = (LPSTR) "Wabbitemu Options";
	psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.ppsp = (LPCPROPSHEETPAGE) &psp;
	psh.pfnCallback = NULL;
	hwndProp = (HWND) PropertySheet(&psh);

	if (IsRectEmpty(&PropRect) == FALSE) {
		SetWindowPos(hwndProp, NULL,
				PropRect.left, PropRect.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}

	printf("PropPageLast: %d\n", PropPageLast);
	if (PropPageLast != -1) {
		PropSheet_SetCurSel(hwndProp, NULL, PropPageLast);
	}

	SlotSave = gslot;
	return;
}


static HWND imgDisplayPreview;
static double displayFPS = 48.0f;

DWORD WINAPI ThreadDisplayPreview( LPVOID lpParam ) {
	CPU_t *cpu = (CPU_t *) lpParam;
	double Time = 0.0f;
	int i;
	clock_t last_time = clock() - (1000/displayFPS);
	clock_t difference = 0;
	for (;;) {
		if (cpu->pio.lcd != calcs[gslot].cpu.pio.lcd) {
			u_char *buffer;
			switch (cpu->imode) {
			case 0: buffer = displayoptionstest_draw_bounce(4,displayFPS,Time); break;
			case 1: buffer = displayoptionstest_draw_scroll(4,displayFPS,Time); break;
			case 2: buffer = displayoptionstest_draw_gradient(displayFPS/10.0f,displayFPS,Time);
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
		clock_t displayTPF = 1000/displayFPS;
		if (cpu->imode == 2) displayTPF = 1000/70.0f;
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

			cpu.pio.devices[0x11] = (device_t) {TRUE, NULL, lcd, (devp) &LCD_data};

			cpu.pio.lcd = lcd;

			cbMode = GetDlgItem(hwndDlg, IDC_CBODISPLAYMODE);
			imgDisplayPreview = GetDlgItem(hwndDlg, IDC_IMGDISPLAYPREVIEW);

			SendMessage(cbMode, CB_ADDSTRING,  0, (LPARAM) "Perfect gray");
			SendMessage(cbMode, CB_ADDSTRING,  0, (LPARAM) "Steady freq");
			SendMessage(cbMode, CB_ADDSTRING,  0, (LPARAM) "Game gray");
			if (lcd->mode == MODE_PERFECT_GRAY)
				SendMessage(cbMode, CB_SETCURSEL, 0, (LPARAM) lcd->mode);
			else if (lcd->mode == MODE_STEADY)
				SendMessage(cbMode, CB_SETCURSEL, 1, (LPARAM) lcd->mode);
			else
				SendMessage(cbMode, CB_SETCURSEL, 2, (LPARAM) lcd->mode);


			cbSource = GetDlgItem(hwndDlg, IDC_CBODISPLAYSOURCE);
			SendMessage(cbSource, CB_ADDSTRING, 0, (LPARAM) "Bounce");
			SendMessage(cbSource, CB_ADDSTRING, 0, (LPARAM) "Scroll");
			SendMessage(cbSource, CB_ADDSTRING, 0, (LPARAM) "Gradient");
			SendMessage(cbSource, CB_ADDSTRING, 0 ,(LPARAM) "Live");
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
			SendMessage(trbSteady, TBM_SETPOS, TRUE, 1.0 / lcd->steady_frame);
			EnableWindow(trbSteady, lcd->mode == MODE_STEADY);

			trbFPS = GetDlgItem(hwndDlg, IDC_TRBDISPLAYFPS);
			SendMessage(trbFPS, TBM_SETRANGE,
						    (WPARAM) TRUE,
						    (LPARAM) MAKELONG(20, 80));
			SendMessage(trbFPS, TBM_SETTICFREQ, 10, 0);
			SendMessage(trbFPS, TBM_SETPOS, TRUE, displayFPS);

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
					SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
					return TRUE;
			}
			break;
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:
					switch (LOWORD(wParam)) {
					case  IDC_CBODISPLAYMODE:
						lcd->mode = SendMessage(cbMode, CB_GETCURSEL, 0, 0);
						EnableWindow(trbSteady, lcd->mode == MODE_STEADY);
						PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
						return TRUE;
					case IDC_CBODISPLAYSOURCE: {
						static int last_index = 0;
						int index = SendMessage(cbSource, CB_GETCURSEL, 0, 0);
						BOOL bEnable = TRUE;
						HWND stcDisplayOption = GetDlgItem(hwndDlg, IDC_STCDISPLAYOPTION);
						switch (index) {
						case 2:
							if (last_index == 3) DupLCDConfig(lcd_old, lcd);
							lcd = lcd_old;
							SendMessage(stcDisplayOption, WM_SETTEXT, 0, (LPARAM) "Levels");
							break;
						case 0:
						case 1:
							SendMessage(stcDisplayOption, WM_SETTEXT, 0, (LPARAM) "FPS");
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
				lcd->shades = SendMessage(trbShades, TBM_GETPOS, 0, 0) - 1;
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;
			}
			if ((HWND) lParam == trbSteady) {
				lcd->steady_frame = 1.0 / SendMessage(trbSteady, TBM_GETPOS, 0, 0);
				PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
				return TRUE;
			}
			if ((HWND) lParam == trbFPS) {
				displayFPS = SendMessage(trbFPS, TBM_GETPOS, 0, 0);
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
	hwndStc 	= GetDlgItem(hwndDlg, IDC_STCAUTOSAVE),
	hwndBtn		= GetDlgItem(hwndDlg, IDC_BTNGIFBROWSE);

	EnableWindow(hwndEdt, bEnable);
	EnableWindow(hwndChk, bEnable);
	EnableWindow(hwndStc, bEnable);
	EnableWindow(hwndBtn, bEnable);
}

void SkinOptionsToggleCustomSkin(HWND hwndDlg, BOOL bEnable)
{
	HWND
	hBrowseSkin = GetDlgItem(hwndDlg, IDC_BROWSESKIN),
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

	char lpstrFilter[] 	= "\
Graphics Interchange Format  (*.gif)\0*.gif\0\
All Files (*.*)\0*.*\0\0";
	char lpstrFile[MAX_PATH];
	unsigned int Flags = 0;

	if (bSave) Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

	int i;
	for (i = strlen(gif_file_name)-1; i && gif_file_name[i] != '\\'; i--);

	if (i) {
		strcpy(lpstrFile, gif_file_name + i + 1);
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
	ofn.lpstrTitle			= "Wabbitemu GIF File Target";
	ofn.Flags				= Flags | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES;
	ofn.lpstrDefExt			= "gif";
	ofn.lCustData			= 0;
	ofn.lpfnHook			= NULL;
	ofn.lpTemplateName		= NULL;
	ofn.pvReserved			= NULL;
	ofn.dwReserved			= 0;
	ofn.FlagsEx				= 0;

	if (!GetSaveFileName(&ofn)) {
		return 1;
	}
	strcpy(gif_file_name, lpstrFile);
	return 0;
}

int BrowseBMPFile(char* lpstrFile[])
{
	OPENFILENAME ofn;

	char lpstrFilter[] 	= "\
	BMP  (*.bmp)\0*.bmp\0\
	All Files (*.*)\0*.*\0\0";

	unsigned int Flags = 0;

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
	ofn.lpstrTitle			= "Wabbitemu Open Bitmap";
	ofn.Flags				= Flags | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES;
	ofn.lpstrDefExt			= "bmp";
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
	static HWND chkCutout, chkCustom, hColorSelect, hBrowseSkin, hBrowseKey, hSkinText, hKeyText;
	switch (Message) {
		case WM_INITDIALOG:
			chkCutout = GetDlgItem(hwndDlg, IDC_CHKCUTOUT);
			chkCustom = GetDlgItem(hwndDlg, IDC_CHKCSTMSKIN);
			hColorSelect = GetDlgItem(hwndDlg, IDC_COLORPICK);
			hBrowseSkin = GetDlgItem(hwndDlg, IDC_BROWSESKIN);
			hBrowseKey = GetDlgItem(hwndDlg, IDC_BROWSEKEY);
			hSkinText = GetDlgItem(hwndDlg, IDC_SKNFILE);
			hKeyText = GetDlgItem(hwndDlg, IDC_KEYFILE);

			BOOL CustomSkinSetting = calcs[SlotSave].bCustomSkin;
			SkinOptionsToggleCustomSkin(hwndDlg, CustomSkinSetting);

			SendMessage(chkCutout, BM_SETCHECK, calcs[SlotSave].bCutout, 0);
			SendMessage(hColorSelect, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) LoadBitmap(g_hInst, "SkinPicker"));
			return 0;
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case CBN_SELCHANGE:
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
					return TRUE;
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
					case IDC_BROWSESKIN:
					case IDC_BROWSEKEY:
					{
						//char lpstrFile[MAX_PATH];
						//BrowseBMPFile(&lpstrFile);
					}
					case IDC_CHKCSTMSKIN:
					{
						BOOL customSkinSetting;
						customSkinSetting = SendMessage(chkCustom, BM_GETCHECK, 0, 0);
						SkinOptionsToggleCustomSkin(hwndDlg, customSkinSetting);
						break;
					}
					case IDC_COLORPICK:
					{
						HDC hColorPicker = GetDC(hColorSelect);
						POINT ptCursor;
						GetCursorPos(&ptCursor);
						RECT rc;
						GetWindowRect(hColorSelect, &rc);
						ptCursor.x -= rc.left;
						ptCursor.y -= rc.top;
						COLORREF selectedColor = GetPixel(hColorPicker, ptCursor.x, ptCursor.y);
						calcs[SlotSave].FaceplateColor = selectedColor;
						gui_frame_update(SlotSave);

					}
						default:
							return FALSE;
					}
					PropSheet_Changed(GetParent(hwndDlg), hwndDlg);
					break;
				}
			break;
		case WM_NOTIFY:

    		switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {

					BOOL chkState =
					SendMessage(chkCutout, BM_GETSTATE, 0, 0) & 0x0003 ?
						TRUE : FALSE;

					if (chkState != calcs[SlotSave].bCutout) {
						calcs[SlotSave].bCutout = chkState;
						gui_frame_update(SlotSave);

						/*if (calcs[gslot].bCutout) {
							EnableCutout(calcs[SlotSave].hwndFrame, NULL);
						} else {
							DisableCutout(calcs[SlotSave].hwndFrame);
						}*/
					}
					SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
					return TRUE;
			}
			return TRUE;
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
			 	speedPos = ((100/gif_base_delay_start)-9)/TBRSTEP;
			}

			SendMessage(hwndSpeed, TBM_SETPOS, TRUE, speedPos);

			int delayMin = (100/(9+(TBRTICS * TBRSTEP)));
			int fpsMax = 100/delayMin;
			HWND hwndMaxSpeed = GetDlgItem(hwndDlg, IDC_STCGIFMAX);
			TCHAR lpszMax[10];
			sprintf(lpszMax, "%d", fpsMax);
			SendMessage(hwndMaxSpeed, WM_SETTEXT, 0, (LPARAM) lpszMax);


			chkAutosave = GetDlgItem(hwndDlg, IDC_CHKENABLEAUTOSAVE);
			SendMessage(chkAutosave, BM_SETCHECK, gif_autosave, 0);
			GIFOptionsToggleAutosave(hwndDlg, gif_autosave);

			edtGIFFilename = GetDlgItem(hwndDlg, IDC_EDTGIFFILENAME);
			SendMessage(edtGIFFilename, WM_SETTEXT, 0, (LPARAM) gif_file_name);

			chkUseIncreasing = GetDlgItem(hwndDlg, IDC_CHKUSEINCREASING);
			SendMessage(chkUseIncreasing, BM_SETCHECK, gif_use_increasing, 0);

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

					int speedPos = SendMessage(hwndSpeed, TBM_GETPOS, 0, 0);
					//printf("spedpos: %d\n",speedPos);

					if (gif_write_state == GIF_IDLE) {
						gif_base_delay_start = 	//GIF_MINIMUM + (GIF_MAXIMUM=GIF_MINIMUM)/6
												(100/(9+(speedPos * TBRSTEP)));
					}
					//printf("gifbasedelay: %d\n",gif_base_delay_start);

					gif_autosave =
					SendMessage(chkAutosave, BM_GETSTATE, 0, 0) & 0x0003 ?
						TRUE : FALSE;

					gif_use_increasing =
					SendMessage(chkUseIncreasing, BM_GETSTATE, 0, 0) & 0x0003 ?
						TRUE : FALSE;

					gif_bw = Button_GetCheck(rbnGray);

					SetWindowLong(hwndDlg, DWL_MSGRESULT, PSNRET_NOERROR);

					gif_size =
					SendMessage(chkSize, BM_GETSTATE, 0, 0) & 0x0003 ?
						2 : 1;

					Edit_GetText(edtGIFFilename, gif_file_name, sizeof(gif_file_name));
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
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
							if (SendMessage(chkAutosave, BM_GETSTATE, 0, 0) & 0x0003) {
								bEnable = TRUE;
							}
							GIFOptionsToggleAutosave(hwndDlg, bEnable);
							break;
						}
						case IDC_BTNGIFBROWSE:
							SetGifName(FALSE);
							SendMessage(edtGIFFilename, WM_SETTEXT, 0, (LPARAM) gif_file_name);
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



int GetROMName(char *lpstrFile) {
	OPENFILENAME ofn;

	char lpstrFilter[] 	= "\
Calculator ROM  (*.rom, *.bin)\0*.rom;*.bin\0\
All Files (*.*)\0*.*\0\0";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= GetForegroundWindow();
	ofn.lpstrFilter			= (LPCTSTR) lpstrFilter;
	ofn.lpstrFile			= lpstrFile;
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrTitle			= "Wabbitemu Load ROM";
	ofn.Flags				= OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_LONGNAMES;

	if (!GetOpenFileName(&ofn)) {
		return 1;
	}
	return 0;
}

int GetExportROMName(char *lpstrFile) {
	OPENFILENAME ofn;
	char lpstrFilter[] 	= "\
ROMS  (*.rom)\0*.rom\0\
BINS  (*.bin)\0*.bin\0\
All Files (*.*)\0*.*\0\0";

	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(lpstrFile, MAX_PATH);

	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= GetForegroundWindow();
	ofn.lpstrFilter		= (LPCTSTR) lpstrFilter;
	ofn.lpstrFile		= lpstrFile;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrTitle		= "Wabbitemu Export Rom";
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_EXPLORER |
						  OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt		= "rom";
	if (!GetSaveFileName(&ofn)) return 1;
	return 0;
}


INT_PTR CALLBACK ROMOptionsProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static HWND edtRom_path, edtRom_version, edtRom_model, edtRom_size, stcRom_image;
	static HBITMAP hbmTI83P = NULL;
	switch (Message) {
		case WM_INITDIALOG: {
			edtRom_path = GetDlgItem(hwnd, IDC_EDTROMPATH);
			edtRom_version = GetDlgItem(hwnd, IDC_EDTROMVERSION);
			edtRom_model = GetDlgItem(hwnd, IDC_EDTROMMODEL);
			edtRom_size = GetDlgItem(hwnd, IDC_EDTROMSIZE);
			stcRom_image = GetDlgItem(hwnd, IDC_STCROMIMAGE);

			return SendMessage(hwnd, WM_USER, 0, 0);
		}
		case WM_COMMAND:
			switch (HIWORD(wParam)) {
				case BN_CLICKED:
					switch (LOWORD(wParam)) {
						case IDC_BTNROMBROWSE: {
							char lpszFile[MAX_PATH] = "\0";
							if (!GetROMName(lpszFile)) {
								strcpy(calcs[SlotSave].rom_path, lpszFile);
								//SendMessage(calcs[gslot].hwndLCD, WM_COMMAND, ECM_CALCRELOAD, 0);
								SendMessage(hwnd, WM_USER, 0, 0);
							}
							break;
						}
						case IDC_BTN1: {
							char lpszFile[MAX_PATH] = "\0";
							if (!GetExportROMName(lpszFile)) {
								FILE* outfile = fopen(lpszFile,"wb");
								char* rom = calcs[SlotSave].mem_c.flash;
								int size = calcs[SlotSave].mem_c.flash_size;
								if (size!=0 && rom!=NULL && outfile!=NULL) {
									int i;
									for(i=0;i<size;i++) {
										fputc(rom[i],outfile);
									}
									fclose(outfile);
								}
							}
							break;
						}
					}
			}
			return TRUE;

		case WM_NOTIFY:
			switch (((NMHDR FAR *) lParam)->code) {
				case PSN_APPLY: {
					SetWindowLong(hwnd, DWL_MSGRESULT, PSNRET_NOERROR);
					return TRUE;
				}
				case PSN_KILLACTIVE:
					SetWindowLong(hwnd, DWL_MSGRESULT, FALSE);
					return TRUE;
			}
			break;

		// Update all of the ROM attributes
		case WM_USER:
			SendMessage(edtRom_path, WM_SETTEXT, 0, (LPARAM) calcs[SlotSave].rom_path);
			SendMessage(edtRom_version, WM_SETTEXT, 0, (LPARAM) calcs[SlotSave].rom_version);
			SendMessage(edtRom_model, WM_SETTEXT, 0, (LPARAM) CalcModelTxt[calcs[SlotSave].model]);
			char szRomSize[16];
			sprintf(szRomSize, "%0.1f KB", (float) calcs[SlotSave].cpu.mem_c->flash_size/1024.0f);
			SendMessage(edtRom_size, WM_SETTEXT, 0, (LPARAM) szRomSize);
			if (hbmTI83P) DeleteObject(hbmTI83P);
			switch (calcs[SlotSave].model) {
				case TI_83PSE:
					hbmTI83P = LoadBitmap(g_hInst, "CalcTI83PSE");
					break;
				default:
					hbmTI83P = LoadBitmap(g_hInst, "CalcTI83P");
					break;
			}
			SendMessage(stcRom_image, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hbmTI83P);
			return TRUE;
	}
	return FALSE;
}
