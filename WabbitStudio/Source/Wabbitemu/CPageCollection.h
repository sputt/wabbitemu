#pragma once

#include "Collection.h"

typedef std::vector< CAdapt< CComPtr<IPage> > > PAGEVECTOR;

class ATL_NO_VTABLE CPageCollection :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl< ComCollImpl<IPage, IPageCollection>, &__uuidof(IPageCollection) >
{
	BEGIN_COM_MAP(CPageCollection)
		COM_INTERFACE_ENTRY(IPageCollection)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(put_Item)(LONG lIndex, IPage *pPage)
	{
		return E_NOTIMPL;
	}

	HRESULT Initialize(PAGEVECTOR &Pages);

};
