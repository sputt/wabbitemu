#pragma once

class CDropSource : public IDropSource
{
public:
	// IUnknown
	HRESULT __stdcall QueryInterface(REFIID iid, LPVOID *ppvObject);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);

	// IDropSource
	HRESULT __stdcall QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT __stdcall GiveFeedback(DWORD dwEffect);

	// Constructor / destructor
	CDropSource();
	~CDropSource();

	IDataObject *m_pDataobject;

private:
	LONG m_lRefCount;
};