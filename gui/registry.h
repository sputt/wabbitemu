#ifndef REGISTRY_H_
#define REGISTRY_H_

#include "calc.h"

HRESULT LoadRegistrySettings(const LPCALC lpCalc);
HRESULT LoadRegistryDefaults();
HRESULT SaveRegistrySettings(const LPCALC lpCalc);
INT_PTR QueryWabbitKey(LPCTSTR lpszName);
void SaveWabbitKey(TCHAR *name, int type, void *value);
//void SaveWabbitKeyA(char *name, int type, void *value);
//void SaveWabbitKeyW(WCHAR *name, int type, void *value);
//#ifdef _UNICODE
//#define SaveWabbitKey SaveWabbitKeyW
//#else
//#define SaveWabbitKey SaveWabbitKeyA
//#endif


LONG_PTR QueryDebugKey(TCHAR *);
void SaveDebugKey(TCHAR *, int type, void *value);
BOOL CheckSetIsPortableMode();
//HRESULT ExportRegistrySettingsFile(LPCTSTR lpszFileName);
//HRESULT LoadRegistrySettingsFile(LPCTSTR lpszFileName);

#endif /*REGISTRY_H_*/
