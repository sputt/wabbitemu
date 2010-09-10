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

STDMETHODIMP CPage::Read(WORD Address, VARIANT varByteCount, LPVARIANT lpvarResult)
{
	int nBytes = 1;
	if ((V_VT(&varByteCount) != VT_EMPTY) || (V_VT(&varByteCount) != VT_ERROR))
	{
		nBytes = V_I4(&varByteCount);
	}

	VARIANT varResult;
	VariantInit(&varResult);
	
	if (nBytes == 1)
	{
		V_VT(&varResult) = VT_UI1;
		V_UI1(&varResult) = m_lpData[Address % PAGE_SIZE];
	}
	else
	{
		V_VT(&varResult) = VT_ARRAY | VT_UI1;

		SAFEARRAYBOUND sab = {0};
		sab.cElements = nBytes;
		sab.lLbound = 0;
		LPSAFEARRAY psa = SafeArrayCreate(VT_UI1, 1, &sab);

		LPBYTE lpData = NULL;
		SafeArrayAccessData(psa, (LPVOID *) &lpData);
		memcpy(lpData, &m_lpData[Address % PAGE_SIZE], nBytes);
		SafeArrayUnaccessData(psa);

		V_ARRAY(&varResult) = psa;
	}
	*lpvarResult = varResult;
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
