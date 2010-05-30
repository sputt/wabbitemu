#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <msxml6.h>
#include "MapHierarchy.h"
#include "MapObjects.h"
#include "TileSets.h"
#include "utils.h"
#include "MapCompressor.h"

extern MAPSETSETTINGS g_MapSet;
extern HINSTANCE g_hInstance;
extern HDC g_hdcTiles;

#define DEFAULT_TABLE_FILENAME	_T("default_Table.asm")
#define MAP_DATA_FILENAME		_T("mapeditor_data.asm")

HRESULT LoadMapDefaults(TCHAR *szFilename, LPMAPVIEWSETTINGS lpmvs);


/*
 * 
 */
static BOOL ReadFileMapData(FILE *file, BYTE *lpMapData, int cbMapData) {
	TCHAR szBuffer[256];
	int i = 0;

	while (_fgetts(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), file) && i < cbMapData) {
		DWORD dwValue;
		if (_stscanf(szBuffer, _T("\t.db $%02X,"), &dwValue) == 0)
			continue;

		lpMapData[i++] = (BYTE) dwValue;

		TCHAR *psz = _tcstok(szBuffer, _T(","));
		psz = _tcstok(NULL, _T(","));
		while (i < cbMapData && psz != NULL) {
			if (_stscanf(psz, _T("$%02X"), &dwValue) > 0)
				lpMapData[i++] = (BYTE) dwValue;
			psz = _tcstok(NULL, _T(","));
		}
	}

	return (i == cbMapData) ? TRUE : FALSE;
}

/*
 * Read one whole map from a file
 */
static BOOL ReadMapData(TCHAR *szFilename, TCHAR *szMapName, BYTE *lpMapData, int cbMapData) {
	FILE *file = _tfopen(szFilename, _T("r"));
	TCHAR szLabel[256];

	if (file == NULL)
		return FALSE;

	if (szMapName != NULL) {
		StringCbCopy(szLabel, sizeof(szLabel), szMapName);
		StringCbCat(szLabel, sizeof(szLabel), _T("_map:\n"));
		SeekFileLabel(file, szLabel);
	}
	return ReadFileMapData(file, lpMapData, cbMapData);
}

LPMAPVIEWSETTINGS LoadMap(HWND hwndParent, TCHAR *szMapFilename, TCHAR *szDefaultsFilename, TCHAR *szName) {
	LPMAPVIEWSETTINGS lpmvs = (LPMAPVIEWSETTINGS) malloc(sizeof(MAPVIEWSETTINGS));
	ZeroMemory(lpmvs, sizeof(MAPVIEWSETTINGS));
	lpmvs->pMapData = (LPBYTE) malloc(g_MapSet.cx * g_MapSet.cy);
	if (szName != NULL)
		StringCbCopy(lpmvs->szName, sizeof(lpmvs->szName), szName);
	StringCbCopy(lpmvs->szFilename, sizeof(lpmvs->szFilename), szMapFilename);
	if (szDefaultsFilename != NULL)
		StringCbCopy(lpmvs->szDefaultsFilename, sizeof(lpmvs->szDefaultsFilename), szDefaultsFilename);

	ReadMapData(szMapFilename, szName, lpmvs->pMapData, g_MapSet.cx * g_MapSet.cy);

	lpmvs->iHot = -1;

	LoadMapDefaults(szDefaultsFilename, lpmvs);

	lpmvs->hwndMap = CreateMapView(hwndParent);
	SetMapViewSettings(lpmvs->hwndMap, lpmvs);
	return lpmvs;
}


