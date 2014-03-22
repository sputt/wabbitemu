#pragma once

#include "core.h"
#include "state.h"

class ATL_NO_VTABLE CTISymbol :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<ITISymbol> {
public:
	BEGIN_COM_MAP(CTISymbol)
		COM_INTERFACE_ENTRY(ITISymbol)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	// ITISymbol methods
	STDMETHOD(get_Type)(SymbolType *lpSymbolType);
	STDMETHOD(get_Version)(BYTE *lpVersion);
	STDMETHOD(get_Page)(IPage **lppPage);
	STDMETHOD(get_Address)(WORD *lpAddr);
	STDMETHOD(get_Name)(BSTR *lpName);

	void Initialize(CalcModel model, IMemoryContext *pMem, symbol83P_t symbol) {
		m_Model = model;
		m_pMem = pMem;
		m_Symbol = symbol;
	}

private:
	CalcModel m_Model;
	CComPtr<IMemoryContext> m_pMem;
	symbol83P_t m_Symbol;
};

