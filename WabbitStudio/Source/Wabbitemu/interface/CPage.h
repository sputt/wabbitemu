#pragma once

#include "core.h"

class CPage :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<IPage>
{
public:
	BEGIN_COM_MAP(CPage)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IReadWrite)
		COM_INTERFACE_ENTRY(IPage)
	END_COM_MAP( )

	// IPage methods
	STDMETHODIMP get_Index(int *lpIndex);
	STDMETHOD(get_IsFlash)(VARIANT_BOOL *pbIsFlash);
	STDMETHOD(get_Item)(LONG lIndex, BYTE *pValue);

	//IReadWrite
	STDMETHOD(ReadByte)(WORD wAddr, LPBYTE lpbResult);
	STDMETHOD(ReadWord)(WORD wAddr, LPWORD lpwResult);
	STDMETHOD(Read)(WORD wAddr, WORD wCount, SAFEARRAY **ppsaResult);
	STDMETHOD(WriteByte)(WORD wAddr, BYTE bValue);
	STDMETHOD(WriteWord)(WORD wAddr, WORD wValue);
	STDMETHOD(Write)(WORD wAddr, SAFEARRAY *psaValue);

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
