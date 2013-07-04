#include "stdafx.h"

#include "CPageCollection.h"

HRESULT CPageCollection::Initialize(PAGEVECTOR &Pages)
{
	m_coll = Pages;
	return S_OK;
}

