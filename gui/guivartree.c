#include "stdafx.h"

#include "calc.h"
#include "link.h"
#include "state.h"
#include "guivartree.h"
#include "guicontext.h"
#include "fileutilities.h"
#include "DropSource.h"
#include "DataObject.h"
#include "gui.h"

#include "exportvar.h"

static HWND g_hwndVarTree;
extern HINSTANCE g_hInst;
extern TCHAR type_ext[][4];
TCHAR export_file_name[512] = _T("Zelda.8xk");

static RECT VTrc = {-1, -1, -1, -1};
static VARTREEVIEW_t Tree[MAX_CALCS];
static BOOL Tree_init = FALSE;

void *FillFileBuffer(HTREEITEM hSelect, char *buffer);
FILEDESCRIPTOR *FillDesc(HTREEITEM hSelect, FILEDESCRIPTOR *fd);
int SetVarName(TCHAR *fileName, int model);

BOOL VarTreeOpen() {
	HWND vardialog = FindWindow(NULL, _T("Calculator Variables"));
	if (vardialog) {
		RefreshTreeView(FALSE);
		return TRUE;
	}
	return FALSE;
}

HWND CreateVarTreeList(HWND hwndParent, LPMAINWINDOW lpMainWindow) {
	INITCOMMONCONTROLSEX icc ;
	if (!VarTreeOpen()) {
		icc.dwSize = sizeof(icc);
		icc.dwICC = ICC_TREEVIEW_CLASSES;
		if (!InitCommonControlsEx(&icc)) {
			return NULL;
		}
		HWND hwndDialog = CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_VARLIST),
			hwndParent, DlgVarlist, (LPARAM) lpMainWindow);
		return hwndDialog;
	}
	return NULL;
}

apphdr_t *GetAppVariable(HTREEITEM hTreeItem, int *slot = NULL) {
	for (int temp = 0; temp < MAX_CALCS; temp++) {
		if (Tree[temp].model) {
			if (slot != NULL) {
				*slot = temp;
			}

			auto it = Tree[temp].apps->find(hTreeItem);
			if (it == Tree[temp].apps->end()) {
				return NULL;
			}

			return it->second;
		}
	}
	return NULL;
}

symbol83P_t *GetSymbolVariable(HTREEITEM hTreeItem, int *slot = NULL) {
	for (int temp = 0; temp < MAX_CALCS; temp++) {
		if (Tree[temp].model) {
			if (Tree[temp].sym.last == NULL || Tree[temp].sym.symbols == NULL) {
				continue;
			}

			int numSymbols = (Tree[temp].sym.last - Tree[temp].sym.symbols + 1);
			if (numSymbols < 0) {
				continue;
			}
			
			if (slot != NULL) {
				*slot = temp;
			}

			auto it = Tree[temp].symbols->find(hTreeItem);
			if (it == Tree[temp].symbols->end()) {
				return NULL;
			}

			return it->second;
		}
	}
	return NULL;
}

