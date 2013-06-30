#include "stdafx.h"

#include "gui.h"
#include "resource.h"
#include "uxtheme.h"

#include "core.h"
#include "calc.h"
#include "label.h"

#include "gifhandle.h"
#include "gif.h"

#include "var.h"
#include "link.h"
#include "keys.h"
#include "fileutilities.h"
#include "exportvar.h"

#include "dbmem.h"
#include "dbreg.h"
#include "dbtoolbar.h"
#include "dbdisasm.h"
#include "dbwatch.h"

#include "guibuttons.h"
#include "guicontext.h"
#include "guicutout.h"
#include "guidebug.h"
#include "guidetached.h"
#include "guiskin.h"
#include "guiupdate.h"
#include "guiglow.h"
#include "guikeylist.h"
#include "guilcd.h"
#include "guiopenfile.h"
#include "guioptions.h"
#include "guisavestate.h"
#include "guispeed.h"
#include "guivartree.h"
#include "guiwizard.h"
#include "guidialog.h"
#include "guicommandline.h"
#include "guiupdate.h"

#include "guidebug.h"
#include "DropTarget.h"
#include "expandpane.h"
#include "registry.h"
#include "sendfileswindows.h"
#include "state.h"
#include "avi_utils.h"
#include "dbghelp.h"
#include "ftp.h"

CWabbitemuModule _Module;

#ifdef _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#define MENU_FILE 0
#define MENU_EDIT 1
#define MENU_CALC 2
#define MENU_HELP 3

TCHAR ExeDir[512];

INT_PTR CALLBACK DlgVarlist(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInst;
HACCEL hacceldebug;
HACCEL haccelmain;
POINT drop_pt;
BOOL gif_anim_advance;
BOOL silent_mode = FALSE;
BOOL is_exiting = FALSE;
HIMAGELIST hImageList = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ToolProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);


void gui_draw(calc_t *lpCalc) {
	if (lpCalc->hwndLCD != NULL) {
		InvalidateRect(lpCalc->hwndLCD, NULL, FALSE);
	}

	if (lpCalc->hwndDetachedLCD != NULL) {
		InvalidateRect(lpCalc->hwndDetachedLCD, NULL, FALSE);
	}

	if (lpCalc->gif_disp_state != GDS_IDLE) {
		static int skip = 0;
		if (skip == 0) {
			gif_anim_advance = TRUE;
			if (lpCalc->hwndFrame != NULL) {
				InvalidateRect(lpCalc->hwndFrame, NULL, FALSE);
			}
		}
		
		skip = (skip + 1) % 4;
	}
}

VOID CALLBACK TimerProc(HWND hwnd, UINT Message, UINT_PTR idEvent, DWORD dwTimer) {
	static long difference;
	static DWORD prevTimer;

	// How different the timer is from where it should be
	// guard from erroneous timer calls with an upper bound
	// that's the limit of time it will take before the
	// calc gives up and claims it lost time
	difference += ((dwTimer - prevTimer) & 0x003F) - TPF;
	prevTimer = dwTimer;

	// Are we greater than Ticks Per Frame that would call for
	// a frame skip?
	if (difference > -TPF) {
		calc_run_all();
		while (difference >= TPF) {
			calc_run_all();
			difference -= TPF;
		}

		int i;
		for (i = 0; i < MAX_CALCS; i++) {
			if (calcs[i].active) {
				gui_draw(&calcs[i]);
			}
		}
	// Frame skip if we're too far ahead.
	} else difference += TPF;
}

HWND gui_debug(LPCALC lpCalc) {
	TCHAR buf[256];
	if (link_connected_hub(lpCalc->slot)) {
		StringCbPrintf(buf, sizeof(buf), _T("Debugger (%d)"), lpCalc->slot + 1);
	} else {
		StringCbCopy(buf, sizeof(buf), _T("Debugger"));
	}
	if (lpCalc->audio != NULL) {
		pausesound(lpCalc->audio);
	}
	BOOL set_place = TRUE;
	int flags = 0;
	RECT pos = {0, 0, 800, 600};
	WINDOWPLACEMENT db_placement = {0};
	LPDEBUGWINDOWINFO lpDebugInfo = (LPDEBUGWINDOWINFO) GetWindowLongPtr(lpCalc->hwndDebug, GWLP_USERDATA);
	if (!lpDebugInfo) {
		db_placement.flags = SW_SHOWNORMAL;
		db_placement.length = sizeof(WINDOWPLACEMENT);
		CopyRect(&db_placement.rcNormalPosition, &pos);
		set_place = FALSE;
		flags = WS_VISIBLE;
	} else {
		db_placement = lpDebugInfo->db_placement;
	}

	lpCalc->running = FALSE;
	calc_pause_linked();
	if (lpCalc->hwndDebug && IsWindow(lpCalc->hwndDebug)) {
		SwitchToThisWindow(lpCalc->hwndDebug, TRUE);
		waddr_t waddr = addr_to_waddr(&lpCalc->mem_c, lpCalc->cpu.pc);
		while (!lpDebugInfo->is_ready) {
			Sleep(100);
		}
		SendMessage(lpCalc->hwndDebug, WM_COMMAND, MAKEWPARAM(DB_DISASM_GOTO_ADDR, 0),(LPARAM) &waddr);
		SendMessage(lpCalc->hwndDebug, WM_USER, DB_RESUME, 0);
		return lpCalc->hwndDebug;
	}
	
	lpCalc->hwndDebug = CreateWindowEx(
		0,
		g_szDebugName,
		buf,
		flags | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		db_placement.rcNormalPosition.left, db_placement.rcNormalPosition.top,
		db_placement.rcNormalPosition.right - db_placement.rcNormalPosition.left,
		db_placement.rcNormalPosition.bottom - db_placement.rcNormalPosition.top,
		0, 0, g_hInst, (LPVOID) lpCalc);
	if (set_place) {
		SetWindowPlacement(lpCalc->hwndDebug, &db_placement);
	}

	SendMessage(lpCalc->hwndDebug, WM_SIZE, 0, 0);
	return lpCalc->hwndDebug;
}

