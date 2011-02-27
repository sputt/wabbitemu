// Z80Assembler.h : Declaration of the CZ80Assembler

#pragma once
#include "resource.h"       // main symbols



#include "SPASM_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CZ80Assembler

class ATL_NO_VTABLE CZ80Assembler :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CZ80Assembler, &CLSID_Z80Assembler>,
	public IDispatchImpl<IZ80Assembler, &IID_IZ80Assembler, &LIBID_SPASM, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CZ80Assembler()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_Z80ASSEMBLER)


BEGIN_COM_MAP(CZ80Assembler)
	COM_INTERFACE_ENTRY(IZ80Assembler)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();
public:
	STDMETHOD(get_Output)(IStream **ppStream);
	STDMETHOD(put_Input)(BSTR bstrInput);
	STDMETHOD(Assemble)(void);

private:
	BSTR m_bstrInput;


};

OBJECT_ENTRY_AUTO(__uuidof(Z80Assembler), CZ80Assembler)
