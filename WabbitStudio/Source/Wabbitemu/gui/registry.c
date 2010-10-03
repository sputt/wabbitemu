#include "stdafx.h"

#include "gui.h"
#include "calc.h"
#include "gif.h"

static HKEY hkeyTarget;

static struct {
	LPCTSTR lpValueName;
	DWORD dwType;
	LONG_PTR Value;
} regDefaults[] = {
	{_T("cutout"), 		REG_DWORD, 	0},
	{_T("skin"),		REG_DWORD,	0},
	{_T("version"), 	REG_SZ, 	(LONG_PTR) L"1.5"},
	{_T("rom_path"), 	REG_SZ, 	(LONG_PTR) L"z.rom"},
	{_T("shades"),		REG_DWORD,	6},
	{_T("gif_path"), 	REG_SZ,		(LONG_PTR) L"wabbitemu.gif"},
	{_T("gif_autosave"),REG_DWORD,	0},
	{_T("gif_useinc"),	REG_DWORD,	0},
	{_T("lcd_mode"),	REG_DWORD,	0}, // perfect gray
	{_T("lcd_freq"),	REG_DWORD,	FPS}, // steady freq
	{_T("screen_scale"),REG_DWORD,  2},
	{_T("faceplate_color"), REG_DWORD, 	0x838587},
	{_T("exit_save_state"), REG_DWORD,  0},
	{_T("load_files_first"), REG_DWORD,  1},
	{_T("do_backups"), REG_DWORD,  0},
	{_T("show_wizard"), REG_DWORD,  1},
	{_T("sync_cores"), REG_DWORD,  0},
	{_T("num_keys"),		REG_DWORD,  5},
	{NULL,			0,			0},
};

