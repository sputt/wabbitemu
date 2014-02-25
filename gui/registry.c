#include "stdafx.h"

#include "gui.h"
#include "calc.h"
#include "gif.h"
#include "registry.h"
#include "dbcommon.h"
#include "expandpane.h"

extern keyprog_t keysti86[256];
extern keyprog_t keysti83[256];
extern keyprog_t defaultkeysti86[256];
extern keyprog_t defaultkeysti83[256];
TCHAR *verString = _T("1.7.2.11");

static HKEY hkeyTarget = NULL;

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
	{_T("gif_framerate"),			REG_DWORD,	4},
	{_T("gif_gray_size"),			REG_DWORD,  2},
	{_T("gif_color_size"),			REG_DWORD,  1},
	{_T("lcd_mode"),				REG_DWORD,	0},		// perfect gray
	{_T("lcd_freq"),				REG_DWORD,	FPS},	// steady freq
	{_T("screen_scale"),			REG_DWORD,  2},
	{_T("skin_scale"),				REG_SZ,		(LONG_PTR) _T("1.0")},
	{_T("faceplate_color"),			REG_DWORD, 	0x838587},
	{_T("exit_save_state"),			REG_DWORD,  FALSE},
	{_T("load_files_first"),		REG_DWORD,  FALSE},
	{_T("do_backups"),				REG_DWORD,  FALSE},
	{_T("sync_cores"),				REG_DWORD,  FALSE},
	{_T("num_accel"),				REG_DWORD,  6},
	{_T("always_on_top"),			REG_DWORD,  FALSE},
	{_T("tios_debug"),				REG_DWORD,  TRUE},
	{_T("custom_skin"),				REG_DWORD,  FALSE},
	{_T("skin_path"), 				REG_SZ, 	(LONG_PTR) _T("TI-83P.png")},
	{_T("keymap_path"), 			REG_SZ, 	(LONG_PTR) _T("TI-83PKeymap.png")},
	{_T("startX"),					REG_DWORD,  CW_USEDEFAULT},
	{_T("startY"),					REG_DWORD,  CW_USEDEFAULT},
	{_T("break_on_exe_violation"),	REG_DWORD,  FALSE},
	{_T("break_on_invalid_flash"),	REG_DWORD,  FALSE},
	{_T("auto_turn_on"),			REG_DWORD,	FALSE},
	{_T("num_backup_per_sec"),		REG_DWORD,  2},
	{_T("ram_version"),				REG_DWORD,  0},
	{_T("lcd_delay"),				REG_DWORD,	60},
	{_T("check_updates"),			REG_DWORD,	TRUE},
	{_T("show_whats_new"),			REG_DWORD,	FALSE},
	//Debugger stuff
	{_T("CPU Status"),				REG_DWORD,	0},
	{_T("Disp Type"),				REG_DWORD,	0},
	{_T("Display"),					REG_DWORD,	0},
	{_T("Flags"),					REG_DWORD,	0},
	{_T("Interrupts"),				REG_DWORD,	0},
	{_T("Memory Map"),				REG_DWORD,	0},
	{_T("Keyboard"),				REG_DWORD,	0},
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
	if (portable_mode) {
		return S_OK;
	}
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
	len = (type == REG_SZ) ? 256 * sizeof(TCHAR) : sizeof(DWORD);
	
	LONG rqvx_res;
	if (regDefaults[i].lpValueName != NULL) {
		TCHAR szKeyName[256];
		StringCbCopy(szKeyName, sizeof(szKeyName), lpszName);

		rqvx_res = RegQueryValueEx(hkeyWabbit, szKeyName, NULL, NULL, (LPBYTE) &result, &len);
		if (rqvx_res == ERROR_FILE_NOT_FOUND) {
			if (type == REG_DWORD) {
				result.dwResult = (DWORD) regDefaults[i].Value;
			} else {
				StringCbCopy(result.szResult, sizeof(result.szResult), (LPTSTR) regDefaults[i].Value);
			}
		}
	} else {
		//MessageBox(NULL, "Could not find registry key", lpszName, MB_OK);
		return NULL;
	}
	return (type == REG_SZ) ? (LONG_PTR) result.szResult : result.dwResult;
}

