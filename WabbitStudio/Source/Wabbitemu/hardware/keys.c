#include "stdafx.h"

#include "keys.h"

#ifdef WINVER
#elif MACVER
#else
#include <gdk/gdkkeysyms.h>
#endif

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

static keyprog_t keygrps[] = {
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
    {O, 20, 0}, // Power On/Off
    {FUNCTION6, 20, 0}, // Power On/Off
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

static keyprog_t defaultkeys[] = {
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
	#ifdef WINVER // temporary
	{ VK_DOWN , 0 , 0 },
	{ VK_LEFT , 0 , 1 },
	{ VK_RIGHT , 0 , 2 },
	{ VK_UP , 0 , 3 },
	#else
	{ GDK_Down , 0 , 0 },
	{ GDK_Left , 0 , 1 },
	{ GDK_Right , 0 , 2 },
	{ GDK_Up , 0 , 3 },
	#endif
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
	#ifdef WINVER
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
#if !_DEBUG
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
	{ VK_NUMPAD0, 20, 0}
#else
	{ VK_F12, 20, 0}
#endif
	#else
	{ GDK_Return , 1 , 0 },
	{ GDK_period , 3 , 0 },
	{ GDK_comma , 4 , 4 },
	{ GDK_plus , 1 , 1 },
	{ GDK_minus , 1 , 2 },
	{ GDK_multiply , 1 , 3 },
	{ GDK_division , 1 , 4 },
	{ GDK_bracketleft , 3 , 4 },
	{ GDK_bracketright , 2 , 4 },
	{ GDK_F1 , 6 , 4 },
	{ GDK_F2 , 6 , 3 },
	{ GDK_F3 , 6 , 2 },
	{ GDK_F4 , 6 , 1 },
	{ GDK_F5 , 6 , 0 },
	{ GDK_Escape , 6 , 6 },
	{ GDK_Shift_L , 6 , 5 },
	{ GDK_Control_L , 5 , 7 },
	{ GDK_Shift_R , 1 , 6 },
	{ GDK_hyphen , 2 , 0 },
	{ GDK_equal , 4 , 7 },
	{ GDK_Prior , 4 , 6 },
	{ GDK_Next , 3 , 6 },
	{ GDK_Insert , 2 , 6 },
	{ GDK_Delete , 6 , 7 },
	{ GDK_Home , 5 , 6 },
	{ GDK_End , 3 , 7 },
	/*
	{ VK_NUMPAD0 , 4 , 0 },
	{ VK_NUMPAD1 , 4 , 1 },
	{ VK_NUMPAD2 , 3 , 1 },
	{ VK_NUMPAD3 , 2 , 1 },
	{ VK_NUMPAD4 , 4 , 2 },
	{ VK_NUMPAD5 , 3 , 2 },
	{ VK_NUMPAD6 , 2 , 2 },
	{ VK_NUMPAD7 , 4 , 3 },
	{ VK_NUMPAD8 , 3 , 3 },
	{ VK_NUMPAD9 , 2 , 3 },
	*/
	{ GDK_decimalpoint , 3 , 0 },
	{ GDK_Super_L, 2, 0 },
	{ GDK_F12, 20, 0 }
	#endif
};
static keyprog_t keygrps[] = {
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
	#ifdef WINVER // temporary
	{ VK_DOWN , 0 , 0 },
	{ VK_LEFT , 0 , 1 },
	{ VK_RIGHT , 0 , 2 },
	{ VK_UP , 0 , 3 },
	#else
	{ GDK_Down , 0 , 0 },
	{ GDK_Left , 0 , 1 },
	{ GDK_Right , 0 , 2 },
	{ GDK_Up , 0 , 3 },
	#endif
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
	#ifdef WINVER
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
#if !_DEBUG
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
	{ VK_NUMPAD0, 5, 0}
#else
	{ VK_F12, 5, 0}
#endif
	#else
	{ GDK_Return , 1 , 0 },
	{ GDK_period , 3 , 0 },
	{ GDK_comma , 4 , 4 },
	{ GDK_plus , 1 , 1 },
	{ GDK_minus , 1 , 2 },
	{ GDK_multiply , 1 , 3 },
	{ GDK_division , 1 , 4 },
	{ GDK_bracketleft , 3 , 4 },
	{ GDK_bracketright , 2 , 4 },
	{ GDK_F1 , 6 , 4 },
	{ GDK_F2 , 6 , 3 },
	{ GDK_F3 , 6 , 2 },
	{ GDK_F4 , 6 , 1 },
	{ GDK_F5 , 6 , 0 },
	{ GDK_Escape , 6 , 6 },
	{ GDK_Shift_L , 6 , 5 },
	{ GDK_Control_L , 5 , 7 },
	{ GDK_Shift_R , 1 , 6 },
	{ GDK_hyphen , 2 , 0 },
	{ GDK_equal , 4 , 7 },
	{ GDK_Prior , 4 , 6 },
	{ GDK_Next , 3 , 6 },
	{ GDK_Insert , 2 , 6 },
	{ GDK_Delete , 6 , 7 },
	{ GDK_Home , 5 , 6 },
	{ GDK_End , 3 , 7 },
	/*
	{ VK_NUMPAD0 , 4 , 0 },
	{ VK_NUMPAD1 , 4 , 1 },
	{ VK_NUMPAD2 , 3 , 1 },
	{ VK_NUMPAD3 , 2 , 1 },
	{ VK_NUMPAD4 , 4 , 2 },
	{ VK_NUMPAD5 , 3 , 2 },
	{ VK_NUMPAD6 , 2 , 2 },
	{ VK_NUMPAD7 , 4 , 3 },
	{ VK_NUMPAD8 , 3 , 3 },
	{ VK_NUMPAD9 , 2 , 3 },
	*/
	{ GDK_decimalpoint , 3 , 0 },
	{ GDK_Super_L, 2, 0 },
	{ GDK_F12, 20, 0 }
	#endif
};
#endif
	
keypad_t *keypad_init(CPU_t *cpu) {
	keypad_t *keypad;
	int b,i;
	
	keypad = (keypad_t *) malloc(sizeof(keypad_t));
	if (!keypad) {
		printf("Couldn't allocate mem for keypad\n");
		return NULL;
	}

	//keygrps = customkeys;
	for(b=0;b<8;b++) {
		for(i=0;i<8;i++) {
			keypad->keys[b][i]=0;
		}
	}
	keypad->on_pressed = 0;
	keypad->group = 0;
	return keypad;
}

void keypad(CPU_t *cpu, device_t *dev) {
	keypad_t *keypad = (keypad_t *) dev->aux;

	if (cpu->input) {
		int i,group,keybit;
		unsigned char result=0;
		unsigned char keymap[8] = {0,0,0,0,0,0,0,0};
		unsigned char keymapbug[8] = {0,0,0,0,0,0,0,0};

		for (group = 0; group < 7; group++) {
			for (keybit = 0; keybit < 8; keybit++) {
				if (keypad->keys[group][keybit]) {
					keymap[group] |= (1<<keybit);
				}
			}
		}
		
		for (group = 0; group < 7; group++) {
			for (i = 0; i < 7; i++) {
				if (keymap[group] & keymap[i]) {
					keymapbug[group] |= keymap[group]|keymap[i];
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

keyprog_t *keypad_key_press(CPU_t *cpu, unsigned int vk) {
	int i;
	keypad_t * keypad = cpu->pio.keypad;

	if (keypad == NULL)
	{
		return NULL;
	}
	/*
#if !defined(WINVER) || !defined(MACVER)
	if (vk >= 'a' && vk <= 'z') vk += 'A' - 'a';
#endif
	 */
	for(i=0; i < NumElm(defaultkeys); i++) {
		if (keygrps[i].vk == vk) {
			if (keygrps[i].group == 5 && keygrps[i].bit == 0) {
				keypad->on_pressed |= KEY_KEYBOARDPRESS;
			}// else {
			keypad->keys[keygrps[i].group][keygrps[i].bit] |= KEY_KEYBOARDPRESS;
			//}
			return &keygrps[i];
		}
	}	
	return NULL;
}

keyprog_t *keypad_key_release(CPU_t *cpu, unsigned int vk) {
	int i;
	keypad_t * keypad = cpu->pio.keypad;
	
	if (keypad == NULL)
	{
		return NULL;
	}
	/*
#if !defined(WINVER) || !defined(MACVER)
	if (vk >= 'a' && vk <= 'z') vk += 'A' - 'a';
#endif
	 */
	
	for(i=0; i < NumElm(defaultkeys); i++) {
		if (keygrps[i].vk == vk) {
			if (keygrps[i].group == 5 && keygrps[i].bit == 0) {
				keypad->on_pressed &= (~KEY_KEYBOARDPRESS);
			} //else {			
			keypad->keys[keygrps[i].group][keygrps[i].bit] &= (~KEY_KEYBOARDPRESS);
			//}
			return &keygrps[i];
		}
	}
	return NULL;
}