LPMAPVIEWSETTINGS LoadMapFromNode(IXMLDOMNode *pXMLNode, HWND hwndParent, LPMAPVIEWSETTINGS prev_lpmvs, MP_ORIENTATION Orientation) {
	BSTR bstr;
	IXMLDOMNode *pXMLNext, *pXMLMap, *pXMLObj, *pXMLTiles, *pXMLName;
	TCHAR szMapFilename[MAX_PATH], szObjFilename[MAX_PATH], szName[64];

	pXMLNode->selectSingleNode(L"@map", &pXMLMap);
	if (pXMLMap != NULL) {
		pXMLMap->get_text(&bstr);
#ifdef _UNICODE
		StringCbCopy(szMapFilename, sizeof(szMapFilename), bstr);
#else
		WideCharToMultiByte(CP_ACP, 0, bstr, -1, (LPSTR) szMapFilename, sizeof(szMapFilename), NULL, NULL);
#endif
		SysFreeString(bstr);
		pXMLMap->Release();
	}

	pXMLNode->selectSingleNode(L"@obj", &pXMLObj);
	if (pXMLObj != NULL) {
		pXMLObj->get_text(&bstr);
#ifdef _UNICODE
		StringCbCopy(szObjFilename, sizeof(szObjFilename), bstr);
#else
		WideCharToMultiByte(CP_ACP, 0, bstr, -1, (LPSTR) szObjFilename, sizeof(szObjFilename), NULL, NULL);
#endif
		SysFreeString(bstr);
		pXMLObj->Release();
	}

	pXMLNode->selectSingleNode(L"@name", &pXMLName);
	if (pXMLName != NULL) {
		pXMLName->get_text(&bstr);
		StringCbCopy(szName, sizeof(szName), bstr);
		SysFreeString(bstr);
		pXMLName->Release();
	}

	LPMAPVIEWSETTINGS lpmvs = LoadMap(hwndParent, szMapFilename, szObjFilename, szName);

	pXMLNode->selectSingleNode(L"@tiles", &pXMLTiles);
	if (pXMLTiles != NULL) {
		pXMLTiles->get_text(&bstr);
		lpmvs->hdcTiles = GetTileset(bstr);
		StringCbCopy(lpmvs->szTilesFilename, sizeof(lpmvs->szTilesFilename), bstr);
		SysFreeString(bstr);
		pXMLTiles->Release();
	}

	AddMap(prev_lpmvs, lpmvs, Orientation);

	pXMLNode->selectSingleNode(L"map[@pos = \"left\"]", &pXMLNext);
	if (pXMLNext != NULL) {
		LoadMapFromNode(pXMLNext, hwndParent, lpmvs, MP_LEFT);
		pXMLNext->Release();
	}
	pXMLNode->selectSingleNode(L"map[@pos = \"right\"]", &pXMLNext);
	if (pXMLNext != NULL) {
		LoadMapFromNode(pXMLNext, hwndParent, lpmvs, MP_RIGHT);
		pXMLNext->Release();
	}
	pXMLNode->selectSingleNode(L"map[@pos = \"above\"]", &pXMLNext);
	if (pXMLNext != NULL) {
		LoadMapFromNode(pXMLNext, hwndParent, lpmvs, MP_ABOVE);
		pXMLNext->Release();
	}
	pXMLNode->selectSingleNode(L"map[@pos = \"below\"]", &pXMLNext);
	if (pXMLNext != NULL) {
		LoadMapFromNode(pXMLNext, hwndParent, lpmvs, MP_BELOW);
		pXMLNext->Release();
	}

	return lpmvs;
}

static void LoadProp(IXMLDOMNode *pXMLNode, int *pprop, WCHAR *szProp) {
	IXMLDOMNode *pXMLProp;
	BSTR bstr;

	pXMLNode->selectSingleNode(szProp, &pXMLProp);
	if (pXMLProp != NULL) {
		pXMLProp->get_text(&bstr);
		*pprop = _wtoi(bstr);
		pXMLProp->Release();
	}
}

BOOL LoadMapSet(HWND hwndParent, LPCTSTR szFilename) {
	FILE *file;
	IXMLDOMDocument2 *pXMLDocument;
	IXMLDOMNode *pXMLNode;
	WCHAR wszFilename[MAX_PATH];

	file = _tfopen(szFilename, _T("r"));
	if (file == NULL)
		return FALSE;

	StringCbCopy(g_MapSet.szFilename, sizeof(g_MapSet.szFilename), szFilename);
	CoInitialize(NULL);

	CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument2, (LPVOID *) &pXMLDocument);
	if (pXMLDocument == NULL)
		return FALSE;

#ifdef _UNICODE
	StringCbCopy(wszFilename, sizeof(wszFilename), szFilename);
