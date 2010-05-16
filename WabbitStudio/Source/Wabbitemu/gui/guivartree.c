#include "stdafx.h"

#include "calc.h"
#include "link.h"
#include "state.h"
#include "guivartree.h"
#include "guicontext.h"

#include "exportvar.h"
#include "resource.h"

static HWND g_hwndVarTree;
extern HINSTANCE g_hInst;
extern unsigned char type_ext[][4];

static RECT VTrc = {-1, -1, -1, -1};
static VARTREEVIEW_t Tree[MAX_CALCS];
static BOOL Tree_init = FALSE;

void test_function(int num);

BOOL VarTreeOpen(BOOL refresh){
	HWND vardialog = FindWindow(NULL,"Calculator Variables");
	if (vardialog) {
		if (refresh) {
			RefreshTreeView(FALSE);
		}
		return TRUE;
	}
	return FALSE;
}

HWND CreateVarTreeList() {
	INITCOMMONCONTROLSEX icc ;
	if (!VarTreeOpen(TRUE)) {
	    icc.dwSize = sizeof(icc);
	    icc.dwICC = ICC_TREEVIEW_CLASSES;
		if (!InitCommonControlsEx(&icc)) return NULL;
	    return  CreateDialog(g_hInst, 
				            MAKEINTRESOURCE(IDD_VARLIST), 
				            NULL, 
				            DlgVarlist);
	}
	return NULL;
}

#ifdef USE_COM
HRESULT CreateDropSource(WB_IDropSource **ppDropSource);
HRESULT CreateDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, WB_IDataObject **ppDataObject);
void SetDropSourceDataObject(WB_IDropSource *pDropSource, WB_IDataObject *pDataObject);
#endif


