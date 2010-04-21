//
//	from DROPTARGET.CPP
//
//	By J Brown 2004
//
//	www.catch22.net
//
//  this is 'pure C' version
#define STRICT

#include <windows.h>
#include "wbded.h"
#include "coretypes.h"
#include "sendfiles.h"
#include "link.h"
#include "calc.h"

HRESULT CreateDropTarget(HWND hwnd, WB_IDropTarget **ppDropTarget);
void DropData(HWND hwnd, IDataObject *pDataObject);

extern POINT drop_pt;

int DropMemoryTarget(HWND hwnd) {
	RECT lr, rr;
	POINT p;
	int ram;

	GetClientRect(hwnd, &lr);
	CopyRect(&rr, &lr);
	lr.right /= 2;			//left half
	rr.left = lr.right;		//right half
	p = drop_pt;			//DragQueryPoint((HDROP) wParam, &p);
//	printf("p %d,%d\n",p.x,p.y);
	SwitchToThisWindow(GetParent(hwnd), TRUE);

	ScreenToClient(hwnd, (LPPOINT) &p);

	if (PtInRect(&rr, p)) ram = SEND_ARC;
	else if (PtInRect(&lr, p)) ram = SEND_RAM;
	else ram = SEND_CUR;

	return ram;
}


/*
 * Query the object to see what types of data it supports
 */

static BOOL QueryDataObject (WB_IDataObject *pDataObject)
{
	FORMATETC fmtetcWabbitShot =
		{RegisterClipboardFormat("WabbitShot"), 0, DVASPECT_CONTENT, 0, TYMED_NULL};

	// Don't let it accept its own screenshots
	if (pDataObject->ido.lpVtbl->QueryGetData((LPDATAOBJECT)pDataObject, &fmtetcWabbitShot) == S_OK)
		return FALSE;

	FORMATETC fmtetc[] = {
			{CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},
			{RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL}
	};

	int i;
	for (i = 0; i < NumElm(fmtetc); i++) {
		if (pDataObject->ido.lpVtbl->QueryGetData((LPDATAOBJECT)pDataObject, &fmtetc[i]) == S_OK)
			return TRUE;
	}

	return FALSE;

}

