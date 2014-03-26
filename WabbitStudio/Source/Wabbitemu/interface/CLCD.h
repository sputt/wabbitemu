#pragma once

#include "lcd.h"
#include "colorlcd.h"

class CLCD :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDispatchImpl<ILCD>
{
public:
	BEGIN_COM_MAP(CLCD)
		COM_INTERFACE_ENTRY(ILCD)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	void Initialize(LCDBase_t *lcd)
	{
		m_lcd = lcd;
	}

	// ILCD methods
	STDMETHOD(get_Display)(LPSAFEARRAY *ppsa);
	STDMETHOD(get_Width)(LPWORD lpWidth);
	STDMETHOD(get_Height)(LPWORD lpHeight);

private:
	LCDBase_t *m_lcd;
};
