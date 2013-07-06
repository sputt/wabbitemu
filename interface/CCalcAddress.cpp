#include "stdafx.h"

#include "CCalcAddress.h"


STDMETHODIMP CCalcAddress::Initialize(IPage *pPage, WORD wAddress)
{
	m_pPage = pPage;
	m_wAddress = wAddress;
	return S_OK;
}

STDMETHODIMP CCalcAddress::get_Page(IPage **ppPage)
{
	return m_pPage->QueryInterface(IID_IPage, (LPVOID *) ppPage);
}

STDMETHODIMP CCalcAddress::get_Address(LPWORD lpwAddress)
{
	*lpwAddress = m_wAddress;
	return S_OK;
}

STDMETHODIMP CCalcAddress::Read(VARIANT varByteCount, LPVARIANT lpvarResult)
{
	//return m_pPage->Read(m_wAddress, varByteCount, lpvarResult);
	return E_NOTIMPL;
}

STDMETHODIMP CCalcAddress::Write(VARIANT varValue)
{
	//return m_pPage->Write(m_wAddress, varValue);
	return E_NOTIMPL;
}
