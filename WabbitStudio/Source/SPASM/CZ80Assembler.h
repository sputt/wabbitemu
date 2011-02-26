#pragma once
#include "resource.h"

#include "SPASM_h.h"

using namespace ATL;

class CZ80Assembler :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZ80Assembler, &CLSID_CZ80Assembler>,
	public IDispatchImpl<IZ80Assembler, &IID_IZ80Assembler, &LIBID_SPASM>
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_RGSSPASM)

	CZ80Assembler();
	~CZ80Assembler();

	BEGIN_COM_MAP(CZ80Assembler)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IZ80Assembler)
	END_COM_MAP()

	STDMETHOD(get_Output)(IStream **ppStream);
	STDMETHOD(put_Input)(BSTR bstrInput);
	STDMETHOD(Assemble)(void);

private:
	BSTR m_bstrInput;
};

OBJECT_ENTRY_AUTO(__uuidof(CZ80Assembler), CZ80Assembler)
