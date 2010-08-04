

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Aug 03 22:21:47 2010
 */
/* Compiler settings for .\Wabbitemu.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Wabbitemu_h_h__
#define __Wabbitemu_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IZ80_FWD_DEFINED__
#define __IZ80_FWD_DEFINED__
typedef interface IZ80 IZ80;
#endif 	/* __IZ80_FWD_DEFINED__ */


#ifndef __IPage_FWD_DEFINED__
#define __IPage_FWD_DEFINED__
typedef interface IPage IPage;
#endif 	/* __IPage_FWD_DEFINED__ */


#ifndef __IWabbitemu_FWD_DEFINED__
#define __IWabbitemu_FWD_DEFINED__
typedef interface IWabbitemu IWabbitemu;
#endif 	/* __IWabbitemu_FWD_DEFINED__ */


#ifndef __Wabbitemu_FWD_DEFINED__
#define __Wabbitemu_FWD_DEFINED__

#ifdef __cplusplus
typedef class Wabbitemu Wabbitemu;
#else
typedef struct Wabbitemu Wabbitemu;
#endif /* __cplusplus */

#endif 	/* __Wabbitemu_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_Wabbitemu_0000_0000 */
/* [local] */ 


enum PAGETYPE
    {	FLASH	= 0,
	RAM	= ( FLASH + 1 ) 
    } ;
/* [uuid] */ struct  DECLSPEC_UUID("36AE85E8-FE2A-41bc-85B5-105B32D8E181") TIApplication
    {
    BSTR Name;
    int Page;
    int PageCount;
    } ;

enum SYMBOLTYPE
    {	SYM_REAL	= 0,
	SYM_LIST	= 0x1,
	SYM_MAT	= 0x2,
	SYM_EQU	= 0x3,
	SYM_STRNG	= 0x4,
	SYM_PROG	= 0x5,
	SYM_PROTPROG	= 0x6,
	SYM_PICT	= 0x7,
	SYM_GDB	= 0x8,
	SYM_UNKNOWN	= 0x9,
	SYM_UNKNOWNEQU	= 0xa,
	SYM_NEWEQU	= 0xb,
	SYM_CPLX	= 0xc,
	SYM_CLIST	= 0xd,
	SYM_UNDEF	= 0xe,
	SYM_WINDOW	= 0xf,
	SYM_ZSTO	= 0x10,
	SYM_TBLRNG	= 0x11,
	SYM_LCD	= 0x12,
	SYM_BACKUP	= 0x13,
	SYM_APP	= 0x14,
	SYM_APPVAR	= 0x15,
	SYM_TEMPPROG	= 0x16,
	SYM_GROUP	= 0x17,
	SYM_EQUOB	= 0x23,
	SYM_NONE	= -1
    } ;
/* [uuid] */ struct  DECLSPEC_UUID("010A714D-CADA-400f-90F9-FC34B2A20752") TISymbol
    {
    enum SYMBOLTYPE Type;
    BYTE Version;
    WORD Address;
    BYTE Page;
    BSTR Name;
    } ;


extern RPC_IF_HANDLE __MIDL_itf_Wabbitemu_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Wabbitemu_0000_0000_v0_0_s_ifspec;

#ifndef __IZ80_INTERFACE_DEFINED__
#define __IZ80_INTERFACE_DEFINED__

/* interface IZ80 */
/* [object][uuid] */ 


