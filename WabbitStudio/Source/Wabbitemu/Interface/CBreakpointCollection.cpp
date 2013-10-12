#include "stdafx.h"

#include "calc.h"
#include "core.h"
#include "CBreakpointCollection.h"
#include "CBreakpoint.h"

#include "CCalcAddress.h"

STDMETHODIMP CBreakpointCollection::Add(ICalcAddress *pAddress, IBreakpoint **ppBreakpoint)
{
	if (pAddress == NULL || ppBreakpoint == NULL)
	{
		return E_INVALIDARG;
	}

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
	if (pBreakpoint == NULL)
	{
		return E_INVALIDARG;
	}

	CComPtr<IUnknown> pUnkBP;
	pBreakpoint->QueryInterface(&pUnkBP);

	CComPtr<ICalcAddress> pAddress;
	pBreakpoint->get_Address(&pAddress);

	waddr_t waddr = CCalcAddress::ToWAddr(pAddress);

	BOOL fClearBreak = TRUE;
	for (auto it = m_coll.begin(); it != m_coll.end(); it++)
	{
		CComPtr<IUnknown> pUnkRefBP;
		it->m_T->QueryInterface(&pUnkRefBP);

		CComQIPtr<IBreakpoint> pBP = pUnkRefBP;
		CComPtr<ICalcAddress> pCurAddress;
		pBreakpoint->get_Address(&pCurAddress);

		waddr_t curwaddr = CCalcAddress::ToWAddr(pCurAddress);

		if (pUnkRefBP.p == pUnkBP.p)
		{
			m_coll.erase(it);
		}
		else if (!memcmp(&waddr, &curwaddr, sizeof(waddr_t)))
		{
			fClearBreak = FALSE;
		}
	}

	if (fClearBreak)
	{
		clear_break(&m_lpCalc->mem_c, waddr);
	}

	return S_OK;
}

