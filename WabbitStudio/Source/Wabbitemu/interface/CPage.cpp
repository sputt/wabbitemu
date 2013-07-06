#include "stdafx.h"

#include "CPage.h"

STDMETHODIMP CPage::get_Index(int *lpIndex)
{
	*lpIndex = m_iPage;
	return S_OK;
}

STDMETHODIMP CPage::get_IsFlash(VARIANT_BOOL *pbIsFlash)
{
	*pbIsFlash = (m_fIsFlash == TRUE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CPage::get_Item(LONG lIndex, BYTE *pValue)
{
	*pValue = m_lpData[lIndex % PAGE_SIZE];
	return S_OK;
}

STDMETHODIMP CPage::ReadByte(WORD wAddr, LPBYTE lpbResult)
{
	*lpbResult = m_lpData[wAddr % PAGE_SIZE];
	return S_OK;
}

STDMETHODIMP CPage::ReadWord(WORD wAddr, LPWORD lpwResult)
{
	*lpwResult = m_lpData[wAddr % PAGE_SIZE] + (m_lpData[(wAddr + 1) % PAGE_SIZE] << 8);
	return S_OK;
}

STDMETHODIMP CPage::Read(WORD wAddr, WORD wCount, LPSAFEARRAY *ppsaResult)
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
		lpData[i] = m_lpData[(wAddr + i) % PAGE_SIZE];
	}
	SafeArrayUnaccessData(sa);

	*ppsaResult = sa.Detach();
	return S_OK;
}

STDMETHODIMP CPage::Write(WORD Address, VARIANT varValue)
{
	if (V_VT(&varValue) & VT_ARRAY)
	{
		LONG LBound, UBound;
		SafeArrayGetLBound(V_ARRAY(&varValue), 1, &LBound);
		SafeArrayGetUBound(V_ARRAY(&varValue), 1, &UBound);

		LPBYTE lpData = NULL;
		SafeArrayAccessData(V_ARRAY(&varValue), (LPVOID *) &lpData);
		memcpy(&m_lpData[Address % PAGE_SIZE], lpData, UBound - LBound + 1);
		SafeArrayUnaccessData(V_ARRAY(&varValue));
	}
	else
	{
		m_lpData[Address % PAGE_SIZE] = (BYTE) V_I4(&varValue);
	}
	return S_OK;
}
