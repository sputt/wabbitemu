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
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(get_RAM)(IPageCollection **ppPageCollection)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Flash)(IPageCollection **ppPageCollection)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Bank)(IPageCollection **ppPageCollection)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(Read)(WORD wAddr, VARIANT vCount, LPVARIANT lpvResult)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(Write)(WORD wAddr, VARIANT vValue)
	{
		return E_NOTIMPL;
	}

	HRESULT Initialize(memc *memory);

private:
	memc *m_memc;
	CComPtr<IPageCollection> m_pRAMPages;
	CComPtr<IPageCollection> m_pFlashPages;
	CComPtr<IPageCollection> m_pBank;
};
