#include "stdafx.h"
#include "keys.h"

#ifdef MACVER
enum {
	A=0,
	B=11,
	C=8,
	D=2,
	E=14,
	F=3,
	G=5,
	H=4,
	I=34,
	J=38,
	K=40,
	L=37,
	M=46,
	N=45,
	O=31,
	P=35,
	Q=12,
	R=15,
	S=1,
	T=17,
	U=32,
	V=9,
	W=13,
	X=7,
	Y=16,
	Z=6,
	SPACE=49,
	UP=126,
	DOWN=125,
	LEFT=123,
	RIGHT=124,
	RETURN=36,
	ENTER=76,
	PERIOD=47,
	DECIMAL=65,
	COMMA=43,
	ADD=69,
	SUBTRACT=78,
	MULTIPLY=67,
	DIVIDE=75,
	LBRACKET=33,
	RBRACKET=30,
	FUNCTION1=122,
	FUNCTION2=120,
	FUNCTION3=99,
	FUNCTION4=118,
	FUNCTION5=96,
	FUNCTION6=97,
	ESCAPE=53,
	TAB=48,
	TILDE=50,
	CLEAR=71,
	MINUS=27,
	EQUAL=24,
	NUMPAD_EQUAL=81,
	PAGE_UP=116,
	PAGE_DOWN=121,
	BACKWARD_SLASH=42,
	DELETE=51,
	FORWARD_DELETE=117,
	HOME=115,
	END=119,
	NUMPAD_ZERO=82,
	NUMPAD_ONE=83,
	NUMPAD_TWO=84,
	NUMPAD_THREE=85,
	NUMPAD_FOUR=86,
	NUMPAD_FIVE=87,
	NUMPAD_SIX=88,
	NUMPAD_SEVEN=89,
	NUMPAD_EIGHT=91,
	NUMPAD_NINE=92,
	SINGLE_QUOTE=39,
	BACKSLASH = 44,
	ONE = 18,
	TWO = 19,
	THREE = 20,
	FOUR = 21,
	FIVE = 23,
	SIX = 22,
	SEVEN = 26,
	EIGHT = 28,
	NINE = 25,
	ZERO = 29,
};
keyprog_t defaultkeys[256] = {
	{N, 6, 5}, // 2nd
	{TAB, 6, 5}, // 2nd
	{ESCAPE, 6, 6}, // Mode
	{DELETE, 6, 7}, // Delete
	{FORWARD_DELETE, 6, 7}, // Delete
	{A, 5, 7}, // Alpha
	{TILDE, 5, 7}, //Alpha
	{EQUAL, 4, 7}, // Default Var
	{NUMPAD_EQUAL, 4, 7}, // Default Var
	{S, 3, 7}, // Stat
	{M, 5, 6}, // Math
	{HOME, 5, 6}, // Math
	{H, 4, 6}, // Apps
	{END, 4, 6}, // Apps
	{J, 3, 6}, // Prgm
	{PAGE_UP, 3, 6}, // Prgm
	{K, 2, 6}, // Vars
	{PAGE_DOWN, 2, 6}, // Vars
	{C, 1, 6}, // Clear
	{CLEAR, 1, 6}, // Clear
	{V, 5, 5}, // Inverse
	{I, 4, 5}, // Sin
	{O, 3, 5}, // Cos
	{T, 2, 5}, // Tan
	{P, 1, 5}, // Power
	{Q, 5, 4}, // Square
	{COMMA, 4, 4}, // Comma
	{LBRACKET, 3, 4}, // (
	{RBRACKET, 2, 4}, // )
	{DIVIDE, 1, 4}, // Divide
	{BACKSLASH, 1, 4}, // Divide
	{G, 5, 3}, // Log
	{SEVEN, 4, 3}, // 7
	{NUMPAD_SEVEN, 4, 3}, // 7
	{EIGHT, 3, 3}, // 8
	{NUMPAD_EIGHT, 3, 3}, // 8
	{NINE, 2, 3}, // 9
	{NUMPAD_NINE, 2, 3}, // 9
	{U, 1, 3}, // Multiply
	{MULTIPLY, 1, 3}, // Multiply
	{L, 5, 2}, // Ln
	{FOUR, 4, 2}, // 4
	{NUMPAD_FOUR, 4, 2}, // 4
	{FIVE, 3, 2}, // 5
	{NUMPAD_FIVE, 3, 2}, // 5
	{SIX, 2, 2}, // 6
	{NUMPAD_SIX, 2, 2}, // 6
	{MINUS, 1, 2}, // Subtract
	{SUBTRACT, 1, 2}, // Subtract
	{X, 5, 1}, // Sto
	{ONE, 4, 1}, // 1
	{NUMPAD_ONE, 4, 1}, // 1
	{TWO, 3, 1}, // 2
	{NUMPAD_TWO, 3, 1}, // 2
	{THREE, 2, 1}, // 3
	{NUMPAD_THREE, 2, 1}, // 3
	{D, 1, 1}, // Add
	{ADD, 1, 1}, // Add
	{O, 5, 0}, // Power On/Off
	{FUNCTION6, 5, 0}, // Power On/Off
	{ZERO, 4, 0}, // 0
	{NUMPAD_ZERO, 4, 0}, // 0
	{PERIOD, 3, 0}, // Decimal Point
	{DECIMAL, 3, 0}, // Decimal Point
	{E, 2, 0}, // Negate
	{ENTER, 1, 0}, // Enter
	{RETURN, 1, 0}, // Enter
	{FUNCTION1, 6, 4}, // Y=
	{FUNCTION2, 6, 3}, // Window
	{FUNCTION3, 6, 2}, // Zoom
	{FUNCTION4, 6, 1}, // Trace
	{FUNCTION5, 6, 0}, // Graph
	{UP, 0, 3}, // Up Arrow
	{DOWN, 0, 0}, // Down Arrow
	{LEFT, 0, 1}, // Left Arrow
	{RIGHT, 0, 2} // Right Arrow
};

