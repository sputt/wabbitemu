

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Sep 06 01:14:33 2010
 */
/* Compiler settings for interface\Wabbitemu.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ILCD,0x023D1C58,0xC323,0x43a7,0x86,0x0B,0x95,0xFB,0x9C,0x20,0x4F,0x91);


MIDL_DEFINE_GUID(IID, IID_IZ80,0x41C7B7BF,0xD390,0x4ba0,0x80,0x75,0x46,0x2F,0xD3,0x56,0x52,0x25);


MIDL_DEFINE_GUID(IID, IID_IPage,0xB61370EF,0x8BEF,0x4bb9,0xAF,0x88,0xFF,0x7F,0xFD,0x0A,0x01,0x2E);


MIDL_DEFINE_GUID(IID, IID_IKeypad,0x9BD2CC22,0x5E1D,0x41c1,0xA1,0x34,0xF2,0x44,0x10,0xEF,0x63,0x01);


MIDL_DEFINE_GUID(IID, IID_IWabbitemu,0x020FB9D5,0x7F45,0x449b,0xB9,0xDF,0x66,0xCF,0xC2,0x48,0x2B,0xD0);


MIDL_DEFINE_GUID(IID, LIBID_WabbitemuLib,0xEDA903F6,0x7BB8,0x437f,0x97,0x8F,0x8F,0x1A,0x2B,0x8B,0x65,0xDB);


MIDL_DEFINE_GUID(CLSID, CLSID_Wabbitemu,0x1A03AFA2,0xC9C7,0x4d5b,0x97,0x32,0xD7,0x80,0x11,0xD5,0x00,0x9B);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