int gui_frame(LPCALC lpCalc) {
	RECT r, desktop = {0};

	if (!lpCalc->scale)
		lpCalc->scale = 2;
	if (lpCalc->SkinEnabled) {
		SetRect(&r, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom);
	} else {
		SetRect(&r, 0, 0, 128 * lpCalc->scale, 64 * lpCalc->scale);
	}
	AdjustWindowRect(&r, WS_CAPTION | WS_TILEDWINDOW, FALSE);
	r.bottom += GetSystemMetrics(SM_CYMENU);

	//this is to do some checks on some bad registry settings we may have saved
	//its also good for multiple monitors, in case one becomeges lost
	POINT pt = { startX, startY };
	desktop.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	desktop.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	if (!PtInRect(&desktop, pt)) {
		//pt is not on the desktop
		startX = CW_USEDEFAULT;
		startY = CW_USEDEFAULT;
	}

	lpCalc->hwndFrame = CreateWindowEx(
		0, //WS_EX_APPWINDOW,
		g_szAppName,
		_T("Z80"),
		(WS_TILEDWINDOW |  (silent_mode ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX /* | WS_SIZEBOX */),
		startX, startY, r.right - r.left, r.bottom - r.top,
		NULL, 0, g_hInst, (LPVOID) lpCalc);

	SetWindowText(lpCalc->hwndFrame, _T("Wabbitemu"));
	HDC hdc = GetDC(lpCalc->hwndFrame);
	lpCalc->hdcSkin = CreateCompatibleDC(hdc);

	if (!_Module.GetParsedCmdArgs()->no_create_calc)
	{
		lpCalc->breakpoint_callback = gui_debug;
	}

extern keyprog_t keygrps[256];
extern keyprog_t keysti83[256];
extern keyprog_t keysti86[256];
	if (lpCalc->model == TI_86 || lpCalc->model == TI_85) {
		memcpy(keygrps, keysti86, sizeof(keyprog_t) * 256);
	} else {
		memcpy(keygrps, keysti83, sizeof(keyprog_t) * 256);
	}

	//this is now (intuitively) created in guicutout.c (Enable/Disable cutout function)
	/*lpCalc->hwndLCD = CreateWindowEx(
		0,
		g_szLCDName,
		"LCD",
		WS_VISIBLE |  WS_CHILD,
		0, 0, lpCalc->cpu.pio.lcd->width*lpCalc->Scale, 64*lpCalc->Scale,
		lpCalc->hwndFrame, (HMENU) 99, g_hInst,  NULL);*/

	if (lpCalc->hwndFrame == NULL /*|| lpCalc->hwndLCD == NULL*/) return -1;

	GetClientRect(lpCalc->hwndFrame, &r);
	lpCalc->running = TRUE;
	lpCalc->speed = 100;
	HMENU hmenu = GetMenu(lpCalc->hwndFrame);
	CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_MAX, IDM_SPEED_NORMAL, MF_BYCOMMAND);
	gui_frame_update(lpCalc);

	if (auto_turn_on) {
		calc_turn_on(lpCalc);
	}

	ReleaseDC(lpCalc->hwndFrame, hdc);
	return 0;
}


/*
 * Searches for a window with Wabbit's registered lcd class
 */
HWND find_existing_lcd(HWND hwndParent) 
{
	HWND FindChildhwnd = FindWindowEx(hwndParent, NULL, g_szLCDName, NULL);
	if (FindChildhwnd == NULL)
		FindChildhwnd = FindWindowEx(NULL, NULL, g_szLCDName, NULL);
	return FindChildhwnd;
}

/*
 * Checks based on the existence of the main window and the LCD window whether we need
 * to spawn a new process
 * returns false if there is no existing process
 * returns true if there is an existing process found
 */
bool check_no_new_process(HWND Findhwnd) {
	if (Findhwnd == NULL) {
		return false;
	} else {
		return find_existing_lcd(Findhwnd) != NULL;
	}
}

extern HWND hwndProp;
extern RECT PropRect;
extern int PropPageLast;

void RegisterWindowClasses(void) {
	WNDCLASSEX wc;

	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInst;
	wc.hIcon = LoadIcon(g_hInst, _T("A"));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN_MENU);
	wc.lpszClassName = g_szAppName;
	wc.hIconSm = LoadIcon(g_hInst, _T("W"));

	RegisterClassEx(&wc);

	// LCD
	wc.lpszClassName = g_szLCDName;
	wc.lpfnWndProc = LCDProc;
	wc.lpszMenuName = NULL;
	RegisterClassEx(&wc);

	// Toolbar
	wc.lpszClassName = g_szToolbar;
	wc.lpfnWndProc = ToolBarProc;
	wc.lpszMenuName = NULL;
	wc.style = 0;
	RegisterClassEx(&wc);

	// Debugger
	wc.lpfnWndProc = DebugProc;
	wc.style = CS_DBLCLKS;
	wc.lpszClassName = g_szDebugName;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_DEBUG_MENU);
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
	RegisterClassEx(&wc);

	// Disassembly
	wc.lpszMenuName = NULL;
	wc.style = 0;
	wc.lpfnWndProc = DisasmProc;
	wc.lpszClassName = g_szDisasmName;
	wc.hbrBackground = (HBRUSH) NULL;
	RegisterClassEx(&wc);

	// Registers
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = RegProc;
	wc.lpszClassName = g_szRegName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Expanding Panes
	wc.style = 0;
	wc.lpfnWndProc = ExpandPaneProc;
	wc.lpszClassName = g_szExpandPane;
	RegisterClassEx(&wc);

	// Memory Viewer
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = MemProc;
	wc.lpszClassName = g_szMemName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Watchpoints
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WatchProc;
	wc.lpszClassName = g_szWatchName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Detached LCD
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = DetachedProc;
	wc.lpszClassName = g_szDetachedName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);

	// Small cutout buttons
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = SmallButtonProc;
	wc.lpszClassName = g_szSmallButtonsName;
	wc.hbrBackground = NULL;
	RegisterClassEx(&wc);
}

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	const PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	const PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	const PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

static BOOL hasCrashed = FALSE;
extern int def(FILE *, FILE *, int);

LONG WINAPI ExceptionFilter(_EXCEPTION_POINTERS *pExceptionInfo) {
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
	if (hasCrashed) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	hasCrashed = TRUE;
	TCHAR szDumpPath[MAX_PATH], szTempDumpPath[MAX_PATH];
	GetAppDataString(szDumpPath, sizeof(szDumpPath));
	StringCbCopy(szTempDumpPath, sizeof(szTempDumpPath), szDumpPath);
	StringCbCat(szTempDumpPath, sizeof(szDumpPath), _T("Wabbitemu.dmp"));
	StringCbCat(szDumpPath, sizeof(szDumpPath), _T("Wabbitemu.dmp.zip"));
	HANDLE hFile = CreateFile(szTempDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile != INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;

		_MINIDUMP_TYPE dumpType;
		
		VS_FIXEDFILEINFO thisFileInfo;
		UINT dwBytes;
		DWORD dwLen = GetFileVersionInfoSize(_T("DbgHelp.dll"), NULL);
		LPBYTE versionData = (LPBYTE) malloc(dwLen);
		GetFileVersionInfo(_T("DbgHelp.dll"), 0, dwLen, versionData);
		VerQueryValue(versionData, _T("\\"), (LPVOID *) &thisFileInfo, &dwBytes);
		if (HIWORD(thisFileInfo.dwFileVersionMS) == 6) {
			if (LOWORD(thisFileInfo.dwFileVersionMS) <= 1) {
				dumpType = (_MINIDUMP_TYPE) (MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithHandleData | MiniDumpWithDataSegs);
			} else {
				dumpType = (_MINIDUMP_TYPE) (MiniDumpWithFullMemoryInfo | MiniDumpWithDataSegs);
			}
		} else {
			dumpType = (_MINIDUMP_TYPE) (MiniDumpWithHandleData | MiniDumpScanMemory | MiniDumpWithDataSegs);
		}

		// write the dump
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, dumpType, &ExInfo, NULL, NULL);
		CloseHandle(hFile);
		FILE *dumpFile, *zipFile;
		_tfopen_s(&dumpFile, szTempDumpPath, _T("rb"));
		_tfopen_s(&zipFile, szDumpPath, _T("wb"));
		def(dumpFile, zipFile, 1);
		fclose(dumpFile);
		fclose(zipFile);

		if (MessageBox(NULL, _T("Unfortunately Wabbitemu has appeared to have crashed. Would you like to send a crash report to the developers so they can fix it?"),
			_T("Crash"), MB_YESNO) == IDYES) {

				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_REPORT_BUG), NULL, (DLGPROC) BugReportDialogProc);

				HINTERNET hInternet = OpenFtpConnection();
				//Get the file friendly time string
				TCHAR timeStringText[MAX_PATH];
				time_t timeUploaded;
				time(&timeUploaded);
				TCHAR timeString[256];
				_tctime_s(timeString, sizeof(timeString), &timeUploaded);
				for (int i = _tcslen(timeString); i >= 0; i--) {
					if (timeString[i] == ':') {
						timeString[i] = '_';
					}
				}
				//get rid of newline
				timeString[_tcslen((timeString)) - 1] = '\0';
				StringCbCopy(timeStringText, sizeof(timeStringText), timeString);
				StringCbCat(timeStringText, sizeof(timeStringText), _T(".dmp.gz"));

				FtpPutFile(hInternet, szDumpPath, timeStringText, FTP_TRANSFER_TYPE_BINARY, NULL);
				InternetCloseHandle(hInternet);
		}
	}
	return EXCEPTION_EXECUTE_HANDLER;
}