//
//	DropEffect private helper routine
//
static DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = 0;

	// 1. check "pt" -> do we allow a drop at the specified coordinates?

	// 2. work out that the drop-effect should be based on grfKeyState
	if(grfKeyState & MK_CONTROL)
	{
		dwEffect = dwAllowed & DROPEFFECT_COPY;
	}
	else if(grfKeyState & MK_SHIFT)
	{
		dwEffect = dwAllowed & DROPEFFECT_MOVE;
	}

	// 3. no key-modifiers were specified (or drop effect not allowed), so
	//    base the effect on those allowed by the dropsource
	if(dwEffect == 0)
	{
		if(dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
		if(dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
	}

	return dwEffect;
}

//
//	IUnknown::AddRef
//
static ULONG STDMETHODCALLTYPE
idt_addref(WB_IDropTarget *This)
{
  return InterlockedIncrement(&This->m_lRefCount);
}

//
//	IUnknown::QueryInterface
//
static HRESULT STDMETHODCALLTYPE
idt_queryinterface(	WB_IDropTarget *This,
			       	REFIID riid,
			       	LPVOID *ppvObject)
{

	if (IsEqualIID (riid, &IID_IUnknown) || IsEqualIID (riid, &IID_IDropTarget))
	{
		idt_addref(This);
		*ppvObject = This;
		return S_OK;
	} else if (IsEqualIID(riid, &IID_IDropTargetHelper) && This->m_pdth) {
		printf("It's asking for IDropTargetHelper...\n");
		This->m_pdth->lpVtbl->AddRef((IDropTargetHelper *) This->m_pdth);
		*ppvObject = This->m_pdth;
		return S_OK;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
    }
}


//
//	IUnknown::Release
//
static ULONG STDMETHODCALLTYPE
idt_release(WB_IDropTarget *This)
{
	LONG count = InterlockedDecrement(&This->m_lRefCount);

	if(count == 0) {
		printf("Freeing droptarget\n");
		g_free(This);
		printf("done freeing\n");
	}

	return count;
}

//
//	IDropTarget::DragEnter
//
//
//
static HRESULT STDMETHODCALLTYPE
idt_dragenter(	WB_IDropTarget *This,
				IDataObject *pDataObject,
				DWORD grfKeyState,
				POINTL pt,
				DWORD *pdwEffect )
{
	if (This->m_pdth) {
		POINT p = {pt.x, pt.y};
		This->m_pdth->lpVtbl->DragEnter(This->m_pdth, This->m_hWnd, (LPDATAOBJECT) pDataObject, &p, *pdwEffect);
	}

	// does the dataobject contain data we want?
	This->m_fAllowDrop = QueryDataObject((WB_IDataObject *) pDataObject);

	if(This->m_fAllowDrop)
	{
		// get the dropeffect based on keyboard state
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);

		SetFocus(This->m_hWnd);

		drop_pt = pt;
		calcs[This->m_slot].do_drag = TRUE;

		FORMATETC fmtetc[] = {{RegisterClipboardFormat("DropDescription"), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }};
		STGMEDIUM stgmed[] = {{0}};

		stgmed[0].tymed = TYMED_HGLOBAL;
		stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(DROPDESCRIPTION));

		DROPDESCRIPTION *pdd = GlobalLock(stgmed[0].hGlobal);

	    pdd->type = DROPIMAGE_COPY;
	    wcscpy(pdd->szMessage, L"Transfer to %1");
	    wcscpy(pdd->szInsert, L"Wabbitemu");

		GlobalUnlock(stgmed[0].hGlobal);

		if (SUCCEEDED(pDataObject->lpVtbl->SetData(pDataObject, &fmtetc[0], &stgmed[0], TRUE))) {
			;
		} else {
			printf("Failed to set the DropDescription... Bad news.\n");
		}
	}
	else
	{
		FORMATETC fmtetc[] = {{RegisterClipboardFormat("DropDescription"), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }};
		STGMEDIUM stgmed[] = {{0}};

		stgmed[0].tymed = TYMED_HGLOBAL;
		stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(DROPDESCRIPTION));

		DROPDESCRIPTION *pdd = GlobalLock(stgmed[0].hGlobal);

	    pdd->type = DROPIMAGE_NONE;
	    wcscpy(pdd->szMessage, L"Cannot transfer to %1");
	    wcscpy(pdd->szInsert, L"Wabbitemu");

		GlobalUnlock(stgmed[0].hGlobal);

		pDataObject->lpVtbl->SetData(pDataObject, &fmtetc[0], &stgmed[0], TRUE);

		*pdwEffect = DROPEFFECT_NONE;
	}

	return S_OK;
}