static size_t FindSettingsKey(FILE *settingsFile, LPCTSTR lpszName) {
	while (!feof(settingsFile)) {
		u_int i;
		TCHAR nameBuffer[256];
		size_t size = 0;
		_ftscanf_s(settingsFile, _T("%s | %d | "), nameBuffer, &size);
		if (!_tcscmp(nameBuffer, lpszName)) {
			return size;
		}
		if (size) {
			for (i = 0; i < size - 1; i++) {
				fgetc(settingsFile);
			}
		}
	}
	return 0;
}

INT_PTR QueryWabbitKey(LPCTSTR lpszName) {
	if (portable_mode) {
		u_int i;
		FILE *file;
		_tfopen_s(&file, portSettingsPath, _T("rb"));
		for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
			if (!_tcsicmp(regDefaults[i].lpValueName, lpszName))
				break;
		}
		size_t size = FindSettingsKey(file, lpszName);
		if (size || regDefaults[i].lpValueName != NULL) {
			DWORD type;
			DWORD len;
			type = regDefaults[i].dwType;
	
			static union {
				DWORD dwResult;
				TCHAR szResult[256];
			} result;
			len = (type == REG_SZ) ? 256 * sizeof(TCHAR) : sizeof(DWORD);

			if (regDefaults[i].lpValueName != NULL) {
				if (size) {
					if (type == REG_DWORD) {
						_ftscanf_s(file, _T("%d\r\n"), &result.dwResult);
					} else {
						fread_s(&result.szResult, 256 * sizeof(TCHAR), sizeof(TCHAR), size, file);
						result.szResult[size] = '\0';
					}
				} else {
					if (type == REG_DWORD){
						result.dwResult = regDefaults[i].Value;
					} else {
						StringCchCopy(result.szResult, 256, (LPCTSTR) regDefaults[i].Value);
					}
				}
			} else {
				return NULL;
			}

				if (file) {
					fclose(file);
				}

			return (type == REG_SZ) ? (LONG_PTR) result.szResult : result.dwResult;
		} else {
			return NULL;
		}
	} else {
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
}

