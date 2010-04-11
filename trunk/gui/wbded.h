/*----------------------------------------------------------------------
  Part of this code is borrowed and adapted from Glib library (GTK project)
  ----------------------------------------------------------------------
  this header may be redundant for some of the catch22.net examples, this because
  it's intended as part of a more general framework targeted to simplify
  the task of writing win32 OLE programs in pure C code.
  This is work in progress.
  ----------------------------------------------------------------------*/

#ifndef WBDED_INC
#define WBDED_INC

#define COBJMACROS

#include <ole2.h>
#include <shlobj.h>
#include <OleIdl.h>

EXTERN_C const IID IID_IDropSourceNotify;
#define INTERFACE IDropSourceNotify
DECLARE_INTERFACE_(IDropSourceNotify,IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;
	STDMETHOD(DragEnterTarget)(THIS_ HWND) PURE;
	STDMETHOD(DragLeaveTarget)(THIS) PURE;
};
#undef INTERFACE

#ifdef COBJMACROS
#define IDataObject_QueryInterface(T,a,b) (T)->lpVtbl->QueryInterface(T,a,b)
#define IDataObject_AddRef(T) (T)->lpVtbl->AddRef(T)
#define IDataObject_Release(T) (T)->lpVtbl->Release(T)
#define IDataObject_QueryContinueDrag(T,a,b) (T)->lpVtbl->QueryContinueDrag(T,a,b)
#define IDataObject_GiveFeedback(T,a) (T)->lpVtbl->GiveFeedback(T,a)
#define IDataObject_SetData(T,a,b,c) (T)->lpVtbl->SetData(T,a,b,c)
#endif


#define g_new0(struct_type, n_structs)		\
    ((struct_type *) g_malloc0 (((gsize) sizeof (struct_type)) * ((gsize) (n_structs))))

#define g_new(struct_type, n_structs)		\
    ((struct_type *) g_malloc (((gsize) sizeof (struct_type)) * ((gsize) (n_structs))))

// glib's configure
typedef unsigned int gsize;
// from gtypes.h
typedef unsigned int guint;
typedef void* gpointer;
typedef unsigned long gulong;

gpointer g_malloc0(gulong n_bytes);
gpointer g_malloc(gulong n_bytes);

typedef struct _GMemVTable GMemVTable;

void g_free(gpointer mem);

typedef DWORD (*BRKCALLBACK)(WORD);

typedef struct {
	WORD AF, BC, HL, DE;
	WORD AFP, BCP, HLP, DEP;

	WORD IX, IY, PC, SP;
	BYTE I, R, Bus, IFF1, IFF2;

	BYTE IMode;
	DOUBLE SecondsElapsed;

	LONG Frequency;
	BOOL Halt;
} __attribute__((packed)) Z80State_t;

typedef struct {
	char name[12];
	DWORD page, page_count;
} __attribute__((packed)) APPENTRY;

typedef struct {
	DWORD TypeID;
	char name[12];
	DWORD page;
	DWORD address;
} __attribute__((packed)) SYMENTRY;

typedef struct MEMSTATE_Tag {
	BOOL is_ram;
	DWORD page;
} __attribute__((packed)) MEMSTATE;

// data object:

typedef struct {
	FORMATETC fe;
	STGMEDIUM stgm;
} DATAENTRY, *LPDATAENTRY;

typedef struct {
	IDataObject ido;
	int ref_count;
	LPDATAENTRY m_rgde;
	LONG m_cde;
	LONG m_lRefCount;
} WB_IDataObject;

typedef struct {
	IEnumFORMATETC ief;
	LONG m_lRefCount;		// Reference count for this COM interface
	ULONG m_nIndex;			// current enumerator index
	LONG m_cde;				// number of FORMATETC members
	LPDATAENTRY m_rgde;
} WB_IEnumFORMATETC;

typedef struct {
	IDropSourceNotify idsn;
	LONG m_lRefCount;
	IDataObject *pDataObject;
} WB_IDropSourceNotify;

typedef struct {
	IDropSource ids;
	LONG m_lRefCount;
	IDataObject *pDataObject;
	IDropSourceNotify *pDropSourceNotify;
}	WB_IDropSource;

typedef struct {
	IClassFactory icf;		// class
	LONG m_lRefCount;		// reference count
} WB_IClassFactory;

typedef struct {
	IUnknown unk;
	LONG m_lRefCount;
	HWND hwndReply;
} WB_IWabbitemu;

typedef struct {
	IDropTarget idt;
	LONG	m_lRefCount;
	HWND	m_hWnd;
	int	m_slot;
	BOOL  m_fAllowDrop;
	IDataObject *m_pDataObject;
	IDropTargetHelper *m_pdth;
} WB_IDropTarget;

#define DECLARE_VTBL(i) \
	typedef CONST_VTABLE struct i##Vtbl i##Vtbl;\
	CONST_VTABLE struct i##Vtbl

#define INHERITS_IUNKNOWN\
	STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;\
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;\
	STDMETHOD_(ULONG,Release)(THIS) PURE

