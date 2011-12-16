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
#include "resource.h"

static HWND g_hwndVarTree;
extern HINSTANCE g_hInst;
extern TCHAR type_ext[][4];
TCHAR export_file_name[512] = _T("Zelda.8xk");

static RECT VTrc = {-1, -1, -1, -1};
static VARTREEVIEW_t Tree[MAX_CALCS];
static BOOL Tree_init = FALSE;

void test_function(int num);

BOOL VarTreeOpen() {
	HWND vardialog = FindWindow(NULL, _T("Calculator Variables"));
	if (vardialog) {
		RefreshTreeView(FALSE);
		return TRUE;
	}
	return FALSE;
}

HWND CreateVarTreeList(HWND hwndParent, LPCALC lpCalc) {
	INITCOMMONCONTROLSEX icc ;
	if (!VarTreeOpen()) {
		icc.dwSize = sizeof(icc);
		icc.dwICC = ICC_TREEVIEW_CLASSES;
		if (!InitCommonControlsEx(&icc)) {
			return NULL;
		}
		HWND hwndDialog = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_VARLIST), NULL, DlgVarlist);
		SetWindowLongPtr(hwndDialog, GWLP_USERDATA, (LONG_PTR) lpCalc);
		return hwndDialog;
	}
	return NULL;
}

//TODO: well this code is a mess. We need to refactor this, so that the HTREEITEM is somehow
//mapped to the to either the symlist_t or applist_t item. Ideally this would be the LPARAM 
//of the LPTREEVIEW, but a dictionary mapping would be fine as well
apphdr_t *GetAppVariable(HTREEITEM hTreeItem, int *slot) {
	for (*slot = 0; *slot < MAX_CALCS; (*slot)++) {
		if (Tree[*slot].model) {
			for(u_int i = 0; i < Tree[*slot].applist.count; i++) {
				if (Tree[*slot].hApps[i] == hTreeItem) {
					return &Tree[*slot].applist.apps[i];
				}
			}
		}
	}
	return NULL;
}

symbol83P_t *GetSymbolVariable(HTREEITEM hTreeItem, int *slot) {
	for (*slot = 0; *slot < MAX_CALCS; (*slot)++) {
		if (Tree[*slot].model) {
			if (Tree[*slot].sym.last == NULL || Tree[*slot].sym.symbols == NULL)
				return NULL;
			for(u_int i = 0; i < (u_int) (Tree[*slot].sym.last - Tree[*slot].sym.symbols + 1); i++) {
				if (Tree[*slot].hVars[i] == hTreeItem) {
					return &Tree[*slot].sym.symbols[i];
				}
			}
		}
	}
	return NULL;
}

