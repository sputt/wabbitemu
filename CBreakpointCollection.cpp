#include "stdafx.h"

#include "calc.h"
#include "core.h"
#include "CBreakpointCollection.h"
#include "CBreakpoint.h"

STDMETHODIMP CBreakpointCollection::Add(ICalcAddress *pAddress)
{
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

	set_break(&m_lpCalc->mem_c, waddr);


	CComObject<CBreakpoint> *pObj;
	CComObject<CBreakpoint>::CreateInstance(&pObj);
	pObj->AddRef();

	pObj->Initialize(m_lpCalc, pAddress);

	m_coll.push_back(CAdapt< CComPtr<IBreakpoint> >(pObj));

	pObj->Release();
	return S_OK;
}
