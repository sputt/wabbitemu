#pragma once

#include "CPageCollection.h"
#include "core.h"

class CBank : public CPageCollection
{
public:
	// Override
	// Delegate to the appropriate page collection
	STDMETHOD(get_Item)(LONG lIndex, IPage **ppPage)
	{
		if (lIndex < 0 || lIndex >= 4)
		{
			return E_INVALIDARG;
		}

		CComPtr<IPageCollection> pPageColl;
		bank_state_t *bs = &m_mem->banks[lIndex];
		if (bs->ram)
		{
			m_pMemC->get_RAM(&pPageColl);
		}
		else
		{
			m_pMemC->get_Flash(&pPageColl);
		}

		return pPageColl->get_Item(bs->page, ppPage);
	}

	STDMETHOD(put_Item)(LONG lIndex, IPage *pPage)
	{
		int index;
		HRESULT hr = pPage->get_Index(&index);
		ATLENSURE_RETURN_HR(SUCCEEDED(hr), hr);

		VARIANT_BOOL isFlash;
		hr = pPage->get_IsFlash(&isFlash);
		ATLENSURE_RETURN_HR(SUCCEEDED(hr), hr);

		change_page(m_mem, lIndex - 1, (u_char)index, (isFlash == VARIANT_FALSE) ? TRUE : FALSE);
		return S_OK;
	}

	HRESULT Initialize(memc *mem, IMemoryContext *pMemC)
	{
		m_mem = mem;
		// No add ref
		m_pMemC = pMemC;
		return S_OK;
	}

private:
	memc *m_mem;
	IMemoryContext *m_pMemC;
};
