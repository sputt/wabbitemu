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
	HRESULT hr = CComObject<CBreakpoint>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pObj->AddRef();

		CComPtr<ICalcAddress> pAddressCopy;
		hr = CCalcAddress::Clone(pAddress, &pAddressCopy);
		if (SUCCEEDED(hr))
		{
			hr = pObj->Initialize(m_lpCalc, pAddressCopy);
			if (SUCCEEDED(hr))
			{
				m_coll.push_back(CAdapt< CComPtr<IBreakpoint> >(pObj));
				hr = pObj->QueryInterface(ppBreakpoint);
			}
		}
		pObj->Release();
	}
	return hr;
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
