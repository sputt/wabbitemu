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
	m_pBank = pBankObj;


	m_memc = mem;
	return S_OK;
}

STDMETHODIMP CMemoryContext::get_RAM(IPageCollection **ppPageCollection)
{
	return m_pRAMPages->QueryInterface(ppPageCollection);
}

STDMETHODIMP CMemoryContext::get_Flash(IPageCollection **ppPageCollection)
{
	return m_pFlashPages->QueryInterface(ppPageCollection);
}

STDMETHODIMP CMemoryContext::get_Bank(IPageCollection **ppPageCollection)
{
	return m_pBank->QueryInterface(ppPageCollection);
}

STDMETHODIMP CMemoryContext::ReadByte(WORD wAddr, LPBYTE lpbResult)
{
	*lpbResult = mem_read(m_memc, wAddr);
	return S_OK;
}

STDMETHODIMP CMemoryContext::ReadWord(WORD wAddr, LPWORD lpwResult)
{
	*lpwResult = mem_read(m_memc, wAddr) + (mem_read(m_memc, wAddr + 1) << 8);
	return S_OK;
}

STDMETHODIMP CMemoryContext::Read(WORD wAddr, WORD wCount, LPSAFEARRAY *ppsaResult)
{
	if (wCount == 0)
	{
		return E_INVALIDARG;
	}

	CComSafeArray<BYTE> sa((LONG) wCount);

	LPBYTE lpData = NULL;
	SafeArrayAccessData(sa, (LPVOID *) &lpData);
	for (WORD i = 0; i < wCount; i++)
	{
		lpData[i] = mem_read(m_memc, wAddr + i);
	}
	SafeArrayUnaccessData(sa);

	*ppsaResult = sa.Detach();
	return S_OK;
}

STDMETHODIMP CMemoryContext::Write(WORD wAddr, VARIANT varValue)
{
	if (V_VT(&varValue) & VT_ARRAY)
	{
		LONG LBound, UBound;
		SafeArrayGetLBound(V_ARRAY(&varValue), 1, &LBound);
		SafeArrayGetUBound(V_ARRAY(&varValue), 1, &UBound);

		LPBYTE lpData = NULL;
		SafeArrayAccessData(V_ARRAY(&varValue), (LPVOID *) &lpData);
		for (int i = 0; i < UBound - LBound + 1; i++)
		{
			mem_write(m_memc, wAddr + i, lpData[i]);
		}
		SafeArrayUnaccessData(V_ARRAY(&varValue));
	}
	else
	{
		mem_write(m_memc, wAddr, (char) V_I4(&varValue));
	}
	return S_OK;
}
