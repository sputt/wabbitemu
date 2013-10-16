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

STDMETHODIMP CCalcAddress::ReadByte(LPBYTE lpbResult)
{
	return m_pPage->ReadByte(m_wAddress, lpbResult);
}

STDMETHODIMP CCalcAddress::ReadWord(LPWORD lpwResult)
{
	return m_pPage->ReadWord(m_wAddress, lpwResult);
}

STDMETHODIMP CCalcAddress::Read(WORD wCount, LPSAFEARRAY *ppsaResult)
{
	return m_pPage->Read(m_wAddress, wCount, ppsaResult);
}

STDMETHODIMP CCalcAddress::WriteByte(BYTE bValue)
{
	return m_pPage->WriteByte(m_wAddress, bValue);
}

STDMETHODIMP CCalcAddress::WriteWord(WORD wValue)
{
	return m_pPage->WriteWord(m_wAddress, wValue);
}

STDMETHODIMP CCalcAddress::Write(SAFEARRAY *psaValue)
{
	return m_pPage->Write(m_wAddress, psaValue);
}