static keyprog_t keygrps[256] = {
	{N, 6, 5}, // 2nd
	{TAB, 6, 5}, // 2nd
	{ESCAPE, 6, 6}, // Mode
	{DELETE, 6, 7}, // Delete
	{FORWARD_DELETE, 6, 7}, // Delete
	{A, 5, 7}, // Alpha
	{TILDE, 5, 7}, //Alpha
	{EQUAL, 4, 7}, // Default Var
	{NUMPAD_EQUAL, 4, 7}, // Default Var
	{S, 3, 7}, // Stat
	{M, 5, 6}, // Math
	{HOME, 5, 6}, // Math
	{H, 4, 6}, // Apps
	{END, 4, 6}, // Apps
	{J, 3, 6}, // Prgm
	{PAGE_UP, 3, 6}, // Prgm
	{K, 2, 6}, // Vars
	{PAGE_DOWN, 2, 6}, // Vars
	{C, 1, 6}, // Clear
	{CLEAR, 1, 6}, // Clear
	{V, 5, 5}, // Inverse
	{I, 4, 5}, // Sin
	{O, 3, 5}, // Cos
	{T, 2, 5}, // Tan
	{P, 1, 5}, // Power
	{Q, 5, 4}, // Square
	{COMMA, 4, 4}, // Comma
	{LBRACKET, 3, 4}, // (
	{RBRACKET, 2, 4}, // )
	{DIVIDE, 1, 4}, // Divide
	{BACKSLASH, 1, 4}, // Divide
	{G, 5, 3}, // Log
	{SEVEN, 4, 3}, // 7
	{NUMPAD_SEVEN, 4, 3}, // 7
	{EIGHT, 3, 3}, // 8
	{NUMPAD_EIGHT, 3, 3}, // 8
	{NINE, 2, 3}, // 9
	{NUMPAD_NINE, 2, 3}, // 9
	{U, 1, 3}, // Multiply
	{MULTIPLY, 1, 3}, // Multiply
	{L, 5, 2}, // Ln
	{FOUR, 4, 2}, // 4
	{NUMPAD_FOUR, 4, 2}, // 4
	{FIVE, 3, 2}, // 5
	{NUMPAD_FIVE, 3, 2}, // 5
	{SIX, 2, 2}, // 6
	{NUMPAD_SIX, 2, 2}, // 6
	{MINUS, 1, 2}, // Subtract
	{SUBTRACT, 1, 2}, // Subtract
	{X, 5, 1}, // Sto
	{ONE, 4, 1}, // 1
	{NUMPAD_ONE, 4, 1}, // 1
	{TWO, 3, 1}, // 2
	{NUMPAD_TWO, 3, 1}, // 2
	{THREE, 2, 1}, // 3
	{NUMPAD_THREE, 2, 1}, // 3
	{D, 1, 1}, // Add
	{ADD, 1, 1}, // Add
	{O, KEYGROUP_ON, KEYBIT_ON}, // Power On/Off
	{FUNCTION6, 5, 0}, // Power On/Off
	{ZERO, 4, 0}, // 0
	{NUMPAD_ZERO, 4, 0}, // 0
	{PERIOD, 3, 0}, // Decimal Point
	{DECIMAL, 3, 0}, // Decimal Point
	{E, 2, 0}, // Negate
	{ENTER, 1, 0}, // Enter
	{RETURN, 1, 0}, // Enter
	{FUNCTION1, 6, 4}, // Y=
	{FUNCTION2, 6, 3}, // Window
	{FUNCTION3, 6, 2}, // Zoom
	{FUNCTION4, 6, 1}, // Trace
	{FUNCTION5, 6, 0}, // Graph
	{UP, 0, 3}, // Up Arrow
	{DOWN, 0, 0}, // Down Arrow
	{LEFT, 0, 1}, // Left Arrow
	{RIGHT, 0, 2} // Right Arrow
};
#else

