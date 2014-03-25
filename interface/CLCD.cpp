#include "stdafx.h"

#include "CLCD.h"

STDMETHODIMP CLCD::get_Display(LPSAFEARRAY *ppsa)
{
	SAFEARRAYBOUND sab = { 0 };
	sab.cElements = m_lcd->width * m_lcd->height * m_lcd->bytes_per_pixel;
	sab.lLbound = 0;
	LPSAFEARRAY psa = SafeArrayCreate(VT_UI1, 1, &sab);

	LPBYTE lpData = NULL;
	SafeArrayAccessData(psa, (LPVOID *)&lpData);
	u_char *image = m_lcd->image(m_lcd);
	memcpy(lpData, image, sab.cElements);
	free(image);
	SafeArrayUnaccessData(psa);

	*ppsa = psa;
	return S_OK;
}

STDMETHODIMP CLCD::get_Width(LPWORD lpWidth)
{
	*lpWidth = (WORD)m_lcd->width;
	return S_OK;
}

STDMETHODIMP CLCD::get_Height(LPWORD lpHeight)
{
	*lpHeight = (WORD)m_lcd->height;
	return S_OK;
}