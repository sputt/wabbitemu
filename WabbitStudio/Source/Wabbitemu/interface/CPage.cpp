#include "stdafx.h"

#include "CPage.h"

STDMETHODIMP CPage::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (riid == IID_IUnknown)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else if (riid == IID_IPage)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP CPage::get_Index(int *lpIndex)
{
	*lpIndex = m_iPage;
	return S_OK;
}

STDMETHODIMP CPage::get_Type(PAGETYPE *lpType)
{
	*lpType = m_Type;
	return S_OK;
}

STDMETHODIMP CPage::Read(WORD Address, LPBYTE lpValue)
{
	*lpValue = m_lpData[Address % PAGE_SIZE];
	return S_OK;
}

STDMETHODIMP CPage::Write(WORD Address, BYTE Value)
{
	m_lpData[Address % PAGE_SIZE] = Value;
	return S_OK;
}