void QueryKeyMappings() {
	if (portable_mode) {
		FILE *file;
		_tfopen_s(&file, portSettingsPath, _T("rb"));
		if (!file) {
			return;
		}

		size_t size = FindSettingsKey(file, _T("accelerators"));
		int num_entries = (int) QueryWabbitKey(_T("num_accel"));
		ACCEL buf[256];
		if (size) {
			char *buf_ptr = (char *) &buf;
			for (u_int i = 0; i < size - 1; i++) {
				*buf_ptr++ = fgetc(file);
			}
			DestroyAcceleratorTable(haccelmain);
			haccelmain = CreateAcceleratorTable(buf, num_entries);
		}

		keyprog_t keys[256];
		size = FindSettingsKey(file, _T("emu_keys"));
		if (size) {
			char *buf_ptr = (char *) &keys;
			for (u_int i = 0; i < size - 1; i++) {
				*buf_ptr++ = fgetc(file);
			}
			memcpy(keysti83, keys, sizeof(keyprog_t) * 256);
		} else {
			memcpy(keysti83, defaultkeysti83, sizeof(keyprog_t) * 256);
		}

		size = FindSettingsKey(file, _T("emu_keys"));
		if (size) {
			char *buf_ptr = (char *) &keys;
			for (u_int i = 0; i < size - 1; i++) {
				*buf_ptr++ = fgetc(file);
			}
			memcpy(keysti86, keys, sizeof(keyprog_t) * 256);
		} else {
			memcpy(keysti86, defaultkeysti86, sizeof(keyprog_t) * 256);
		}

		fclose(file);
	} else {
		HKEY hkeySoftware = NULL;
		HKEY hkeyWabbit = NULL;
		LONG res;
		RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
	
		DWORD dwDisposition;
		RegCreateKeyEx(hkeySoftware, _T("Wabbitemu"), 0, 
				NULL, REG_OPTION_NON_VOLATILE, 
				KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
		int num_entries = (int) QueryWabbitKey(_T("num_accel"));
		ACCEL buf[256];
		DWORD dwCount = sizeof(buf);
		res = RegQueryValueEx(hkeyWabbit, _T("accelerators"), NULL, NULL, (LPBYTE) buf, &dwCount);
		if (res == ERROR_SUCCESS) {
			DestroyAcceleratorTable(haccelmain);
			haccelmain = CreateAcceleratorTable(buf, num_entries);
		}
		RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
		RegOpenKeyEx(hkeySoftware, _T("Wabbitemu"), 0, KEY_ALL_ACCESS, &hkeyWabbit);
		keyprog_t keys[256];
		dwCount = sizeof(keys);
		res = RegQueryValueEx(hkeyWabbit, _T("emu_keys"), NULL, NULL, (LPBYTE) keys, &dwCount);
		if (res == ERROR_SUCCESS) {
			memcpy(keysti83, keys, sizeof(keyprog_t) * 256);
		} else {
			memcpy(keysti83, defaultkeysti83, sizeof(keyprog_t) * 256);
		}
		dwCount = sizeof(keys);
		res = RegQueryValueEx(hkeyWabbit, _T("emu_keys86"), NULL, NULL, (LPBYTE) keys, &dwCount);
		if (res == ERROR_SUCCESS) {
			memcpy(keysti86, keys, sizeof(keyprog_t) * 256);
		}else {
			memcpy(keysti86, defaultkeysti86, sizeof(keyprog_t) * 256);
		}
	
		RegCloseKey(hkeyWabbit);
		RegCloseKey(hkeySoftware);
	}	
	return;
}

BOOL CheckSetIsPortableMode() {
	GetCurrentDirectory(MAX_PATH, portSettingsPath);
	StringCchCat(portSettingsPath, MAX_PATH, _T("\\Wabbitemu.dat"));
	FILE *file;
	_tfopen_s(&file, portSettingsPath, _T("r"));
	if (file) {
		portable_mode = TRUE;
		fclose(file);
		return TRUE;
	}
	return FALSE;
}

HRESULT LoadRegistrySettings(const LPCALC lpCalc) {
	HKEY hkeySoftware = NULL;
	if (!portable_mode) {
		RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_ALL_ACCESS, &hkeySoftware);
	}
	
	HKEY hkeyWabbit = NULL;
	DWORD dwDisposition = 0;
	if (!portable_mode) {
		RegCreateKeyEx(hkeySoftware, _T("Wabbitemu"), 0, 
				NULL, REG_OPTION_NON_VOLATILE, 
				KEY_ALL_ACCESS, NULL, &hkeyWabbit, &dwDisposition);
	}
	
	if (dwDisposition == REG_CREATED_NEW_KEY) {
		LoadRegistryDefaults(hkeyWabbit);
	}

	TCHAR skinScale[32];
	
	StringCbCopy(lpCalc->rom_path, sizeof(lpCalc->rom_path), (TCHAR *) QueryWabbitKey(_T("rom_path")));
	StringCbCopy(lpCalc->skin_path, sizeof(lpCalc->skin_path), (TCHAR *) QueryWabbitKey(_T("skin_path")));
	StringCbCopy(lpCalc->keymap_path, sizeof(lpCalc->keymap_path), (TCHAR *) QueryWabbitKey(_T("keymap_path")));
	lpCalc->bSkinEnabled = (BOOL) QueryWabbitKey(_T("skin"));
	lpCalc->bCutout = (BOOL) QueryWabbitKey(_T("cutout"));
	lpCalc->bAlphaBlendLCD = (BOOL) QueryWabbitKey(_T("alphablend_lcd"));
	lpCalc->scale = (int) QueryWabbitKey(_T("screen_scale"));
	StringCbCopy(skinScale, sizeof(skinScale), (TCHAR *) QueryWabbitKey(_T("skin_scale")));
	lpCalc->skin_scale = atof(skinScale);
	lpCalc->FaceplateColor = (COLORREF) QueryWabbitKey(_T("faceplate_color"));
	exit_save_state = (BOOL) QueryWabbitKey(_T("exit_save_state"));
	new_calc_on_load_files = (BOOL) QueryWabbitKey(_T("load_files_first"));
	do_backups = (BOOL) QueryWabbitKey(_T("do_backups"));
	sync_cores = (BOOL) QueryWabbitKey(_T("sync_cores"));
	startX = (int) QueryWabbitKey(_T("startX"));
	startY = (int) QueryWabbitKey(_T("startY"));
	lpCalc->bAlwaysOnTop = (BOOL) QueryWabbitKey(_T("always_on_top"));
	lpCalc->bCustomSkin = (BOOL) QueryWabbitKey(_T("custom_skin"));
	lpCalc->mem_c.ram_version = (int) QueryWabbitKey(_T("ram_version"));
	lpCalc->bTIOSDebug = (BOOL) QueryWabbitKey(_T("tios_debug"));
	QueryKeyMappings();

	StringCbCopy(screenshot_file_name, sizeof(screenshot_file_name), (TCHAR *) QueryWabbitKey(_T("gif_path")));
	screenshot_autosave = (BOOL) QueryWabbitKey(_T("gif_autosave"));
	screenshot_use_increasing = (BOOL) QueryWabbitKey(_T("gif_useinc"));
	gif_base_delay_start = (BOOL) QueryWabbitKey(_T("gif_framerate"));
	screenshot_size = (int)QueryWabbitKey(_T("gif_gray_size"));
	screenshot_color_size = (int)QueryWabbitKey(_T("gif_color_size"));
	break_on_exe_violation = (BOOL) QueryWabbitKey(_T("break_on_exe_violation"));
	break_on_invalid_flash = (BOOL) QueryWabbitKey(_T("break_on_invalid_flash"));
	auto_turn_on = (BOOL) QueryWabbitKey(_T("auto_turn_on"));
	check_updates = (BOOL) QueryWabbitKey(_T("check_updates"));
	show_whats_new = (BOOL) QueryWabbitKey(_T("show_whats_new"));
	
	if (!portable_mode) {
		hkeyTarget = hkeyWabbit;
		RegCloseKey(hkeySoftware);
	}
	
	return S_OK;
}

