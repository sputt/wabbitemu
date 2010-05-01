#include <Windows.h>
#include <Ole2.h>
#include <ShlGuid.h>

#include "DropTarget.h"

CDropTarget::CDropTarget(HWND hwnd) {
	m_hwndTarget = hwnd;
	m_lRefCount = 1;
	m_fAllowDrop = FALSE;

	m_nRequired = 0;
	m_pRequired = NULL;

	m_nAccepted = 0;
	m_pAccepted = NULL;

	CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER, IID_IDropTargetHelper, (LPVOID *) &m_pDropTargetHelper);
}

CDropTarget::~CDropTarget() {
	free(m_pRequired);
	free(m_pAccepted);
}

void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget) {
	IDropTarget *pDropTarget = new CDropTarget(hwnd);
	CoLockObjectExternal((IUnknown *) pDropTarget, TRUE, FALSE);

	RegisterDragDrop(hwnd, pDropTarget);
	*ppDropTarget = pDropTarget;
}

HRESULT CDropTarget::AddRequiredFormat(FORMATETC *pFormatEtc) {
	if (m_nRequired == 0) {
		m_pRequired = (FORMATETC *) malloc(sizeof(FORMATETC));
	} else {
		m_pRequired = (FORMATETC *) realloc(m_pRequired, sizeof(FORMATETC) * (m_nRequired + 1));
	}

	m_pRequired[m_nRequired++] = *pFormatEtc;
	return S_OK;
}


HRESULT CDropTarget::AddAcceptedFormat(FORMATETC *pFormatEtc) {
	if (m_nAccepted == 0) {
		m_pAccepted = (FORMATETC *) malloc(sizeof(FORMATETC));
	} else {
		m_pAccepted = (FORMATETC *) realloc(m_pAccepted, sizeof(FORMATETC) * (m_nAccepted + 1));
	}

	m_pAccepted[m_nAccepted++] = *pFormatEtc;
	return S_OK;
}


void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget) {
	if (pDropTarget == NULL)
		return;

	RevokeDragDrop(hwnd);

	CoLockObjectExternal((IUnknown *) pDropTarget, FALSE, TRUE);
	pDropTarget->Release();
}


/*
 * IUnknown methods
 */
ULONG __stdcall CDropTarget::AddRef(void) {
	return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDropTarget::Release(void) {
	LONG lRefCount = InterlockedDecrement(&m_lRefCount);
	if (lRefCount == 0) {
		m_pDropTargetHelper->Release();
		delete this;
	}
	return lRefCount;
}

HRESULT __stdcall CDropTarget::QueryInterface(REFIID riid, LPVOID *ppvObject) {
	if (riid == IID_IUnknown) {
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	} else if (riid == IID_IDropTarget) {
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	} else if (m_pDropTargetHelper != NULL && riid == IID_IDropTargetHelper) {
		m_pDropTargetHelper->AddRef();
		*ppvObject = m_pDropTargetHelper;
		return S_OK;
	} else {
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}


HRESULT __stdcall CDropTarget::DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
	DWORD i;

	if (m_pDropTargetHelper != NULL) {
		POINT p = {pt.x, pt.y};
		m_pDropTargetHelper->DragEnter(m_hwndTarget, pDataObject, &p, *pdwEffect);
	}

	m_fAllowDrop = FALSE;
	for (i = 0; i < m_nRequired; i++) {
		if (FAILED(pDataObject->QueryGetData(&m_pRequired[i])))
			break;
	}

	if (i > 0) {
		m_fAllowDrop = TRUE;
	}

	for (i = 0; i < m_nAccepted; i++) {
		if (SUCCEEDED(pDataObject->QueryGetData(&m_pAccepted[i])))
			break;
	}

	if (i < m_nAccepted) {
		m_fAllowDrop = TRUE;
	}


	FORMATETC fmtetc[] = {{RegisterClipboardFormat("DropDescription"), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }};
	STGMEDIUM stgmed[] = {{0}};
	
	stgmed[0].tymed = TYMED_HGLOBAL;
	stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(DROPDESCRIPTION));
	
	DROPDESCRIPTION *pdd = (DROPDESCRIPTION *) GlobalLock(stgmed[0].hGlobal);

	if (m_fAllowDrop == TRUE) {
		pdd->type = DROPIMAGE_COPY;
		wcscpy(pdd->szMessage, L"Transfer to %1");
		wcscpy(pdd->szInsert, L"Wabbitemu");

		*pdwEffect = *pdwEffect & DROPEFFECT_COPY;
	} else {
		pdd->type = DROPIMAGE_INVALID;
		wcscpy(pdd->szMessage, L"Cannot transfer to %1");
		wcscpy(pdd->szInsert, L"Wabbitemu");

		*pdwEffect = DROPEFFECT_NONE;
	}
	GlobalUnlock(stgmed[0].hGlobal);

	return pDataObject->SetData(fmtetc, stgmed, TRUE);
}

HRESULT __stdcall CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
	if (m_pDropTargetHelper != NULL) {
		POINT p = {pt.x, pt.y};
		m_pDropTargetHelper->DragOver(&p, *pdwEffect);
	}

	if (m_fAllowDrop == TRUE) {
		*pdwEffect = *pdwEffect & DROPEFFECT_COPY;
	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}
	return S_OK;
}

HRESULT __stdcall CDropTarget::DragLeave() {
	if (m_pDropTargetHelper != NULL) {
		m_pDropTargetHelper->DragLeave();
	}

	return S_OK;
}

HRESULT __stdcall CDropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
	if (m_pDropTargetHelper != NULL) {
		POINT p = {pt.x, pt.y};
		m_pDropTargetHelper->Drop(pDataObject, &p, *pdwEffect);
	}

	if (m_fAllowDrop) {
		STGMEDIUM stgmed;

		if (m_nRequired > 0) {

		}

		for (UINT i = 0; i < m_nAccepted; i++) {
			if (SUCCEEDED(pDataObject->GetData(&m_pAccepted[i], &stgmed))) {
				switch (m_pAccepted[i].cfFormat)
				{
				case CF_HDROP:
					{
						PVOID pData = GlobalLock(stgmed.hGlobal);
						SendMessage(m_hwndTarget, WM_DROPFILES, (WPARAM) pData, NULL);

						GlobalUnlock(stgmed.hGlobal);
						ReleaseStgMedium(&stgmed);
						break;
					}
				}
			}

		}

	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}
	return S_OK;
}