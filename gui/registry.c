#include "stdafx.h"

#include "gui.h"
#include "calc.h"
#include "gif.h"
#include "registry.h"
#include "dbcommon.h"
#include "expandpane.h"

extern keyprog_t keygrps[256];
TCHAR *verString = _T("1.5.12.13");

static HKEY hkeyTarget;

static struct {
	LPCTSTR lpValueName;
	DWORD dwType;
	LONG_PTR Value;
} regDefaults[] = {
	{_T("cutout"), 					REG_DWORD, 	FALSE},
	{_T("skin"),					REG_DWORD,	FALSE},
	{_T("alphablend_lcd"),			REG_DWORD,	TRUE},
	{_T("version"), 				REG_SZ, 	(LONG_PTR) verString},
	{_T("rom_path"), 				REG_SZ, 	(LONG_PTR) _T("z.rom")},
	{_T("shades"),					REG_DWORD,	6},
	{_T("gif_path"), 				REG_SZ,		(LONG_PTR) _T("wabbitemu.gif")},
	{_T("gif_autosave"),			REG_DWORD,	FALSE},
	{_T("gif_useinc"),				REG_DWORD,	FALSE},
	{_T("lcd_mode"),				REG_DWORD,	0},		// perfect gray
	{_T("lcd_freq"),				REG_DWORD,	FPS},	// steady freq
	{_T("screen_scale"),			REG_DWORD,  2},
	{_T("faceplate_color"),			REG_DWORD, 	0x838587},
	{_T("exit_save_state"),			REG_DWORD,  FALSE},
	{_T("load_files_first"),		REG_DWORD,  FALSE},
	{_T("do_backups"),				REG_DWORD,  FALSE},
	{_T("show_wizard"),				REG_DWORD,  TRUE},
	{_T("sync_cores"),				REG_DWORD,  FALSE},
	{_T("num_accel"),				REG_DWORD,  6},
	{_T("always_on_top"),			REG_DWORD,  FALSE},
	{_T("custom_skin"),				REG_DWORD,  FALSE},
	{_T("skin_path"), 				REG_SZ, 	(LONG_PTR) _T("TI-83P.png")},
	{_T("keymap_path"), 			REG_SZ, 	(LONG_PTR) _T("TI-83PKeymap.png")},
	{_T("startX"),					REG_DWORD,  CW_USEDEFAULT},
	{_T("startY"),					REG_DWORD,  CW_USEDEFAULT},
	{_T("break_on_exe_violation"),	REG_DWORD,  TRUE},
	{_T("break_on_invalid_flash"),	REG_DWORD,  TRUE},
	{_T("auto_turn_on"),			REG_DWORD,	FALSE},
	{_T("num_backup_per_sec"),		REG_DWORD,  2},
	{_T("ram_version"),				REG_DWORD,  0},
	{_T("lcd_delay"),				REG_DWORD,	60},
	//Debugger stuff
	{_T("CPU Status"),				REG_DWORD,	0},
	{_T("Disp Type"),				REG_DWORD,	0},
	{_T("Display"),					REG_DWORD,	0},
	{_T("Flags"),					REG_DWORD,	0},
	{_T("Interrupts"),				REG_DWORD,	0},
	{_T("Memory Map"),				REG_DWORD,	0},
	{_T("Mem0"),					REG_DWORD,	0},
	{_T("Mem1"),					REG_DWORD,	0},
	{_T("Mem2"),					REG_DWORD,	0},
	{_T("Mem3"),					REG_DWORD,	0},
	{_T("Mem4"),					REG_DWORD,	0},
	{_T("Mem5"),					REG_DWORD,	0},
	{_T("MemSelIndex"),				REG_DWORD,	0},
	{_T("NumMemPanes"),				REG_DWORD,	0},
	{_T("NumWatchKey"),				REG_DWORD,	0},
	{_T("Registers"),				REG_DWORD,	0},
	{_T("WatchLocsKey"),			REG_SZ,		(LONG_PTR) _T("")},
	{NULL,							0,			0},
};

HRESULT LoadRegistryDefaults(HKEY hkey) {

	u_int i;
	for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
		DWORD cbData;
		BYTE *lpData;
		switch (regDefaults[i].dwType) {
		case REG_DWORD:
			cbData = sizeof(DWORD);
			lpData = (LPBYTE) &regDefaults[i].Value;
			break;
		case REG_SZ:
			cbData = lstrlen((TCHAR *) regDefaults[i].Value) + 1;
			lpData = (LPBYTE) regDefaults[i].Value;
			break;
		default:
			cbData = 0;
			lpData = NULL;
		}
		RegSetValueEx(
				hkey,
				regDefaults[i].lpValueName,
				0,
				regDefaults[i].dwType,
				lpData,
				cbData);
	}
	return S_OK;
}

