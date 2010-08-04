#pragma once

#include <windows.h>
#include <tchar.h>
#include "Wabbitemu_h.h"
#include "core.h"

class CPage : IPage
{
public:
	// IUnknown methods
	STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObject);
	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_lRefCount);
	};
	STDMETHODIMP_(ULONG) Release()
	{
		if (InterlockedDecrement(&m_lRefCount) == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return m_lRefCount;
		}
	};

	// IBank methods
	STDMETHODIMP get_Index(int *lpIndex);
	STDMETHODIMP get_Type(PAGETYPE *lpType);
	STDMETHODIMP Read(WORD Address, LPBYTE lpValue);
	STDMETHODIMP Write(WORD Address, BYTE Value);

	CPage(const memory_context_t *pMem, PAGETYPE Type, int iPage)
	{
		m_lRefCount = 1;
		
		m_Type = Type;
		m_iPage = iPage;
		if (Type == PAGETYPE::FLASH)
		{
			m_lpData = &pMem->flash[iPage * PAGE_SIZE];
		}
		else if (Type == PAGETYPE::RAM)
		{
			m_lpData = &pMem->ram[iPage * PAGE_SIZE];
		}
		else
		{
			m_lpData = NULL;
		}
	};

private:
	LONG m_lRefCount;

	PAGETYPE m_Type;
	int m_iPage;
	LPBYTE m_lpData;
};
