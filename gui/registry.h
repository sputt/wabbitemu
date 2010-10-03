#ifndef REGISTRY_H_
#define REGISTRY_H_

#include "calc.h"

HRESULT LoadRegistrySettings(void);
HRESULT LoadRegistryDefaults(void);
HRESULT SaveRegistrySettings(const LPCALC lpCalc);
INT_PTR QueryWabbitKey(LPCTSTR lpszName);
void SaveWabbitKey(char *, int, void *);

#endif /*REGISTRY_H_*/