INT_PTR CALLBACK DlgVarlist(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (Message) {
		case WM_INITDIALOG:
		{
			lpMainWindow = (LPMAINWINDOW) lParam;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) lpMainWindow);

			g_hwndVarTree = GetDlgItem(hwnd, IDC_TRV1);
			HIMAGELIST hIL = ImageList_LoadImage(g_hInst, _T("TIvarIcons"), 
													16, 0, RGB(0,255,0),
													IMAGE_BITMAP, LR_CREATEDIBSECTION);
			HICON hIcon = LoadIcon(g_hInst, _T("w"));
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)	hIcon);
			DeleteObject(hIcon);
			if (!hIL) {
				MessageBox(hwnd, _T("Image list not loaded"), _T("Error"), MB_OK | MB_ICONERROR);
			} else {
				TreeView_SetImageList(g_hwndVarTree, hIL, TVSIL_NORMAL);
			}
			
			if (VTrc.bottom == -1) {
				GetWindowRect(hwnd, &VTrc);	
			} else {
				MoveWindow(hwnd, VTrc.left, VTrc.top, VTrc.right - VTrc.left, VTrc.bottom - VTrc.top, TRUE);
			}
			RefreshTreeView(TRUE);
			SendMessage(hwnd, WM_SIZE, 0, 0);
			return TRUE;
		}
		case WM_SIZE: {
			GetWindowRect(hwnd, &VTrc);
			int width = VTrc.right - VTrc.left - 14 - 6 - 8;
			int height = VTrc.bottom - VTrc.top - 38 - 30 - 73;
			MoveWindow(g_hwndVarTree, 6, 30, width, height, TRUE);
			SetWindowPos(GetDlgItem(hwnd, IDC_TXT_NAME), NULL, 15, height + 2 + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
			SetWindowPos(GetDlgItem(hwnd, IDC_LAB_ADDRESS), NULL, 6, height + 20 + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
			SetWindowPos(GetDlgItem(hwnd, IDC_TXT_ADDRESS), NULL, 65, height + 20 + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
			SetWindowPos(GetDlgItem(hwnd, IDC_LAB_PAGE), NULL, 6, height + 40 + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
			SetWindowPos(GetDlgItem(hwnd, IDC_TXT_PAGE), NULL, 65, height + 40 + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
			SetWindowPos(GetDlgItem(hwnd, IDC_LAB_RAM), NULL, 150, height + 20 + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
			SetWindowPos(GetDlgItem(hwnd, IDC_TXT_RAM), NULL, 205, height + 20 + 35, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);
			return 0;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_REFRESH_VAR_LIST:
					RefreshTreeView(FALSE);
					break;
				case IDM_VARGOTODEBUGGER: {
					HTREEITEM hTreeItem = TreeView_GetSelection(g_hwndVarTree);
					waddr_t waddr;
					symbol83P_t *symbol = NULL;
					apphdr_t *app = GetAppVariable(hTreeItem);
					if (app) {
						waddr.page = (uint8_t) app->page;
						waddr.addr = 0x4080;
						waddr.is_ram = FALSE;
					} else {
						symbol = GetSymbolVariable(hTreeItem);
						if (symbol != NULL) {
							waddr.is_ram = symbol->page == 0;
							if (waddr.is_ram == TRUE) {
								waddr = addr16_to_waddr(&lpMainWindow->lpCalc->mem_c, symbol->address);
							} else {
								waddr.page = symbol->page;
								waddr.addr = symbol->address;
							}
						}
					}
					
					if (app != NULL || symbol != NULL) {
						HWND hwndDebug = gui_debug_hwnd(lpMainWindow);
						SendMessage(hwndDebug, WM_COMMAND, MAKEWPARAM(DB_DISASM_GOTO_ADDR, 0),(LPARAM) &waddr);
					}
					break;
				}
				case IDM_VARTREEEXPORT:
				case IDC_EXPORT_VAR: {
					if (lpMainWindow == NULL) {
						break;
					}

					LPCALC lpCalc = lpMainWindow->lpCalc;
					char *buf;
					FILE *file;
					HTREEITEM item = TreeView_GetSelection(g_hwndVarTree);
					FILEDESCRIPTOR *fd;
					fd = (FILEDESCRIPTOR *) malloc(sizeof(FILEDESCRIPTOR));
					if (fd == NULL) {
						exit(1);
						break;
					}

					if (!FillDesc(item, fd)) {
						free (fd);
						break;
					}

					buf =  (char *) malloc(fd->nFileSizeLow + 1);
					ZeroMemory(buf, fd->nFileSizeLow + 1);
					FillFileBuffer(item, buf);
					if (SetVarName(fd->cFileName, lpCalc->model)) {
						free(buf);
						free(fd);
						break;
					}
					_tfopen_s(&file, export_file_name, _T("wb"));
					fwrite(buf, 1, fd->nFileSizeLow, file);
					fclose(file);
					free(buf);
					free(fd);
					break;
				}
				default:
					break;
			}
			return TRUE;
		}
		case WM_NOTIFY: {
			NMTREEVIEW *nmtv = (LPNMTREEVIEW) lParam;
			switch (((NMHDR*) lParam)->code) {
				case TVN_SELCHANGED: {
					int slot;
					TCHAR string[MAX_PATH] = { 0 };
					apphdr_t *app = GetAppVariable(nmtv->itemNew.hItem, &slot);
					if (app != NULL) {
						if (App_Name_to_String(app, string)) {
							SetDlgItemText(hwnd, IDC_VAR_NAME, string);
							StringCbPrintf(string, sizeof(string), _T("%02X"), app->page);
							SetDlgItemText(hwnd, IDC_VAR_PAGE, string);
						}
					} else {
						symbol83P_t *symbol = GetSymbolVariable(nmtv->itemNew.hItem, &slot);
						if (symbol != NULL) {
							if (Symbol_Name_to_String(Tree[slot].model, symbol, string)) {
								SetDlgItemText(hwnd, IDC_VAR_NAME, string);
								StringCbPrintf(string, sizeof(string), _T("%04X"), symbol->address);
								SetDlgItemText(hwnd, IDC_VAR_ADDRESS, string);
								StringCbPrintf(string, sizeof(string), _T("%02X"), symbol->page);
								SetDlgItemText(hwnd, IDC_VAR_PAGE, string);
								SetDlgItemText(hwnd, IDC_VAR_RAM, symbol->page == 0 || Tree[slot].model == TI_86 ?
									_T("True") : _T("False"));
							}
						}
					}
					break;
				}
				case NM_RCLICK: {
					HMENU hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_VARTREE_MENU));
					hmenu = GetSubMenu(hmenu, 0); 

					POINT p, pt = {0};
					GetCursorPos(&p);
					pt.x = p.x;
					pt.y = p.y;
					BOOL success = ScreenToClient(g_hwndVarTree, &p);
					if (success == FALSE) {
						break;
					}

					TVHITTESTINFO tvht;
					tvht.pt = p;
					TreeView_HitTest(g_hwndVarTree, &tvht);

					HTREEITEM hTreeItem = tvht.hItem;
					if (!hTreeItem) {
						DestroyMenu(hmenu);
						return TRUE;
					}

					TreeView_SelectItem(g_hwndVarTree, hTreeItem);

					int slot;
					apphdr_t *app = GetAppVariable(hTreeItem, &slot);
					symbol83P_t *symbol = GetSymbolVariable(hTreeItem, &slot);
					if (app != NULL || symbol != NULL) {
						OnContextMenu(hwnd, pt.x, pt.y, hmenu);
					}

					DestroyMenu(hmenu); 
					return TRUE;
				}
				case TVN_BEGINDRAG:	{
					TreeView_SelectItem(g_hwndVarTree, nmtv->itemNew.hItem);
					FORMATETC fmtetc[2] = {
						{(CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
						{(CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, 0, TYMED_HGLOBAL } };
					STGMEDIUM stgmed[2] = {
						{ TYMED_HGLOBAL, { 0 }, 0 },
						{ TYMED_HGLOBAL, { 0 }, 0 }};
					CDataObject *pDataObject;
					CDropSource *pDropSource;
					DWORD dwEffect = DROPEFFECT_NONE;
					// transfer the current selection into the CDataObject
					stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(FILEGROUPDESCRIPTOR) + sizeof(FILEDESCRIPTOR));
					stgmed[0].tymed = TYMED_HGLOBAL;

					FILEGROUPDESCRIPTOR *fgd = (FILEGROUPDESCRIPTOR *) GlobalLock(stgmed[0].hGlobal);
					fgd->cItems = 1;

					FILEDESCRIPTOR *fd = fgd->fgd;
					ZeroMemory(fd, sizeof(FILEDESCRIPTOR));

					if ((FillDesc(nmtv->itemNew.hItem, fd) == NULL) || (fd->nFileSizeLow == 0))
					{
						GlobalFree(stgmed[0].hGlobal);
						return FALSE;
					}
					GlobalUnlock(stgmed[0].hGlobal);

					stgmed[1].hGlobal = GlobalAlloc(GHND, fd->nFileSizeLow);
					stgmed[1].tymed = TYMED_HGLOBAL;

					char *buf = (char *) GlobalLock(stgmed[1].hGlobal);
					FillFileBuffer(nmtv->itemNew.hItem, buf);
					GlobalUnlock(stgmed[1].hGlobal);

					// Create IDataObject and IDropSource COM objects
					pDropSource = new CDropSource();
					CreateDataObject(fmtetc, stgmed, 2, (IDataObject **) &pDataObject);
		
					//
					//	** ** ** The drag-drop operation starts here! ** ** **
					//
					pDataObject->QueryInterface(IID_IDataObject, (LPVOID *) &pDropSource->m_pDataobject);
					//SetDropSourceDataObject(pDropSource, pDataObject);
					DoDragDrop((IDataObject *) pDataObject, (IDropSource *) pDropSource, DROPEFFECT_COPY, &dwEffect);
					return TRUE;
				}
			}
			
			return TRUE;
		}

		case WM_CLOSE:
		case WM_DESTROY:
			GetWindowRect(hwnd, &VTrc);	
			DestroyWindow(hwnd);
			return TRUE;
	}
   return FALSE;
}

TCHAR *GetFilterString(int model) {
	switch (model) {
	case TI_73:
		return _T("Programs  (*.73p)\0*.73p\0\
Applications (*.73k)\0*.73k\0\
App Vars (*.73v)\0*.73v\0\
Lists  (*.73l)\0*.73l\0\
Real/Complex Variables  (*.73n)\0*.73n\0\
Pictures  (*.73i)\0*.73i\0\
GDBs  (*.73d)\0*.73d\0\
Matrices  (*.73m)\0*.73m\0\
Strings  (*.73s)\0*.73s\0\
Groups  (*.73g)\0*.73g\0\
All Files (*.*)\0*.*\0\0");
	case TI_84PCSE:
		return _T("Programs  (*.8xp)\0*.8xp\0\
Applications (*.8ck)\0*.8ck\0\
App Vars (*.8xv)\0*.8xv\0\
Lists  (*.8xl)\0*.8xl\0\
Real/Complex Variables  (*.8xn)\0*.8xn\0\
Pictures  (*.8xi)\0*.8xi\0\
GDBs  (*.8xd)\0*.8xd\0\
Matrices  (*.8xm)\0*.8xm\0\
Strings  (*.8xs)\0*.8xs\0\
Groups  (*.8xg)\0*.8xg\0\
All Files (*.*)\0*.*\0\0");
	default:
		return _T("Programs  (*.8xp)\0*.8xp\0\
Applications (*.8xk)\0*.8xk\0\
App Vars (*.8xv)\0*.8xv\0\
Lists  (*.8xl)\0*.8xl\0\
Real/Complex Variables  (*.8xn)\0*.8xn\0\
Pictures  (*.8xi)\0*.8xi\0\
GDBs  (*.8xd)\0*.8xd\0\
Matrices  (*.8xm)\0*.8xm\0\
Strings  (*.8xs)\0*.8xs\0\
Groups  (*.8xg)\0*.8xg\0\
All Files (*.*)\0*.*\0\0");
	}
}

int SetVarName(TCHAR *fileName, int model) {
	TCHAR *defExt;
	int filterIndex;
	const TCHAR *lpstrFilter = GetFilterString(model);
	const TCHAR lpstrTitle[] = _T("Wabbitemu Export");
	TCHAR lpstrFile[MAX_PATH];
	StringCbCopy(lpstrFile, sizeof(lpstrFile), fileName);
	size_t i = _tcslen(lpstrFile);
	lpstrFile[i] = '\0';
	defExt = &lpstrFile[i];
	while (*defExt != '.') {
		defExt--;
	}

	switch (defExt[3]) {
		case 'p':
			filterIndex = 1;
			break;
		case 'k':
			filterIndex = 2;
			break;
		case 'v':
			filterIndex = 3;
			break;
		case 'l':
			filterIndex = 4;
			break;
		case 'n':
			filterIndex = 5;
			break;
		case 'i':
			filterIndex = 6;
			break;
		case 'd':
			filterIndex = 7;
			break;
		case 'm':
			filterIndex = 8;
			break;
		case 's':
			filterIndex = 9;
			break;
		case 'g':
			filterIndex = 10;
			break;
		default:
			filterIndex = 11;
			break;
	}

	if (SaveFile(lpstrFile, lpstrFilter, lpstrTitle, defExt, OFN_PATHMUSTEXIST, filterIndex)) {
		return 1;
	}

	StringCbCopy(export_file_name, sizeof(export_file_name), lpstrFile);
	return 0;
}


/* deletes parent's children, not parent */
void DeleteChildren(HWND hwnd, HTREEITEM parent) {
	HTREEITEM Child = TreeView_GetChild(hwnd, parent);
	while(Child != NULL) {
		TreeView_DeleteItem(hwnd, Child);
		Child = TreeView_GetChild(hwnd, parent);
	}
}


HTREEITEM InsertVar(HTREEITEM parent, TCHAR *Name, int icon) {
	TVINSERTSTRUCT tvs;
	tvs.hParent				= parent;
	tvs.hInsertAfter		= TVI_SORT;
	tvs.item.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE |TVIF_TEXT;
	tvs.item.pszText		= (LPTSTR)(Name);
	tvs.item.cchTextMax		= (int) _tcslen(Name) + 1;
	tvs.item.iImage			= icon;
	tvs.item.iSelectedImage	= tvs.item.iImage;
	return TreeView_InsertItem(g_hwndVarTree, &tvs);
}

#define SetupNode(hTreeItem, ItemName, Icon)  if (!hTreeItem || tree->model == 0)\
	hTreeItem = InsertVar(tree->hRoot, ItemName, Icon);\
	else DeleteChildren(g_hwndVarTree, hTreeItem)

/* updates the tree view */
void RefreshTreeView(BOOL New) {
	u_int i;
	int slot;
	float ver;
	TVINSERTSTRUCT tvs;

	if (Tree_init == FALSE || New != FALSE) {
		memset(Tree, 0, sizeof(Tree));
		Tree_init = TRUE;
	}

	/* run through all active calcs */
	for(slot = 0; slot < MAX_CALCS; slot++) {
		VARTREEVIEW_t *tree = &Tree[slot];
		if (!calcs[slot].active && tree->model != 0) {
			TreeView_DeleteItem(g_hwndVarTree, tree->hRoot);
			tree->model = 0;
		}

		if (calcs[slot].active && tree->model != calcs[slot].model && tree->model != 0) {
			TreeView_DeleteItem(g_hwndVarTree, tree->hRoot);
			tree->model = 0;
		}
		
		
		/* It's an 83+ compatible with a known rom(hopefully) */
		if (calcs[slot].active && (calcs[slot].model >= TI_83P || calcs[slot].model == TI_86) &&
			sscanf_s(calcs[slot].rom_version, "%f", &ver) == 1) {
			/* This slot has not yet been initialized. */
			/* so set up the Root */
			if (tree->model == 0) {
				tvs.hParent = TVI_ROOT;
				tvs.hInsertAfter = TVI_ROOT;
				tvs.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
				tvs.item.pszText = (LPTSTR)calc_get_model_string(calcs[slot].model);
				tvs.item.cchTextMax = (int)_tcslen(tvs.item.pszText) + 1;
				tvs.item.iImage = TI_ICON_84PSE;
				tvs.item.iSelectedImage = tvs.item.iImage;
				tree->hRoot = TreeView_InsertItem(g_hwndVarTree, &tvs);
			}

			/* If nodes haven't been init or the model is reset, create nodes */
			/* otherwise delete children so the vars can be appended */
			SetupNode(tree->hApplication, _T("Application"), TI_ICON_APP);
			SetupNode(tree->hProgram, _T("Program"), TI_ICON_PROGRAM);
			SetupNode(tree->hAppVar, _T("Application Variable"), TI_ICON_APPVAR);
			SetupNode(tree->hPic, _T("Picture"), TI_ICON_PIC);
			SetupNode(tree->hGDB, _T("Graph Database"), TI_ICON_GDB);
			SetupNode(tree->hString, _T("String"), TI_ICON_STRING);
			SetupNode(tree->hNumber, _T("Number"), TI_ICON_NUMBER);
			SetupNode(tree->hList, _T("List"), TI_ICON_LIST);
			SetupNode(tree->hMatrix, _T("Matrix"), TI_ICON_MATRIX);
			SetupNode(tree->hGroup, _T("Group"), TI_ICON_GROUP);
			SetupNode(tree->hEquation, _T("Equation"), TI_ICON_EQUATIONS);

			tree->model		= calcs[slot].cpu.pio.model;
			
			if (tree->apps != NULL) {
				delete tree->apps;
			}

			if (tree->symbols != NULL) {
				delete tree->symbols;
			}

			tree->apps = new map<HTREEITEM, apphdr_t *>;
			tree->symbols = new map<HTREEITEM, symbol83P_t *>;

			/* Apps are handled outside of the symbol table*/
			state_build_applist(&calcs[slot].cpu, &tree->applist);
			for(i = 0; i < tree->applist.count; i++) {
				apphdr_t *app = &tree->applist.apps[i];
				HTREEITEM hTreeItem = InsertVar(tree->hApplication, app->name, TI_ICON_FILE_ARC);
				if (hTreeItem == NULL) {
					continue;
				}
				tree->apps->insert(pair<HTREEITEM, apphdr_t *>(hTreeItem, app));
			}

			symlist_t* sym;
			if (calcs[slot].model == TI_86) {
				sym = state_build_symlist_86(&calcs[slot].cpu, &tree->sym);
			} else {
				sym = state_build_symlist_83P(&calcs[slot].cpu, &tree->sym);
			}
			if (sym != NULL) {
				for(i = 0; (&sym->symbols[i]) <= sym->last; i++) {
					TCHAR tmpstring[64];
					int icon;

					/* whether its archived or not */
					/* depends on the page its stored */
					if (sym->symbols[i].page && tree->model != TI_86) {
						icon = TI_ICON_FILE_ARC;  // red
					} else {
						icon = TI_ICON_FILE_RAM;  // green
					}

					if (Symbol_Name_to_String(tree->model, &sym->symbols[i], tmpstring) == NULL) {
						continue;
					}
					
					HTREEITEM item = NULL;
					if (tree->model == TI_86) {
						switch(sym->symbols[i].type_ID) {
							case ProgObj86:
								item = InsertVar(tree->hProgram, tmpstring, icon);
								break;
							case PictObj86:
								item = InsertVar(tree->hPic, tmpstring, icon);
								break;
							case FuncGDBObj86:
							case DiffEquGDBObj86:
							case ParamGDBObj86:
							case PolarGDBObj86:
								item = InsertVar(tree->hGDB, tmpstring, icon);
								break;
							case StrngObj86:
								item = InsertVar(tree->hString, tmpstring, icon);
								break;
							case ConstObj86:
							case RealObj86:
							case CplxObj86:
								item = InsertVar(tree->hNumber, tmpstring, icon);
								break;
							case ListObj86:
							case CListObj86:
								item = InsertVar(tree->hList, tmpstring, icon);
								break;
							case MatObj86:
							case CMatObj86:
							case VectObj86:
								item = InsertVar(tree->hMatrix, tmpstring, icon);
								break;
							case EquObj86:
							case EquObj_286:
								item = InsertVar(tree->hEquation, tmpstring, icon);
								break;
						}
					} else {
						switch(sym->symbols[i].type_ID) {
							case ProgObj:
							case ProtProgObj:
								item = InsertVar(tree->hProgram, tmpstring, icon);
								break;
							case AppVarObj:
								item = InsertVar(tree->hAppVar, tmpstring, icon);
								break;
							case GroupObj:
								item = InsertVar(tree->hGroup, tmpstring, icon);
								break;
							case PictObj:
								item = InsertVar(tree->hPic, tmpstring, icon);
								break;
							case GDBObj:
								item = InsertVar(tree->hGDB, tmpstring, icon);
								break;
							case StrngObj:
								item = InsertVar(tree->hString, tmpstring, icon);
								break;
							case RealObj:
							case CplxObj:
								item = InsertVar(tree->hNumber, tmpstring, icon);
								break;
							case ListObj:
							case CListObj:
								item = InsertVar(tree->hList, tmpstring, icon);
								break;
							case MatObj:
								item = InsertVar(tree->hMatrix, tmpstring, icon);
								break;
//							case EquObj:
							case EquObj_2:
								item = InsertVar(tree->hEquation, tmpstring, icon);
								break;
						}
					}

					if (item != NULL) {
						tree->symbols->insert(pair<HTREEITEM, symbol83P_t*>(item, &sym->symbols[i]));
					}
				}
			}
			// If no children are found kill parent.
			for(i = 0; i < 11; i++) {
				if (tree->hTypes[i]) {
					if (TreeView_GetChild(g_hwndVarTree, tree->hTypes[i]) == NULL) {
						TreeView_DeleteItem(g_hwndVarTree, tree->hTypes[i]);
						tree->hTypes[i] = NULL;
					}
				}
			}
			TreeView_Expand(g_hwndVarTree, tree->hRoot, TVE_EXPAND);
		}
	}
}
	
	
FILEDESCRIPTOR *FillDesc(HTREEITEM hSelect,  FILEDESCRIPTOR *fd) {
	int slot;
	TCHAR string[MAX_PATH] = { 0 };

	apphdr_t *app = GetAppVariable(hSelect, &slot);
	if (app != NULL) {
		if (App_Name_to_String(app, string)) {
			StringCbCat(string, sizeof(string), _T(".8xk"));
			MFILE *outfile = ExportApp(&calcs[slot], NULL, app);
			fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
			fd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
			fd->nFileSizeLow = msize(outfile);
			StringCbCopy(fd->cFileName, sizeof(fd->cFileName), string);
			mclose(outfile);
			return fd;
		}
	} else {
		symbol83P_t *symbol = GetSymbolVariable(hSelect, &slot);
		if (Symbol_Name_to_String(Tree[slot].model, symbol, string)) {
			StringCbCat(string, sizeof(string), _T("."));
			StringCbCat(string, sizeof(string), (const TCHAR *)type_ext[symbol->type_ID]);
			MFILE *outfile = ExportVar(&calcs[slot], NULL, symbol);
			fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
			fd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
			fd->nFileSizeLow = msize(outfile);
			StringCbCopy(fd->cFileName, sizeof(fd->cFileName), string);
			mclose(outfile);
			return fd;
		}
	}

	MessageBox(NULL, _T("Unable to export variable"), _T("Error"), MB_OK | MB_ICONERROR);
	return NULL;
}
	
void *FillFileBuffer(HTREEITEM hSelect, char *buf) {
	int slot;
	TCHAR string[64] = { 0 };
	MFILE *outfile;

	apphdr_t *app = GetAppVariable(hSelect, &slot);
	if (app != NULL) {
		outfile = ExportApp(&calcs[slot], NULL, app);
		if (outfile == NULL) {
			return NULL;
		}
	} else {
		symbol83P_t *symbol = GetSymbolVariable(hSelect, &slot);
		if (Symbol_Name_to_String(Tree[slot].model, symbol, string)) {
			outfile = ExportVar(&calcs[slot], NULL, symbol);
			if (outfile == NULL) {
				return NULL;
			}
		}
	}

	for (unsigned int b = 0; b < outfile->size; b++) {
		buf[b] = outfile->data[b];
	}

	mclose(outfile);
	return buf;

}