LONG_PTR GetKeyData(HKEY hkeyWabbit, LPCTSTR lpszName, BOOL skip_defaults = FALSE) {
	DWORD type;
	DWORD len;
	u_int i;
	for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
		if (!_tcsicmp(regDefaults[i].lpValueName, lpszName))
			break;
	}
	type = regDefaults[i].dwType;
	
	static union {
		DWORD dwResult;
		TCHAR szResult[256];
	} result;
	len = (type == REG_SZ) ? 256 * sizeof(WCHAR) : sizeof(DWORD);
	
	LONG rqvx_res;
	if (regDefaults[i].lpValueName != NULL) {
		TCHAR szKeyName[256];
		StringCbCopy(szKeyName, sizeof(szKeyName), lpszName);

		rqvx_res = RegQueryValueEx(hkeyWabbit, szKeyName, NULL, NULL, (LPBYTE) &result, &len);
		if (rqvx_res == ERROR_FILE_NOT_FOUND) {
			if (type == REG_DWORD) {
				result.dwResult = (DWORD) regDefaults[i].Value;
			} else {
#ifdef _UNICODE
				StringCbCopy(result.szResult, sizeof(result.szResult), (LPWSTR) regDefaults[i].Value);
#else
				WideCharToMultiByte(CP_ACP, 0, (LPWSTR) regDefaults[i].Value, -1, result.szResult, sizeof(result.szResult), NULL, NULL);
#endif
			}
		}
	} else {
		//MessageBox(NULL, "Could not find registry key", lpszName, MB_OK);
		return NULL;
	}
	return (type == REG_SZ) ? (LONG_PTR) result.szResult : result.dwResult;
}

INT_PTR QueryWabbitKey(LPCTSTR lpszName) {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
	
	HKEY hkeyWabbit;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, _T("Wabbitemu"), 0, 
			NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	
	LONG_PTR result = GetKeyData(hkeyWabbit, lpszName);
	
	RegCloseKey(hkeyWabbit);
	RegCloseKey(hkeySoftware);
	
	return result;
}

void QueryKeyMappings() {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
	
	HKEY hkeyWabbit;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, _T("Wabbitemu"), 0, 
			NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	
	int num_entries = (int) QueryWabbitKey(_T("num_accel"));
	ACCEL buf[256];
	DWORD dwCount = sizeof(buf);
	LONG res = RegQueryValueEx(hkeyWabbit, _T("accelerators"), NULL, NULL, (LPBYTE) buf, &dwCount);
	if (res == ERROR_SUCCESS) {
		DestroyAcceleratorTable(haccelmain);
		haccelmain = CreateAcceleratorTable(buf, num_entries);
	}
	RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
	RegOpenKeyEx(hkeySoftware, _T("Wabbitemu"), 0, KEY_ALL_ACCESS, &hkeyWabbit);
	keyprog_t keys[256];
	dwCount = sizeof(keygrps);
	res = RegQueryValueEx(hkeyWabbit, _T("emu_keys"), NULL, NULL, (LPBYTE) keys, &dwCount);
	if (res == ERROR_SUCCESS) {
		memcpy(keygrps, keys, sizeof(keyprog_t) * 256);
	}
	
	RegCloseKey(hkeyWabbit);
	RegCloseKey(hkeySoftware);
	
	return;
}


extern DISPLAY_BASE dispType;
HRESULT LoadRegistrySettings(const LPCALC lpCalc) {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
	
	HKEY hkeyWabbit;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, _T("Wabbitemu"), 0, 
			NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	
	if (dwDisposition == REG_CREATED_NEW_KEY)
		LoadRegistryDefaults(hkeyWabbit);
	
	StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), (TCHAR *) QueryWabbitKey(_T("rom_path")));
	StringCbCopy(lpCalc->skin_path, sizeof(lpCalc->skin_path), (TCHAR *) QueryWabbitKey(_T("skin_path")));
	StringCbCopy(lpCalc->keymap_path, sizeof(lpCalc->keymap_path), (TCHAR *) QueryWabbitKey(_T("keymap_path")));
	lpCalc->SkinEnabled = (BOOL) QueryWabbitKey(_T("skin"));
	lpCalc->bCutout = (BOOL) QueryWabbitKey(_T("cutout"));
	lpCalc->bAlphaBlendLCD = (BOOL) QueryWabbitKey(_T("alphablend_lcd"));
	lpCalc->scale = (int) QueryWabbitKey(_T("screen_scale"));
	lpCalc->FaceplateColor = (COLORREF) QueryWabbitKey(_T("faceplate_color"));
	exit_save_state = (BOOL) QueryWabbitKey(_T("exit_save_state"));
	new_calc_on_load_files = (BOOL) QueryWabbitKey(_T("load_files_first"));
	do_backups = (BOOL) QueryWabbitKey(_T("do_backups"));
	show_wizard = (BOOL) QueryWabbitKey(_T("show_wizard"));
	sync_cores = (BOOL) QueryWabbitKey(_T("sync_cores"));
	startX = (int) QueryWabbitKey(_T("startX"));
	startY = (int) QueryWabbitKey(_T("startY"));
