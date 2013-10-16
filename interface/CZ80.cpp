#include "stdafx.h"

#include "CZ80.h"

STDMETHODIMP CZ80::get_Halt(VARIANT_BOOL *pbHalt)
{
	*pbHalt = m_cpu->halt ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CZ80::put_Halt(VARIANT_BOOL bHalt)
{
	m_cpu->halt = (bHalt == VARIANT_TRUE) ? TRUE : FALSE;
	return S_OK;
}
