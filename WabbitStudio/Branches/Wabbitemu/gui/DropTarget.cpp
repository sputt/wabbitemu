#include "stdafx.h"
using namespace std;

#include "calc.h"
#include "DropTarget.h"
#include "SendFilesWindows.h"
#include "fileutilities.h"

extern POINT drop_pt;

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
	pDropTarget->Release();

	CoLockObjectExternal((IUnknown *) pDropTarget, FALSE, FALSE);
}


/*
 * IUnknown methods
 */
ULONG __stdcall CDropTarget::AddRef(void) {
	OutputDebugString(_T("CDropTarget AddRef\n"));
	return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDropTarget::Release(void) {
	OutputDebugString(_T("CDropTarget Release\n"));
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
		drop_pt.x = pt.x;
		drop_pt.y = pt.y;
	}

	for (i = 0; i < m_nAccepted; i++) {
		if (SUCCEEDED(pDataObject->QueryGetData(&m_pAccepted[i]))) {
			BOOL valid = CheckValidData(pDataObject);
			if (!valid)
				i = m_nAccepted;
			break;
		}
	}

	if (i < m_nAccepted) {
		m_fAllowDrop = TRUE;
	}


	FORMATETC fmtetc[] = {{RegisterClipboardFormat(_T("DropDescription")), 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }};
	STGMEDIUM stgmed[] = {{0}};
	
	stgmed[0].tymed = TYMED_HGLOBAL;
	stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(DROPDESCRIPTION));
	
	DROPDESCRIPTION *pdd = (DROPDESCRIPTION *) GlobalLock(stgmed[0].hGlobal);

	calc_t *lpCalc = (LPCALC) GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
	if (m_fAllowDrop == TRUE) {
		pdd->type = DROPIMAGE_COPY;
		wcscpy_s(pdd->szMessage, L"Transfer to %1");
		wcscpy_s(pdd->szInsert, L"Wabbitemu");
		lpCalc->do_drag = true;

		*pdwEffect = *pdwEffect & DROPEFFECT_COPY;
	} else {
		pdd->type = DROPIMAGE_INVALID;
		wcscpy_s(pdd->szMessage, L"Cannot transfer to %1");
		wcscpy_s(pdd->szInsert, L"Wabbitemu");
		lpCalc->do_drag = false;

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
		drop_pt.x = pt.x;
		drop_pt.y = pt.y;
	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}
	return S_OK;
}

HRESULT __stdcall CDropTarget::DragLeave() {
	if (m_pDropTargetHelper != NULL) {
		m_pDropTargetHelper->DragLeave();
	}

	LPCALC lpCalc = (LPCALC) GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
	if (lpCalc != NULL) {
		lpCalc->do_drag = false;
	}

	return S_OK;
}

static BOOL WriteStreamToFile(IStream *pStream, LPCTSTR lpszFileName) {
	//pStream->g
}

extern BOOL is_archive_only;
extern BOOL is_calc_file;
BOOL CDropTarget::CheckValidData(IDataObject *pDataObject) {
	STGMEDIUM stgmed;
	TCHAR path[MAX_PATH];
	ZeroMemory(path, sizeof(path));
	BOOL valid = TRUE;
	is_archive_only = TRUE;
	is_calc_file = TRUE;
	for (UINT i = 0; i < m_nAccepted; i++) {
		if (SUCCEEDED(pDataObject->GetData(&m_pAccepted[i], &stgmed))) {
			switch (m_pAccepted[i].cfFormat) {
				case CF_HDROP: {
					PVOID pData = GlobalLock(stgmed.hGlobal);
					int count = DragQueryFile((HDROP) pData, ~0, path, 256);
					while (count--) {
						DragQueryFile((HDROP) pData, count, path, ARRAYSIZE(path));
						TIFILE_t *tifile = importvar(path, TRUE);
						valid = tifile != NULL;
						//check if we can go in either ram or archive
						if (tifile && tifile->flash == NULL)
							is_archive_only = FALSE;
						//check if were a file that doesn't go in ram or archive
						if (tifile && (tifile->type == ROM_TYPE || tifile->type == SAV_TYPE || tifile->type == LABEL_TYPE || tifile->type == BREAKPOINT_TYPE))
							is_calc_file = FALSE;
						calc_t *lpCalc = (LPCALC) GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
						if (tifile && tifile->backup != NULL && (lpCalc->model != TI_82 && lpCalc->model != TI_73 && lpCalc->model != TI_85))
							valid = FALSE;
						FreeTiFile(tifile);
					}
					GlobalUnlock(stgmed.hGlobal);
					break;
				}
				default: {
					if (m_pAccepted[i].cfFormat == RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW)) {
						LPFILEGROUPDESCRIPTORW lpfgd = (LPFILEGROUPDESCRIPTORW) GlobalLock(stgmed.hGlobal);
						LPTSTR lpszFileGroup = NULL;

						for (u_int i = 0; i < lpfgd->cItems; i++) {
							TCHAR szTemp[L_tmpnam_s];
							_ttmpnam_s(szTemp);

							GetAppDataString(path, sizeof(path));
							StringCbCat(path, sizeof(path), szTemp);

							FORMATETC fmtstm = {RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, i, TYMED_ISTREAM};
							STGMEDIUM stgmedData = {0};
							if (SUCCEEDED(pDataObject->GetData(&fmtstm, &stgmedData))) {
								LPBYTE lpBuffer = (LPBYTE) malloc(lpfgd->fgd[i].nFileSizeLow);

								ULONG cbRead = 0;
								if (SUCCEEDED(stgmedData.pstm->Read(lpBuffer, lpfgd->fgd[i].nFileSizeLow, &cbRead))) {
									FILE *file;
									_tfopen_s(&file, path, _T("wb"));
									if (file != NULL) {
										fwrite(lpBuffer, lpfgd->fgd[i].nFileSizeLow, 1, file);
										fclose(file);

										TIFILE_t *tifile = importvar(path, TRUE);
										valid = tifile != NULL;
										if (tifile->flash == NULL)
											is_archive_only = FALSE;
										if (tifile->rom || tifile->save)
											is_calc_file = FALSE;
										calc_t *lpCalc = (LPCALC) GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
										if (tifile && tifile->backup != NULL && (lpCalc->model != TI_82 && lpCalc->model != TI_73 && lpCalc->model != TI_85))
											valid = FALSE;
										FreeTiFile(tifile);
									}
									_tremove(path);
								}
								free(lpBuffer);
								ReleaseStgMedium(&stgmedData);
							}
						}
						GlobalUnlock(stgmed.hGlobal);
					}

					break;
				}
			}
		}
	}
	return valid;
}