#ifdef WITH_BACKUPS
	num_backup_per_sec = (int) QueryWabbitKey(_T("num_backup_per_sec"));
#endif
	lpCalc->bAlwaysOnTop = (BOOL) QueryWabbitKey(_T("always_on_top"));
	lpCalc->bCustomSkin = (BOOL) QueryWabbitKey(_T("custom_skin"));
	lpCalc->mem_c.ram_version = (int) QueryWabbitKey(_T("ram_version"));
	QueryKeyMappings();

	StringCbCopy(gif_file_name, sizeof(gif_file_name), (TCHAR *) QueryWabbitKey(_T("gif_path")));
	gif_autosave = (BOOL) QueryWabbitKey(_T("gif_autosave"));
	gif_use_increasing = (BOOL) QueryWabbitKey(_T("gif_useinc"));
	break_on_exe_violation = (BOOL) QueryWabbitKey(_T("break_on_exe_violation"));
	break_on_invalid_flash = (BOOL) QueryWabbitKey(_T("break_on_invalid_flash"));
	auto_turn_on = (BOOL) QueryWabbitKey(_T("auto_turn_on"));

	dispType = (DISPLAY_BASE) QueryDebugKey((TCHAR *) DisplayTypeString);
	
	//RegCloseKey(hkeyWabbit);
	hkeyTarget = hkeyWabbit;
	RegCloseKey(hkeySoftware);
	
	return S_OK;
}


//void SaveWabbitKeyA(char *name, int type, void *value) {
//	size_t len;
//
//	if (type == REG_DWORD)
//		len = sizeof(DWORD);
//	else if (type == REG_SZ)
//		StringCbLengthA((char *) value, MAX_PATH, &len);
//	
//	RegSetValueExA(hkeyTarget, name, 0, type, (LPBYTE) value, len);	
//}
//
//void SaveWabbitKeyW(WCHAR *name, int type, void *value) {
//	size_t len;
//
//	if (type == REG_DWORD) {
//		len = sizeof(DWORD);
//	} else if (type == REG_SZ) {
//		StringCbLengthW((WCHAR *) value, MAX_PATH, &len);
//	}
//	
//	RegSetValueExW(hkeyTarget, name, 0, type, (LPBYTE) value, len);	
//}

void SaveWabbitKey(TCHAR *name, int type, void *value) {
	size_t len;

	if (type == REG_DWORD) {
		len = sizeof(DWORD);
	} else if (type == REG_SZ) {
		StringCbLength((TCHAR *) value, MAX_PATH, &len);
	}
	
	RegSetValueEx(hkeyTarget, name, 0, type, (LPBYTE) value, len);	
}


