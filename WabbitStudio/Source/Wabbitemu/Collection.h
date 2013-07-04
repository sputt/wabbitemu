#pragma once

#include "VariantAdapt.h"

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
