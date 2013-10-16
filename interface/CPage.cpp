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

STDMETHODIMP CPage::WriteByte(WORD wAddr, BYTE bValue)
{
	m_lpData[wAddr % PAGE_SIZE] = bValue;
	return S_OK;
}

STDMETHODIMP CPage::WriteWord(WORD wAddr, WORD wValue)
{
	m_lpData[wAddr % PAGE_SIZE] = wValue & 0xFF;
	m_lpData[(wAddr + 1) % PAGE_SIZE] = (wValue >> 8) & 0xFF;
	return S_OK;
}

STDMETHODIMP CPage::Write(WORD wAddr, SAFEARRAY *psaValue)
{
	LONG LBound, UBound;
	SafeArrayGetLBound(psaValue, 1, &LBound);
	SafeArrayGetUBound(psaValue, 1, &UBound);

	LPBYTE lpData = NULL;
	SafeArrayAccessData(psaValue, (LPVOID *) &lpData);

	for (int i = 0; i < UBound - LBound + 1; i++)
	{
		WriteByte(wAddr + i, lpData[i]);
	}
	SafeArrayUnaccessData(psaValue);
	return S_OK;
}