EXTERN_C const IID IID_IZ80;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("41C7B7BF-D390-4ba0-8075-462FD3565225")
    IZ80 : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AF( 
            /* [retval][out] */ WORD *AF) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_A( 
            /* [retval][out] */ BYTE *A) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_F( 
            /* [retval][out] */ BYTE *F) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_AF( 
            /* [in] */ WORD AF) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_A( 
            /* [in] */ BYTE A) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_F( 
            /* [in] */ BYTE F) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IZ80Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IZ80 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IZ80 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IZ80 * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_AF )( 
            IZ80 * This,
            /* [retval][out] */ WORD *AF);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_A )( 
            IZ80 * This,
            /* [retval][out] */ BYTE *A);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_F )( 
            IZ80 * This,
            /* [retval][out] */ BYTE *F);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_AF )( 
            IZ80 * This,
            /* [in] */ WORD AF);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_A )( 
            IZ80 * This,
            /* [in] */ BYTE A);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_F )( 
            IZ80 * This,
            /* [in] */ BYTE F);
        
        END_INTERFACE
    } IZ80Vtbl;

    interface IZ80
    {
        CONST_VTBL struct IZ80Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IZ80_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IZ80_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IZ80_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IZ80_get_AF(This,AF)	\
    ( (This)->lpVtbl -> get_AF(This,AF) ) 

#define IZ80_get_A(This,A)	\
    ( (This)->lpVtbl -> get_A(This,A) ) 

#define IZ80_get_F(This,F)	\
    ( (This)->lpVtbl -> get_F(This,F) ) 

#define IZ80_put_AF(This,AF)	\
    ( (This)->lpVtbl -> put_AF(This,AF) ) 

#define IZ80_put_A(This,A)	\
    ( (This)->lpVtbl -> put_A(This,A) ) 

#define IZ80_put_F(This,F)	\
    ( (This)->lpVtbl -> put_F(This,F) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IZ80_INTERFACE_DEFINED__ */


#ifndef __IPage_INTERFACE_DEFINED__
#define __IPage_INTERFACE_DEFINED__

/* interface IPage */
/* [object][uuid] */ 


EXTERN_C const IID IID_IPage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B61370EF-8BEF-4bb9-AF88-FF7FFD0A012E")
    IPage : public IUnknown
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ int *pIndex) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ enum PAGETYPE *pType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Read( 
            /* [in] */ WORD Address,
            /* [retval][out] */ BYTE *lpValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Write( 
            /* [in] */ WORD Address,
            /* [in] */ BYTE Value) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPage * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Index )( 
            IPage * This,
            /* [retval][out] */ int *pIndex);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IPage * This,
            /* [retval][out] */ enum PAGETYPE *pType);
        
        HRESULT ( STDMETHODCALLTYPE *Read )( 
            IPage * This,
            /* [in] */ WORD Address,
            /* [retval][out] */ BYTE *lpValue);
        
        HRESULT ( STDMETHODCALLTYPE *Write )( 
            IPage * This,
            /* [in] */ WORD Address,
            /* [in] */ BYTE Value);
        
        END_INTERFACE
    } IPageVtbl;

    interface IPage
    {
        CONST_VTBL struct IPageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPage_get_Index(This,pIndex)	\
    ( (This)->lpVtbl -> get_Index(This,pIndex) ) 

#define IPage_get_Type(This,pType)	\
    ( (This)->lpVtbl -> get_Type(This,pType) ) 

#define IPage_Read(This,Address,lpValue)	\
    ( (This)->lpVtbl -> Read(This,Address,lpValue) ) 

#define IPage_Write(This,Address,Value)	\
    ( (This)->lpVtbl -> Write(This,Address,Value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPage_INTERFACE_DEFINED__ */


#ifndef __IWabbitemu_INTERFACE_DEFINED__
#define __IWabbitemu_INTERFACE_DEFINED__

/* interface IWabbitemu */
/* [object][uuid] */ 


EXTERN_C const IID IID_IWabbitemu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("020FB9D5-7F45-449b-B9DF-66CFC2482BD0")
    IWabbitemu : public IUnknown
    {
    public:
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL fVisible) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL *lpVisible) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CPU( 
            /* [retval][out] */ IZ80 **ppZ80) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RAM( 
            /* [in] */ int Index,
            /* [retval][out] */ IPage **ppPage) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Flash( 
            /* [in] */ int Index,
            /* [retval][out] */ IPage **ppPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Step( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StepOver( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetBreakpoint( 
            /* [in] */ IPage *Page,
            /* [in] */ WORD Address) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Read( 
            /* [in] */ WORD Address,
            /* [retval][out] */ BYTE *lpValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Write( 
            /* [in] */ WORD Address,
            /* [in] */ BYTE Value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LoadFile( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Apps( 
            /* [retval][out] */ SAFEARRAY * *AppList) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Symbols( 
            /* [retval][out] */ SAFEARRAY * *SymList) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWabbitemuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWabbitemu * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWabbitemu * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWabbitemu * This);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IWabbitemu * This,
            /* [in] */ VARIANT_BOOL fVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IWabbitemu * This,
            /* [retval][out] */ VARIANT_BOOL *lpVisible);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CPU )( 
            IWabbitemu * This,
            /* [retval][out] */ IZ80 **ppZ80);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RAM )( 
            IWabbitemu * This,
            /* [in] */ int Index,
            /* [retval][out] */ IPage **ppPage);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Flash )( 
            IWabbitemu * This,
            /* [in] */ int Index,
            /* [retval][out] */ IPage **ppPage);
        
        HRESULT ( STDMETHODCALLTYPE *Step )( 
            IWabbitemu * This);
        
        HRESULT ( STDMETHODCALLTYPE *StepOver )( 
            IWabbitemu * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetBreakpoint )( 
            IWabbitemu * This,
            /* [in] */ IPage *Page,
            /* [in] */ WORD Address);
        
        HRESULT ( STDMETHODCALLTYPE *Read )( 
            IWabbitemu * This,
            /* [in] */ WORD Address,
            /* [retval][out] */ BYTE *lpValue);
        
        HRESULT ( STDMETHODCALLTYPE *Write )( 
            IWabbitemu * This,
            /* [in] */ WORD Address,
            /* [in] */ BYTE Value);
        
        HRESULT ( STDMETHODCALLTYPE *LoadFile )( 
            IWabbitemu * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Apps )( 
            IWabbitemu * This,
            /* [retval][out] */ SAFEARRAY * *AppList);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Symbols )( 
            IWabbitemu * This,
            /* [retval][out] */ SAFEARRAY * *SymList);
        
        END_INTERFACE
    } IWabbitemuVtbl;

    interface IWabbitemu
    {
        CONST_VTBL struct IWabbitemuVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWabbitemu_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWabbitemu_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWabbitemu_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWabbitemu_put_Visible(This,fVisible)	\
    ( (This)->lpVtbl -> put_Visible(This,fVisible) ) 

#define IWabbitemu_get_Visible(This,lpVisible)	\
    ( (This)->lpVtbl -> get_Visible(This,lpVisible) ) 

#define IWabbitemu_get_CPU(This,ppZ80)	\
    ( (This)->lpVtbl -> get_CPU(This,ppZ80) ) 

#define IWabbitemu_RAM(This,Index,ppPage)	\
    ( (This)->lpVtbl -> RAM(This,Index,ppPage) ) 

#define IWabbitemu_Flash(This,Index,ppPage)	\
    ( (This)->lpVtbl -> Flash(This,Index,ppPage) ) 

#define IWabbitemu_Step(This)	\
    ( (This)->lpVtbl -> Step(This) ) 

#define IWabbitemu_StepOver(This)	\
    ( (This)->lpVtbl -> StepOver(This) ) 

#define IWabbitemu_SetBreakpoint(This,Page,Address)	\
    ( (This)->lpVtbl -> SetBreakpoint(This,Page,Address) ) 

#define IWabbitemu_Read(This,Address,lpValue)	\
    ( (This)->lpVtbl -> Read(This,Address,lpValue) ) 

#define IWabbitemu_Write(This,Address,Value)	\
    ( (This)->lpVtbl -> Write(This,Address,Value) ) 

#define IWabbitemu_LoadFile(This,FileName)	\
    ( (This)->lpVtbl -> LoadFile(This,FileName) ) 

#define IWabbitemu_get_Apps(This,AppList)	\
    ( (This)->lpVtbl -> get_Apps(This,AppList) ) 

#define IWabbitemu_get_Symbols(This,SymList)	\
    ( (This)->lpVtbl -> get_Symbols(This,SymList) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWabbitemu_INTERFACE_DEFINED__ */



#ifndef __WabbitemuLib_LIBRARY_DEFINED__
#define __WabbitemuLib_LIBRARY_DEFINED__

/* library WabbitemuLib */
/* [version][uuid] */ 




EXTERN_C const IID LIBID_WabbitemuLib;

EXTERN_C const CLSID CLSID_Wabbitemu;

#ifdef __cplusplus

class DECLSPEC_UUID("1A03AFA2-C9C7-4d5b-9732-D78011D5009B")
Wabbitemu;
#endif
#endif /* __WabbitemuLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


