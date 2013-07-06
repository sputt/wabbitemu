#pragma once

#include "CPageCollection.h"
#include "core.h"

class ATL_NO_VTABLE CMemoryContext :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<IMemoryContext>
{
public:
	BEGIN_COM_MAP(CMemoryContext)
		COM_INTERFACE_ENTRY(IMemoryContext)
		COM_INTERFACE_ENTRY(IReadable)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(get_RAM)(IPageCollection **ppPageCollection);
	STDMETHOD(get_Flash)(IPageCollection **ppPageCollection);
	STDMETHOD(get_Bank)(IPageCollection **ppPageCollection);

	//IReadable
	STDMETHOD(ReadByte)(WORD wAddr, LPBYTE lpbResult);
	STDMETHOD(ReadWord)(WORD wAddr, LPWORD lpwResult);
	STDMETHOD(Read)(WORD wAddr, WORD wCount, LPSAFEARRAY *ppsaResult);

	STDMETHOD(Write)(WORD wAddr, VARIANT vValue);

	HRESULT Initialize(memc *memory);

private:
	memc *m_memc;
	CComPtr<IPageCollection> m_pRAMPages;
	CComPtr<IPageCollection> m_pFlashPages;
	CComPtr<IPageCollection> m_pBank;
};