HRESULT __stdcall CDropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
	calc_t *lpCalc = (LPCALC) GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);

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
				switch (m_pAccepted[i].cfFormat) {
					case CF_HDROP: {
							PVOID pData = GlobalLock(stgmed.hGlobal);
							SendMessage(m_hwndTarget, WM_DROPFILES, (WPARAM) pData, NULL);

							GlobalUnlock(stgmed.hGlobal);
							break;
						}
					default: {
							if (m_pAccepted[i].cfFormat == RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW)) {
								LPFILEGROUPDESCRIPTORW lpfgd = (LPFILEGROUPDESCRIPTORW) GlobalLock(stgmed.hGlobal);
								LPTSTR lpszFileGroup = NULL;
								list<TCHAR *> files(lpfgd->cItems, NULL);

								for (u_int i = 0; i < lpfgd->cItems; i++) {
									TCHAR szFileName[MAX_PATH];
									ZeroMemory(szFileName, sizeof(szFileName));

									TCHAR szTemp[L_tmpnam_s];
									ZeroMemory(szTemp, sizeof(szTemp));	
									_ttmpnam_s(szTemp);

									GetAppDataString(szFileName, sizeof(szFileName));
									StringCbCat(szFileName, sizeof(szFileName), szTemp);

									FORMATETC fmtstm = {RegisterClipboardFormat(CFSTR_FILECONTENTS), 0, DVASPECT_CONTENT, i, TYMED_ISTREAM};
									STGMEDIUM stgmedData = {0};
									if (SUCCEEDED(pDataObject->GetData(&fmtstm, &stgmedData))) {
										LPBYTE lpBuffer = (LPBYTE) malloc(lpfgd->fgd[i].nFileSizeLow);

										ULONG cbRead = 0;
										if (SUCCEEDED(stgmedData.pstm->Read(lpBuffer, lpfgd->fgd[i].nFileSizeLow, &cbRead))) {
											FILE *file;
											_tfopen_s(&file, szFileName, _T("wb"));
											if (file != NULL) {
												fwrite(lpBuffer, lpfgd->fgd[i].nFileSizeLow, 1, file);
												fclose(file);
												SendFileToCalc(lpCalc, szFileName, TRUE, DropMemoryTarget(m_hwndTarget));
												files.push_back(szFileName);
											}
										}
										free(lpBuffer);
										ReleaseStgMedium(&stgmedData);
									}
								}
								list<TCHAR *>::iterator it;
								for (it = files.begin(); it != files.end(); it++)
									_tremove(*it);
								GlobalUnlock(stgmed.hGlobal);
							}

							break;
						}
				}
				ReleaseStgMedium(&stgmed);
			}

		}

	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}
	
	lpCalc->do_drag = false;
	return S_OK;
}

SEND_FLAG DropMemoryTarget(HWND hwnd) {
	RECT lr, rr;
	POINT p;
	SEND_FLAG ram;

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