INT_PTR CALLBACK DlgVarlist(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		case WM_INITDIALOG:
		{
			g_hwndVarTree = GetDlgItem(hwnd, IDC_TRV1);
			HIMAGELIST hIL = ImageList_LoadImage(g_hInst, _T("TIvarIcons"), 
													16, 0, RGB(0,255,0),
													IMAGE_BITMAP, LR_CREATEDIBSECTION);
			if (!hIL) {
				_tprintf_s(_T("Image list not loaded"));
			} else {
				TreeView_SetImageList(g_hwndVarTree, hIL, TVSIL_NORMAL);
			}
			
			if (VTrc.bottom == -1) {
				GetWindowRect(hwnd, &VTrc);	
			} else {
				MoveWindow(hwnd, VTrc.left, VTrc.top, VTrc.right - VTrc.left, VTrc.bottom - VTrc.top, TRUE);
			}
			RefreshTreeView(TRUE);
			return TRUE;
		}
		case WM_SIZE: {
			GetWindowRect(hwnd, &VTrc);
			int width = VTrc.right - VTrc.left - 14 - 6 - 8;
			int height = VTrc.bottom-VTrc.top - 38 - 30 - 73;
			MoveWindow(g_hwndVarTree, 6, 30, width, height, TRUE);
			break;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDC_REFRESH_VAR_LIST:
					RefreshTreeView(FALSE);
					break;
				case IDM_VARGOTODEBUGGER: {
					LPCALC lpCalc = (LPCALC) GetWindowLongPtr(hwnd, GWLP_USERDATA);
					HTREEITEM hTreeItem = TreeView_GetSelection(g_hwndVarTree);
					waddr_t waddr;
					symbol83P_t *symbol = NULL;
					int slot;
					apphdr_t *app = GetAppVariable(hTreeItem, &slot);
					if (app) {
						waddr.page = app->page;
						waddr.addr = 0x4080;
						waddr.is_ram = FALSE;
					} else {
						symbol = GetSymbolVariable(hTreeItem, &slot);
						if (symbol) {
							waddr.page = symbol->page;
							waddr.addr = symbol->address;
							waddr.is_ram = symbol->page == 0;
						}
					}
					
					if (app || symbol) {
						HWND hwndDebugger = gui_debug(lpCalc);
						SendMessage(hwndDebugger, WM_COMMAND, MAKEWPARAM(DB_DISASM_GOTO_ADDR, 0),(LPARAM) &waddr);
					}
					break;
				}
				case IDM_VARTREEEXPORT:
				case IDC_EXPORT_VAR: {
					char *buf;
					FILE *file;
					HTREEITEM item = TreeView_GetSelection(g_hwndVarTree);
					//HACK: yes i know FILEDESCRIPTOR is not meant for this.
					//but i don't want to rework the routines to return the attributes differently
					FILEDESCRIPTOR *fd;
					fd = (FILEDESCRIPTOR *) malloc(sizeof(FILEDESCRIPTOR));
					if (fd == NULL) {
						MessageBox(NULL, _T("BAD"), _T("FUCK"), MB_OK);
					}
					if (!FillDesc(item, fd)) {
						free (fd);
						break;
					}
					buf =  (char *) malloc(fd->nFileSizeLow);
					FillFileBuffer(item, buf);
					if (SetVarName(fd)) {
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
					TCHAR string[MAX_PATH];
					apphdr_t *app;
					symbol83P_t *symbol;
					int slot;
					if (app = GetAppVariable(nmtv->itemNew.hItem, &slot)) {
						if (App_Name_to_String(app, string)) {
							SetDlgItemText(hwnd, IDC_VAR_NAME, string);
							StringCbPrintf(string, sizeof(string), _T("%02X"), app->page);
							SetDlgItemText(hwnd, IDC_VAR_PAGE, string);
						}
					} else if (symbol = GetSymbolVariable(nmtv->itemNew.hItem, &slot)) {
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
					break;
				}
				case NM_RCLICK: {
					HMENU hmenu;
					hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_VARTREE_MENU));
					
					hmenu = GetSubMenu(hmenu, 0); 

					POINT p, pt = {0};
					GetCursorPos(&p);
					pt.x = p.x;
					pt.y = p.y;
					int error = ScreenToClient(g_hwndVarTree, &p);

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
					if (app || symbol) {
						OnContextMenu(hwnd, pt.x, pt.y, hmenu);
					}

					DestroyMenu(hmenu); 
					return TRUE;
				}
				case TVN_BEGINDRAG:	{
					TreeView_SelectItem(g_hwndVarTree, nmtv->itemNew.hItem);
					FORMATETC fmtetc[2] = {
						{RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
						{RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, 0, TYMED_HGLOBAL }};
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
					HRESULT hr = DoDragDrop((IDataObject *) pDataObject, (IDropSource *) pDropSource, DROPEFFECT_COPY, &dwEffect);
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

int SetVarName(FILEDESCRIPTOR *fd) {
	OPENFILENAME ofn;
	TCHAR *defExt;
	int filterIndex;
	const TCHAR lpstrFilter[] = _T("Programs  (*.8xp)\0*.8xp\0\
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
	const TCHAR lpstrTitle[] = _T("Wabbitemu Export");
	TCHAR lpstrFile[MAX_PATH];
	StringCbCopy(lpstrFile, sizeof(lpstrFile), fd->cFileName);
	size_t i = _tcslen(lpstrFile);
	lpstrFile[i] = '\0';
	defExt = &lpstrFile[i];
	while (*defExt != '.')
		defExt--;
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

	if (SaveFile(lpstrFile, lpstrFilter, lpstrTitle, defExt, OFN_PATHMUSTEXIST, filterIndex))
		return 1;
	StringCbCopy(export_file_name, sizeof(export_file_name), lpstrFile);
	return 0;
}


/* deletes parent's children, not parent*/
void DeleteChildren(HWND hwnd, HTREEITEM parent) {
	HTREEITEM Child;
	while(Child = TreeView_GetChild(hwnd,parent)) {
		TreeView_DeleteItem(hwnd, Child);
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
		if (!calcs[slot].active && Tree[slot].model != 0) {
			TreeView_DeleteItem(g_hwndVarTree, Tree[slot].hRoot);
			Tree[slot].model = 0;
		}

		if (calcs[slot].active && Tree[slot].model != calcs[slot].model && Tree[slot].model != 0) {
			TreeView_DeleteItem(g_hwndVarTree, Tree[slot].hRoot);
			Tree[slot].model = 0;
		}
		
		
		/*It's an 83+ compatible with a known rom(hopefully)*/
		if (calcs[slot].active && (calcs[slot].model >= TI_83P || calcs[slot].model == TI_86) &&
			sscanf_s(calcs[slot].rom_version, "%f", &ver) == 1) {

			/* This slot has not yet been initialized. */
			/* so set up the Root */
			if (Tree[slot].model == 0) {
				tvs.hParent				= TVI_ROOT;
				tvs.hInsertAfter		= TVI_ROOT;
				tvs.item.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE |TVIF_TEXT;
				tvs.item.pszText		= (LPTSTR) CalcModelTxt[calcs[slot].model];
				tvs.item.cchTextMax		= (int) _tcslen(tvs.item.pszText) + 1;
				tvs.item.iImage			= TI_ICON_84PSE;
				tvs.item.iSelectedImage	= tvs.item.iImage;
				Tree[slot].hRoot		= TreeView_InsertItem(g_hwndVarTree, &tvs);
			}

			/* If nodes haven't been init or the model is reset, create nodes */
			/* otherwise delete children so the vars can be appended */
			if (!Tree[slot].hApplication || Tree[slot].model == 0) {
				Tree[slot].hApplication = InsertVar(Tree[slot].hRoot, _T("Application"), TI_ICON_APP);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hApplication);

			if (!Tree[slot].hProgram || Tree[slot].model == 0) {
				Tree[slot].hProgram = InsertVar(Tree[slot].hRoot, _T("Program"), TI_ICON_PROGRAM);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hProgram);

			if (!Tree[slot].hAppVar || Tree[slot].model == 0) {
				Tree[slot].hAppVar = InsertVar(Tree[slot].hRoot, _T("Application Variable"), TI_ICON_APPVAR);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hAppVar);

			if (!Tree[slot].hPic || Tree[slot].model == 0) {
				Tree[slot].hPic = InsertVar(Tree[slot].hRoot, _T("Picture"), TI_ICON_PIC);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hPic);

			if (!Tree[slot].hGDB || Tree[slot].model == 0) {
				Tree[slot].hGDB = InsertVar(Tree[slot].hRoot, _T("Graph Database"), TI_ICON_GDB);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hGDB);

			if (!Tree[slot].hString || Tree[slot].model == 0) {
				Tree[slot].hString = InsertVar(Tree[slot].hRoot, _T("String"), TI_ICON_STRING);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hString);

			if (!Tree[slot].hNumber || Tree[slot].model == 0) {
				Tree[slot].hNumber = InsertVar(Tree[slot].hRoot, _T("Number"), TI_ICON_NUMBER);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hNumber);

			if (!Tree[slot].hList || Tree[slot].model == 0) {
				Tree[slot].hList = InsertVar(Tree[slot].hRoot, _T("List"), TI_ICON_LIST);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hList);

			if (!Tree[slot].hMatrix || Tree[slot].model == 0) {
				Tree[slot].hMatrix = InsertVar(Tree[slot].hRoot, _T("Matrix"), TI_ICON_MATRIX);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hMatrix);

			if (!Tree[slot].hGroup || Tree[slot].model == 0) {
				Tree[slot].hGroup = InsertVar(Tree[slot].hRoot, _T("Group"), TI_ICON_GROUP);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hGroup);

			if (!Tree[slot].hEquation || Tree[slot].model == 0) {
				Tree[slot].hEquation = InsertVar(Tree[slot].hRoot, _T("Equation"), TI_ICON_EQUATIONS);
			} else DeleteChildren(g_hwndVarTree, Tree[slot].hEquation);

			Tree[slot].model		= calcs[slot].cpu.pio.model;
			
			/* Apps are handled outside of the symbol table*/
			state_build_applist(&calcs[slot].cpu, &Tree[slot].applist);
			for(i = 0; i < Tree[slot].applist.count; i++) {
				Tree[slot].hApps[i] = InsertVar(Tree[slot].hApplication, Tree[slot].applist.apps[i].name, TI_ICON_FILE_ARC);
			}
			symlist_t* sym;
			if (calcs[slot].model == TI_86) {
				sym = state_build_symlist_86(&calcs[slot].cpu, &Tree[slot].sym);
			} else {
				sym = state_build_symlist_83P(&calcs[slot].cpu, &Tree[slot].sym);
			}
			if (sym) {
				// FIXME
				for(i = 0; (&sym->symbols[i]) <= sym->last; i++) {
					TCHAR tmpstring[64];
					int icon;
					
					/* whether its archived or not */
					/* depends on the page its stored */
					if (sym->symbols[i].page && Tree[slot].model != TI_86) {
						icon = TI_ICON_FILE_ARC;  //red
					} else {
						icon = TI_ICON_FILE_RAM;  //green
					}
					
					if (Symbol_Name_to_String(Tree[slot].model, &sym->symbols[i], tmpstring)) {
						if (Tree[slot].model == TI_86) {
							switch(sym->symbols[i].type_ID) {
								case ProgObj86:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hProgram, tmpstring, icon);
									break;
								case PictObj86:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hPic, tmpstring, icon);
									break;
								case FuncGDBObj86:
								case DiffEquGDBObj86:
								case ParamGDBObj86:
								case PolarGDBObj86:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hGDB, tmpstring, icon);
									break;
								case StrngObj86:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hString, tmpstring, icon);
									break;
								case RealObj86:
								case CplxObj86:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hNumber, tmpstring, icon);
									break;
								case ListObj86:
								case CListObj86:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hList, tmpstring, icon);
									break;
								case MatObj86:
								case CMatObj86:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hMatrix, tmpstring, icon);
									break;
								case EquObj86:
								case EquObj_286:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hEquation, tmpstring, icon);
									break;
							}
						} else {
							switch(sym->symbols[i].type_ID) {
								case ProgObj:
								case ProtProgObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hProgram, tmpstring, icon);
									break;
								case AppVarObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hAppVar, tmpstring, icon);
									break;
								case GroupObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hGroup, tmpstring, icon);
									break;
								case PictObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hPic, tmpstring, icon);
									break;
								case GDBObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hGDB, tmpstring, icon);
									break;
								case StrngObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hString, tmpstring, icon);
									break;
								case RealObj:
								case CplxObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hNumber, tmpstring, icon);
									break;
								case ListObj:
								case CListObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hList, tmpstring, icon);
									break;
								case MatObj:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hMatrix, tmpstring, icon);
									break;
	//							case EquObj:
								case EquObj_2:
									Tree[slot].hVars[i] = InsertVar(Tree[slot].hEquation, tmpstring, icon);
									break;
							}
						}
					}
				}
				//free(sym);
			}
			// If no children are found kill parent.
			for(i = 0; i < 11; i++) {
				if (Tree[slot].hTypes[i]) {
					if (TreeView_GetChild(g_hwndVarTree, Tree[slot].hTypes[i]) == NULL) {
						TreeView_DeleteItem(g_hwndVarTree, Tree[slot].hTypes[i]);
						Tree[slot].hTypes[i] = NULL;
					}
				}
			}
			TreeView_Expand(g_hwndVarTree, Tree[slot].hRoot, TVE_EXPAND);
		}
	}
}
	
	
FILEDESCRIPTOR *FillDesc(HTREEITEM hSelect,  FILEDESCRIPTOR *fd) {
	int slot;
	u_int i;
	TCHAR string[MAX_PATH];
	memset(string, 0, sizeof(string));
	for(slot = 0; slot < MAX_CALCS; slot++) {
		if (Tree[slot].model) {
			for(i = 0; i < Tree[slot].applist.count; i++) {
				if (Tree[slot].hApps[i] == hSelect) {
					if (App_Name_to_String(&Tree[slot].applist.apps[i], string)) {
						StringCbCat(string, sizeof(string), _T(".8xk"));
						MFILE *outfile = ExportApp(&calcs[slot], NULL, &Tree[slot].applist.apps[i]);
						fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
						fd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
						fd->nFileSizeLow = msize(outfile);
						StringCbCopy(fd->cFileName, sizeof(fd->cFileName), string);
						mclose(outfile);
						return fd;
					}
					_tprintf_s(_T("%s\n"), Tree[slot].applist.apps[i].name);
					return NULL;
				}
			}
			for(i = 0; i < (u_int) (Tree[slot].sym.last - Tree[slot].sym.symbols + 1); i++) {
				if (Tree[slot].hVars[i] == hSelect) {
					if (Symbol_Name_to_String(Tree[slot].model, &Tree[slot].sym.symbols[i], string)) {
						StringCbCat(string, sizeof(string), _T("."));
						StringCbCat(string, sizeof(string), (const TCHAR *) type_ext[Tree[slot].sym.symbols[i].type_ID]);
						MFILE *outfile = ExportVar(&calcs[slot], NULL, &Tree[slot].sym.symbols[i]);
						fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
						fd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
						fd->nFileSizeLow = msize(outfile);
						StringCbCopy(fd->cFileName, sizeof(fd->cFileName), string);
						mclose(outfile);
						return fd;
					}
				}
			}
		}
	}
	return NULL;
}
	
