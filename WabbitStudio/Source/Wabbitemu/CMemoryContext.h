#pragma once

class ATL_NO_VTABLE CMemoryContext :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<IMemoryContext>
{
public:
	BEGIN_COM_MAP(CMemoryContext)
		COM_INTERFACE_ENTRY(IMemoryContext)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(get_RAM)(LPSAFEARRAY lpsaRAM);
	STDMETHOD(get_Flash)(LPSAFEARRAY lpsaFlash);
	STDMETHOD(get_Bank)(LPSAFEARRAY lpsaBank);

	STDMETHOD(Read)(WORD wAddr, VARIANT vCount, LPVARIANT lpvResult);
	STDMETHOD(Write)(WORD wAddr, VARIANT vValue);
};