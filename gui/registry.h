#ifndef REGISTRY_H_
#define REGISTRY_H_

#include "calc.h"

HRESULT LoadRegistrySettings(const LPCALC lpCalc);
HRESULT LoadRegistryDefaults();
HRESULT SaveRegistrySettings(const LPCALC lpCalc);
INT_PTR QueryWabbitKey(LPCTSTR lpszName);
void SaveWabbitKey(TCHAR *, int, void *);

#endif /*REGISTRY_H_*/
