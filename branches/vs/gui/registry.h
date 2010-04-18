#ifndef REGISTRY_H_
#define REGISTRY_H_

HRESULT LoadRegistrySettings(void);
HRESULT LoadRegistryDefaults(void);
HRESULT SaveRegistrySettings(void);
INT_PTR QueryWabbitKey(char *name);

#endif /*REGISTRY_H_*/
