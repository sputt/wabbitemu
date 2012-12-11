

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Mon Dec 10 20:15:26 2012
 */
/* Compiler settings for SPASM.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0595 
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

#ifndef __IZ80Label_FWD_DEFINED__
#define __IZ80Label_FWD_DEFINED__
typedef interface IZ80Label IZ80Label;

#endif 	/* __IZ80Label_FWD_DEFINED__ */


#ifndef __IZ80Assembler_FWD_DEFINED__
#define __IZ80Assembler_FWD_DEFINED__
typedef interface IZ80Assembler IZ80Assembler;

#endif 	/* __IZ80Assembler_FWD_DEFINED__ */


#ifndef __IZ80Label_FWD_DEFINED__
#define __IZ80Label_FWD_DEFINED__
typedef interface IZ80Label IZ80Label;

#endif 	/* __IZ80Label_FWD_DEFINED__ */


#ifndef __Z80Assembler_FWD_DEFINED__
#define __Z80Assembler_FWD_DEFINED__

#ifdef __cplusplus
typedef class Z80Assembler Z80Assembler;
#else
typedef struct Z80Assembler Z80Assembler;
#endif /* __cplusplus */

#endif 	/* __Z80Assembler_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IZ80Label_INTERFACE_DEFINED__
#define __IZ80Label_INTERFACE_DEFINED__

/* interface IZ80Label */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IZ80Label;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D9992A64-45EC-492B-8577-3F3EC929076A")
    IZ80Label : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *Name) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ DWORD *Value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IZ80LabelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IZ80Label * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IZ80Label * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IZ80Label * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IZ80Label * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IZ80Label * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IZ80Label * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IZ80Label * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IZ80Label * This,
            /* [retval][out] */ BSTR *Name);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            IZ80Label * This,
            /* [retval][out] */ DWORD *Value);
        
        END_INTERFACE
    } IZ80LabelVtbl;

    interface IZ80Label
    {
        CONST_VTBL struct IZ80LabelVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IZ80Label_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IZ80Label_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IZ80Label_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IZ80Label_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IZ80Label_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IZ80Label_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IZ80Label_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IZ80Label_get_Name(This,Name)	\
    ( (This)->lpVtbl -> get_Name(This,Name) ) 

#define IZ80Label_get_Value(This,Value)	\
    ( (This)->lpVtbl -> get_Value(This,Value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IZ80Label_INTERFACE_DEFINED__ */


#ifndef __IZ80Assembler_INTERFACE_DEFINED__
#define __IZ80Assembler_INTERFACE_DEFINED__

/* interface IZ80Assembler */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IZ80Assembler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("38516350-2EF5-48DE-8C13-FEEAD1CAC91A")
    IZ80Assembler : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Output( 
            /* [retval][out] */ IStream **Stream) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_InputFile( 
            /* [retval][out] */ BSTR *FileName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_InputFile( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_OutputFile( 
            /* [retval][out] */ BSTR *FileName) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_OutputFile( 
            /* [in] */ BSTR FileName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ClearDefines( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddDefine( 
            /* [in] */ BSTR Name,
            /* [optional][in] */ VARIANT Value) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ClearIncludeDirectories( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddIncludeDirectory( 
            /* [in] */ BSTR Directory) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Assemble( 
            /* [optional][in] */ VARIANT Input,
            /* [retval][out] */ int *Return) = 0;
        
        virtual /* [propget][helpstring] */ HRESULT STDMETHODCALLTYPE get_Labels( 
            /* [retval][out] */ SAFEARRAY * *Labels) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IZ80AssemblerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IZ80Assembler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
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
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Output )( 
            IZ80Assembler * This,
            /* [retval][out] */ IStream **Stream);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InputFile )( 
            IZ80Assembler * This,
            /* [retval][out] */ BSTR *FileName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_InputFile )( 
            IZ80Assembler * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OutputFile )( 
            IZ80Assembler * This,
            /* [retval][out] */ BSTR *FileName);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OutputFile )( 
            IZ80Assembler * This,
            /* [in] */ BSTR FileName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ClearDefines )( 
            IZ80Assembler * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddDefine )( 
            IZ80Assembler * This,
            /* [in] */ BSTR Name,
            /* [optional][in] */ VARIANT Value);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ClearIncludeDirectories )( 
            IZ80Assembler * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddIncludeDirectory )( 
            IZ80Assembler * This,
            /* [in] */ BSTR Directory);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Assemble )( 
            IZ80Assembler * This,
            /* [optional][in] */ VARIANT Input,
            /* [retval][out] */ int *Return);
        
        /* [propget][helpstring] */ HRESULT ( STDMETHODCALLTYPE *get_Labels )( 
            IZ80Assembler * This,
            /* [retval][out] */ SAFEARRAY * *Labels);
        
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

#define IZ80Assembler_get_InputFile(This,FileName)	\
    ( (This)->lpVtbl -> get_InputFile(This,FileName) ) 

#define IZ80Assembler_put_InputFile(This,FileName)	\
    ( (This)->lpVtbl -> put_InputFile(This,FileName) ) 

#define IZ80Assembler_get_OutputFile(This,FileName)	\
    ( (This)->lpVtbl -> get_OutputFile(This,FileName) ) 

#define IZ80Assembler_put_OutputFile(This,FileName)	\
    ( (This)->lpVtbl -> put_OutputFile(This,FileName) ) 

#define IZ80Assembler_ClearDefines(This)	\
    ( (This)->lpVtbl -> ClearDefines(This) ) 

#define IZ80Assembler_AddDefine(This,Name,Value)	\
    ( (This)->lpVtbl -> AddDefine(This,Name,Value) ) 

#define IZ80Assembler_ClearIncludeDirectories(This)	\
    ( (This)->lpVtbl -> ClearIncludeDirectories(This) ) 

#define IZ80Assembler_AddIncludeDirectory(This,Directory)	\
    ( (This)->lpVtbl -> AddIncludeDirectory(This,Directory) ) 

#define IZ80Assembler_Assemble(This,Input,Return)	\
    ( (This)->lpVtbl -> Assemble(This,Input,Return) ) 

#define IZ80Assembler_get_Labels(This,Labels)	\
    ( (This)->lpVtbl -> get_Labels(This,Labels) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IZ80Assembler_INTERFACE_DEFINED__ */



#ifndef __SPASM_LIBRARY_DEFINED__
#define __SPASM_LIBRARY_DEFINED__

/* library SPASM */
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_SPASM;

EXTERN_C const CLSID CLSID_Z80Assembler;

#ifdef __cplusplus

class DECLSPEC_UUID("BDD26FAE-A388-4860-9D4E-669809BC8EC2")
Z80Assembler;
#endif
#endif /* __SPASM_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