#else
	MultiByteToWideChar(CP_ACP, szFilename, -1, wszFilename, sizeof(wszFilename) / sizeof(WCHAR));
#endif
	VARIANT_BOOL VariantResult;

	VARIANT VariantFilename;

	V_VT(&VariantFilename) = VT_BSTR;
	V_BSTR(&VariantFilename) = SysAllocString(wszFilename);
	
	pXMLDocument->load(VariantFilename, &VariantResult);
	if (VariantResult != VARIANT_TRUE) {
		BSTR bstr;
		IXMLDOMParseError *pXMLParseError;
		pXMLDocument->get_parseError(&pXMLParseError);
		pXMLParseError->get_reason(&bstr);
		OutputDebugString(bstr);
		pXMLDocument->Release();
		return FALSE;
	}

	pXMLDocument->selectSingleNode(L"//map-set-settings/name", &pXMLNode);
	if (pXMLNode != NULL) {
		BSTR bstr;
		pXMLNode->get_text(&bstr);
		StringCbCopyW(g_MapSet.szName, sizeof(g_MapSet.szName), bstr);
		SysFreeString(bstr);
	}

	pXMLDocument->selectSingleNode(L"//map-set-settings", &pXMLNode);
	if (pXMLNode != NULL) {
		IXMLDOMNode *pXMLTiles;
		IXMLDOMNodeList *pXMLTileSets;
		LoadProp(pXMLNode, &g_MapSet.cx, L"map-width");
		LoadProp(pXMLNode, &g_MapSet.cy, L"map-height");
		LoadProp(pXMLNode, &g_MapSet.cxTile, L"tile-width");
		LoadProp(pXMLNode, &g_MapSet.cyTile, L"tile-height");

		g_MapSet.Scale = 1.0;

		pXMLNode->selectNodes(L"tiles", &pXMLTileSets);
		if (pXMLTileSets != NULL) {
			long Length;
			pXMLTileSets->get_length(&Length);
			for (int i = 0; i < Length; i++) {
				BSTR bstr;
				TCHAR szName[64];
				IXMLDOMNode *pXMLTileSetName;
				pXMLTileSets->get_item(i, &pXMLTiles);
				if (pXMLTiles != NULL) {
					pXMLTiles->selectSingleNode(L"@name", &pXMLTileSetName);
					if (pXMLTileSetName != NULL) {
						pXMLTileSetName->get_text(&bstr);

#ifdef _UNICODE
						StringCbCopy(szName, sizeof(szName), bstr);
#endif
						pXMLTiles->get_text(&bstr);
						AddTileset(szName, bstr);
						pXMLTileSetName->Release();
					}

					pXMLTiles->Release();
				}
			}
			pXMLTileSets->Release();
		}
		pXMLNode->Release();
	}

	pXMLDocument->selectSingleNode(L"//map", &pXMLNode);
	if (pXMLNode != NULL) {
		LoadMapFromNode(pXMLNode, hwndParent, NULL, MP_ROOT);
		pXMLNode->Release();
	}

	pXMLDocument->Release();

	FlattenMapTree(hwndParent, (LPMAPVIEWSETTINGS **) &g_MapSet.MapHierarchy, &g_MapSet.cxMapHierarchy, NULL);
	return TRUE;
}


