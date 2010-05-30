#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <strsafe.h>

#define MAX_OBJECT_IDS	256

static TCHAR g_szObjectIDs[MAX_OBJECT_IDS][64];
static int g_ObjectIDCount = 0;

void InitObjectIDs(const TCHAR *lpszFilename) {
	FILE *file = _tfopen(lpszFilename, _T("r"));
	if (file == NULL)
		return;

	TCHAR szBuffer[256];
	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file)) {
		if (_tcsncmp(szBuffer, _T(".obj"), 4) == 0) {
			TCHAR *psz = szBuffer + 5;
			psz = _tcstok(psz, _T("\t ,"));
			StringCbCopy(g_szObjectIDs[g_ObjectIDCount++], sizeof(g_szObjectIDs[0]), psz);
		}
	}
	fclose(file);
}



void PopulateComboBoxWithObjectIDs(HWND hwndCombo) {
	ComboBox_ResetContent(hwndCombo);

	for (int i = 0; i < g_ObjectIDCount; i++) {
		ComboBox_AddString(hwndCombo, g_szObjectIDs[i]);
		ComboBox_SetItemData(hwndCombo, i, g_szObjectIDs[i]);
	}
}