void SaveWabbitKey(TCHAR *name, int type, void *value) {
	size_t len;

	if (type == REG_DWORD) {
		len = sizeof(DWORD);
	} else if (type == REG_SZ) {
		StringCbLength((TCHAR *) value, MAX_PATH, &len);
	}
	if (portable_mode) {
		FILE *file;
		_tfopen_s(&file, portSettingsPath, _T("ab"));
		if (!file) {
			return;
		}
		if (type == REG_DWORD) {
			_ftprintf(file, _T("%s | %d | %d\r\n"), name, len, *(LPDWORD) value);
		} else {
			_ftprintf(file, _T("%s | %d | %s\r\n"), name, len, value);
		}
		fclose(file);
	} else {	
		RegSetValueEx(hkeyTarget, name, 0, type, (LPBYTE) value, len);	
	}
}


HRESULT SaveRegistrySettings(const LPCALC lpCalc) {
	if (hkeyTarget && !portable_mode) {
		RegCloseKey(hkeyTarget);
		hkeyTarget = NULL;
	}

	if (portable_mode) {
		FILE *file;
		_tfopen_s(&file, portSettingsPath, _T("w"));
		if (file) {
			fclose(file);
		}
	}
	
	HKEY hkeyWabbit = NULL;
	HRESULT res = 0;
	if (!portable_mode) {
		res = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\Wabbitemu"), 0, KEY_ALL_ACCESS, &hkeyWabbit);
	}
	if (portable_mode || SUCCEEDED(res)) {
		hkeyTarget = hkeyWabbit;
		
		SaveWabbitKey(_T("cutout"), REG_DWORD, &lpCalc->bCutout);
		SaveWabbitKey(_T("alphablend_lcd"), REG_DWORD, &lpCalc->bAlphaBlendLCD);
		SaveWabbitKey(_T("skin"), REG_DWORD, &lpCalc->bSkinEnabled);
		SaveWabbitKey(_T("rom_path"), REG_SZ, &lpCalc->rom_path);
		SaveWabbitKey(_T("gif_path"), REG_SZ, &screenshot_file_name);
		SaveWabbitKey(_T("gif_autosave"), REG_DWORD, &screenshot_autosave);
		SaveWabbitKey(_T("gif_useinc"), REG_DWORD, &screenshot_use_increasing);
		SaveWabbitKey(_T("gif_framerate"), REG_DWORD, &gif_base_delay_start);
		SaveWabbitKey(_T("gif_gray_size"), REG_DWORD, &screenshot_size);
		SaveWabbitKey(_T("gif_color_size"), REG_DWORD, &screenshot_color_size);
		SaveWabbitKey(_T("exit_save_state"), REG_DWORD, &exit_save_state);
		SaveWabbitKey(_T("load_files_first"), REG_DWORD, &new_calc_on_load_files);
		SaveWabbitKey(_T("do_backups"), REG_DWORD, &do_backups);
		SaveWabbitKey(_T("sync_cores"), REG_DWORD, &sync_cores);
		SaveWabbitKey(_T("break_on_exe_violation"), REG_DWORD, &break_on_exe_violation);
		SaveWabbitKey(_T("break_on_invalid_flash"), REG_DWORD, &break_on_invalid_flash);
		SaveWabbitKey(_T("auto_turn_on"), REG_DWORD, &auto_turn_on);
		SaveWabbitKey(_T("check_updates"), REG_DWORD, &check_updates);
		SaveWabbitKey(_T("show_whats_new"), REG_DWORD, &show_whats_new);

		SaveWabbitKey(_T("faceplate_color"), REG_DWORD, &lpCalc->FaceplateColor);
		SaveWabbitKey(_T("custom_skin"), REG_DWORD, &lpCalc->bCustomSkin);		
		SaveWabbitKey(_T("skin_path"), REG_SZ, &lpCalc->skin_path);
		SaveWabbitKey(_T("keymap_path"), REG_SZ, &lpCalc->keymap_path);
		WINDOWPLACEMENT wp;
		GetWindowPlacement(lpCalc->hwndFrame, &wp);
		SaveWabbitKey(_T("startX"), REG_DWORD, &wp.rcNormalPosition.left);
		SaveWabbitKey(_T("startY"), REG_DWORD, &wp.rcNormalPosition.top);
		SaveWabbitKey(_T("always_on_top"), REG_DWORD, &lpCalc->bAlwaysOnTop);

		SaveWabbitKey(_T("ram_version"), REG_DWORD, &lpCalc->mem_c.ram_version);
		SaveWabbitKey(_T("tios_debug"), REG_DWORD, &lpCalc->bTIOSDebug);
		ACCEL buf[256];
		int numEntries = CopyAcceleratorTable(haccelmain, NULL, 0);
		int nUsed = CopyAcceleratorTable(haccelmain, buf, numEntries);
		DWORD dwCount = nUsed * sizeof(ACCEL);
		DWORD dwType = REG_BINARY;
		LONG res = RegSetValueEx(hkeyWabbit, _T("accelerators"), NULL, dwType, (LPBYTE) buf, dwCount);
		SaveWabbitKey(_T("num_accel"), REG_DWORD, &nUsed);

		dwCount = 256 * sizeof(keyprog_t);
		res = RegSetValueEx(hkeyWabbit, _T("emu_keys"), NULL, dwType, (LPBYTE) keysti83, dwCount);
		res = RegSetValueEx(hkeyWabbit, _T("emu_keys86"), NULL, dwType, (LPBYTE) keysti86, dwCount);
		
		if (lpCalc->model != TI_84PCSE) {
			LCD_t *lcd = (LCD_t *)lpCalc->cpu.pio.lcd;
			SaveWabbitKey(_T("shades"), REG_DWORD, &lcd->shades);
			SaveWabbitKey(_T("lcd_mode"), REG_DWORD, &lcd->mode);
			DWORD steady = (DWORD)(1.0 / lcd->steady_frame);
			SaveWabbitKey(_T("lcd_freq"), REG_DWORD, &steady);
			SaveWabbitKey(_T("lcd_delay"), REG_DWORD, &lcd->lcd_delay);
		}
		SaveWabbitKey(_T("screen_scale"), REG_DWORD, &lpCalc->scale);
		TCHAR scaleString[32];
		StringCbPrintf(scaleString, sizeof(scaleString), _T("%lf"), lpCalc->skin_scale / lpCalc->default_skin_scale);
		SaveWabbitKey(_T("skin_scale"), REG_SZ, &scaleString);

		TCHAR versionBuffer[32];
		GetFileCurrentVersionString(versionBuffer, sizeof(versionBuffer));
		SaveWabbitKey(_T("version"), REG_SZ, versionBuffer);
	}
	RegCloseKey(hkeyWabbit);
	return S_OK;
}

