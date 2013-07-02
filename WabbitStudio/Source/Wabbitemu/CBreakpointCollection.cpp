#include "stdafx.h"

#include "calc.h"
#include "core.h"
#include "CBreakpointCollection.h"
#include "CBreakpoint.h"

#include "CCalcAddress.h"

STDMETHODIMP CBreakpointCollection::Add(ICalcAddress *pAddress, IBreakpoint **ppBreakpoint)
{
	waddr_t waddr = CCalcAddress::ToWAddr(pAddress);

	set_break(&m_lpCalc->mem_c, waddr);
	
	CComObject<CBreakpoint> *pObj;
	CComObject<CBreakpoint>::CreateInstance(&pObj);
	pObj->AddRef();

	pObj->Initialize(m_lpCalc, pAddress);

	m_coll.push_back(CAdapt< CComPtr<IBreakpoint> >(pObj));

	pObj->QueryInterface(ppBreakpoint);

	pObj->Release();
	return S_OK;
}

STDMETHODIMP CBreakpointCollection::Remove(IBreakpoint *pBreakpoint)
{
	CComPtr<IUnknown> pUnkBP;
	pBreakpoint->QueryInterface(&pUnkBP);

	for (auto it = m_coll.begin(); it != m_coll.end(); it++)
	{
		CComPtr<IUnknown> pUnkRefBP;
		it->m_T->QueryInterface(&pUnkRefBP);

		if (pUnkRefBP.p == pUnkBP.p)
		{
			m_coll.erase(it);

			CComPtr<ICalcAddress> pAddress;
			pBreakpoint->get_Address(&pAddress);

			clear_break(&m_lpCalc->mem_c, CCalcAddress::ToWAddr(pAddress));
			return S_OK;
		}
	}

	return S_FALSE;
}
