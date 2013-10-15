#pragma once

#include "lcd.h"

class CLCD :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<ILCD>
{
public:
	BEGIN_COM_MAP(CLCD)
		COM_INTERFACE_ENTRY(ILCD)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	void Initialize(LCD_t *lcd)
	{
		m_lcd = lcd;
	}

	// ILCD methods
	STDMETHOD (get_Display)(LPSAFEARRAY *ppsa)
	{
		SAFEARRAYBOUND sab = {0};
		sab.cElements = 64 * 128;
		sab.lLbound = 0;
		LPSAFEARRAY psa = SafeArrayCreate(VT_UI1, 1, &sab);

		LPBYTE lpData = NULL;
		SafeArrayAccessData(psa, (LPVOID *) &lpData);
		memcpy(lpData, LCD_image(m_lcd), 128 * 64);
		SafeArrayUnaccessData(psa);

		*ppsa = psa;
		return S_OK;
	}

	STDMETHOD (get_Width)(LPWORD lpWidth) 
	{
		*lpWidth = m_lcd->width;
		return S_OK;
	}

	STDMETHOD (get_Height)(LPWORD lpHeight)
	{
		*lpHeight = LCD_HEIGHT;
		return S_OK;
	}

	STDMETHOD(Draw)(BYTE Display[8192])
	{
		memcpy(Display, LCD_image(m_lcd), 128 * 64);
		return S_OK;
	}

	STDMETHOD(GetByteArray)(BYTE Display[8192])
	{
		memcpy(Display, LCD_image(m_lcd), 128 * 64);
		return S_OK;
	}

private:
	LCD_t *m_lcd;
};
