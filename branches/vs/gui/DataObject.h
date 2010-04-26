#include <windows.h>
#include <ole2.h>

typedef struct {
	FORMATETC fmtetc;
	STGMEDIUM stgmed;
} DATAENTRY, *LPDATAENTRY;

class CDataObject : public IDataObject
{
public:
	// IUnknown
	HRESULT __stdcall QueryInterface(REFIID iid, LPVOID *ppvObject);
	ULONG __stdcall AddRef(void);
	ULONG __stdcall Release(void);

	// IDataObject
	HRESULT __stdcall GetData(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium);
	HRESULT __stdcall GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium);
	HRESULT __stdcall QueryGetData(FORMATETC *pFormatEtc);
	HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC *pFormatEtc, FORMATETC *pFormatEtcOut);
	HRESULT __stdcall SetData(FORMATETC *pFormatEtc, STGMEDIUM *pStgMedium, BOOL fRelease);
	HRESULT __stdcall EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
	HRESULT __stdcall DAdvise(FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
	HRESULT __stdcall DUnadvise(DWORD dwConnection);
	HRESULT __stdcall EnumDAdvise(IEnumSTATDATA **ppEnumAdvise);

	// Constructor / destructor
	CDataObject(FORMATETC *pFormatEtc, STGMEDIUM *pMedium, DWORD dwCount);
	~CDataObject();

private:

	LONG m_lRefCount;
	DWORD m_dwNumFormats;
	DATAENTRY *m_pDataEntry;

	HRESULT LookupFormatEtc(FORMATETC *pFormatEtc, LPDATAENTRY *pDataEntry, BOOL fAdd);
	HRESULT AddRefStgMedium(STGMEDIUM *pStgMediumIn, STGMEDIUM *pStgMediumOut, BOOL fCopyIn);
};