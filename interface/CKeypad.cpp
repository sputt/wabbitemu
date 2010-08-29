#include "stdafx.h"

#include "CKeypad.h"

static const struct {KEYS Key; WORD wVirtCode;} g_KeyMaps[] =
{
	{KEY_UP, VK_UP},
	{KEY_LEFT, VK_LEFT},
	{KEY_RIGHT, VK_RIGHT},
	{KEY_DOWN, VK_DOWN},
	{KEY_2ND, VK_SHIFT},
	{KEY_ALPHA, VK_LCONTROL},
	{KEY_DEL, VK_DELETE},
	{KEY_APPS, VK_PRIOR},
	{KEY_PRGM, VK_NEXT},
};

static int LookupVirtCode(KEYS Key)
{
	for (int i = 0; i < ARRAYSIZE(g_KeyMaps); i++)
	{
		if (g_KeyMaps[i].Key == Key)
		{
			return (int) g_KeyMaps[i].wVirtCode;
		}
	}
	return -1;
}

STDMETHODIMP CKeypad::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
	if (riid == IID_IUnknown)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else if (riid == IID_IKeypad)
	{
		this->AddRef();
		*ppvObject = this;
		return S_OK;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}


STDMETHODIMP CKeypad::PressKey(KEYS Key)
{
	int wVirtCode = LookupVirtCode(Key);
	if (wVirtCode == -1)
	{
		return E_NOTIMPL;
	}
	keypad_key_press(m_cpu, wVirtCode);
	return S_OK;
}

STDMETHODIMP CKeypad::ReleaseKey(KEYS Key)
{
	int wVirtCode = LookupVirtCode(Key);
	if (wVirtCode == -1)
	{
		return E_NOTIMPL;
	}
	keypad_key_release(m_cpu, wVirtCode);
	return S_OK;
}

STDMETHODIMP CKeypad::IsKeyPressed(KEYS Key, VARIANT_BOOL *lpfIsPressed)
{
	*lpfIsPressed = VARIANT_FALSE;
	return E_NOTIMPL;
}