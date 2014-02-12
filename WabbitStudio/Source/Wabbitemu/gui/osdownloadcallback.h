#pragma once

class OSDownloadCallback : public IBindStatusCallback  
{
public:
	OSDownloadCallback();
	~OSDownloadCallback();

    STDMETHOD(OnStartBinding)(DWORD, IBinding __RPC_FAR *) { return E_NOTIMPL; }

    STDMETHOD(GetPriority)(LONG __RPC_FAR *) { return E_NOTIMPL; }

    STDMETHOD(OnLowResource)(DWORD) { return E_NOTIMPL; }

    STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR wszStatusText);

    STDMETHOD(OnStopBinding)(HRESULT, LPCWSTR) { return E_NOTIMPL; }

    STDMETHOD(GetBindInfo)(DWORD *, BINDINFO *){ return E_NOTIMPL; }

    STDMETHOD(OnDataAvailable)(DWORD, DWORD, FORMATETC __RPC_FAR *, STGMEDIUM __RPC_FAR *)
        { return E_NOTIMPL; }

    STDMETHOD(OnObjectAvailable)(REFIID, IUnknown __RPC_FAR *) { return E_NOTIMPL; }

    STDMETHOD_(ULONG,AddRef)() { return 0; }

    STDMETHOD_(ULONG,Release)() { return 0; }

    STDMETHOD(QueryInterface)(REFIID, void __RPC_FAR *__RPC_FAR *) { return E_NOTIMPL; }
};


OSDownloadCallback::OSDownloadCallback()
{
}

OSDownloadCallback::~OSDownloadCallback()
{
}