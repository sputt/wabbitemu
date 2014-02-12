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


	STDMETHOD(get_Item)(LONG lIndex, IPage **ppPage)
	{
		return ComCollImpl<IPage, IPageCollection>::get_Item(lIndex + 1, ppPage);
	}

	STDMETHOD(put_Item)(LONG, IPage *)
	{
		return E_NOTIMPL;
	}

	HRESULT Initialize(PAGEVECTOR &Pages);

};