//
//	IDropTarget::DragOver
//
//
//
static HRESULT STDMETHODCALLTYPE
idt_dragover(	WB_IDropTarget *This,
				DWORD grfKeyState,
				POINTL pt,
				PDWORD pdwEffect )
{
	if (This->m_pdth) {
		POINT p = {pt.x, pt.y};
		This->m_pdth->lpVtbl->DragOver(This->m_pdth, &p, *pdwEffect);
	}

	if(This->m_fAllowDrop)
	{
		drop_pt = pt;

		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	return S_OK;
}

//
//	IDropTarget::DragLeave
//
static HRESULT STDMETHODCALLTYPE
idt_dragleave(WB_IDropTarget *This)
{
	if (This->m_pdth) {
		This->m_pdth->lpVtbl->DragLeave(This->m_pdth);
	}
	calcs[This->m_slot].do_drag = FALSE;
	return S_OK;
}

//
//	IDropTarget::Drop
//
//
static HRESULT STDMETHODCALLTYPE
idt_drop(	WB_IDropTarget *This,
			IDataObject *pDataObject,
			DWORD grfKeyState,
			POINTL pt,
			DWORD *pdwEffect)
{
	if (This->m_pdth) {
		POINT p = {pt.x, pt.y};
		This->m_pdth->lpVtbl->Drop(This->m_pdth, pDataObject, &p, *pdwEffect);
	}
	if(This->m_fAllowDrop)
	{
		calcs[This->m_slot].do_drag = FALSE;
		drop_pt = pt;
		gslot = This->m_slot;
		DropData(This->m_hWnd, pDataObject);
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	return S_OK;
}

static WB_IDropTargetVtbl idt_vtbl = {
	idt_queryinterface,
	idt_addref,
	idt_release,
	idt_dragenter,
	idt_dragover,
	idt_dragleave,
	idt_drop
};

void DropData(HWND hwnd, IDataObject *pDataObject)
{
	// construct a FORMATETC object
	FORMATETC fmtetc[] = {
			{CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},
			{RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL},

	};
	STGMEDIUM stgmed;

	if(pDataObject->lpVtbl->QueryGetData(pDataObject, &fmtetc[0]) == S_OK)
	{
		// Yippie! the data is there, so go get it!
		if(pDataObject->lpVtbl->GetData(pDataObject, &fmtetc[0], &stgmed) == S_OK)
		{

			// we asked for the data as a HGLOBAL, so access it appropriately
			PVOID data = GlobalLock(stgmed.hGlobal);

			SendMessage(hwnd, WM_DROPFILES, (WPARAM) data, 0);

			GlobalUnlock(stgmed.hGlobal);

			// release the data using the COM API
			ReleaseStgMedium(&stgmed);

		}
	} else
	if(pDataObject->lpVtbl->QueryGetData(pDataObject, &fmtetc[1]) == S_OK)
	{
		if(pDataObject->lpVtbl->GetData(pDataObject, &fmtetc[1], &stgmed) == S_OK)
		{
			LPFILEGROUPDESCRIPTORW lpfgd = GlobalLock(stgmed.hGlobal);
			char *filegroup = NULL;

			int i;
			for (i = 0; i < lpfgd->cItems; i++) {
				char tmpfn[L_tmpnam];
				tmpnam(tmpfn);
				FILE *file = fopen(tmpfn, "wb");

				FORMATETC fmtstm = {RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, i, TYMED_ISTREAM};
				STGMEDIUM stgmedData;
				if(pDataObject->lpVtbl->GetData(pDataObject, &fmtstm, &stgmedData) == S_OK)
				{
					IStream *pstm = stgmedData.pstm;

					char *buffer = malloc(lpfgd->fgd[i].nFileSizeLow);
					ULONG cbRead;
					pstm->lpVtbl->Read(pstm, buffer, lpfgd->fgd[i].nFileSizeLow, &cbRead);

					fwrite(buffer, lpfgd->fgd[i].nFileSizeLow, 1, file);
					free(buffer);

					pstm->lpVtbl->Release(pstm);

					ReleaseStgMedium(&stgmedData);
				}
				filegroup = AppendName(filegroup, tmpfn);
				fclose(file);
			}

			ThreadSend(filegroup, DropMemoryTarget(hwnd));

			GlobalUnlock(stgmed.hGlobal);
			ReleaseStgMedium(&stgmed);

		}
	}
}

void RegisterDropWindow(HWND hwnd, WB_IDropTarget **ppDropTarget)
{
	WB_IDropTarget *pDropTarget;

	CreateDropTarget(hwnd, &pDropTarget);

	// acquire a strong lock (to make sure the object stays in memory)
	CoLockObjectExternal((struct IUnknown*)pDropTarget, TRUE, FALSE);

	// tell OLE that the window is a drop target
	RegisterDragDrop(hwnd, (LPDROPTARGET)pDropTarget);

	*ppDropTarget = pDropTarget;
}

void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget)
{
	if (pDropTarget == NULL)
		return;

	// remove drag+drop
	RevokeDragDrop(hwnd);

	// remove the strong lock
	CoLockObjectExternal((struct IUnknown*)pDropTarget, FALSE, TRUE);

	printf("Release our own reference \n");
	// release our own reference
	pDropTarget->lpVtbl->Release(pDropTarget);
	printf("Done unregistering drop window\n");
}

//	Constructor for the CDropTarget class
//

WB_IDropTarget *WB_IDropTarget_new(HWND hwnd)
{
  WB_IDropTarget *result;

  result = g_new0(WB_IDropTarget, 1);

  result->idt.lpVtbl = (IDropTargetVtbl*)&idt_vtbl;

  result->m_lRefCount  = 1;
  result->m_hWnd = hwnd;
  result->m_slot = gslot;
  result->m_fAllowDrop = FALSE;

   /* This call might fail, in which case OLE sets m_pdth = NULL */
   CoCreateInstance((REFCLSID) &CLSID_DragDropHelper,
  		 			NULL,
  		 			CLSCTX_INPROC_SERVER,
                    (REFIID) &IID_IDropTargetHelper,
                    (LPVOID*)&result->m_pdth);

  if (result->m_pdth == NULL) {
	  printf("Failed to create the DragDropHelper ... that's trouble\n");
  }
  return result;
}

HRESULT CreateDropTarget(HWND hwnd, WB_IDropTarget **ppDropTarget)
{
	if(ppDropTarget == 0)
		return E_INVALIDARG;

	*ppDropTarget = WB_IDropTarget_new(hwnd);

	return (*ppDropTarget ? S_OK : E_OUTOFMEMORY);

}

