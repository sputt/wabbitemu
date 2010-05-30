#pragma once

#include "MapObjects.h"
#include "AnimatedTiles.h"

#define FPS 50
#define TPF (1000 / FPS)

__declspec(dllimport) int rom_load(int slot, char * FileName);
__declspec(dllimport) int calc_run_all(void);
__declspec(dllimport) ZOBJECT *GetZeldaObjectArray();
__declspec(dllimport) ZANIMATE *GetZeldaAnimateArray();
__declspec(dllimport) void PressKey(WORD);
__declspec(dllimport) void ReleaseKey(WORD);
__declspec(dllimport) unsigned char *GetLCDImage();
__declspec(dllimport) void EnableCalc(BOOL);
__declspec(dllimport) void SendFile(char *FileName, int ram);
__declspec(dllimport) unsigned char *GetZeldaMapData();
__declspec(dllimport) void *GetZeldaDrawQueue();
__declspec(dllimport) int GetZeldaDrawQueueCount();
__declspec(dllimport) void HookZelda(void (*)(void));
__declspec(dllimport) int ReadVariable(char *name, void *data, int size);
__declspec(dllimport) int FindAnimationInfo(int type, unsigned short addr, int *frame, unsigned short *base_addr);
void CreateCalcPreview(void);