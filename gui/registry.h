#ifndef REGISTRY_H_
#define REGISTRY_H_

#include "calc.h"

HRESULT LoadRegistrySettings(const LPCALC lpCalc);
HRESULT LoadRegistryDefaults();
HRESULT SaveRegistrySettings(const LPCALC lpCalc);
INT_PTR QueryWabbitKey(LPCTSTR lpszName);
void SaveWabbitKeyA(char *name, int type, void *value);
void SaveWabbitKeyW(WCHAR *name, int type, void *value);
#ifdef _UNICODE
#define SaveWabbitKey SaveWabbitKeyW
#else
#define SaveWabbitKey SaveWabbitKeyA
#endif

#endif /*REGISTRY_H_*/
