#pragma once

#include "core.h"
#include "state.h"
#include "calc.h"

class ATL_NO_VTABLE CTIApplication :
	public CComObjectRootEx<CComMultiThreadModel>,
	public IDispatchImpl<ITIApplication>
{
public:
	BEGIN_COM_MAP(CTIApplication)
		COM_INTERFACE_ENTRY(ITIApplication)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	// ITIApplication methods
	STDMETHOD(get_PageCount)(int *lpPageCount);
	STDMETHOD(get_Page)(IPage **lppPage);
	STDMETHOD(get_Name)(BSTR *lpName);

	STDMETHOD(Export)(BSTR lpFileName);

	void Initialize(IMemoryContext *pMem, LPCALC lpCalc, apphdr_t app)
	{
		m_lpCalc = lpCalc;
		m_pMem = pMem;
		m_App = app;
	}

private:
	CComPtr<IMemoryContext> m_pMem;
	apphdr_t m_App;
	LPCALC m_lpCalc;
};

