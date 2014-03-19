#ifndef KEYS_H
#define KEYS_H
#include "core.h"

typedef struct keypad {
	unsigned char group;
	unsigned char keys[8][8], on_pressed;
	unsigned long long last_pressed[8][8], on_last_pressed;
} keypad_t;

typedef struct KEYPROG {
	UINT vk;
	int group;
	int bit;
} keyprog_t;

#define MAX_KEY_MAPPINGS 256
#define VK_EQUAL	VK_OEM_PLUS
#define VK_LBRACKET VK_OEM_4
#define VK_RBRACKET VK_OEM_6

keypad_t *keypad_init();
void keypad(CPU_t *, device_t *);

keyprog_t *keypad_key_press(CPU_t*, unsigned int vk, BOOL *changed);
keyprog_t *keypad_key_release(CPU_t*, unsigned int vk);
void keypad_press(CPU_t *cpu, int group, int bit);
void keypad_release(CPU_t *cpu, int group, int bit);
keyprog_t * keypad_keyprog_from_groupbit(CPU_t *cpu, int group, int bit);

#define KEY_VALUE_MASK		(0x0F)

#define KEY_KEYBOARDPRESS	0x01
#define KEY_MOUSEPRESS		0x02
#define KEY_LOCKPRESS		0x04
#define KEY_FALSEPRESS		0x08
#define KEY_STATEDOWN		0x10

#define KEYGROUP_ON			0x05
#define KEYBIT_ON			0x00

#endif /*#ifndef KEYS_H*/