bool CWabbitemuModule::ParseCommandLine(LPCTSTR lpszCommandLine, HRESULT *phres)
{
	ParseCommandLineArgs(&m_parsedArgs);
	return __super::ParseCommandLine(lpszCommandLine, phres);
}


HRESULT CWabbitemuModule::PreMessageLoop(int nShowCmd)
{
	HRESULT hr =  __super::PreMessageLoop(nShowCmd);

	g_hInst = _AtlBaseModule.GetModuleInstance();
	RegisterWindowClasses();
	InitCommonControls();


	//DWORD dwReg;
	//::CoRegisterClassObject(CLSID_Wabbitemu, NULL, CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &dwReg);

	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	if (m_parsedArgs.no_create_calc)
	{
		return hr; 
	}

	bool alreadyRunningWabbit = false;

	//Create our appdata folder
	TCHAR appData[MAX_PATH];
	GetAppDataString(appData, sizeof(appData));
	int error = CreateDirectory(appData, NULL);
	if (!error) {
		error = GetLastError();
		if (error != ERROR_ALREADY_EXISTS) {
			MessageBox(NULL , _T("Unable to create appdata folder"), _T("Error"), MB_OK);
		}
	}

#ifndef _DEBUG
	SetUnhandledExceptionFilter(ExceptionFilter);
#endif

	CheckSetIsPortableMode();

	//this is here so we get our load_files_first setting
	new_calc_on_load_files = QueryWabbitKey(_T("load_files_first")) || m_parsedArgs.force_new_instance;

	HWND alreadyRunningHwnd = NULL;
	alreadyRunningHwnd = FindWindow(g_szAppName, NULL);
	alreadyRunningWabbit = check_no_new_process(alreadyRunningHwnd);
	// If there is a setting to load files into a new calc each time and there is a calc already running
	// ask it to create a new core to load into
	if (new_calc_on_load_files && alreadyRunningHwnd) {
		HWND tempHwnd;
		SendMessage(alreadyRunningHwnd, WM_COMMAND, IDM_FILE_NEW, 0);
		for (int i = 9001; i > 0; i--) {
			tempHwnd = FindWindow(g_szAppName, NULL);
			if (tempHwnd != alreadyRunningHwnd) {
				break;
			}
		}
		alreadyRunningHwnd = tempHwnd;
	}
	
	if (alreadyRunningWabbit) {
		LoadCommandlineFiles(&m_parsedArgs, (LPARAM) find_existing_lcd(alreadyRunningHwnd), LoadAlreadyExistingWabbit);
		if (m_parsedArgs.force_focus) {
			SwitchToThisWindow(alreadyRunningHwnd, TRUE);
		}
		return 0;
	}

	silent_mode = m_parsedArgs.silent_mode;

	LPCALC lpCalc = calc_slot_new();
	LoadRegistrySettings(lpCalc);

	if (rom_load(lpCalc, lpCalc->rom_path) == TRUE) {
		gui_frame(lpCalc);
	} else {

		BOOL loadedRom = FALSE;
		if (m_parsedArgs.num_rom_files > 0) {
			for (int i = 0; i < m_parsedArgs.num_rom_files; i++) {
				if (rom_load(lpCalc, m_parsedArgs.rom_files[i])) {
					gui_frame(lpCalc);
					loadedRom = TRUE;
					break;
				}
			}
		}
		if (!loadedRom) {
			calc_slot_free(lpCalc);

			BOOL wizardError = DoWizardSheet(NULL);
			//save wizard show
			SaveWabbitKey(_T("rom_path"), REG_SZ, &lpCalc->rom_path);
			if (wizardError) {
				return EXIT_FAILURE;
			}
			LoadRegistrySettings(lpCalc);
		}
	}

	//Check for any updates
	if (check_updates) {
		CreateThread(NULL, 0, CheckForUpdates, lpCalc->hwndFrame, NULL, NULL);
	}
	if (show_whats_new) {
		ShowWhatsNew(FALSE);
	}

	StringCbCopy(lpCalc->labelfn, sizeof(lpCalc->labelfn), _T("labels.lab"));

	state_build_applist(&lpCalc->cpu, &lpCalc->applist);
	VoidLabels(lpCalc);
	LoadCommandlineFiles(&m_parsedArgs, (LPARAM) lpCalc, LoadToLPCALC);

#ifdef WITH_AVI
	is_recording = FALSE;
#endif

	// Set the one global timer for all calcs
	SetTimer(NULL, 0, TPF, TimerProc);

	hacceldebug = LoadAccelerators(g_hInst, _T("DisasmAccel"));
	if (!haccelmain)
		haccelmain = LoadAccelerators(g_hInst, _T("Z80Accel"));

	m_lpCalc = lpCalc;
	return hr;
}

