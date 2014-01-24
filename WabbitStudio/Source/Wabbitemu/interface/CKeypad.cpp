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
	{KEY_ON, VK_F12},
	{KEY_MATH, 'A'},
	{KEY_APPS, 'B'},
	{KEY_PRGM, 'C'},
	{KEY_VARS, VK_INSERT},
	{KEY_CLEAR, VK_RSHIFT},
	{KEY_MODE, VK_ESCAPE},
	{KEY_XTON, VK_ADD},
	{KEY_STAT, VK_END},

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

STDMETHODIMP CKeypad::PressKey(KEYS Key)
{
	int wVirtCode = LookupVirtCode(Key);
	if (wVirtCode == -1)
	{
		return E_NOTIMPL;
	}
	keypad_key_press(m_cpu, wVirtCode, NULL);
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

STDMETHODIMP CKeypad::PressVirtKey(int Key)
{
	keypad_key_press(m_cpu, Key, NULL);
	return S_OK;
}

STDMETHODIMP CKeypad::ReleaseVirtKey(int Key)
{
	keypad_key_release(m_cpu, Key);
	return S_OK;
}

STDMETHODIMP CKeypad::IsKeyPressed(KEYS Key, VARIANT_BOOL *lpfIsPressed)
{
	*lpfIsPressed = VARIANT_FALSE;
	return E_NOTIMPL;
}