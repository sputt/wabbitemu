

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Feb 26 15:44:26 2011
 */
/* Compiler settings for SPASM.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

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

#ifndef __SPASM_h_h__
#define __SPASM_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IZ80Assembler_FWD_DEFINED__
#define __IZ80Assembler_FWD_DEFINED__
typedef interface IZ80Assembler IZ80Assembler;
#endif 	/* __IZ80Assembler_FWD_DEFINED__ */


#ifndef __CZ80Assembler_FWD_DEFINED__
#define __CZ80Assembler_FWD_DEFINED__

#ifdef __cplusplus
typedef class CZ80Assembler CZ80Assembler;
#else
typedef struct CZ80Assembler CZ80Assembler;
#endif /* __cplusplus */

#endif 	/* __CZ80Assembler_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IZ80Assembler_INTERFACE_DEFINED__
#define __IZ80Assembler_INTERFACE_DEFINED__

/* interface IZ80Assembler */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IZ80Assembler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("81B27D7E-B1B5-402f-84E5-0948A38D5497")
    IZ80Assembler : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Output( 
            /* [retval][out] */ IStream **Stream) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Input( 
            /* [in] */ BSTR Input) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Assemble( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IZ80AssemblerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IZ80Assembler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IZ80Assembler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IZ80Assembler * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IZ80Assembler * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IZ80Assembler * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IZ80Assembler * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IZ80Assembler * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Output )( 
            IZ80Assembler * This,
            /* [retval][out] */ IStream **Stream);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Input )( 
            IZ80Assembler * This,
            /* [in] */ BSTR Input);
        
        HRESULT ( STDMETHODCALLTYPE *Assemble )( 
            IZ80Assembler * This);
        
        END_INTERFACE
    } IZ80AssemblerVtbl;

    interface IZ80Assembler
    {
        CONST_VTBL struct IZ80AssemblerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IZ80Assembler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IZ80Assembler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IZ80Assembler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IZ80Assembler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IZ80Assembler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IZ80Assembler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IZ80Assembler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IZ80Assembler_get_Output(This,Stream)	\
    ( (This)->lpVtbl -> get_Output(This,Stream) ) 

#define IZ80Assembler_put_Input(This,Input)	\
    ( (This)->lpVtbl -> put_Input(This,Input) ) 

#define IZ80Assembler_Assemble(This)	\
    ( (This)->lpVtbl -> Assemble(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IZ80Assembler_INTERFACE_DEFINED__ */



#ifndef __SPASM_LIBRARY_DEFINED__
#define __SPASM_LIBRARY_DEFINED__

/* library SPASM */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_SPASM;

EXTERN_C const CLSID CLSID_CZ80Assembler;

#ifdef __cplusplus

class DECLSPEC_UUID("FD2AD3D2-78AE-4761-AD02-F8D9263A527D")
CZ80Assembler;
#endif
#endif /* __SPASM_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