void CWabbitemuModule::RunMessageLoop()
{
	if (m_parsedArgs.no_create_calc) {
		return __super::RunMessageLoop();
	}

	MSG Msg;
	extern HWND hPortMon, hBreakpoints;
	while (GetMessage(&Msg, NULL, 0, 0)) {
		HACCEL haccel = haccelmain;
		HWND hwndtop = GetForegroundWindow();
		if (hwndtop) {
			if (hwndtop == FindWindow(g_szDebugName, NULL) ) {
				haccel = hacceldebug;
			} else if (hwndtop == FindWindow(g_szAppName, NULL) ) {
				haccel = haccelmain;
				if (m_lpCalc->bCutout && m_lpCalc->SkinEnabled) {
					hwndtop = FindWindow(g_szLCDName, NULL);
				} else {
					hwndtop = FindWindowEx(hwndtop, NULL, g_szLCDName, NULL);
				}
				SetForegroundWindow(hwndtop);
			} else if (m_lpCalc->bCutout && m_lpCalc->SkinEnabled) {
				if (hwndtop == FindWindow(g_szLCDName, NULL) || hwndtop == FindWindow(g_szAppName, NULL) ||
					hwndtop == FindWindow(g_szSmallButtonsName, g_szSmallClose) ||
					hwndtop == FindWindow(g_szSmallButtonsName, g_szSmallMinimize)) {
					hwndtop = FindWindow(g_szLCDName, NULL);
				} else {
					haccel = NULL;	
				}
			} else {
				haccel = NULL;
			}
		}

		if (hwndProp != NULL) {
			HWND propPage = PropSheet_GetCurrentPageHwnd(hwndProp);
			if (propPage == NULL) {
				GetWindowRect(hwndProp, &PropRect);
				DestroyWindow(hwndProp);
				hwndProp = NULL;
			}
		}

		if (Msg.hwnd != NULL && Msg.hwnd == hwndProp)
		{
			// Get the current tab
			HWND hwndPropTabCtrl = PropSheet_GetTabControl(hwndProp);
			PropPageLast = TabCtrl_GetCurSel(hwndPropTabCtrl);

			if (Msg.message == WM_KEYDOWN)
			{
				OutputDebugString(_T("Hard key down man"));
			}

			if (PropSheet_IsDialogMessage(hwndProp, &Msg) == TRUE)
			{
				continue;
			}
		}

		if (!TranslateAccelerator(hwndtop, haccel, &Msg)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

}

HRESULT CWabbitemuModule::PostMessageLoop()
{
	// Make sure the GIF has terminated
	if (gif_write_state == GIF_FRAME) {
		gif_write_state = GIF_END;
		handle_screenshot();
	}
	
	// Shutdown GDI+
	GdiplusShutdown(m_gdiplusToken);
	return __super::PostMessageLoop();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpszCmdParam, int nCmdShow)
{
	return _Module.WinMain(nCmdShow);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	//static HDC hdcKeymap;
	static POINT ctxtPt;
	LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (Message) {
		case WM_CREATE: {
			LPCALC lpCalc = (LPCALC) ((LPCREATESTRUCT) lParam)->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpCalc);

			//RegisterDropWindow(hwnd, (IDropTarget **) &lpCalc->pDropTarget);

			// Force the current skin setting to be enacted
			lpCalc->SkinEnabled = !lpCalc->SkinEnabled;
			SendMessage(hwnd, WM_COMMAND, IDM_VIEW_SKIN, 0);
			return 0;
		}
		case WM_USER:
			gui_frame_update(lpCalc);
			break;
		case WM_PAINT: {
#define GIFGRAD_PEAK 15
#define GIFGRAD_TROUGH 10

			static int GIFGRADWIDTH = 1;
			static int GIFADD = 1;

			if (gif_anim_advance) {
				switch (lpCalc->gif_disp_state) {
					case GDS_STARTING:
						if (GIFGRADWIDTH > 15) {
							lpCalc->gif_disp_state = GDS_RECORDING;
							GIFADD = -1;
						} else {
							GIFGRADWIDTH ++;
						}
						break;
					case GDS_RECORDING:
						GIFGRADWIDTH += GIFADD;
						if (GIFGRADWIDTH > GIFGRAD_PEAK) GIFADD = -1;
						else if (GIFGRADWIDTH < GIFGRAD_TROUGH) GIFADD = 1;
						break;
					case GDS_ENDING:
						if (GIFGRADWIDTH) GIFGRADWIDTH--;
						else {
							lpCalc->gif_disp_state = GDS_IDLE;							
							gui_frame_update(lpCalc);						
						}						
						break;
					case GDS_IDLE:
						break;
				}
				gif_anim_advance = FALSE;
			}

			if (lpCalc->gif_disp_state != GDS_IDLE) {
				RECT screen;
				GetWindowRect(lpCalc->hwndLCD, &screen);
				MapWindowRect(NULL, lpCalc->hwndFrame, &screen);

				int grayred = (int) (((double) GIFGRADWIDTH / GIFGRAD_PEAK) * 50);
				HDC hWindow = GetDC(hwnd);
				DrawGlow(lpCalc->hdcSkin, hWindow, &screen, RGB(127 - grayred, 127 - grayred, 127 + grayred),
							GIFGRADWIDTH, lpCalc->SkinEnabled);				
				ReleaseDC(hwnd, hWindow);
				InflateRect(&screen, GIFGRADWIDTH, GIFGRADWIDTH);
				ValidateRect(hwnd, &screen);
			}

			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			if (lpCalc->SkinEnabled) {
				BitBlt(hdc, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, lpCalc->hdcButtons, 0, 0, SRCCOPY);
				BitBlt(lpCalc->hdcButtons, 0, 0, lpCalc->rectSkin.right, lpCalc->rectSkin.bottom, lpCalc->hdcSkin, 0, 0, SRCCOPY);
			} else {
				RECT rc;
				GetClientRect(lpCalc->hwndFrame, &rc);
				FillRect(hdc, &rc, GetStockBrush(GRAY_BRUSH));
			}
			ReleaseDC(hwnd, hdc);
			EndPaint(hwnd, &ps);

			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDM_FILE_NEW: {
						LPCALC lpCalcNew = calc_slot_new();
						if (rom_load(lpCalcNew, lpCalc->rom_path) || rom_load(lpCalcNew, (LPCTSTR) QueryWabbitKey(_T("rom_path")))) {
							lpCalcNew->SkinEnabled = lpCalc->SkinEnabled;
							lpCalcNew->bCutout = lpCalc->bCutout;
							lpCalcNew->scale = lpCalc->scale;
							lpCalcNew->FaceplateColor = lpCalc->FaceplateColor;
							lpCalcNew->bAlphaBlendLCD = lpCalc->bAlphaBlendLCD;
							lpCalcNew->cpu.pio.lcd->shades = lpCalc->cpu.pio.lcd->shades;

							if (!lpCalcNew->cpu.pio.lcd->active) {
								calc_turn_on(lpCalcNew);
							}
							gui_frame(lpCalcNew);

							RECT rc;
							GetWindowRect(hwnd, &rc);
							RECT newrc;
							GetWindowRect(lpCalc->hwndFrame, &newrc);
							SetWindowPos(lpCalcNew->hwndFrame, NULL, newrc.left + rc.right - rc.left, newrc.top, 0, 0,
								SWP_NOSIZE | SWP_NOZORDER | (silent_mode ? SWP_HIDEWINDOW : 0));
						} else {
							calc_slot_free(lpCalcNew);
							SendMessage(hwnd, WM_COMMAND, IDM_HELP_WIZARD, 0);
						}
						break;
				}
				case IDM_FILE_OPEN: {
					GetOpenSendFileName(hwnd);
					SetWindowText(hwnd, _T("Wabbitemu"));
					break;
				}
				case IDM_FILE_SAVE: {
					TCHAR FileName[MAX_PATH];
					const TCHAR lpstrFilter[] = _T("Known File types ( *.sav; *.rom; *.bin) \0*.sav;*.rom;*.bin\0\
														Save States  (*.sav)\0*.sav\0\
														ROMS  (*.rom; .bin)\0*.rom;*.bin\0\
														OSes (*.8xu)\0*.8xu\0\
														All Files (*.*)\0*.*\0\0");
					ZeroMemory(FileName, MAX_PATH);
					if (!SaveFile(FileName, (TCHAR *) lpstrFilter, _T("Wabbitemu Save State"), _T("sav"), OFN_PATHMUSTEXIST)) {
						TCHAR extension[5] = _T("");
						const TCHAR *pext = _tcsrchr(FileName, _T('.'));
						if (pext != NULL)
						{
							StringCbCopy(extension, sizeof(extension), pext);
						}
						if (!_tcsicmp(extension, _T(".rom")) || !_tcsicmp(extension, _T(".bin"))) {
							MFILE *file = ExportRom(FileName, lpCalc);
							StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), FileName);
							mclose(file);
						} else if (!_tcsicmp(extension, _T(".8xu"))) {
							HWND hExportOS = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_EXPORT_OS), hwnd, (DLGPROC) ExportOSDialogProc, (LPARAM) FileName);
							ShowWindow(hExportOS, SW_SHOW);
						} else {
							SAVESTATE_t *save = SaveSlot(lpCalc);
							gui_savestate(hwnd, save, FileName, lpCalc);
						}
					}
					break;
				}
				case IDM_FILE_GIF: {
					HMENU hmenu = GetMenu(hwnd);
					static BOOL calcBackupRunning[MAX_CALCS];
					if (gif_write_state == GIF_IDLE) {
						int i;
						for (i = 0; i < MAX_CALCS; i++) {
							if (calcs[i].active == TRUE) {
								calcBackupRunning[i] = calcs[i].running;
							}
						}
						BOOL start_screenshot = get_gif_filename();
						if (!start_screenshot) {
							lpCalc->running = TRUE;
							break;
						}
						gif_write_state = GIF_START;
						for (int i = 0; i < MAX_CALCS; i++) {
							if (calcs[i].active) {
								calcs[i].gif_disp_state = GDS_STARTING;
							}
						}
						CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_GIF, MF_BYCOMMAND | MF_CHECKED);
						lpCalc->running = TRUE;
					} else {
						gif_write_state = GIF_END;
						for (int i = 0; i < MAX_CALCS; i++) {
							if (calcs[i].active) {
								calcs[i].gif_disp_state = GDS_ENDING;
							}
						}
						CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_GIF, MF_BYCOMMAND | MF_UNCHECKED);

						int i;
						for (i = 0; i < MAX_CALCS; i++) {
							if (calcs[i].active == TRUE) {
								calcs[i].running = calcBackupRunning[i];
							}
						}
					}
					break;
				}
				case IDM_FILE_STILLGIF: {
					lpCalc->running = FALSE;
					BOOL start_screenshot = get_gif_filename();
					if (start_screenshot) {
						LCD_t *lcd = lpCalc->cpu.pio.lcd;
						gif_xs = lcd->width * gif_size;
						gif_ys = SCRYSIZE * gif_size;
						GIFGREYLCD(lcd);

						unsigned int i, j;
						for (i = 0; i < SCRYSIZE * gif_size; i++) {
							for (j = 0; j < lcd->width * gif_size; j++) {
								gif_frame[i * gif_xs + j] = lpCalc->cpu.pio.lcd->gif[i][j];
							}
						}
						gif_write_state = GIF_START;
						gif_writer(lcd->shades);

						gif_write_state = GIF_END;
						gif_writer(lcd->shades);
					}
					lpCalc->running = TRUE;
					break;
				}
				case IDM_FILE_AVI: {
					HMENU hmenu = GetMenu(hwnd);
#ifdef WITH_AVI
					if (is_recording) {
						delete currentAvi;
						currentAvi = NULL;
						//CloseAvi(recording_avi);
						is_recording = FALSE;
						CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_AVI, MF_BYCOMMAND | MF_UNCHECKED);
					} else {
						lpCalc->running = FALSE;
						TCHAR lpszFile[MAX_PATH];
						if (!SaveFile(lpszFile, _T("AVIs (*.avi)\0*.avi\0All Files (*.*)\0*.*\0\0"),
											_T("Wabbitemu Export AVI"), _T("avi"), OFN_PATHMUSTEXIST)) {

							GetCompression();
							currentAvi = new CAviFile(lpszFile, /*mmioFOURCC('U', 'Y', 'V', 'Y')*/0x63646976, FPS);
							//recording_avi = CreateAvi(lpszFile, FPS, NULL);
							//create an initial first frame so we can set compression
							is_recording = TRUE;
							CheckMenuItem(GetSubMenu(hmenu, MENU_FILE), IDM_FILE_AVI, MF_BYCOMMAND | MF_CHECKED);
						}
						lpCalc->running = TRUE;
					}
