#pragma once

#include <windows.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlcom.h>

#include "Wabbitemu_h.h"
#include "core.h"

class CPage :
	public IDispatchImpl<IPage, &IID_IPage, &LIBID_WabbitemuLib>,
	public CComObjectRoot
{
public:
	CPage()
	{
		BOOL m_fIsFlash = FALSE;
		int m_iPage = -1;
	};

	BEGIN_COM_MAP(CPage)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IPage)
	END_COM_MAP( )

	// IPage methods
	STDMETHODIMP get_Index(int *lpIndex);
	STDMETHOD(get_IsFlash)(VARIANT_BOOL *pbIsFlash);
	STDMETHODIMP Read(WORD Address, VARIANT varByteCount, LPVARIANT lpvarResult);
	STDMETHODIMP Write(WORD Address, VARIANT varValue);

	void Initialize(memory_context_t *mem, BOOL fIsFlash, int iPage)
	{
		m_fIsFlash = fIsFlash;
		m_iPage = iPage;
		if (fIsFlash)
		{
			m_lpData = &mem->flash[iPage * PAGE_SIZE];
		}
		else
		{
			m_lpData = &mem->ram[iPage * PAGE_SIZE];
		}
	}

private:
	BOOL m_fIsFlash;
	int m_iPage;
	LPBYTE m_lpData;
};
