#include "stdafx.h"

#include "CLabelServer.h"

#include "calc.h"
#include "label.h"

STDMETHODIMP CLabelServer::get_CaseSensitive(VARIANT_BOOL *pfIsCaseSensitive)
{
	*pfIsCaseSensitive = m_fIsCaseSensitive;
	return S_OK;
}

STDMETHODIMP CLabelServer::put_CaseSensitive(VARIANT_BOOL fIsCaseSensitive)
{
	m_fIsCaseSensitive = fIsCaseSensitive;
	return S_OK;
}

STDMETHODIMP CLabelServer::Find(VARIANT varCriteria, ILabel **ppLabel)
{
	if (V_VT(&varCriteria) == VT_BSTR)
	{
#ifdef _UNICODE
		label_struct *lab = lookup_label(V_BSTR(&varCriteria));
#else
		TCHAR szLabelName[256];
		WideCharToMultiByte(CP_ACP, 0, V_BSTR(&varCriteria), -1, szLabelName, sizeof(szLabelName), NULL, NULL);
		//HACK: fix somehow
		label_struct *lab = lookup_label(&calcs[0], szLabelName);
#endif
		if (lab == NULL)
		{
			*ppLabel = NULL;
			return S_FALSE;
		}

		CComObject<CLabel> *pLabel = new CComObject<CLabel>();
		pLabel->Initialize(V_BSTR(&varCriteria), m_pWabbitemu, !lab->IsRAM, lab->page, lab->addr);

		return pLabel->QueryInterface(IID_ILabel, (LPVOID *) ppLabel);
	}
	else
	{
		VARIANT varNumber;
		HRESULT hr = VariantChangeType(&varNumber, &varCriteria, 0, VT_UI2);

		if (SUCCEEDED(hr))
		{
//			ILabel *pLabel = m_mwLabels[V_UI2(&varNumber)];
//			return pLabel->QueryInterface(IID_ILabel, (LPVOID *) ppLabel);
			return hr;
		}
		else
		{
			return hr;
		}
	}
}

STDMETHODIMP CLabelServer::Load(BSTR bstrFileName)
{
#ifdef _UNICODE
	labels_app_load(0, bstrFileName);
#else
	TCHAR szFileName[MAX_PATH];
	WideCharToMultiByte(CP_ACP, 0, bstrFileName, -1, szFileName, sizeof(szFileName), NULL, NULL);
	labels_app_load(0, szFileName);
#endif
	return S_OK;
}


STDMETHODIMP CLabel::get_Calc(IWabbitemu **ppCalc)
{
	return m_CalcAddress.get_Calc(ppCalc);
}

STDMETHODIMP CLabel::get_Page(IPage **ppPage)
{
	return m_CalcAddress.get_Page(ppPage);
}

STDMETHODIMP CLabel::get_Address(LPWORD lpwAddress)
{
	return m_CalcAddress.get_Address(lpwAddress);
}

STDMETHODIMP CLabel::Read(VARIANT varByteCount, LPVARIANT lpvarResult)
{
	return m_CalcAddress.Read(varByteCount, lpvarResult);
}

STDMETHODIMP CLabel::Write(VARIANT varValue)
{
	return m_CalcAddress.Write(varValue);
}

STDMETHODIMP CLabel::get_Name(BSTR *lpbstrName)
{
	*lpbstrName = m_bstrName;
	return S_OK;
}