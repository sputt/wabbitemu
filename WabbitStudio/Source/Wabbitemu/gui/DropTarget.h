#pragma once

class CDropTarget : public IDropTarget
{
public:
	// IUnknown
	HRESULT __stdcall QueryInterface(REFIID iid, LPVOID *ppvObject);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);

	// IDropTarget
	HRESULT __stdcall DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	HRESULT __stdcall DragLeave();
	HRESULT __stdcall Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);

	// Misc
	HRESULT AddRequiredFormat(FORMATETC *pFormatEtc);
	HRESULT AddAcceptedFormat(FORMATETC *pFormatEtc);

	CDropTarget(HWND);
	~CDropTarget();

private:
	LONG m_lRefCount;
	BOOL m_fAllowDrop;
	HWND m_hwndTarget;
	IDropTargetHelper *m_pDropTargetHelper;
	
	FORMATETC *m_pRequired;
	UINT m_nRequired;

	FORMATETC *m_pAccepted;
	UINT m_nAccepted;
};

void RegisterDropWindow(HWND hwnd,   IDropTarget **ppDropTarget);
void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget);

int DropMemoryTarget(HWND);