keyprog_t defaultkeys[256] = {
#ifdef _WINDOWS
	{ 'A' , 5 , 6 },
	{ 'B' , 4 , 6 },
	{ 'C' , 3 , 6 },
	{ 'D' , 5 , 5 },
	{ 'E' , 4 , 5 },
	{ 'F' , 3 , 5 },
	{ 'G' , 2 , 5 },
	{ 'H' , 1 , 5 },
	{ 'I' , 5 , 4 },
	{ 'J' , 4 , 4 },
	{ 'K' , 3 , 4 },
	{ 'L' , 2 , 4 },
	{ 'M' , 1 , 4 },
	{ 'N' , 5 , 3 },
	{ 'O' , 4 , 3 },
	{ 'P' , 3 , 3 },
	{ 'Q' , 2 , 3 },
	{ 'R' , 1 , 3 },
	{ 'S' , 5 , 2 },
	{ 'T' , 4 , 2 },
	{ 'U' , 3 , 2 },
	{ 'V' , 2 , 2 },
	{ 'W' , 1 , 2 },
	{ 'X' , 5 , 1 },
	{ 'Y' , 4 , 1 },
	{ 'Z' , 3 , 1 },
	{ ' ' , 4 , 0 },
	{ VK_DOWN , 0 , 0 },
	{ VK_LEFT , 0 , 1 },
	{ VK_RIGHT , 0 , 2 },
	{ VK_UP , 0 , 3 },
	{ '0' , 4 , 0 },
	{ '1' , 4 , 1 },
	{ '2' , 3 , 1 },
	{ '3' , 2 , 1 },
	{ '4' , 4 , 2 },
	{ '5' , 3 , 2 },
	{ '6' , 2 , 2 },
	{ '7' , 4 , 3 },
	{ '8' , 3 , 3 },
	{ '9' , 2 , 3 },
	{ VK_RETURN , 1 , 0 },
	{ VKF_PERIOD , 3 , 0 },
	{ VKF_COMMA , 4 , 4 },
	{ VK_ADD , 1 , 1 },
	{ VK_SUBTRACT , 1 , 2 },
	{ VK_MULTIPLY , 1 , 3 },
	{ VK_DIVIDE , 1 , 4 },
	{ VKF_LBRACKET , 3 , 4 },
	{ VKF_RBRACKET , 2 , 4 },
	{ VK_F1 , 6 , 4 },
	{ VK_F2 , 6 , 3 },
	{ VK_F3 , 6 , 2 },
	{ VK_F4 , 6 , 1 },
	{ VK_F5 , 6 , 0 },
	{ VK_ESCAPE , 6 , 6 },
	{ VK_LSHIFT , 6 , 5 },			// l shift
	{ VK_CONTROL, 5 , 7 },			// l control
	{ VK_RSHIFT , 1 , 6 },
	{ VKF_MINUS , 2 , 0 },
	{ VKF_EQUAL , 4 , 7 },
	{ VK_PRIOR , 4 , 6 },
	{ VK_NEXT , 3 , 6 },
	{ VK_INSERT , 2 , 6 },
	{ VK_DELETE , 6 , 7 },
	{ VK_HOME , 5 , 6 },
	{ VK_END , 3 , 7 },
#ifndef _DEBUG
	{ VK_NUMPAD0 , 4 , 0 },
#endif
	{ VK_NUMPAD1 , 4 , 1 },
	{ VK_NUMPAD2 , 3 , 1 },
	{ VK_NUMPAD3 , 2 , 1 },
	{ VK_NUMPAD4 , 4 , 2 },
	{ VK_NUMPAD5 , 3 , 2 },
	{ VK_NUMPAD6 , 2 , 2 },
	{ VK_NUMPAD7 , 4 , 3 },
	{ VK_NUMPAD8 , 3 , 3 },
	{ VK_NUMPAD9 , 2 , 3 },
	{ VK_DECIMAL , 3 , 0 },
	{ VK_OEM_2, 2, 0 },
//so much better than harcoding the changes :P
#ifdef _DEBUG
	{ VK_NUMPAD0, KEYGROUP_ON, KEYBIT_ON},
#else
	{ VK_F12, KEYGROUP_ON, KEYBIT_ON},
#endif
#endif
	{ -1, -1, -1},
};
keyprog_t keysti86[256] = {
#ifdef _WINDOWS
	{ 'A' , 5 , 5 },
	{ 'B' , 4 , 5 },
	{ 'C' , 3 , 5 },
	{ 'D' , 2 , 5 },
	{ 'E' , 1 , 5 },
	{ 'F' , 5 , 4 },
	{ 'G' , 4 , 4 },
	{ 'H' , 3 , 4 },
	{ 'I' , 2 , 4 },
	{ 'J' , 1 , 4 },
	{ 'K' , 5 , 3 },
	{ 'L' , 4 , 3 },
	{ 'M' , 3 , 3 },
	{ 'N' , 2 , 3 },
	{ 'O' , 1 , 3 },
	{ 'P' , 5 , 2 },
	{ 'Q' , 4 , 2 },
	{ 'R' , 3 , 2 },
	{ 'S' , 2 , 2 },
	{ 'T' , 1 , 2 },
	{ 'U' , 4 , 1 },
	{ 'V' , 3 , 1 },
	{ 'W' , 2 , 1 },
	{ 'X' , 1 , 1 },
	{ 'Y' , 4 , 0 },
	{ 'Z' , 3 , 0 },
	{ ' ' , 2 , 0 },
	{ VK_DOWN , 0 , 0 },
	{ VK_LEFT , 0 , 1 },
	{ VK_RIGHT , 0 , 2 },
	{ VK_UP , 0 , 3 },
	{ '0' , 4 , 0 },
	{ '1' , 4 , 1 },
	{ '2' , 3 , 1 },
	{ '3' , 2 , 1 },
	{ '4' , 4 , 2 },
	{ '5' , 3 , 2 },
	{ '6' , 2 , 2 },
	{ '7' , 4 , 3 },
	{ '8' , 3 , 3 },
	{ '9' , 2 , 3 },
	{ VK_RETURN , 1 , 0 },
	{ VKF_PERIOD , 3 , 0 },
	{ VKF_COMMA , 4 , 4 },
	{ VK_ADD , 1 , 1 },
	{ VK_SUBTRACT , 1 , 2 },
	{ VK_MULTIPLY , 1 , 3 },
	{ VK_DIVIDE , 1 , 4 },
	{ VKF_LBRACKET , 3 , 4 },
	{ VKF_RBRACKET , 2 , 4 },
	{ VK_F1 , 6 , 4 },
	{ VK_F2 , 6 , 3 },
	{ VK_F3 , 6 , 2 },
	{ VK_F4 , 6 , 1 },
	{ VK_F5 , 6 , 0 },
	{ VK_ESCAPE , 6 , 6 },
	{ VK_LSHIFT , 6 , 5 },			// l shift
	{ VK_CONTROL, 5 , 7 },			// l control
	{ VK_RSHIFT , 1 , 6 },
	{ VKF_MINUS , 2 , 0 },
	{ VKF_EQUAL , 4 , 7 },
	{ VK_PRIOR , 4 , 6 },
	{ VK_NEXT , 3 , 6 },
	{ VK_INSERT , 2 , 6 },
	{ VK_DELETE , 3 , 7 },
	{ VK_HOME , 5 , 6 },
	{ VK_END , 6 , 7 },
#ifndef _DEBUG
	{ VK_NUMPAD0 , 4 , 0 },
#endif
	{ VK_NUMPAD1 , 4 , 1 },
	{ VK_NUMPAD2 , 3 , 1 },
	{ VK_NUMPAD3 , 2 , 1 },
	{ VK_NUMPAD4 , 4 , 2 },
	{ VK_NUMPAD5 , 3 , 2 },
	{ VK_NUMPAD6 , 2 , 2 },
	{ VK_NUMPAD7 , 4 , 3 },
	{ VK_NUMPAD8 , 3 , 3 },
	{ VK_NUMPAD9 , 2 , 3 },
	{ VK_DECIMAL , 3 , 0 },
	{ VK_OEM_2, 2, 0 },
//so much better than hardcoding the changes :P
#ifdef _DEBUG
	{ VK_NUMPAD0, KEYGROUP_ON, KEYBIT_ON},
#else
	{ VK_F12, KEYGROUP_ON, KEYBIT_ON},
#endif
#endif
	{ -1, -1, -1},
};
keyprog_t keygrps[256] = {
#ifdef _WINDOWS
	{ 'A' , 5 , 6 },
	{ 'B' , 4 , 6 },
	{ 'C' , 3 , 6 },
	{ 'D' , 5 , 5 },
	{ 'E' , 4 , 5 },
	{ 'F' , 3 , 5 },
	{ 'G' , 2 , 5 },
	{ 'H' , 1 , 5 },
	{ 'I' , 5 , 4 },
	{ 'J' , 4 , 4 },
	{ 'K' , 3 , 4 },
	{ 'L' , 2 , 4 },
	{ 'M' , 1 , 4 },
	{ 'N' , 5 , 3 },
	{ 'O' , 4 , 3 },
	{ 'P' , 3 , 3 },
	{ 'Q' , 2 , 3 },
	{ 'R' , 1 , 3 },
	{ 'S' , 5 , 2 },
	{ 'T' , 4 , 2 },
	{ 'U' , 3 , 2 },
	{ 'V' , 2 , 2 },
	{ 'W' , 1 , 2 },
	{ 'X' , 5 , 1 },
	{ 'Y' , 4 , 1 },
	{ 'Z' , 3 , 1 },
	{ ' ' , 4 , 0 },
	{ VK_DOWN , 0 , 0 },
	{ VK_LEFT , 0 , 1 },
	{ VK_RIGHT , 0 , 2 },
	{ VK_UP , 0 , 3 },
	{ '0' , 4 , 0 },
	{ '1' , 4 , 1 },
	{ '2' , 3 , 1 },
	{ '3' , 2 , 1 },
	{ '4' , 4 , 2 },
	{ '5' , 3 , 2 },
	{ '6' , 2 , 2 },
	{ '7' , 4 , 3 },
	{ '8' , 3 , 3 },
	{ '9' , 2 , 3 },
	{ VK_RETURN , 1 , 0 },
	{ VKF_PERIOD , 3 , 0 },
	{ VKF_COMMA , 4 , 4 },
	{ VK_ADD , 1 , 1 },
	{ VK_SUBTRACT , 1 , 2 },
	{ VK_MULTIPLY , 1 , 3 },
	{ VK_DIVIDE , 1 , 4 },
	{ VKF_LBRACKET , 3 , 4 },
	{ VKF_RBRACKET , 2 , 4 },
	{ VK_F1 , 6 , 4 },
	{ VK_F2 , 6 , 3 },
	{ VK_F3 , 6 , 2 },
	{ VK_F4 , 6 , 1 },
	{ VK_F5 , 6 , 0 },
	{ VK_ESCAPE , 6 , 6 },
	{ VK_LSHIFT , 6 , 5 },			// l shift
	{ VK_CONTROL, 5 , 7 },			// l control
	{ VK_RSHIFT , 1 , 6 },
	{ VKF_MINUS , 2 , 0 },
	{ VKF_EQUAL , 4 , 7 },
	{ VK_PRIOR , 4 , 6 },
	{ VK_NEXT , 3 , 6 },
	{ VK_INSERT , 2 , 6 },
	{ VK_DELETE , 6 , 7 },
	{ VK_HOME , 5 , 6 },
	{ VK_END , 3 , 7 },
#ifndef _DEBUG
	{ VK_NUMPAD0 , 4 , 0 },
#endif
	{ VK_NUMPAD1 , 4 , 1 },
	{ VK_NUMPAD2 , 3 , 1 },
	{ VK_NUMPAD3 , 2 , 1 },
	{ VK_NUMPAD4 , 4 , 2 },
	{ VK_NUMPAD5 , 3 , 2 },
	{ VK_NUMPAD6 , 2 , 2 },
	{ VK_NUMPAD7 , 4 , 3 },
	{ VK_NUMPAD8 , 3 , 3 },
	{ VK_NUMPAD9 , 2 , 3 },
	{ VK_DECIMAL , 3 , 0 },
	{ VK_OEM_2, 2, 0 },
//so much better than hardcoding the changes :P
#ifdef _DEBUG
	{ VK_NUMPAD0, KEYGROUP_ON, KEYBIT_ON},
#else
	{ VK_F12, KEYGROUP_ON, KEYBIT_ON},
#endif
#endif
	{ -1, -1, -1},
};
#endif
	