INT_PTR CALLBACK DlgVarlist(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch (Message) {
        case WM_INITDIALOG:
        {
            g_hwndVarTree = GetDlgItem(hwnd, IDC_TRV1);
            HIMAGELIST hIL = ImageList_LoadImage(g_hInst, "TIvarIcons", 
                                                    16, 0, RGB(0,255,0),
													IMAGE_BITMAP, LR_CREATEDIBSECTION);
            if (!hIL) printf("Imagelist not loaded");
            else TreeView_SetImageList(g_hwndVarTree, hIL, TVSIL_NORMAL);
            
            if (VTrc.bottom == -1) {
	            GetWindowRect(hwnd, &VTrc);	
			} else {
				MoveWindow(hwnd,VTrc.left,VTrc.top,VTrc.right-VTrc.left,VTrc.bottom-VTrc.top,TRUE);
			}
            RefreshTreeView(TRUE);
            return TRUE;
        }
        case WM_SIZE:
			{
				GetWindowRect(hwnd, &VTrc);
				int width = (VTrc.right-VTrc.left)-14-6;
				int height = (VTrc.bottom-VTrc.top)-38-30;
				MoveWindow(g_hwndVarTree,6,30,width,height,TRUE);

				break;
			}
        case WM_COMMAND:
        {
            switch (LOWORD(wParam)) {
                case IDC_REFRESH_VAR_LIST:
					RefreshTreeView(FALSE);
                    break;
				default:
					break;
            }
            return TRUE;
        }
        case WM_NOTIFY:
		{
			NMTREEVIEW *nmtv = (LPNMTREEVIEW) lParam;
			switch (((NMHDR*) lParam)->code) {
				case NM_DBLCLK:	{
					int slot,i;
//					for(slot=0;slot<MAX_CALCS;slot++) {
//						DispSymbols(&Tree[slot].sym);
//					}
					break;
				}
				case NM_RCLICK:
				{
					// Working on this ...
					HMENU hmenu;
					hmenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_VARTREE_MENU));
					
					hmenu = GetSubMenu(hmenu, 0); 

					POINT p;
					GetCursorPos(&p);
					
					OnContextMenu(hwnd, p.x, p.y, hmenu);

					DestroyMenu(hmenu); 
					return TRUE;
				}
#ifdef USE_COM
				case TVN_BEGINDRAG:	{
					TreeView_SelectItem(g_hwndVarTree, nmtv->itemNew.hItem);

					WB_IDataObject *pDataObject;
					WB_IDropSource *pDropSource;
					DWORD		 dwEffect;
		
					FORMATETC fmtetc[2] = {
						{RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL },
						{RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, 0, TYMED_HGLOBAL }};
					STGMEDIUM stgmed[2] = {
						{ TYMED_HGLOBAL, { 0 }, 0 },
						{ TYMED_HGLOBAL, { 0 }, 0 }};
		
					// transfer the current selection into the IDataObject
					stgmed[0].hGlobal = GlobalAlloc(GHND, 
										sizeof(FILEGROUPDESCRIPTOR) + sizeof(FILEDESCRIPTOR));
										
					FILEGROUPDESCRIPTOR *fgd = (FILEGROUPDESCRIPTOR*) GlobalLock(stgmed[0].hGlobal);
					
					fgd->cItems = 1;
					FILEDESCRIPTOR *fd = fgd->fgd;
					
					ZeroMemory(fd, sizeof(FILEDESCRIPTOR));
					
					if (FillDesc(nmtv->itemNew.hItem, fd) == NULL || fd->nFileSizeLow == 0) {
						GlobalFree(stgmed[0].hGlobal);
						return FALSE;
					}
					GlobalUnlock(stgmed[0].hGlobal);
		
					stgmed[1].hGlobal = GlobalAlloc(GHND, fd->nFileSizeLow);
					char *buf = GlobalLock(stgmed[1].hGlobal);
					
					FillFileBuffer(nmtv->itemNew.hItem, buf);
		
					GlobalUnlock(stgmed[1].hGlobal);
					
					// Create IDataObject and IDropSource COM objects
					CreateDropSource(&pDropSource);
					CreateDataObject(fmtetc, stgmed, 2, &pDataObject);
		
					//
					//	** ** ** The drag-drop operation starts here! ** ** **
					//
					SetDropSourceDataObject(pDropSource, pDataObject);
					DoDragDrop((IDataObject*)pDataObject, (struct IDropSource*)pDropSource, DROPEFFECT_COPY, &dwEffect);
					return TRUE;
				}
#endif
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



/* deletes parent's children, not parent*/
void DeleteChildren(HWND hwnd, HTREEITEM parent) {
	HTREEITEM Child;
	while(Child = TreeView_GetChild(hwnd,parent)) {
		TreeView_DeleteItem(hwnd,Child);
	}
}


HTREEITEM InsertVar(HTREEITEM parent, char* Name, int icon) {
	TVINSERTSTRUCT tvs;
	tvs.hParent				= parent;
	tvs.hInsertAfter		= TVI_SORT;
	tvs.item.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE |TVIF_TEXT;
	tvs.item.pszText		= (LPTSTR)(Name);
	tvs.item.cchTextMax		= strlen((char*)Name)+1;
	tvs.item.iImage			= icon;
	tvs.item.iSelectedImage	= tvs.item.iImage;
	return TreeView_InsertItem(g_hwndVarTree, &tvs);
}

/* updates the tree view */
void RefreshTreeView(BOOL New) {
	int i,slot,b;
	float ver;
	TVINSERTSTRUCT tvs;

	if (Tree_init == FALSE || New != FALSE) {
		memset(Tree,0,sizeof(Tree));
		Tree_init = TRUE;
	}

	/* run through all active calcs */
	for(slot = 0;slot<MAX_CALCS;slot++) {


		if (!calcs[slot].active && Tree[slot].model!=0) {
			TreeView_DeleteItem(g_hwndVarTree,Tree[slot].hRoot);
			Tree[slot].model = 0;
		}

		if (calcs[slot].active && Tree[slot].model!=calcs[slot].model && Tree[slot].model!=0) {
			TreeView_DeleteItem(g_hwndVarTree,Tree[slot].hRoot);
			Tree[slot].model = 0;
		}
		
		
		/*It's an 83+ compatible with a known rom(hopefully)*/
		if (calcs[slot].active && calcs[slot].model>=TI_83P &&
			sscanf(calcs[slot].rom_version,"%f",&ver) == 1) {
				

			/* This slot has not yet been initlised. */
			/* so set up the Root */
			if (Tree[slot].model==0) {
				tvs.hParent				= TVI_ROOT;
				tvs.hInsertAfter		= TVI_ROOT;
				tvs.item.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE |TVIF_TEXT;
				tvs.item.pszText		= (LPTSTR)CalcModelTxt[calcs[slot].model];
				tvs.item.cchTextMax		= strlen((char*)tvs.item.pszText)+1;
				tvs.item.iImage			= TI_ICON_84PSE;
				tvs.item.iSelectedImage	= tvs.item.iImage;
				Tree[slot].hRoot		= TreeView_InsertItem(g_hwndVarTree, &tvs);
			}

			/* If nodes haven't been init or the model is reset, create nodes */
			/* otherwise delete children so the vars can be appended */
			if (!Tree[slot].hApplication || Tree[slot].model==0) {
				Tree[slot].hApplication = InsertVar(Tree[slot].hRoot,"Application",TI_ICON_APP);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hApplication);

			if (!Tree[slot].hProgram || Tree[slot].model==0) {
				Tree[slot].hProgram = InsertVar(Tree[slot].hRoot,"Program",TI_ICON_PROGRAM);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hProgram);

			if (!Tree[slot].hAppVar || Tree[slot].model==0) {
				Tree[slot].hAppVar = InsertVar(Tree[slot].hRoot,"Application Variable",TI_ICON_APPVAR);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hAppVar);

			if (!Tree[slot].hPic || Tree[slot].model==0) {
				Tree[slot].hPic = InsertVar(Tree[slot].hRoot,"Picture",TI_ICON_PIC);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hPic);

			if (!Tree[slot].hGDB || Tree[slot].model==0) {
				Tree[slot].hGDB = InsertVar(Tree[slot].hRoot,"Graph Database",TI_ICON_GDB);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hGDB);

			if (!Tree[slot].hString || Tree[slot].model==0) {
				Tree[slot].hString = InsertVar(Tree[slot].hRoot,"String",TI_ICON_STRING);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hString);

			if (!Tree[slot].hNumber || Tree[slot].model==0) {
				Tree[slot].hNumber = InsertVar(Tree[slot].hRoot,"Number",TI_ICON_NUMBER);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hNumber);

			if (!Tree[slot].hList || Tree[slot].model==0) {
				Tree[slot].hList = InsertVar(Tree[slot].hRoot,"List",TI_ICON_LIST);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hList);

			if (!Tree[slot].hMatrix || Tree[slot].model==0) {
				Tree[slot].hMatrix = InsertVar(Tree[slot].hRoot,"Matrix",TI_ICON_MATRIX);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hMatrix);

			if (!Tree[slot].hGroup || Tree[slot].model==0) {
				Tree[slot].hGroup = InsertVar(Tree[slot].hRoot,"Group",TI_ICON_GROUP);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hGroup);

			if (!Tree[slot].hEquation || Tree[slot].model==0) {
				Tree[slot].hEquation = InsertVar(Tree[slot].hRoot,"Equation",TI_ICON_EQUATIONS);
			} else DeleteChildren(g_hwndVarTree,Tree[slot].hEquation);

			Tree[slot].model		= calcs[slot].model;
			
			/* Apps are handled outside of the symbol table*/
			state_build_applist(&calcs[slot].cpu,&Tree[slot].applist);
			for(i=0;i<Tree[slot].applist.count;i++) {
				Tree[slot].hApps[i] = InsertVar(Tree[slot].hApplication,Tree[slot].applist.apps[i].name,TI_ICON_FILE_ARC);
			}
			symlist_t* sym = state_build_symlist_83P(&calcs[slot].cpu,&Tree[slot].sym);
			if (sym) {
				// FIXME
				for(i=0;(&sym->symbols[i])<=(sym->last);i++) {
					char tmpstring[64];
					int icon;
					
					/* whether its archived or not */
					/* depends on the page its stored */
					if (sym->symbols[i].page) {
						icon = TI_ICON_FILE_ARC;  //red
					} else {
						icon = TI_ICON_FILE_RAM;  //green
					}
					
					if (Symbol_Name_to_String(&sym->symbols[i],tmpstring)) {
						switch(sym->symbols[i].type_ID) {
							case ProgObj:
							case ProtProgObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hProgram,tmpstring,icon);
								break;
							case AppVarObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hAppVar,tmpstring,icon);
								break;
							case GroupObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hGroup,tmpstring,icon);
								break;
							case PictObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hPic,tmpstring,icon);
								break;
							case GDBObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hGDB,tmpstring,icon);
								break;
							case StrngObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hString,tmpstring,icon);
								break;
							case RealObj:
							case CplxObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hNumber,tmpstring,icon);
								break;
							case ListObj:
							case CListObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hList,tmpstring,icon);
								break;
							case MatObj:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hMatrix,tmpstring,icon);
								break;
