

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Mon Nov 11 12:45:25 2013
 */
/* Compiler settings for ..\..\src\smartcom_proxy2\smartcom_proxy2.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
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

MIDL_DEFINE_GUID(IID, IID_IMainSmartComEngineCallback,0xDEDF458D,0x7AFB,0x495D,0x9B,0x96,0xD9,0xAA,0xEE,0x29,0x92,0xC4);


MIDL_DEFINE_GUID(IID, IID_IMainSmartComEngine,0x44670410,0xF00B,0x42E6,0xBF,0xD0,0x27,0x8D,0x54,0x26,0x2F,0xD9);


MIDL_DEFINE_GUID(IID, LIBID_smartcom_proxy2Lib,0x42519A4B,0x1067,0x4F08,0xB3,0x50,0x43,0x31,0x56,0xAE,0x50,0xD6);


MIDL_DEFINE_GUID(CLSID, CLSID_MainSmartComEngine,0x149C2130,0x0832,0x47D4,0xA0,0xD8,0xEE,0x14,0x1D,0x31,0xC6,0xB9);


MIDL_DEFINE_GUID(CLSID, CLSID_MainSmartComEngineCallback,0xED10803B,0x9E22,0x4C0C,0xB8,0x59,0x52,0xC6,0xE8,0x62,0xBE,0x41);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



