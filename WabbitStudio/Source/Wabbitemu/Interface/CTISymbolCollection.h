#pragma once

#include "stdafx.h"
#include "Collection.h"
#include "calc.h"

typedef std::vector< CAdapt< CComPtr<ITISymbol> > > SYMBOLVECTOR;

class ATL_NO_VTABLE CTISymbolCollection :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl< ComCollImpl<ITISymbol, ITISymbolCollection>, &__uuidof(ITISymbolCollection) >
{
	BEGIN_COM_MAP(CTISymbolCollection)
		COM_INTERFACE_ENTRY(ITISymbolCollection)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()


	STDMETHOD(get_Item)(LONG lIndex, ITISymbol **ppSymbol)
	{
		return ComCollImpl<ITISymbol, ITISymbolCollection>::get_Item(lIndex + 1, ppSymbol);
	}

	STDMETHOD(put_Item)(LONG, ITISymbol *)
	{
		return E_NOTIMPL;
	}

	HRESULT Initialize(LPCALC lpCalc, IMemoryContext *pMem);

};
