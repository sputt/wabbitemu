#pragma once
struct _CopyVariantFromAdaptBstr {
	static HRESULT copy(VARIANT* p1, const CAdapt<CComBSTR>* p2) {
		p1->vt = VT_BSTR;
		p1->bstrVal = p2->m_T.Copy();
		return (p1->bstrVal ? S_OK : E_OUTOFMEMORY);
	}

	static void init(VARIANT* p) { VariantInit(p); }
	static void destroy(VARIANT* p) { VariantClear(p); }
};

typedef CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT,
	_CopyVariantFromAdaptBstr,
	std::vector< CAdapt<CComBSTR> > >
	CComEnumVariantOnVectorOfAdaptBstr;

struct _CopyBstrFromAdaptBstr {
	static HRESULT copy(BSTR* p1, const CAdapt<CComBSTR>* p2) {
		*p1 = SysAllocString(p2->m_T);
		return (p1 ? S_OK : E_OUTOFMEMORY);
	}

	static void init(BSTR* p) { }
	static void destroy(BSTR* p) { SysFreeString(*p); }
};

typedef ICollectionOnSTLImpl<IDispatchImpl<IIncludeDirectoryCollection, &__uuidof(IIncludeDirectoryCollection), &LIBID_SPASM, 1, 2>,
	std::vector< CAdapt<CComBSTR> >,
	BSTR,
	_CopyBstrFromAdaptBstr,
	CComEnumVariantOnVectorOfAdaptBstr>
	IIncludeDirectoryCollectionImpl;


class ATL_NO_VTABLE CIncludeDirectoryCollection :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IIncludeDirectoryCollectionImpl
{
public:
	BEGIN_COM_MAP(CIncludeDirectoryCollection)
		COM_INTERFACE_ENTRY(IIncludeDirectoryCollection)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(Add)(BSTR bstrDirectory)
	{
		m_coll.push_back(CAdapt< CComBSTR >(bstrDirectory));
		return S_OK;
	}

	STDMETHOD(Remove)(BSTR bstrDirectory)
	{
		for (auto it = m_coll.begin(); it != m_coll.end(); it++)
		{
			if (wcscmp((*it).m_T, bstrDirectory) == 0)
			{
				m_coll.erase(it);
				return S_OK;
			}
		}
		return S_FALSE;
	}

	STDMETHOD(Clear)()
	{
		m_coll.clear();
		return S_OK;
	}
};
