#pragma once

#include "stdafx.h"
#include "Collection.h"
#include "calc.h"

typedef std::vector< CAdapt< CComPtr<ITIApplication> > > APPLICATIONVECTOR;

class ATL_NO_VTABLE CTIApplicationCollection :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl< ComCollImpl<ITIApplication, ITIApplicationCollection>, &__uuidof(ITIApplicationCollection) >
{
	BEGIN_COM_MAP(CTIApplicationCollection)
		COM_INTERFACE_ENTRY(ITIApplicationCollection)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()


	STDMETHOD(get_Item)(LONG lIndex, ITIApplication **ppApplication)
	{
		return ComCollImpl<ITIApplication, ITIApplicationCollection>::get_Item(lIndex + 1, ppApplication);
	}

	STDMETHOD(put_Item)(LONG, ITIApplication *)
	{
		return E_NOTIMPL;
	}

	HRESULT Initialize(IMemoryContext *pMem, LPCALC lpCalc);

};