void SaveMapFromSet(FILE *file, HWND hwndMap, MP_ORIENTATION Orientation, int *pIndex) {
	MAPVIEWSETTINGS mvs;

	if (hwndMap == NULL)
		return;

	GetMapViewSettings(hwndMap, &mvs);
	mvs.Index = (*pIndex)++;
	SetMapViewSettings(hwndMap, &mvs);

	FILE *defaults_table = _tfopen(DEFAULT_TABLE_FILENAME, _T("a"));
	_ftprintf(defaults_table, _T(".dw %s_defaults\n"), mvs.szName);

	fclose(defaults_table);

	_ftprintf(file, _T("<map name=\"%s\" pos=\""), mvs.szName);
	switch (Orientation)
	{
	case MP_ROOT:
		_ftprintf(file, _T("root"));
		break;
	case MP_LEFT:
		_ftprintf(file, _T("left"));
		break;
	case MP_RIGHT:
		_ftprintf(file, _T("right"));
		break;
	case MP_ABOVE:
		_ftprintf(file, _T("above"));
		break;
	case MP_BELOW:
		_ftprintf(file, _T("below"));
		break;
	}
	_ftprintf(file, _T("\" map=\"%s\" obj=\"%s\" tiles=\"%s\">\n"), mvs.szFilename, mvs.szDefaultsFilename, mvs.szTilesFilename);

	SaveMapFromSet(file, mvs.Left, MP_LEFT, pIndex);
	SaveMapFromSet(file, mvs.Right, MP_RIGHT, pIndex);
	SaveMapFromSet(file, mvs.Above, MP_ABOVE, pIndex);
	SaveMapFromSet(file, mvs.Below, MP_BELOW, pIndex);

	FILE *defaults = _tfopen(MAP_DATA_FILENAME, _T("a"));
	//mvs.szDefaultsFilename
	_ftprintf(defaults, _T("#ifdef MAP_EDITOR_DATA\n"));
	_ftprintf(defaults, _T("%s_map:\n"), mvs.szName);
	for (int i = 0; i < g_MapSet.cx * g_MapSet.cy; i++) {
		TCHAR szByte[4];
		StringCbPrintf(szByte, sizeof(szByte), _T("$%02X"), mvs.pMapData[i]);
		if (i % 16 == 0)
			_ftprintf(defaults, _T("\t.db %s,"), szByte);
		else if (i % 16 == 15)
			_ftprintf(defaults, _T("%s\n"), szByte);
		else
			_ftprintf(defaults, _T("%s,"), szByte);
	}
	_ftprintf(defaults, _T("#endif\n"));

	_ftprintf(defaults, _T("#if pageof($)=5\n"));
	_ftprintf(defaults, _T("%s_compressed_map:\ndm%d:\n"), mvs.szName, mvs.Index);

	int nMaxCompressed = g_MapSet.cx * g_MapSet.cy * 10 / 8;
	LPBYTE lpCompressedMap = (LPBYTE) malloc(nMaxCompressed);
	ZeroMemory(lpCompressedMap, nMaxCompressed);
	int CompSize = CompressMap(&mvs, lpCompressedMap, nMaxCompressed);
	for (int i = 0; i < CompSize; i++) {
		TCHAR szByte[4];
		StringCbPrintf(szByte, sizeof(szByte), _T("$%02X"), lpCompressedMap[i]);
		if (i % 16 == 0) {
			_ftprintf(defaults, _T("\t.db %s"), szByte);
			if ((i != CompSize - 1)) {
				_ftprintf(defaults, _T(","));
			}
		} else if ((i % 16 == 15) || (i == CompSize - 1))
			_ftprintf(defaults, _T("%s\n"), szByte);
		else
			_ftprintf(defaults, _T("%s,"), szByte);
	}
	_ftprintf(defaults, _T("\n#endif\n"));

	// Write the animate section
	_ftprintf(defaults, _T("#if pageof($)=3\n"));
	_ftprintf(defaults, _T("\n%s_defaults:\n"), mvs.szName);
	_ftprintf(defaults, _T("animate_section()\n"));
	for (int i = 0; i < mvs.AnimateCount; i++) {
		LPANIMATETYPE lpType = mvs.AnimateArray[i].lpType;
		TCHAR wszArgs[256];
		char szArgs[256];
		StringCbPrintfA(szArgs, sizeof(szArgs), "%s(%d,%d)", lpType->szMacro, mvs.AnimateArray[i].x, mvs.AnimateArray[i].y);
		MultiByteToWideChar(CP_ACP, 0, szArgs, -1, wszArgs, 256);
		_ftprintf(defaults, _T("\t%s\n"), wszArgs);
	}
	// Write the objects section
	_ftprintf(defaults, _T("object_section()\n"));
	for (int i = 0; i < mvs.ObjectCount; i++) {
		LPOBJECTTYPE lpType = mvs.ObjectArray[i].lpType;
		if (lpType != NULL) {
			char szArgs[256];
			TCHAR wszArgs[256];
			CreateObjectOutput(&mvs.ObjectArray[i], szArgs, sizeof(szArgs), TRUE);
			MultiByteToWideChar(CP_ACP, 0, szArgs, -1, wszArgs, 256);
			_ftprintf(defaults, _T("\t%s\n"), wszArgs);
		}
	}

	// Enemies section
	_ftprintf(defaults, _T("enemy_section()\n"));
	for (int i = 0; i < mvs.EnemyCount; i++) {
		LPOBJECTTYPE lpType = mvs.EnemyArray[i].lpType;
		if (lpType != NULL) {
			char szArgs[256];
			TCHAR wszArgs[256];
			CreateObjectOutput(&mvs.EnemyArray[i], szArgs, sizeof(szArgs), TRUE);
			MultiByteToWideChar(CP_ACP, 0, szArgs, -1, wszArgs, 256);
			_ftprintf(defaults, _T("\t%s\n"), wszArgs);
		}
	}

	// Write the misc section
	_ftprintf(defaults, _T("misc_section()\n"));
	extern MISCTYPE MiscTypes[256];
	extern DWORD MiscTypeCount;
	for (int i = 0; i < mvs.MiscCount; i++) {
		LPMISC lpm = &mvs.MiscArray[i];
		_ftprintf(defaults, _T("\t%s(%d, %d, %d, %d"), MiscTypes[lpm->MiscID].szMacro, lpm->x, lpm->y, lpm->w, lpm->h);
		if (MiscTypes[lpm->MiscID].ArgCount > 4) {
			for (int i = 4; i < MiscTypes[lpm->MiscID].ArgCount; i++) {
				_ftprintf(defaults, _T(", %s"), lpm->szArgs[i]);
			}
		}
		_ftprintf(defaults, _T(")\n"));
	}

	_ftprintf(defaults, _T("end_section()\n\n"));
	_ftprintf(defaults, _T("#endif\n"));
	fclose(defaults);

	_ftprintf(file, _T("</map>\n"));
}

