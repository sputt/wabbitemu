#include "stdafx.h"

#include "Z80Label.h"

STDMETHODIMP CZ80Label::get_Name(BSTR *lpbstr)
{
	_bstr_t str(m_label->name);
	*lpbstr = SysAllocString(str);
	return S_OK;
}

STDMETHODIMP CZ80Label::get_Value(DWORD *lpdwValue)
{
	*lpdwValue = m_label->value;
	return S_OK;
}