#define INTERFACE WB_IDataObject
DECLARE_VTBL(WB_IDataObject)
{
	INHERITS_IUNKNOWN;
	STDMETHOD(GetData)(THIS_ FORMATETC*,STGMEDIUM*) PURE;
	STDMETHOD(GetDataHere)(THIS_ FORMATETC*,STGMEDIUM*) PURE;
	STDMETHOD(QueryGetData)(THIS_ FORMATETC*) PURE;
	STDMETHOD(GetCanonicalFormatEtc)(THIS_ FORMATETC*,FORMATETC*) PURE;
	STDMETHOD(SetData)(THIS_ FORMATETC*,STGMEDIUM*,BOOL) PURE;
	STDMETHOD(EnumFormatEtc)(THIS_ DWORD,IEnumFORMATETC**) PURE;
	STDMETHOD(DAdvise)(THIS_ FORMATETC*,DWORD,IAdviseSink*,PDWORD) PURE;
	STDMETHOD(DUnadvise)(THIS_ DWORD) PURE;
	STDMETHOD(EnumDAdvise)(THIS_ IEnumSTATDATA**) PURE;
};
#undef INTERFACE

#define INTERFACE WB_IClassFactory
DECLARE_VTBL(WB_IClassFactory)
{
	INHERITS_IUNKNOWN;
	STDMETHOD(CreateInstance)(THIS_ IUnknown *,REFIID, LPVOID *) PURE;
	STDMETHOD(LockServer)(THIS_ BOOL fLock) PURE;
};
#undef INTERFACE

#define INTERFACE WB_IWabbitemu
DECLARE_VTBL(WB_IWabbitemu)
{
	INHERITS_IUNKNOWN;
	STDMETHOD(Run)(THIS_ ULONG) PURE;
	STDMETHOD(GetState)(THIS_ ULONG, Z80State_t *) PURE;
	STDMETHOD(SetState)(THIS_ ULONG, Z80State_t *) PURE;
	STDMETHOD(KeyPress)(THIS_ ULONG, DWORD) PURE;
	STDMETHOD(KeyRelease)(THIS_ ULONG, DWORD) PURE;
	STDMETHOD(ReadMem)(THIS_ ULONG, WORD, LPBYTE) PURE;
	STDMETHOD(WriteMem)(THIS_ ULONG, WORD, BYTE) PURE;
	STDMETHOD(DrawScreen)(THIS_ ULONG, BYTE*) PURE;
	STDMETHOD(SetBreakpoint)(THIS_ ULONG, HWND, BOOL, BYTE, WORD) PURE;
	STDMETHOD(ClearBreakpoint)(THIS_ ULONG, BOOL, BYTE, WORD) PURE;
	STDMETHOD(GetAppList)(THIS_ ULONG, APPENTRY *, DWORD *) PURE;
	STDMETHOD(GetSymList)(THIS_ ULONG, SYMENTRY *, DWORD *) PURE;
	STDMETHOD(GetMemState)(THIS_ ULONG, MEMSTATE *) PURE;
	STDMETHOD(SetMemState)(THIS_ ULONG, MEMSTATE *) PURE;
	STDMETHOD(Step)(THIS_ ULONG) PURE;
	STDMETHOD(StepOver)(THIS_ ULONG) PURE;
	STDMETHOD(StepOut)(THIS_ ULONG) PURE;	
};
#undef INTERFACE

#define INTERFACE WB_IEnumFORMATETC
DECLARE_VTBL(WB_IEnumFORMATETC)
{
	INHERITS_IUNKNOWN;
	STDMETHOD(Next)(THIS_ ULONG,FORMATETC*,ULONG*) PURE;
	STDMETHOD(Skip)(THIS_ ULONG) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(Clone)(THIS_ INTERFACE**) PURE;
};
#undef INTERFACE

#define INTERFACE WB_IDropSource
DECLARE_VTBL(WB_IDropSource)
{
	INHERITS_IUNKNOWN;
	STDMETHOD(QueryContinueDrag)(THIS_ BOOL,DWORD) PURE;
	STDMETHOD(GiveFeedback)(THIS_ DWORD) PURE;
};
#undef INTERFACE

#define INTERFACE WB_IDropSourceNotify
DECLARE_VTBL(WB_IDropSourceNotify)
{
	INHERITS_IUNKNOWN;
	STDMETHOD(DragEnterTarget)(THIS_ HWND) PURE;
	STDMETHOD(DragLeaveTarget)(THIS) PURE;
};
#undef INTERFACE

#define INTERFACE WB_IDropTarget
DECLARE_VTBL(WB_IDropTarget)
{
	INHERITS_IUNKNOWN;
	STDMETHOD(DragEnter)(THIS_ WB_IDataObject*,DWORD,POINTL,PDWORD) PURE;
	STDMETHOD(DragOver)(THIS_ DWORD,POINTL,PDWORD) PURE;
	STDMETHOD(DragLeave)(THIS) PURE;
	STDMETHOD(Drop)(THIS_ LPDATAOBJECT,DWORD,POINTL,PDWORD) PURE;
};
#undef INTERFACE

typedef WB_IDataObject* LPWBDATAOBJECT;
typedef WB_IEnumFORMATETC* LPWBFORMATETC;

WB_IEnumFORMATETC *WB_IEnumFORMATETC_new (UINT, DATAENTRY *);
#endif