keypad_t *keypad_init(CPU_t *cpu) {
	keypad_t *keypad;
	int b,i;
	
	keypad = (keypad_t *) malloc(sizeof(keypad_t));
	if (!keypad) {
		return NULL;
	}

	memset(keypad->keys, 0, sizeof(keypad->keys));
	keypad->on_pressed = 0;
	keypad->group = 0;
	memset(keypad->last_pressed, 0, sizeof(keypad->last_pressed));
	keypad->on_last_pressed = 0;
	return keypad;
}

void keypad(CPU_t *cpu, device_t *dev) {
	keypad_t *keypad = (keypad_t *) dev->aux;

	if (cpu->input) {
		int i, group, keybit;
		unsigned char result=0;
		unsigned char keymap[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		unsigned char keymapbug[8] = {0, 0, 0, 0, 0, 0, 0, 0};

		for (group = 0; group < 7; group++) {
			for (keybit = 0; keybit < 8; keybit++) {
				if (keypad->keys[group][keybit]) {
					keymap[group] |= (1 << keybit);
				}
			}
		}
		
		for (group = 0; group < 7; group++) {
			for (i = 0; i < 7; i++) {
				if (keymap[group] & keymap[i]) {
					keymapbug[group] |= keymap[group] | keymap[i];
				}
			}
		}
		
		for (group = 0; group < 7; group++) {
			if (keypad->group & (1<<group)) {
				result |= keymapbug[group];
			}
		}
	

		cpu->bus = ~result;
		cpu->input = FALSE;
	} else if (cpu->output) {
		unsigned char group = ~cpu->bus;
		if (group != 0) keypad->group = group;
		else keypad->group = 0;
		cpu->output = FALSE;
	}	
}

void keypad_press(CPU_t *cpu, int group, int bit)
{
	if (group == KEYGROUP_ON && bit == KEYBIT_ON)
	{
		cpu->pio.keypad->on_pressed |= KEY_KEYBOARDPRESS;
	}
	else
	{
		cpu->pio.keypad->keys[group][bit] |= KEY_KEYBOARDPRESS;
	}
}

keyprog_t *keypad_key_press(CPU_t *cpu, unsigned int vk, BOOL *changed)
{
	int i;
	keypad_t *keypad = cpu->pio.keypad;

	if (keypad == NULL) {
		return NULL;
	}
	for(i = 0; i < NumElm(keygrps); i++)
	{
		if (keygrps[i].vk == vk)
		{
			int orig, group = keygrps[i].group, bit = keygrps[i].bit;
			if (group == KEYGROUP_ON && bit == KEYBIT_ON) {
				orig = keypad->on_pressed;
			} else {
				orig = keypad->keys[group][bit];
			}
			keypad_press(cpu, group, bit);
			if (changed) {
				if (group == KEYGROUP_ON && bit == KEYBIT_ON) {
					*changed = orig != keypad->on_pressed;
				} else {
					*changed = orig != keypad->keys[group][bit];
				}
			}
			return &keygrps[i];
		}
	}	
	return NULL;
}

void keypad_release(CPU_t *cpu, int group, int bit)
{
	if (group == KEYGROUP_ON && bit == KEYBIT_ON)
	{
		cpu->pio.keypad->on_pressed &= ~KEY_KEYBOARDPRESS;
	}
	else
	{
		cpu->pio.keypad->keys[group][bit] &= ~KEY_KEYBOARDPRESS;
	}
}

keyprog_t *keypad_key_release(CPU_t *cpu, unsigned int vk) {
	keypad_t *keypad = cpu->pio.keypad;
	
	if (keypad == NULL)
	{
		return NULL;
	}

	for (int i = 0; i < NumElm(keygrps); i++)
	{
		if (keygrps[i].vk == vk)
		{	
			keypad_release(cpu, keygrps[i].group, keygrps[i].bit);
			return &keygrps[i];
		}
	}
	return NULL;
}

#ifdef WINVER
void keypad_vk_release(HWND hwnd, int group, int bit) {
	for (int i = 0; i < ARRAYSIZE(defaultkeys); i++) {
		if (keygrps[i].group == group && keygrps[i].bit == bit) {	
			//TODO: fix lparam
			//this is sent as a message and not HandleKeyUp because
			//i can't get an LPCALC in here. It would be nice
			//to somehow separate the skin logic from that but
			//still update the skin somehow
			SendMessage(hwnd, WM_KEYUP, keygrps[i].vk, 0);
		}
	}
}
#endif
