#include "stdafx.h"

#include "gui.h"
#include "calc.h"
#include "DropTarget.h"
#include "SendFilesWindows.h"
#include "fileutilities.h"
#include "var.h"

CDropTarget::CDropTarget(HWND hwnd) {
	m_hwndTarget = hwnd;
	m_lRefCount = 1;
	m_fAllowDrop = FALSE;

	m_nRequired = 0;
	m_pRequired = NULL;

	m_nAccepted = 0;
	m_pAccepted = NULL;

	CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_SERVER, 
		IID_IDropTargetHelper, (LPVOID *) &m_pDropTargetHelper);
}

CDropTarget::~CDropTarget() {
	free(m_pRequired);
	free(m_pAccepted);
	list<tstring>::iterator it;
	for (it = tempFiles.begin(); it != tempFiles.end(); it++) {
		_tremove((*it).c_str());
	}
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
		if (m_pDropTargetHelper != NULL)
		{
			m_pDropTargetHelper->Release();
		}
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


HRESULT __stdcall CDropTarget::DragEnter(IDataObject *pDataObject, DWORD,
	POINTL pt, DWORD *pdwEffect)
{
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
		m_DropPt.x = pt.x;
		m_DropPt.y = pt.y;
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


	CLIPFORMAT format = (CLIPFORMAT)RegisterClipboardFormat(_T("DropDescription"));
	if (format == NULL) {
		return E_FAIL;
	}

	FORMATETC fmtetc[] = {{format, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }};
	STGMEDIUM stgmed[] = {{0}};
	
	stgmed[0].tymed = TYMED_HGLOBAL;
	stgmed[0].hGlobal = GlobalAlloc(GHND, sizeof(DROPDESCRIPTION));
	
	DROPDESCRIPTION *pdd = (DROPDESCRIPTION *) GlobalLock(stgmed[0].hGlobal);

	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
	if (lpMainWindow == NULL) {
		return E_POINTER;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return E_POINTER;
	}

	if (m_fAllowDrop == TRUE) {
		pdd->type = DROPIMAGE_COPY;
		wcscpy_s(pdd->szMessage, L"Transfer to %1");
		wcscpy_s(pdd->szInsert, L"Wabbitemu");
		lpMainWindow->bDoDrag = TRUE;

		*pdwEffect = *pdwEffect & DROPEFFECT_COPY;
	} else {
		pdd->type = DROPIMAGE_INVALID;
		wcscpy_s(pdd->szMessage, L"Cannot transfer to %1");
		wcscpy_s(pdd->szInsert, L"Wabbitemu");
		lpMainWindow->bDoDrag = FALSE;

		*pdwEffect = DROPEFFECT_NONE;
	}
	GlobalUnlock(stgmed[0].hGlobal);

	return pDataObject->SetData(fmtetc, stgmed, TRUE);
}

HRESULT __stdcall CDropTarget::DragOver(DWORD, POINTL pt, DWORD *pdwEffect) {
	if (m_pDropTargetHelper != NULL) {
		POINT p = {pt.x, pt.y};
		m_pDropTargetHelper->DragOver(&p, *pdwEffect);
	}

	if (m_fAllowDrop == TRUE) {
		*pdwEffect = *pdwEffect & DROPEFFECT_COPY;
		m_DropPt.x = pt.x;
		m_DropPt.y = pt.y;
	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}
	return S_OK;
}

HRESULT __stdcall CDropTarget::DragLeave() {
	if (m_pDropTargetHelper != NULL) {
		m_pDropTargetHelper->DragLeave();
	}

	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
	if (lpMainWindow == NULL) {
		return E_POINTER;
	}

	lpMainWindow->bDoDrag = false;

	return S_OK;
}

BOOL CDropTarget::CheckValidData(IDataObject *pDataObject) {
	STGMEDIUM stgmed;
	TCHAR path[MAX_PATH];
	ZeroMemory(path, sizeof(path));
	BOOL valid = FALSE;
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
	if (lpMainWindow == NULL) {
		return E_POINTER;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return E_POINTER;
	}

	lpMainWindow->is_archive_only = TRUE;
	lpMainWindow->is_calc_file = FALSE;

	for (UINT i = 0; i < m_nAccepted && !valid; i++) {
		if (SUCCEEDED(pDataObject->GetData(&m_pAccepted[i], &stgmed))) {
			switch (m_pAccepted[i].cfFormat) {
				case CF_HDROP: {
					PVOID pData = GlobalLock(stgmed.hGlobal);
					UINT count = DragQueryFile((HDROP) pData, (UINT) ~0, path, 256);
					while (count--) {
						DragQueryFile((HDROP) pData, count, path, ARRAYSIZE(path));
						TIFILE_t *tifile = importvar(path, TRUE);
						if (tifile == NULL) {
							continue;
						}

						switch (tifile->type) {
						case FLASH_TYPE:
							if (lpCalc->model >= TI_73) {
								valid = TRUE;
								lpMainWindow->is_calc_file = TRUE;
							}
							break;
						case ROM_TYPE:
						case LABEL_TYPE:
						case SAV_TYPE:
						case BREAKPOINT_TYPE:
							valid = TRUE;
							break;
						case BACKUP_TYPE:
							if (tifile->backup != NULL && (lpCalc->model == TI_82 &&
								lpCalc->model == TI_73 && lpCalc->model == TI_85)) {
								valid = TRUE;
							}
							break;
						case VAR_TYPE:
						case GROUP_TYPE:
						case ZIP_TYPE:
							lpMainWindow->is_archive_only = FALSE;
							lpMainWindow->is_calc_file = TRUE;
							valid = TRUE;
							break;
						}

						FreeTiFile(tifile);
					}
					GlobalUnlock(stgmed.hGlobal);
					break;
				}
				default: {
					if (m_pAccepted[i].cfFormat == RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW)) {
						LPFILEGROUPDESCRIPTORW lpfgd = (LPFILEGROUPDESCRIPTORW)GlobalLock(stgmed.hGlobal);

						for (u_int i = 0; i < lpfgd->cItems; i++) {
							TCHAR szTemp[L_tmpnam_s];
							_ttmpnam_s(szTemp);

							GetStorageString(path, sizeof(path));
							StringCbCat(path, sizeof(path), szTemp);

							CLIPFORMAT format = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILECONTENTS);
							FORMATETC fmtstm = { format, 0, DVASPECT_CONTENT, i, TYMED_ISTREAM };
							STGMEDIUM stgmedData = { 0 };
							if (SUCCEEDED(pDataObject->GetData(&fmtstm, &stgmedData))) {
								LPBYTE lpBuffer = (LPBYTE)malloc(lpfgd->fgd[i].nFileSizeLow);

								ULONG cbRead = 0;
								if (SUCCEEDED(stgmedData.pstm->Read(lpBuffer, lpfgd->fgd[i].nFileSizeLow, &cbRead))) {
									FILE *file;
									_tfopen_s(&file, path, _T("wb"));
									if (file != NULL) {
										fwrite(lpBuffer, lpfgd->fgd[i].nFileSizeLow, 1, file);
										fclose(file);

										TIFILE_t *tifile = importvar(path, TRUE);
										valid = tifile != NULL;
										if (valid &&  tifile->flash == NULL) {
											lpMainWindow->is_archive_only = FALSE;
										}

										if (valid && (tifile->rom || tifile->save)) {
											lpMainWindow->is_calc_file = FALSE;
										}

										if (valid && tifile->backup != NULL && (lpCalc->model != TI_82 &&
											lpCalc->model != TI_73 && lpCalc->model != TI_85))
										{
											valid = FALSE;
										}

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

HRESULT __stdcall CDropTarget::Drop(IDataObject *pDataObject, DWORD,
	POINTL pt, DWORD *pdwEffect) 
{
	LPMAINWINDOW lpMainWindow = (LPMAINWINDOW)GetWindowLongPtr(m_hwndTarget, GWLP_USERDATA);
	if (lpMainWindow == NULL) {
		return E_POINTER;
	}

	LPCALC lpCalc = lpMainWindow->lpCalc;
	if (lpCalc == NULL) {
		return E_POINTER;
	}

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
					lpMainWindow->bDoDrag = false;
					return S_OK;
				}
				default: {
					if (m_pAccepted[i].cfFormat != RegisterClipboardFormat(CFSTR_FILEDESCRIPTORW)) {
						break;
					}

					LPFILEGROUPDESCRIPTORW lpfgd = (LPFILEGROUPDESCRIPTORW) GlobalLock(stgmed.hGlobal);

					for (u_int i = 0; i < lpfgd->cItems; i++) {
						TCHAR szFileName[MAX_PATH] = { 0 };

						TCHAR szTemp[L_tmpnam_s] = { 0 };
						_ttmpnam_s(szTemp);

						GetStorageString(szFileName, sizeof(szFileName));
						StringCbCat(szFileName, sizeof(szFileName), szTemp);

						CLIPFORMAT format = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILECONTENTS);
						FORMATETC fmtstm = {format, 0, DVASPECT_CONTENT, i, TYMED_ISTREAM};
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
									SendFileToCalc(lpMainWindow->hwndFrame, lpCalc, szFileName, TRUE, DropMemoryTarget(m_hwndTarget));
									tempFiles.push_back(szFileName);
								}
							}
							free(lpBuffer);
							ReleaseStgMedium(&stgmedData);
						}
					}
					GlobalUnlock(stgmed.hGlobal);
					break;
				}
			}

			ReleaseStgMedium(&stgmed);
			}
		}

	} else {
		*pdwEffect = DROPEFFECT_NONE;
	}
	
	lpMainWindow->bDoDrag = false;
	return S_OK;
}

SEND_FLAG CDropTarget::DropMemoryTarget(HWND hwnd) {
	RECT lr, rr;
	POINT p;
	SEND_FLAG ram;

	GetClientRect(hwnd, &lr);
	CopyRect(&rr, &lr);
	lr.right /= 2;			//left half
	rr.left = lr.right;		//right half
	p = m_DropPt;			//DragQueryPoint((HDROP) wParam, &p);
//	printf("p %d,%d\n",p.x,p.y);
	SwitchToThisWindow(GetParent(hwnd), TRUE);

	ScreenToClient(hwnd, (LPPOINT) &p);

	if (PtInRect(&rr, p)) ram = SEND_ARC;
	else if (PtInRect(&lr, p)) ram = SEND_RAM;
	else ram = SEND_CUR;

	return ram;
}
