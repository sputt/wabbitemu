#include "stdafx.h"

#include "CPage.h"
#include "CMemoryContext.h"
#include "CPageCollection.h"

#include "CBank.h"

HRESULT CMemoryContext::Initialize(memc *mem)
{
	HRESULT hr;

	// RAM
	CComObject<CPageCollection> *pRAMObj;
	hr = CComObject<CPageCollection>::CreateInstance(&pRAMObj);
	ATLENSURE_RETURN_HR(SUCCEEDED(hr), hr);

	PAGEVECTOR ramvec;
	for (int i = 0; i < mem->ram_pages; i++)
	{
		CComObject<CPage> *pPageObj;
		CComObject<CPage>::CreateInstance(&pPageObj);

		pPageObj->Initialize(mem, FALSE, i);
		CAdapt< CComPtr<IPage> > pPage = pPageObj;
		ramvec.push_back(pPage);
	}

	pRAMObj->Initialize(ramvec);
	m_pRAMPages = pRAMObj;

	CComObject<CPageCollection> *pFlashObj;
	hr = CComObject<CPageCollection>::CreateInstance(&pFlashObj);
	ATLENSURE_RETURN_HR(SUCCEEDED(hr), hr);

	PAGEVECTOR flashvec;
	for (int i = 0; i < mem->flash_pages; i++)
	{
		CComObject<CPage> *pPageObj;
		CComObject<CPage>::CreateInstance(&pPageObj);

		pPageObj->Initialize(mem, TRUE, i);
		CAdapt< CComPtr<IPage> > pPage = pPageObj;
		flashvec.push_back(pPage);
	}

	pFlashObj->Initialize(flashvec);
	m_pFlashPages = pFlashObj;


	CComObject<CBank> *pBankObj;
	hr = CComObject<CBank>::CreateInstance(&pBankObj);
	ATLENSURE_RETURN_HR(SUCCEEDED(hr), hr);

	hr = pBankObj->Initialize(mem, this);
	ATLENSURE_RETURN_HR(SUCCEEDED(hr), hr);

	return S_OK;
}
