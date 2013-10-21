#pragma once

class ATL_NO_VTABLE CTextStream :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ITextStream, &__uuidof(ITextStream), &__uuidof(__Scripting), 1, 0>
{
public:
	BEGIN_COM_MAP(CTextStream)
		COM_INTERFACE_ENTRY(ITextStream)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	HRESULT FinalConstruct();

	STDMETHOD(get_Line)(LONG *plLine)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_Column)(LONG *plColumn)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(get_AtEndOfLine)(VARIANT_BOOL *bEOL);
	STDMETHOD(get_AtEndOfStream)(VARIANT_BOOL *bEOS);

	STDMETHOD(Read)(LONG cch, LPBSTR lpbstrResult);
	STDMETHOD(ReadLine)(LPBSTR lpbstrLine);
	STDMETHOD(ReadAll)(LPBSTR lpbstrResult);

	STDMETHOD(Write)(BSTR bstrText);
	STDMETHOD(WriteLine)(BSTR bstrLine);
	STDMETHOD(WriteBlankLines)(LONG nLines);

	STDMETHOD(Skip)(LONG cch);
	STDMETHOD(SkipLine)();
	STDMETHOD(Close)();

private:
	HANDLE m_hRead;
};
