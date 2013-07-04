#pragma once

#include "Collection.h"

struct tagCALC;

class ATL_NO_VTABLE CBreakpointCollection :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl< ComCollImpl<IBreakpoint, IBreakpointCollection>, &__uuidof(IBreakpointCollection)>
{
public:
	BEGIN_COM_MAP(CBreakpointCollection)
		COM_INTERFACE_ENTRY(IBreakpointCollection)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(Add)(ICalcAddress *pCalcAddress, IBreakpoint **ppBreakpoint);
	STDMETHOD(Remove)(IBreakpoint *ppBreakpoint);

	HRESULT Initialize(tagCALC *calc)
	{
		m_lpCalc = calc;
		return S_OK;
	}

	HRESULT LookupBreakpoint(waddr_t waddrRef, IBreakpoint **ppBP)
	{
		for (auto it = m_coll.begin(); it != m_coll.end(); it++)
		{
			CComPtr<ICalcAddress> pAddress;
			it->m_T->get_Address(&pAddress);

			waddr_t waddr;
			pAddress->get_Address(&waddr.addr);

			CComPtr<IPage> pPage;
			pAddress->get_Page(&pPage);

			VARIANT_BOOL isFlash;
			pPage->get_IsFlash(&isFlash);
			waddr.is_ram = isFlash == VARIANT_TRUE ? FALSE : TRUE;
			int nPage;
			pPage->get_Index(&nPage);
			waddr.page = nPage;

			if (memcmp(&waddrRef, &waddr, sizeof(waddr)) == 0)
			{
				return it->m_T->QueryInterface(ppBP);
			}
		}
		return S_FALSE;
	}

private:
	tagCALC *m_lpCalc;
};
