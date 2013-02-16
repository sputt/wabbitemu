#include "stdafx.h"

#include "CCalcAddress.h"

STDMETHODIMP CCalcAddress::get_Calc(IWabbitemu **ppCalc)
{
	return m_pWabbitemu->QueryInterface(IID_IWabbitemu, (LPVOID *) ppCalc);
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
	return m_pPage->Read(m_wAddress, varByteCount, lpvarResult);
}

STDMETHODIMP CCalcAddress::Write(VARIANT varValue)
{
	return m_pPage->Write(m_wAddress, varValue);
}


void CCalcAddress::Initialize(IWabbitemu *pCalc, BOOL IsFlash, int iPage, WORD wAddress)
{
	pCalc->QueryInterface(IID_IWabbitemu, (LPVOID *) &m_pWabbitemu);

	if (IsFlash == FALSE)
	{
		m_pWabbitemu->RAM(iPage, &m_pPage);
	}
	else
	{
		m_pWabbitemu->Flash(iPage, &m_pPage);
	}

	m_wAddress = wAddress;
	
}