static BOOL TilesetsCallback(TCHAR *szName, LPVOID lpParam) {
	TCHAR szValue[MAX_PATH + 64];
	TCHAR szFilename[MAX_PATH];
	GetTilesetFilename(szName, szFilename, sizeof(szFilename));

	StringCbPrintf(szValue, sizeof(szValue), _T("<tiles name=\"%s\">%s</tiles>\n"), szName, szFilename);
	_tcscat((TCHAR *) lpParam, szValue);
	return TRUE;
}

BOOL SaveMapSet(TCHAR *szFilename, LPMAPSETSETTINGS lpmss) {
	TCHAR szTilesets[MAX_PATH * 8] = _T("");
	FILE *defaults = _tfopen(MAP_DATA_FILENAME, _T("w"));
	_ftprintf(defaults, _T(";; Assembly export for %s\n\n"), g_MapSet.szName);
	fclose(defaults);

	FILE *default_table = _tfopen(DEFAULT_TABLE_FILENAME, _T("w"));
	_ftprintf(default_table, _T(";; Default table export for %s\n\n"), g_MapSet.szName);
	fclose(default_table);

	FILE *file = _tfopen(szFilename, _T("w"));
	if (file == NULL)
		return FALSE;

	//MAPVIEWSETTINGS mvs;
	//GetMapViewSettings(g_MapSet.hwndRoot, &mvs);

	EnumTilesets(TilesetsCallback, szTilesets);

	_ftprintf(file,
		_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n") \
		_T("<map-set>\n") \
		_T("<map-set-settings>\n") \
		_T("<name>%s</name>\n") \
		_T("<map-width>%d</map-width>\n") \
		_T("<map-height>%d</map-height>\n") \
		_T("<tile-width>%d</tile-width>\n") \
		_T("<tile-height>%d</tile-height>\n") \
		_T("%s\n")
		_T("</map-set-settings>\n"), g_MapSet.szName, g_MapSet.cx, g_MapSet.cy, g_MapSet.cxTile, g_MapSet.cyTile, szTilesets);


	int Index = 0;
	SaveMapFromSet(file, g_MapSet.hwndRoot, MP_ROOT, &Index);

	_ftprintf(file, _T("</map-set>"));

	fclose(file);
	return TRUE;
}