#endif
					break;
				}
				case IDM_FILE_CLOSE:
					return SendMessage(hwnd, WM_CLOSE, 0, 0);
				case IDM_FILE_EXIT:
					if (calc_count() > 1) {
						TCHAR buf[256];
						StringCbPrintf(buf, sizeof(buf), _T("If you exit now, %d other running calculator(s) will be closed. \
															Are you sure you want to exit?"), calc_count() - 1);
						int res = MessageBox(NULL, buf, _T("Wabbitemu"), MB_YESNO);
						if (res == IDCANCEL || res == IDNO) {
							break;
						}
						for (int i = 0; i < MAX_CALCS; i++) {
							if (calcs[i].active) {
								SendMessage(calcs[i].hwndFrame, WM_CLOSE, 0, 0);
							}
						}
						is_exiting = TRUE;
					} else {
						SendMessage(hwnd, WM_CLOSE, 0, 0);
					}
					PostQuitMessage(0);
					break;
				case IDM_CALC_COPY: {
					HLOCAL ans;
					ans = (HLOCAL) GetRealAns(&lpCalc->cpu);
					OpenClipboard(hwnd);
					EmptyClipboard();
					SetClipboardData(CF_TEXT, ans);
					CloseClipboard();
					break;
				}
				case IDM_EDIT_PASTE: {
					
					break;
				}
				case IDM_VIEW_SKIN: {
					lpCalc->SkinEnabled = !lpCalc->SkinEnabled;
					gui_frame_update(lpCalc);
					break;
				}
				case IDM_VIEW_LCD: {
					if (lpCalc->hwndDetachedLCD || lpCalc->hwndDetachedFrame) {
						break;
					}
					RECT r;
					SetRect(&r, 0, 0, (lpCalc->rectLCD.right - lpCalc->rectLCD.left) / 2 * lpCalc->scale, 64 * lpCalc->scale);
					AdjustWindowRect(&r, WS_CAPTION | WS_TILEDWINDOW, FALSE);

					lpCalc->hwndDetachedFrame  = CreateWindowEx(
						0,
						g_szDetachedName,
						_T("Z80"),
						(WS_TILEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX),
						startX, startY, r.right - r.left, r.bottom - r.top,
						NULL, 0, g_hInst, (LPVOID) lpCalc);

					SetWindowText(lpCalc->hwndDetachedFrame, _T("LCD"));

					if (lpCalc->hwndDetachedFrame == NULL) {
						return -1;
					}

					break;
				}
				case IDM_CALC_SOUND: {
					togglesound(lpCalc->audio);
					CheckMenuItem(GetSubMenu(GetMenu(hwnd), 2), IDM_CALC_SOUND, MF_BYCOMMAND | (lpCalc->audio->enabled ? MF_CHECKED : MF_UNCHECKED));
					break;
				}
				case ID_DEBUG_TURNONCALC:
					{
						calc_turn_on(lpCalc);
						break;
					}
				case IDM_CALC_CONNECT: {
					/*if (!calcs[0].active || !calcs[1].active || link_connect(&calcs[0].cpu, &calcs[1].cpu))						
						MessageBox(NULL, _T("Connection Failed"), _T("Error"), MB_OK);					
					else*/
					link_connect_hub(lpCalc->slot, &lpCalc->cpu);
					TCHAR buf[64];
					StringCbCopy(buf, sizeof(buf), CalcModelTxt[lpCalc->model]);
					StringCbCat(buf, sizeof(buf), _T(" Connected"));
					SendMessage(lpCalc->hwndStatusBar, SB_SETTEXT, 1, (LPARAM) buf);
					StringCbPrintf(buf, sizeof(buf), _T("Wabbitemu (%d)"), lpCalc->slot + 1);
					SetWindowText(hwnd, buf);			
					break;
				}
				case IDM_CALC_PAUSE: {
					HMENU hmenu = GetMenu(hwnd);
					if (lpCalc->running) {
						CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_PAUSE, MF_BYCOMMAND | MF_CHECKED);
						lpCalc->running = FALSE;
					} else {
						CheckMenuItem(GetSubMenu(hmenu, 2), IDM_CALC_PAUSE, MF_BYCOMMAND | MF_UNCHECKED);
						lpCalc->running = TRUE;
					}
					break;
				}
				case IDM_VIEW_VARIABLES:
					CreateVarTreeList(hwnd, lpCalc);
					break;
				case IDM_VIEW_KEYSPRESSED:
					if (IsWindow(lpCalc->hwndKeyListDialog)) {
						SwitchToThisWindow(lpCalc->hwndKeyListDialog, TRUE);
					} else {
						lpCalc->hwndKeyListDialog = (HWND) CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_KEYS_LIST), hwnd, (DLGPROC) KeysListProc);
						ShowWindow(lpCalc->hwndKeyListDialog, SW_SHOW);
					}
					break;
				case IDM_CALC_OPTIONS:
					DoPropertySheet(hwnd);
					break;
				case IDM_DEBUG_RESET: {
					calc_reset(lpCalc);
					break;
				}
				case IDM_DEBUG_OPEN:
					gui_debug(lpCalc);
					break;
				case IDM_HELP_ABOUT:
					lpCalc->running = FALSE;
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGABOUT), hwnd, (DLGPROC) AboutDialogProc);					
					lpCalc->running = TRUE;
					break;
				case IDM_HELP_WHATSNEW:
					ShowWhatsNew(TRUE);
					break;
				case IDM_HELP_WIZARD: {
					int count = calc_count();
					DoWizardSheet(hwnd);
					int newCount = calc_count();
					if (count < newCount) {
						SetFocus(calcs[newCount - 1].hwndFrame);
						RECT rc;
						GetWindowRect(hwnd, &rc);
						RECT newrc;
						GetWindowRect(lpCalc->hwndFrame, &newrc);
						SetWindowPos(calcs[newCount - 1].hwndFrame, NULL, newrc.left + rc.right - rc.left, newrc.top, 0, 0,
							SWP_NOSIZE | SWP_NOZORDER | (silent_mode ? SWP_HIDEWINDOW : 0));
					}
					break;
				}
				case IDM_HELP_WEBSITE:					
					ShellExecute(NULL, _T("open"), g_szWebPage, NULL, NULL, SW_SHOWNORMAL);
					break;
				case IDM_FRAME_BTOGGLE:
					SendMessage(hwnd, WM_MBUTTONDOWN, MK_MBUTTON, MAKELPARAM(ctxtPt.x, ctxtPt.y));
					break;
				case IDM_FRAME_BUNLOCK: {
					RECT rc;
					keypad_t *kp = (keypad_t *) lpCalc->cpu.pio.devices[1].aux;
					int group, bit;
					GetClientRect(hwnd, &rc);
					for(group = 0; group < 7; group++) {
						for(bit = 0; bit < 8; bit++) {
							kp->keys[group][bit] &= (~KEY_LOCKPRESS);
						}
					}
					lpCalc->cpu.pio.keypad->on_pressed &= (~KEY_LOCKPRESS);

					FinalizeButtons(lpCalc);
					break;
				}
				case IDM_SPEED_QUARTER: {
					lpCalc->speed = 25;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUARTER, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_HALF: {
					lpCalc->speed = 50;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_HALF, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_NORMAL: {
					lpCalc->speed = 100;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_NORMAL, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_DOUBLE: {
					lpCalc->speed = 200;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_DOUBLE, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_QUADRUPLE: {
					lpCalc->speed = 400;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUADRUPLE, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_MAX: {
					lpCalc->speed = MAX_SPEED;
					HMENU hmenu = GetMenu(hwnd);
					CheckMenuRadioItem(GetSubMenu(hmenu, 2), IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_MAX, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
				case IDM_SPEED_SET: {
					int dialog = DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_DLGSPEED), hwnd, (DLGPROC) SetSpeedProc, (LPARAM) lpCalc);
					if (dialog == IDOK) {
						HMENU hMenu = GetMenu(hwnd);
						switch(lpCalc->speed)
						{
							case 25:
								CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUARTER, MF_BYCOMMAND| MF_CHECKED);
								break;
							case 50:
								CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_HALF, MF_BYCOMMAND| MF_CHECKED);
								break;
							case 100:
								CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_NORMAL, MF_BYCOMMAND| MF_CHECKED);
								break;
							case 200:
								CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_DOUBLE, MF_BYCOMMAND| MF_CHECKED);
								break;
							case 400:
								CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_QUADRUPLE, MF_BYCOMMAND| MF_CHECKED);
								break;
							default:
								CheckMenuRadioItem(hMenu, IDM_SPEED_QUARTER, IDM_SPEED_SET, IDM_SPEED_SET, MF_BYCOMMAND| MF_CHECKED);
								break;
						}
					}
					SetFocus(hwnd);
					break;
				}
				case IDM_HELP_UPDATE: {
					BOOL hasUpdates = CheckForUpdates(hwnd);
					if (!hasUpdates) {
						MessageBox(hwnd, _T("No update is available"), _T("Wabbitemu"), MB_OK);
					}
					if (!SUCCEEDED(hasUpdates)) {
						MessageBox(hwnd, _T("Error checking for updates"), _T("Error"), MB_OK);
					}
					break;
				}
				case IDM_HELP_BUG: {
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_REPORT_BUG), hwnd, (DLGPROC) BugReportDialogProc);
					break;
				}
			}
			return 0;
		}
		case WM_LBUTTONUP:
		{
			int group, bit;
			static POINT pt;
			BOOL repostMessage = FALSE;
			keypad_t *kp = lpCalc->cpu.pio.keypad;

			ReleaseCapture();
#define KEY_TIMER 1
			KillTimer(hwnd, KEY_TIMER);

			for (group = 0; group < 7; group++) {
				for (bit = 0; bit < 8; bit++) {
#define MIN_KEY_DELAY 400
					if (kp->last_pressed[group][bit] - lpCalc->cpu.timer_c->tstates >= MIN_KEY_DELAY || !lpCalc->running) {
						kp->keys[group][bit] &= (~KEY_MOUSEPRESS);
					} else {
						repostMessage = TRUE;
					}
				}
			}

			if (kp->on_last_pressed - lpCalc->cpu.timer_c->tstates >= MIN_KEY_DELAY || !lpCalc->running) {
				lpCalc->cpu.pio.keypad->on_pressed &= ~KEY_MOUSEPRESS;
			} else {
				repostMessage = TRUE;
			}

			if (repostMessage) {
				SetTimer(hwnd, KEY_TIMER, 50, NULL);
			}

			FinalizeButtons(lpCalc);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			int group, bit;
			static POINT pt;
			keypad_t *kp = lpCalc->cpu.pio.keypad;

			SetCapture(hwnd);
			pt.x	= GET_X_LPARAM(lParam);
			pt.y	= GET_Y_LPARAM(lParam);
			if (lpCalc->bCutout) {
				pt.y += GetSystemMetrics(SM_CYCAPTION);	
				pt.x += GetSystemMetrics(SM_CXSIZEFRAME);
			}

			for (group = 0; group < 7; group++) {
				for (bit = 0; bit < 8; bit++) {
					kp->keys[group][bit] &= (~KEY_MOUSEPRESS);
				}
			}

			kp->on_pressed &= ~KEY_MOUSEPRESS;

			COLORREF c = GetPixel(lpCalc->hdcKeymap, pt.x, pt.y);
			if (GetRValue(c) == 0xFF) {
				FinalizeButtons(lpCalc);
				return 0;
			}

			group = GetGValue(c) >> 4;
			bit	= GetBValue(c) >> 4;
			LogKeypress(lpCalc, group, bit);
			if (group == KEYGROUP_ON && bit == KEYBIT_ON){
				kp->on_pressed |= KEY_MOUSEPRESS;
				kp->on_last_pressed = lpCalc->cpu.timer_c->tstates;
			} else {
				kp->keys[group][bit] |= KEY_MOUSEPRESS;
				if ((kp->keys[group][bit] & KEY_STATEDOWN) == 0) {
					kp->keys[group][bit] |= KEY_STATEDOWN;
					kp->last_pressed[group][bit] = lpCalc->cpu.timer_c->tstates;
				}
			}
			FinalizeButtons(lpCalc);
			return 0;
		}
		case WM_TIMER: {
			if (wParam == KEY_TIMER) {
				PostMessage(hwnd, WM_LBUTTONUP, 0, 0);
			}
			break;
		}
		case WM_MBUTTONDOWN: {
			int group,bit;
			POINT pt;
			keypad_t *kp = (keypad_t *) (&lpCalc->cpu)->pio.devices[1].aux;

			pt.x	= GET_X_LPARAM(lParam);
			pt.y	= GET_Y_LPARAM(lParam);
			if (lpCalc->bCutout) {
				pt.y += GetSystemMetrics(SM_CYCAPTION);	
				pt.x += GetSystemMetrics(SM_CXSIZEFRAME);
			}

			COLORREF c = GetPixel(lpCalc->hdcKeymap, pt.x, pt.y);
			if (GetRValue(c) == 0xFF) return 0;
			group	= GetGValue(c) >> 4;
			bit		= GetBValue(c) >> 4;

			if (group == KEYGROUP_ON && bit == KEYBIT_ON) {
				lpCalc->cpu.pio.keypad->on_pressed ^= KEY_LOCKPRESS;
			} else {
				kp->keys[group][bit] ^= KEY_LOCKPRESS;
			}
			FinalizeButtons(lpCalc);
			return 0;
		}

		case WM_KEYDOWN: {
			HandleKeyDown(lpCalc, wParam);
			return 0;
		}
		case WM_KEYUP:
			if (wParam) {
				HandleKeyUp(lpCalc, wParam);
			}
			return 0;
		case WM_SIZING: {
			if (lpCalc->SkinEnabled) {
				return TRUE;
			}
			RECT *prc = (RECT *) lParam;
			LONG ClientAdjustWidth, ClientAdjustHeight;
			LONG AdjustWidth, AdjustHeight;

			// Adjust for border and menu
			RECT rc = {0, 0, 0, 0};
			AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW, FALSE);
			if (GetMenu(hwnd) != NULL)
			{
				rc.bottom += GetSystemMetrics(SM_CYMENU);
			}

			RECT src;
			if (lpCalc->hwndStatusBar != NULL) {
				GetWindowRect(lpCalc->hwndStatusBar, &src);
				rc.bottom += src.bottom - src.top;
			}
			//don't allow resizing from the sides
			if (wParam == WMSZ_LEFT || wParam == WMSZ_RIGHT 
				|| wParam == WMSZ_TOP || wParam == WMSZ_BOTTOM) {
					GetWindowRect(hwnd, &rc);
					memcpy(prc, &rc, sizeof(RECT));
					return TRUE;
			}

			ClientAdjustWidth = rc.right - rc.left;
			ClientAdjustHeight = rc.bottom - rc.top;


			switch (wParam) {
			case WMSZ_BOTTOMLEFT:
			case WMSZ_LEFT:
			case WMSZ_TOPLEFT:
				prc->left -= 128 / 4;
				break;
			default:
				prc->right += 128 / 4;
				break;
			}

			switch (wParam) {
			case WMSZ_TOPLEFT:
			case WMSZ_TOP:
			case WMSZ_TOPRIGHT:
				prc->top -= 64 / 4;
				break;
			default:
				prc->bottom += 64 / 4;
				break;
			}


			// Make sure the width is a nice clean proportional sizing
			AdjustWidth = (prc->right - prc->left - ClientAdjustWidth) % 128;
			AdjustHeight = (prc->bottom - prc->top - ClientAdjustHeight) % 64;

			int cx_mult = (prc->right - prc->left - ClientAdjustWidth) / 128;
			int cy_mult = (prc->bottom - prc->top - ClientAdjustHeight) / 64;

			while (cx_mult < 2 || cy_mult < 2) {
				if (cx_mult < 2) {cx_mult++; AdjustWidth -= 128;}
				if (cy_mult < 2) {cy_mult++; AdjustHeight -= 64;}
			}

			if (cx_mult > cy_mult) {
				AdjustWidth += (cx_mult - cy_mult) * 128;
			} else if (cy_mult > cx_mult) {
				AdjustHeight += (cy_mult - cx_mult) * 64;
			}


			lpCalc->scale = min(cx_mult, cy_mult);

			switch (wParam) {
			case WMSZ_BOTTOMLEFT:
			case WMSZ_LEFT:
			case WMSZ_TOPLEFT:
				prc->left += AdjustWidth;
				break;
			default:
				prc->right -= AdjustWidth;
				break;
			}

			switch (wParam) {
			case WMSZ_TOPLEFT:
			case WMSZ_TOP:
			case WMSZ_TOPRIGHT:
				prc->top += AdjustHeight;
				break;
			default:
				prc->bottom -= AdjustHeight;
				break;
			}
			RECT rect;
			GetClientRect(hwnd, &rect);
			InvalidateRect(hwnd, &rect, TRUE);
			return TRUE;
		}
		case WM_SIZE: {
			RECT rc, screen;
			GetClientRect(hwnd, &rc);
			HMENU hmenu = GetMenu(hwnd);
			int cyMenu = hmenu == NULL ? 0 : GetSystemMetrics(SM_CYMENU);
			if ((lpCalc->bCutout && lpCalc->SkinEnabled))	
				rc.bottom += cyMenu;
			int desired_height = lpCalc->SkinEnabled ?  lpCalc->rectSkin.bottom : 128;

			int status_height;
			if (lpCalc->hwndStatusBar == NULL) {
				status_height = 0;
			} else {
				RECT src;
				GetWindowRect(lpCalc->hwndStatusBar, &src);

				status_height = src.bottom - src.top;
				desired_height += status_height;
			}

			rc.bottom -= status_height;

			float xc = 1, yc = 1;
			if (!lpCalc->SkinEnabled) {
				xc = ((float) rc.right) / 256.0f;
				yc = ((float) rc.bottom) / 128.0f;
			}
			int width = lpCalc->rectLCD.right - lpCalc->rectLCD.left;
			SetRect(&screen,
				0, 0,
				(int) (width * xc),
				(int) (64 * 2 * yc));

			if (lpCalc->SkinEnabled)
				OffsetRect(&screen, lpCalc->rectLCD.left, lpCalc->rectLCD.top);
			else
				OffsetRect(&screen, (int) ((rc.right - width * xc) / 2), 0);

			if ((rc.right - rc.left) & 1) rc.right++;
			if ((rc.bottom - rc.top) & 1) rc.bottom++;

			RECT client;
			client.top = 0;
			client.left = 0;
			if (lpCalc->SkinEnabled) {
				if (lpCalc->bCutout) {
					GetWindowRect(hwnd, &client);
				}
				RECT correctSize = lpCalc->rectSkin;
				AdjustWindowRect(&correctSize, (WS_TILEDWINDOW |  WS_VISIBLE | WS_CLIPCHILDREN) & ~(WS_MAXIMIZEBOX), cyMenu);
				if (correctSize.left < 0) {
					correctSize.right -= correctSize.left;
				}
				if (correctSize.top < 0) {
					correctSize.bottom -= correctSize.top;
				}
				SetWindowPos(hwnd, NULL, 0, 0, correctSize.right, correctSize.bottom, SWP_NOACTIVATE | SWP_NOOWNERZORDER |
					SWP_NOMOVE | SWP_DRAWFRAME | (silent_mode ? SWP_HIDEWINDOW : 0));
			}
			RECT windowRect;
			GetWindowRect(hwnd, &windowRect);

			if (windowRect.bottom - windowRect.top != screen.bottom - screen.top ||
				windowRect.right - windowRect.left != screen.right - screen.left)
			{
				MoveWindow(lpCalc->hwndLCD, screen.left + client.left, screen.top + client.top,
					screen.right-screen.left, screen.bottom-screen.top, FALSE);
			}
			ValidateRect(hwnd, &screen);
			//printf("screen: %d\n", screen.right - screen.left);
			if (lpCalc->hwndStatusBar != NULL)
				SendMessage(lpCalc->hwndStatusBar, WM_SIZE, 0, 0);

			//force little buttons to be correct
			PositionLittleButtons(hwnd);
			UpdateWindow(lpCalc->hwndLCD);
			//InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		//case WM_MOVING:
		case WM_MOVE: {
			if (lpCalc->bCutout && lpCalc->SkinEnabled) {
				HDWP hdwp = BeginDeferWindowPos(3);
				RECT rc;
				GetWindowRect(hwnd, &rc);
				OffsetRect(&rc, lpCalc->rectLCD.left, lpCalc->rectLCD.top);
				DeferWindowPos(hdwp, lpCalc->hwndLCD, HWND_TOP, rc.left, rc.top, 0, 0, SWP_NOSIZE);
				EndDeferWindowPos(hdwp);
				PositionLittleButtons(hwnd);
			}
			return 0;
		}
		case WM_CONTEXTMENU: {
			ctxtPt.x = GET_X_LPARAM(lParam);
			ctxtPt.y = GET_Y_LPARAM(lParam);

			HMENU hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_FRAME_MENU));
			// TrackPopupMenu cannot display the menu bar so get
			// a handle to the first shortcut menu.
			hmenu = GetSubMenu(hmenu, 0);

			if (!OnContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hmenu)) {
				DefWindowProc(hwnd, Message, wParam, lParam);
			}
			ScreenToClient(hwnd, &ctxtPt);
			DestroyMenu(hmenu);
			return 0;
		}
		case WM_GETMINMAXINFO: {
			if (lpCalc == NULL)
				return 0;
			if (!lpCalc->SkinEnabled)
				break;
			MINMAXINFO *info = (MINMAXINFO *) lParam;
			RECT rc = { 0, 0, SKIN_WIDTH, SKIN_HEIGHT };
			AdjustWindowRect(&rc, WS_CAPTION | WS_TILEDWINDOW, FALSE);
			info->ptMinTrackSize.x = rc.right - rc.left;
			info->ptMinTrackSize.y = rc.bottom - rc.top;
			info->ptMaxTrackSize.x = rc.right - rc.left;
			info->ptMaxTrackSize.y = rc.bottom - rc.top;
			return 0;
		}
		case WM_KILLFOCUS: {
			keypad_t *keypad = lpCalc->cpu.pio.keypad;
			//handle keys already down (just send release)
			//i send the message here so that things like logging are handled
			for (int group = 0; group < 8; group++) {
				for (int bit = 0; bit < 8; bit++) {
					if (keypad->keys[group][bit]) {
						keypad_vk_release(hwnd, group, bit);
					}
				}
			}
			return 0;
		}
		case WM_CLOSE:
			if (calc_count() == 1) {
				if (exit_save_state)
				{
					TCHAR tempSave[MAX_PATH] = {0};
					if (portable_mode) {
						StringCbCopy(tempSave, sizeof(tempSave), portSettingsPath);
						for (u_int i = strlen(portSettingsPath) - 1; i >= 0; i--) {
							if (tempSave[i] == '\\') {
								tempSave[i] = '\0';
								break;
							}
							tempSave[i] = '\0';
						}
					} else {
						GetAppDataString(tempSave, sizeof(tempSave));
					}
					StringCbCat(tempSave, sizeof(tempSave), _T("\\wabbitemu.sav"));
					StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), tempSave);
					SAVESTATE_t *save = SaveSlot(lpCalc);
					WriteSave(tempSave, save, true);
					FreeSave(save);
				}

				DestroyCutoutResources();

				SaveRegistrySettings(lpCalc);

			}
			DestroyWindow(hwnd);
			calc_slot_free(lpCalc);
			if (calc_count() == 0) {
				PostQuitMessage(0);
			}
			return 0;
		case WM_DESTROY: {
				DeleteDC(lpCalc->hdcKeymap);
				DeleteDC(lpCalc->hdcSkin);
				lpCalc->hdcKeymap = NULL;
				lpCalc->hdcSkin = NULL;

				if (lpCalc->hwndDebug)
					DestroyWindow(lpCalc->hwndDebug);
				lpCalc->hwndDebug = NULL;

				if (lpCalc->hwndStatusBar)
					DestroyWindow(lpCalc->hwndStatusBar);
				lpCalc->hwndStatusBar = NULL;

				if (lpCalc->hwndSmallClose)
					DestroyWindow(lpCalc->hwndSmallClose);
				lpCalc->hwndSmallClose = NULL;

				if (lpCalc->hwndSmallMinimize)
					DestroyWindow(lpCalc->hwndSmallMinimize);
				lpCalc->hwndSmallMinimize = NULL;

				//if (link_connected(lpCalc->slot))
				//	link_disconnect(&lpCalc->cpu);

				lpCalc->hwndFrame = NULL;
				return 0;
			}
		case WM_NCHITTEST:
		{
			int htRet = (int) DefWindowProc(hwnd, Message, wParam, lParam);
			if (htRet != HTCLIENT) return htRet;

			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			if (lpCalc->bCutout && lpCalc->SkinEnabled) {
				pt.y += GetSystemMetrics(SM_CYCAPTION);
				pt.x += GetSystemMetrics(SM_CXFIXEDFRAME);
			}
			ScreenToClient(hwnd, &pt);
			if (GetRValue(GetPixel(lpCalc->hdcKeymap, pt.x, pt.y)) != 0xFF)
				return htRet;
			return HTCAPTION;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}