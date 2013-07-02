#pragma once

struct tagCALC;

template <typename T>
struct _CopyVariantFromAdaptItf
{
  static HRESULT copy(VARIANT* p1, const CAdapt< CComPtr<T> >* p2)
  {
    HRESULT hr = p2->m_T->QueryInterface(
      IID_IDispatch,
      (void**)&p1->pdispVal
      );
    if (SUCCEEDED(hr))
    {
      p1->vt = VT_DISPATCH;
    }
    else
    {
      hr = p2->m_T->QueryInterface(
        IID_IUnknown,
        (void**)&p1->punkVal
        );
      if( SUCCEEDED(hr) )
      {
        p1->vt = VT_UNKNOWN;
      }
    }

    return hr;
  }

  static void init(VARIANT* p) { VariantInit(p); }
  static void destroy(VARIANT* p) { VariantClear(p); }
};

template <typename T>
struct _CopyItfFromAdaptItf
{
  static HRESULT copy(T** p1, const CAdapt< CComPtr<T> >* p2)
  {
    if( *p1 = p2->m_T )
    {
      return (*p1)->AddRef(), S_OK;
    }
    return E_POINTER;
  }

  static void init(T** p) {}
  static void destroy(T** p) { if( *p ) (*p)->Release(); }
};

template< typename T >
struct ComCollEnum : public CComEnumOnSTL<
                                IEnumVARIANT,
                                &IID_IEnumVARIANT,
                                VARIANT,
                                _CopyVariantFromAdaptItf<T>,
                                std::vector< CAdapt< CComPtr<T> > >
                                >
{
};

template < typename ItemTypeT, typename CollectionTypeT >
struct ComCollImpl : public ICollectionOnSTLImpl<
                              IDispatchImpl<CollectionTypeT, &__uuidof(CollectionTypeT)>,
                              std::vector< CAdapt< CComPtr<ItemTypeT> > >,
                              ItemTypeT*,
                              _CopyItfFromAdaptItf< ItemTypeT >,
                              ComCollEnum< ItemTypeT >
                              >
{
};


class ATL_NO_VTABLE CBreakpointCollection :
 public CComObjectRootEx<CComObjectThreadModel>,
 public IDispatchImpl<ComCollImpl<IBreakpoint, IBreakpointCollection>, &IID_IBreakpointCollection, 
 &LIBID_WabbitemuLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	BEGIN_COM_MAP(CBreakpointCollection)
		COM_INTERFACE_ENTRY(IBreakpointCollection)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	STDMETHOD(Add)(ICalcAddress *pCalcAddress);

	HRESULT Initialize(tagCALC *calc)
	{
		m_lpCalc = calc;
		return S_OK;
	}

	HRESULT LookupBreakpoint(waddr_t waddrRef, IBreakpoint **ppBP)
	{
		for (auto it = m_coll.begin(); it != m_coll.end(); it++)
		{
			CComPtr<ICalcAddress> pAddress;
			it->m_T->get_Address(&pAddress);

			waddr_t waddr;
			pAddress->get_Address(&waddr.addr);

			CComPtr<IPage> pPage;
			pAddress->get_Page(&pPage);

			VARIANT_BOOL isFlash;
			pPage->get_IsFlash(&isFlash);
			waddr.is_ram = isFlash == VARIANT_TRUE ? FALSE : TRUE;
			int nPage;
			pPage->get_Index(&nPage);
			waddr.page = nPage;

			if (memcmp(&waddrRef, &waddr, sizeof(waddr)) == 0)
			{
				return it->m_T->QueryInterface(ppBP);
			}
		}
		return S_FALSE;
	}

private:
	tagCALC *m_lpCalc;
};