HRESULT LoadRegistryDefaults(HKEY hkey) {

	u_int i;
	for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
		DWORD cbData;
		BYTE *lpData;
		switch (regDefaults[i].dwType) {
		case REG_DWORD:
			cbData = sizeof(DWORD);
			lpData = (BYTE*) &regDefaults[i].Value;
			break;
		case REG_SZ:
			cbData = lstrlen((char *) regDefaults[i].Value) + 1;
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

INT_PTR QueryWabbitKey(LPCTSTR lpszName) {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
	
	HKEY hkeyWabbit;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, _T("Wabbitemu"), 0, 
			NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	
	DWORD type;
	DWORD len;
	u_int i;
	for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
		if (_tcsicmp(regDefaults[i].lpValueName, lpszName) == 0) break;
	}
	type = regDefaults[i].dwType;
	
	static union {
		DWORD dwResult;
		TCHAR szResult[256];
	} result;
	len = (type == REG_SZ) ? 256 * sizeof(WCHAR) : sizeof(DWORD);
	
	LONG rqvx_res;
	if (regDefaults[i].lpValueName != NULL) {
		WCHAR wszKeyName[256];
#ifdef UNICODE
		StringCbCopy(wszKeyName, sizeof(wszKeyName), lpszName);
#else
		MultiByteToWideChar(CP_ACP, 0, lpszName, -1, wszKeyName, ARRAYSIZE(wszKeyName));
#endif
		rqvx_res = RegQueryValueExW(hkeyWabbit, wszKeyName, NULL, NULL, (LPBYTE) &result, &len);
		if (rqvx_res == ERROR_FILE_NOT_FOUND) {
			if (type == REG_DWORD)
			{
				result.dwResult = (DWORD) regDefaults[i].Value;
			}
			else
			{
#ifdef UNICODE
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
	
	RegCloseKey(hkeyWabbit);
	RegCloseKey(hkeySoftware);
	
	return (type == REG_SZ) ? (LONG_PTR) result.szResult : result.dwResult;
}

HRESULT LoadRegistrySettings(void) {
	HKEY hkeySoftware;
	RegOpenKeyEx(HKEY_CURRENT_USER, "software", 0, KEY_ALL_ACCESS, &hkeySoftware);
	
	HKEY hkeyWabbit;
	DWORD dwDisposition;
	RegCreateKeyEx(hkeySoftware, "Wabbitemu", 0, 
			NULL, REG_OPTION_NON_VOLATILE, 
			KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	
	if (dwDisposition == REG_CREATED_NEW_KEY)
		LoadRegistryDefaults(hkeyWabbit);
	
	strcpy_s(calcs[gslot].rom_path, (char *) QueryWabbitKey("rom_path"));
	calcs[gslot].SkinEnabled = (BOOL) QueryWabbitKey("skin");
	calcs[gslot].bCutout = (BOOL) QueryWabbitKey("cutout");
	calcs[gslot].Scale = (int) QueryWabbitKey("screen_scale");
	calcs[gslot].FaceplateColor = (COLORREF) QueryWabbitKey("faceplate_color");
	exit_save_state = (BOOL) QueryWabbitKey("exit_save_state");
	load_files_first = (BOOL) QueryWabbitKey("load_files_first");
	do_backups = (BOOL) QueryWabbitKey("do_backups");
	show_wizard = (BOOL) QueryWabbitKey("show_wizard");
	sync_cores = (BOOL) QueryWabbitKey("sync_cores");
	int num_entries = (int) QueryWabbitKey("num_keys");
	//need to load accelerators
	// querywabbitkey doesnt work because its a REG_BINARY
	/*ACCEL buf[256];
	DWORD dwCount = sizeof(buf);
	LONG res = RegQueryValueEx(hkeyWabbit, "accelerators", NULL, NULL, (LPBYTE)buf, &dwCount);
	if (res == ERROR_SUCCESS){
		DestroyAcceleratorTable(haccelmain);
		haccelmain = CreateAcceleratorTable(buf, num_entries);
	}*/
	/*
	 * 		{"gif_path", 	REG_SZ,		"wabbitemu.gif"},
		{"gif_autosave",REG_DWORD,	0},
		{"gif_useinc",	REG_DWORD,	0},
		*/
	
	strcpy_s(gif_file_name, (char *) QueryWabbitKey("gif_path"));
	gif_autosave = (BOOL) QueryWabbitKey("gif_autosave");
	gif_use_increasing = (BOOL) QueryWabbitKey("gif_useinc");
	
	//RegCloseKey(hkeyWabbit);
	hkeyTarget = hkeyWabbit;
	RegCloseKey(hkeySoftware);

	calcs[gslot].bCutout = (BOOL) QueryWabbitKey("cutout");
	
	return S_OK;
}

void SaveWabbitKey(char *name, int type, void *value) {
	int len;

	if (type == REG_DWORD) {
		len = sizeof(DWORD);
	} else if (type == REG_SZ) {
		len = (int) strlen((char *) value) + 1;
	}
	
	RegSetValueEx(hkeyTarget, name, 0, type, (LPBYTE) value, len);
	
}


HRESULT SaveRegistrySettings(const LPCALC lpCalc) {
	if (hkeyTarget)
		RegCloseKey(hkeyTarget);
	HKEY hkeyWabbit;
	HRESULT res;
	res = RegOpenKeyEx(HKEY_CURRENT_USER, "software\\Wabbitemu", 0, KEY_ALL_ACCESS, &hkeyWabbit);
	if (SUCCEEDED(res)) {
		hkeyTarget = hkeyWabbit;
		
		SaveWabbitKey("cutout", REG_DWORD, &lpCalc->bCutout);
		SaveWabbitKey("skin", REG_DWORD, &lpCalc->SkinEnabled);
		SaveWabbitKey("rom_path", REG_SZ, &lpCalc->rom_path);
		SaveWabbitKey("gif_path", REG_SZ, &gif_file_name);
		SaveWabbitKey("gif_autosave", REG_DWORD, &gif_autosave);
		SaveWabbitKey("gif_useinc", REG_DWORD, &gif_use_increasing);
		SaveWabbitKey("exit_save_state", REG_DWORD, &exit_save_state);
		SaveWabbitKey("load_files_first", REG_DWORD, &load_files_first);
		SaveWabbitKey("do_backups", REG_DWORD, &do_backups);
		SaveWabbitKey("show_wizard", REG_DWORD, &show_wizard);
		SaveWabbitKey("sync_cores", REG_DWORD, &sync_cores);

		SaveWabbitKey("faceplate_color", REG_DWORD, &lpCalc->FaceplateColor);
		SaveWabbitKey("custom_skin", REG_DWORD, &lpCalc->bCustomSkin);
		SaveWabbitKey("skin_path", REG_SZ, &lpCalc->skin_path);
		SaveWabbitKey("keymap_path", REG_SZ, &lpCalc->keymap_path);
		/*ACCEL buf[256];
		DWORD dwCount = sizeof(buf);
		DWORD dwType = NULL;
		LONG res = RegQueryValueEx(hkeyWabbit, "accelerators", NULL, &dwType,
			(LPBYTE)buf, &dwCount);
		if (res == ERROR_SUCCESS){
			DestroyAcceleratorTable(haccelmain);
			haccelmain = CreateAcceleratorTable(buf, dwCount);
		}*/
		
		SaveWabbitKey("shades", REG_DWORD, &lpCalc->cpu.pio.lcd->shades);
		SaveWabbitKey("lcd_mode", REG_DWORD, &lpCalc->cpu.pio.lcd->mode);
		DWORD steady = (DWORD) ( 1.0 / calcs[gslot].cpu.pio.lcd->steady_frame);
		SaveWabbitKey("lcd_freq", REG_DWORD, &steady);
		
		SaveWabbitKey("screen_scale", REG_DWORD, &lpCalc->Scale);

	}
	RegCloseKey(hkeyWabbit);
	return S_OK;
}