//If you query a REG_SZ you MUST free the buffer it returns
LONG_PTR QueryDebugKey(TCHAR *name) {
	if (portable_mode) {
		u_int i;
		FILE *file;
		_tfopen_s(&file, portSettingsPath, _T("rb"));
		for (i = 0; regDefaults[i].lpValueName != NULL; i++) {
			if (!_tcsicmp(regDefaults[i].lpValueName, name))
				break;
		}
		size_t size = FindSettingsKey(file, name);
		if (size || regDefaults[i].lpValueName != NULL) {
			DWORD type;
			DWORD len;
			type = regDefaults[i].dwType;
	
			static union {
				DWORD dwResult;
				TCHAR szResult[256];
			} result;
			len = (type == REG_SZ) ? 256 * sizeof(TCHAR) : sizeof(DWORD);

			if (regDefaults[i].lpValueName != NULL) {
				if (size) {
					if (type == REG_DWORD) {
						_ftscanf_s(file, _T("%d\r\n"), &result.dwResult);
					} else {
						_ftscanf_s(file, _T("%s\r\n"), &result.szResult);
					}
				} else {
					if (type == REG_DWORD){
						result.dwResult = regDefaults[i].Value;
					} else {
						StringCchCopy(result.szResult, 256, (LPCTSTR) regDefaults[i].Value);
					}
				}
			} else {
				return NULL;
			}

				if (file) {
					fclose(file);
				}

			return (type == REG_SZ) ? (LONG_PTR) result.szResult : result.dwResult;
		} else {
			return NULL;
		}
	} else {
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
}

void SaveDebugKey(TCHAR *name, int type, void *value) {
	size_t len;
	if (type == REG_DWORD) {
		len = sizeof(DWORD);
	} else if (type == REG_SZ) {
		StringCbLength((TCHAR *) value, 4096, &len);
	}
	if (portable_mode) {
		/*FILE *file;
		TCHAR buffer[MAX_PATH];
		GetPortableSetingsFilePath(buffer);
		_tfopen_s(&file, buffer, _T("ab"));
		if (!file) {
			return;
		}
		if (type == REG_DWORD) {
			_ftprintf(file, _T("%s | %d | %d\r\n"), name, len, *(LPDWORD) value);
		} else {
			_ftprintf(file, _T("%s | %d | %s\r\n"), name, len, value);
		}
		fclose(file);*/
	} else {
		HKEY hkeyDebugger;
		HRESULT res;
		res = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\Wabbitemu\\Debugger"), 0, KEY_ALL_ACCESS, &hkeyDebugger);
		if (FAILED(res)) {
			_tprintf_s(_T("Failed opening Debug registry"));
			return;
		}

		int error = RegSetValueEx(hkeyDebugger, name, 0, type, (LPBYTE) value, len);
		if (error != ERROR_SUCCESS) {
			error +=1;
		}
	}
}