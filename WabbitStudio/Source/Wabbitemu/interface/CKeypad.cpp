#include "stdafx.h"

#include "CKeypad.h"
#include "gui.h"

static const struct { CalcKey Key; int group; int bit; } g_KeyMaps[] =
{
	{ KEY_UP, 0, 3},
	{ KEY_LEFT, 0, 1 },
	{ KEY_RIGHT, 0, 2 },
	{ KEY_DOWN, 0, 0 },
	{ KEY_2ND, 6, 5 },
	{ KEY_MODE, 6, 6 },
	{ KEY_DEL, 6, 7 },
	{ KEY_ALPHA, 5, 7 },
	{ KEY_XTON, 4, 7 },
	{ KEY_STAT, 3, 7 },
	{ KEY_YEQU, 6, 4 },
	{ KEY_WINDOW, 6, 3 },
	{ KEY_ZOOM, 6, 2 },
	{ KEY_TRACE, 6, 1 },
	{ KEY_GRAPH, 6, 0 },
	{ KEY_MATH, 5, 6 },
	{ KEY_APPS, 4, 6 },
	{ KEY_PRGM, 3, 6 },
	{ KEY_VARS, 2, 6 },
	{ KEY_CLEAR, 1, 6 },
	{ KEY_INVERSE, 5, 5 },
	{ KEY_SIN, 4, 5 },
	{ KEY_COS, 3, 5 },
	{ KEY_TAN, 2, 5 },
	{ KEY_EXP, 1, 5 },
	{ KEY_SQUARE, 5, 4 },
	{ KEY_COMMA, 4, 4},
	{ KEY_LPAREN, 3, 4 },
	{ KEY_RPAREN, 2, 4 },
	{ KEY_DIV, 1, 4 },
	{ KEY_LOG, 5, 3 },
	{ KEY_7, 4, 3 },
	{ KEY_8, 3, 3 },
	{ KEY_9, 2, 3 },
	{ KEY_MULT, 1, 3 },
	{ KEY_LN, 5, 2 },
	{ KEY_4, 4, 2 },
	{ KEY_5, 3, 2 },
	{ KEY_6, 2, 2 },
	{ KEY_SUB, 1, 2 },
	{ KEY_STO, 5, 1 },
	{ KEY_1, 4, 1 },
	{ KEY_2, 3, 1 },
	{ KEY_3, 2, 1 },
	{ KEY_ADD, 1, 1 },
	{ KEY_ON, 5, 0 },
	{ KEY_0, 4, 0 },
	{ KEY_PERIOD, 3, 0 },
	{ KEY_NEGATE, 2, 0},
	{ KEY_ENTER, 1, 0 },
};

/* True if found, false otherwise */
static BOOL LookupKeyprog(CalcKey Key, keyprog_t *prog)
{
	for (int i = 0; i < ARRAYSIZE(g_KeyMaps); i++)
	{
		if (g_KeyMaps[i].Key == Key)
		{
			prog->group = g_KeyMaps[i].group;
			prog->bit = g_KeyMaps[i].bit;
			return TRUE;
		}
	}

	return FALSE;
}

STDMETHODIMP CKeypad::PressKey(CalcKey Key)
{
	keyprog_t prog;
	BOOL found = LookupKeyprog(Key, &prog);

	if (found == FALSE)
	{
		return E_INVALIDARG;
	}

	keypad_press(&m_lpCalc->cpu, prog.group, prog.bit);
	return S_OK;
}

STDMETHODIMP CKeypad::ReleaseKey(CalcKey Key)
{
	keyprog_t prog;
	BOOL found = LookupKeyprog(Key, &prog);

	if (found == FALSE) {
		return E_INVALIDARG;
	}

	keypad_release(&m_lpCalc->cpu, prog.group, prog.bit);
	return S_OK;
}

STDMETHODIMP CKeypad::PressReleaseKey(CalcKey Key)
{
	keyprog_t prog;
	BOOL found = LookupKeyprog(Key, &prog);

	if (found == FALSE) {
		return E_INVALIDARG;
	}

	press_key(m_lpCalc, prog.group, prog.bit);
	return S_OK;
}

STDMETHODIMP CKeypad::IsKeyPressed(CalcKey Key, VARIANT_BOOL *lpfIsPressed)
{
	keyprog_t prog;
	BOOL found = LookupKeyprog(Key, &prog);

	if (found == FALSE) {
		return E_INVALIDARG;
	}

	keypad_t *keypad = m_lpCalc->cpu.pio.keypad;
	*lpfIsPressed = keypad->keys[prog.group][prog.bit] ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}