void *FillFileBuffer(HTREEITEM hSelect, void *buf) {
	u_int slot, i, b;
	_TUCHAR *buffer = (_TUCHAR *) buf;
	TCHAR string[64];
	memset(string, 0, sizeof(string));
	_tprintf_s(_T("Fill file buffer\n"));
	for(slot = 0; slot < MAX_CALCS; slot++) {
		if (Tree[slot].model) {
			_tprintf_s(_T("model found\n"));
			for(i = 0; i < Tree[slot].applist.count; i++) {
				if (Tree[slot].hApps[i] == hSelect) {
					MFILE *outfile = ExportApp(&calcs[slot], NULL, &Tree[slot].applist.apps[i]);
					if(!outfile) _putts(_T("MFile not found"));
					_tprintf_s(_T("size: %d\n"), outfile->size);
					for(b = 0; b < outfile->size; b++) {
						_tprintf_s(_T("%02X"), outfile->data[b]);
						buffer[b] = outfile->data[b];
					}
					_tprintf_s(_T("\n"));
					mclose(outfile);
					return buffer;
				}
			}
			for(i = 0; i < (u_int) (Tree[slot].sym.last - Tree[slot].sym.symbols + 1); i++) {
				if (Tree[slot].hVars[i] == hSelect) {
					if (Symbol_Name_to_String(Tree[slot].model, &Tree[slot].sym.symbols[i], string)) {
						MFILE *outfile = ExportVar(&calcs[slot], NULL, &Tree[slot].sym.symbols[i]);
						if(!outfile) _putts(_T("MFile not found"));
						_tprintf_s(_T("size: %d\n"), outfile->size);
						for(b = 0; b < outfile->size; b++) {
							_tprintf_s(_T("%02X"), outfile->data[b]);
							buffer[b] = outfile->data[b];
						}
						_tprintf_s(_T("\n"));
						mclose(outfile);
						return buffer;
					}
				}
			}
		}
	}
	return NULL;
}