HRESULT SaveRegistrySettings(const LPCALC lpCalc) {
	if (hkeyTarget)
		RegCloseKey(hkeyTarget);
	HKEY hkeyWabbit;
	HRESULT res;
	res = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\Wabbitemu"), 0, KEY_ALL_ACCESS, &hkeyWabbit);
	if (SUCCEEDED(res)) {
		hkeyTarget = hkeyWabbit;
		
		SaveWabbitKey(_T("cutout"), REG_DWORD, &lpCalc->bCutout);
		SaveWabbitKey(_T("alphablend_lcd"), REG_DWORD, &lpCalc->bAlphaBlendLCD);
		SaveWabbitKey(_T("skin"), REG_DWORD, &lpCalc->SkinEnabled);
		SaveWabbitKey(_T("rom_path"), REG_SZ, &lpCalc->rom_path);
		SaveWabbitKey(_T("gif_path"), REG_SZ, &gif_file_name);
		SaveWabbitKey(_T("gif_autosave"), REG_DWORD, &gif_autosave);
		SaveWabbitKey(_T("gif_useinc"), REG_DWORD, &gif_use_increasing);
		SaveWabbitKey(_T("exit_save_state"), REG_DWORD, &exit_save_state);
		SaveWabbitKey(_T("load_files_first"), REG_DWORD, &new_calc_on_load_files);
		SaveWabbitKey(_T("do_backups"), REG_DWORD, &do_backups);
		SaveWabbitKey(_T("show_wizard"), REG_DWORD, &show_wizard);
		SaveWabbitKey(_T("sync_cores"), REG_DWORD, &sync_cores);
		SaveWabbitKey(_T("break_on_exe_violation"), REG_DWORD, &break_on_exe_violation);
		SaveWabbitKey(_T("break_on_invalid_flash"), REG_DWORD, &break_on_invalid_flash);
		SaveWabbitKey(_T("auto_turn_on"), REG_DWORD, &auto_turn_on);

		SaveWabbitKey(_T("faceplate_color"), REG_DWORD, &lpCalc->FaceplateColor);
		SaveWabbitKey(_T("custom_skin"), REG_DWORD, &lpCalc->bCustomSkin);		
		SaveWabbitKey(_T("skin_path"), REG_SZ, &lpCalc->skin_path);
		SaveWabbitKey(_T("keymap_path"), REG_SZ, &lpCalc->keymap_path);
		RECT rc;
		GetWindowRect(lpCalc->hwndFrame, &rc);
		SaveWabbitKey(_T("startX"), REG_DWORD, &rc.left);
		SaveWabbitKey(_T("startY"), REG_DWORD, &rc.top);

		SaveWabbitKey(_T("ram_version"), REG_DWORD, &lpCalc->mem_c.ram_version);
		ACCEL buf[256];
		int numEntries = CopyAcceleratorTable(haccelmain, NULL, 0);
		int nUsed = CopyAcceleratorTable(haccelmain, buf, numEntries);
		DWORD dwCount = nUsed * sizeof(ACCEL);
		DWORD dwType = REG_BINARY;
		LONG res = RegSetValueEx(hkeyWabbit, _T("accelerators"), NULL, dwType, (LPBYTE) buf, dwCount);
		SaveWabbitKey(_T("num_accel"), REG_DWORD, &nUsed);

		dwCount = 256 * sizeof(keyprog_t);
		res = RegSetValueEx(hkeyWabbit, _T("emu_keys"), NULL, dwType, (LPBYTE) keygrps, dwCount);
		
		SaveWabbitKey(_T("shades"), REG_DWORD, &lpCalc->cpu.pio.lcd->shades);
		SaveWabbitKey(_T("lcd_mode"), REG_DWORD, &lpCalc->cpu.pio.lcd->mode);
		DWORD steady = (DWORD) ( 1.0 / lpCalc->cpu.pio.lcd->steady_frame);
		SaveWabbitKey(_T("lcd_freq"), REG_DWORD, &steady);
		SaveWabbitKey(_T("lcd_delay"), REG_DWORD, &lpCalc->cpu.pio.lcd->lcd_delay);
#ifdef WITH_BACKUPS
		SaveWabbitKey(_T("num_backup_per_sec"), REG_DWORD, &num_backup_per_sec);
#endif
		SaveWabbitKey(_T("screen_scale"), REG_DWORD, &lpCalc->scale);

		SaveDebugKey((TCHAR *) DisplayTypeString, REG_DWORD, &dispType);

	}
	RegCloseKey(hkeyWabbit);
	return S_OK;
}

//If you query a REG_SZ you MUST free the buffer it returns
LONG_PTR QueryDebugKey(TCHAR *name) {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);

	HKEY hkeyWabbit, hkeyDebugger;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, _T("Wabbitemu"), 0,
			NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);

	RegCreateKeyEx(hkeyWabbit, _T("Debugger"), 0,
				NULL, REG_OPTION_NON_VOLATILE,
				KEY_ALL_ACCESS, NULL, &hkeyDebugger, &dwDisposition);

	LONG_PTR result = GetKeyData(hkeyDebugger, name);

	RegCloseKey(hkeyDebugger);
	RegCloseKey(hkeyWabbit);
	RegCloseKey(hkeySoftware);

	return result;
}

void SaveDebugKey(TCHAR *name, int type, void *value) {
	HKEY hkeyDebugger;
	HRESULT res;
	res = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\Wabbitemu\\Debugger"), 0, KEY_ALL_ACCESS, &hkeyDebugger);
	if (FAILED(res))
	{
		_tprintf_s(_T("Failed opening Debug registry"));
		return;
	}

	size_t len;
	if (type == REG_DWORD)
		len = sizeof(DWORD);
	else if (type == REG_SZ)
		StringCbLength((TCHAR *) value, 4096, &len);

	int error = RegSetValueEx(hkeyDebugger, name, 0, type, (LPBYTE) value, len);
	if (error != ERROR_SUCCESS)
		error +=1;
}