//							case EquObj:
							case EquObj_2:
								Tree[slot].hVars[i] = InsertVar(Tree[slot].hEquation,tmpstring,icon);
								break;
						}
					}
				}
				free(sym);
			}
			// If no children are found kill parent.
			for(i=0;i<11;i++) {
				if (Tree[slot].hTypes[i]) {
					if (TreeView_GetChild(g_hwndVarTree,Tree[slot].hTypes[i]) == NULL) {
						TreeView_DeleteItem(g_hwndVarTree,Tree[slot].hTypes[i]);
						Tree[slot].hTypes[i] = NULL;
					}
				}
			}
		}
	}
}
	
	
FILEDESCRIPTOR *FillDesc(HTREEITEM hSelect,  FILEDESCRIPTOR *fd) {
	int slot,i,b;
	char string[MAX_PATH];
	for(slot=0;slot<MAX_CALCS;slot++) {
		if (Tree[slot].model) {
			
			for(i=0;i<Tree[slot].applist.count;i++) {
				if (Tree[slot].hApps[i]==hSelect) {
					printf("%s\n",Tree[slot].applist.apps[i].name);
					return NULL;
				}
			}
			for(i=0;i<Tree[slot].sym.last - Tree[slot].sym.symbols + 1;i++) {
				if (Tree[slot].hVars[i]==hSelect) {
					if (Symbol_Name_to_String(&Tree[slot].sym.symbols[i],string)) {
						strcat(string,".");
						strcat(string, (const char *) type_ext[Tree[slot].sym.symbols[i].type_ID]);
						MFILE *outfile = ExportVar(slot,NULL, &Tree[slot].sym.symbols[i]);
						fd->dwFlags = FD_ATTRIBUTES | FD_FILESIZE;
						fd->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
						fd->nFileSizeLow = msize(outfile);
						strcpy(fd->cFileName,string);
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
	int slot,i,b;
	unsigned char* buffer = (unsigned char *) buf;
	char string[64];
	printf("Fill file buffer\n");
	for(slot=0;slot<MAX_CALCS;slot++) {
		if (Tree[slot].model) {
			printf("model found\n");
			for(i=0;i<Tree[slot].applist.count;i++) {
				if (Tree[slot].hApps[i]==hSelect) {
					return NULL;
				}
			}
			for(i=0;i<Tree[slot].sym.last - Tree[slot].sym.symbols+1;i++) {
				if (Tree[slot].hVars[i]==hSelect) {
					if (Symbol_Name_to_String(&Tree[slot].sym.symbols[i],string)) {
						MFILE *outfile = ExportVar(slot,NULL, &Tree[slot].sym.symbols[i]);
						if(!outfile) puts("MFile not found");
						printf("size: %d\n",outfile->size);
						for(b=0;b<outfile->size;b++) {
							printf("%02X",outfile->data[b]);
							buffer[b] = outfile->data[b];
						}
						printf("\n");
						mclose(outfile);
						return buffer;
					}
				}
			}
		}
	}
	return NULL;
}
