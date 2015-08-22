

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif /* __RPCPROXY_H_VERSION__ */


#include "smartcom_proxy2_i.h"

#define TYPE_FORMAT_STRING_SIZE   1079                              
#define PROC_FORMAT_STRING_SIZE   1375                              
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

typedef struct _smartcom_proxy2_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } smartcom_proxy2_MIDL_TYPE_FORMAT_STRING;

typedef struct _smartcom_proxy2_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } smartcom_proxy2_MIDL_PROC_FORMAT_STRING;

typedef struct _smartcom_proxy2_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } smartcom_proxy2_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const smartcom_proxy2_MIDL_TYPE_FORMAT_STRING smartcom_proxy2__MIDL_TypeFormatString;
extern const smartcom_proxy2_MIDL_PROC_FORMAT_STRING smartcom_proxy2__MIDL_ProcFormatString;
extern const smartcom_proxy2_MIDL_EXPR_FORMAT_STRING smartcom_proxy2__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMainSmartComEngineCallback_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IMainSmartComEngineCallback_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IMainSmartComEngine_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IMainSmartComEngine_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const smartcom_proxy2_MIDL_PROC_FORMAT_STRING smartcom_proxy2__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure onOrderSucceded */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x50 ),	/* x86 Stack size/offset = 80 */
/* 10 */	NdrFcShort( 0x78 ),	/* 120 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xe,		/* 14 */
/* 16 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter symbol */

/* 30 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter state */

/* 36 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 38 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 40 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter action */

/* 42 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter type */

/* 48 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 50 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 52 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter validity */

/* 54 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 56 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 58 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter price */

/* 60 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 62 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 64 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter amount */

/* 66 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 68 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 70 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter stop */

/* 72 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 74 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 76 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter filled */

/* 78 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 80 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 82 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter datetime */

/* 84 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 86 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 88 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter orderid */

/* 90 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 92 */	NdrFcShort( 0x44 ),	/* x86 Stack size/offset = 68 */
/* 94 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter orderno */

/* 96 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 98 */	NdrFcShort( 0x48 ),	/* x86 Stack size/offset = 72 */
/* 100 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 102 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 104 */	NdrFcShort( 0x4c ),	/* x86 Stack size/offset = 76 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onOrderFailed */

/* 108 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 110 */	NdrFcLong( 0x0 ),	/* 0 */
/* 114 */	NdrFcShort( 0x4 ),	/* 4 */
/* 116 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 120 */	NdrFcShort( 0x8 ),	/* 8 */
/* 122 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 124 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 126 */	NdrFcShort( 0x0 ),	/* 0 */
/* 128 */	NdrFcShort( 0x1 ),	/* 1 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 132 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 134 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter orderid */

/* 138 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 140 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 142 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter reason */

/* 144 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 148 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 152 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onConnect */

/* 156 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x5 ),	/* 5 */
/* 164 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 170 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 172 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onDisconnect */

/* 186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x6 ),	/* 6 */
/* 194 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 200 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 202 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 206 */	NdrFcShort( 0x1 ),	/* 1 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter reason */

/* 210 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 214 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onAddTrade */

/* 222 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x7 ),	/* 7 */
/* 230 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 232 */	NdrFcShort( 0x38 ),	/* 56 */
/* 234 */	NdrFcShort( 0x8 ),	/* 8 */
/* 236 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 238 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x1 ),	/* 1 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 246 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 248 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter portfolio */

/* 252 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 254 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 256 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter symbol */

/* 258 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 260 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 262 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter orderno */

/* 264 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 266 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 268 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter orderid */

/* 270 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 272 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 274 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter price */

/* 276 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 278 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 280 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter amount */

/* 282 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 284 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 286 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter datetime */

/* 288 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 290 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 292 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter tradeno */

/* 294 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 296 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 298 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 300 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 302 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onSetMyTrade */

/* 306 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 312 */	NdrFcShort( 0x8 ),	/* 8 */
/* 314 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 316 */	NdrFcShort( 0x38 ),	/* 56 */
/* 318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 320 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 322 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0x1 ),	/* 1 */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter portfolio */

/* 330 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 332 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 334 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter symbol */

/* 336 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 338 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 340 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter datetime */

/* 342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 344 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 346 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter price */

/* 348 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 350 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 352 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter volume */

/* 354 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 356 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 358 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter tradeno */

/* 360 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 362 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 364 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter buysell */

/* 366 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 368 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 370 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter orderno */

/* 372 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 374 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 376 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter orderid */

/* 378 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 380 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 382 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Return value */

/* 384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 386 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onFinishBarsHistory */

/* 390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0x9 ),	/* 9 */
/* 398 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 400 */	NdrFcShort( 0x16 ),	/* 22 */
/* 402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 404 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 406 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 408 */	NdrFcShort( 0x0 ),	/* 0 */
/* 410 */	NdrFcShort( 0x1 ),	/* 1 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 414 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 416 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter symbol */

/* 420 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 422 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 424 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter result */

/* 426 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 428 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 430 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter count */

/* 432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 434 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter eventClientPtr */

/* 438 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 440 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 442 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */

	/* Return value */

/* 444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 446 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onTimer_1Sec */

/* 450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0xa ),	/* 10 */
/* 458 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 464 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 466 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 474 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure libInit */

/* 480 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 482 */	NdrFcLong( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x7 ),	/* 7 */
/* 488 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x24 ),	/* 36 */
/* 494 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 496 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 502 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter eventClient */

/* 504 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 506 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 508 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Parameter cookie */

/* 510 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 512 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 518 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure libDeinit */

/* 522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 530 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 532 */	NdrFcShort( 0x8 ),	/* 8 */
/* 534 */	NdrFcShort( 0x8 ),	/* 8 */
/* 536 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 538 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 546 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 548 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 550 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 552 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 554 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 556 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure connect */

/* 558 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 560 */	NdrFcLong( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x9 ),	/* 9 */
/* 566 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 568 */	NdrFcShort( 0x14 ),	/* 20 */
/* 570 */	NdrFcShort( 0x8 ),	/* 8 */
/* 572 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xa,		/* 10 */
/* 574 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 576 */	NdrFcShort( 0x0 ),	/* 0 */
/* 578 */	NdrFcShort( 0x1 ),	/* 1 */
/* 580 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter rtProvider */

/* 582 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 584 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 586 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter rtProviderBarPostfix */

/* 588 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 590 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 592 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter accompanyTicks1minBar */

/* 594 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 596 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 598 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter host */

/* 600 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 602 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 604 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter port */

/* 606 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 608 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 610 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter login */

/* 612 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 614 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 616 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter password */

/* 618 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 620 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 622 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter portfolioList */

/* 624 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 626 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 628 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Parameter loglevel */

/* 630 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 632 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 636 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 638 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 640 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure subscribeForTicks */

/* 642 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 644 */	NdrFcLong( 0x0 ),	/* 0 */
/* 648 */	NdrFcShort( 0xa ),	/* 10 */
/* 650 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 654 */	NdrFcShort( 0x8 ),	/* 8 */
/* 656 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 658 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x1 ),	/* 1 */
/* 664 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbols */

/* 666 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 668 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 670 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Return value */

/* 672 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 674 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure unsubscribeForTicks */

/* 678 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 680 */	NdrFcLong( 0x0 ),	/* 0 */
/* 684 */	NdrFcShort( 0xb ),	/* 11 */
/* 686 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x8 ),	/* 8 */
/* 692 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 694 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 696 */	NdrFcShort( 0x0 ),	/* 0 */
/* 698 */	NdrFcShort( 0x1 ),	/* 1 */
/* 700 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbols */

/* 702 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 704 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 706 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Return value */

/* 708 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 710 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure querySymbols */

/* 714 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 716 */	NdrFcLong( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0xc ),	/* 12 */
/* 722 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 724 */	NdrFcShort( 0x0 ),	/* 0 */
/* 726 */	NdrFcShort( 0x8 ),	/* 8 */
/* 728 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 730 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 732 */	NdrFcShort( 0x0 ),	/* 0 */
/* 734 */	NdrFcShort( 0x0 ),	/* 0 */
/* 736 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 738 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 740 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure disconnect */

/* 744 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0xd ),	/* 13 */
/* 752 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 754 */	NdrFcShort( 0x0 ),	/* 0 */
/* 756 */	NdrFcShort( 0x8 ),	/* 8 */
/* 758 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 760 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 768 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 770 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure getPrintHistory */

/* 774 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 776 */	NdrFcLong( 0x0 ),	/* 0 */
/* 780 */	NdrFcShort( 0xe ),	/* 14 */
/* 782 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 784 */	NdrFcShort( 0x18 ),	/* 24 */
/* 786 */	NdrFcShort( 0x8 ),	/* 8 */
/* 788 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 790 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 792 */	NdrFcShort( 0x0 ),	/* 0 */
/* 794 */	NdrFcShort( 0x1 ),	/* 1 */
/* 796 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbols */

/* 798 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 800 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 802 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Parameter from_date */

/* 804 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 806 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 808 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter from_count */

/* 810 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 812 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 816 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 818 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 820 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure subscribeForLevel1 */

/* 822 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 824 */	NdrFcLong( 0x0 ),	/* 0 */
/* 828 */	NdrFcShort( 0xf ),	/* 15 */
/* 830 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 836 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 838 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 840 */	NdrFcShort( 0x0 ),	/* 0 */
/* 842 */	NdrFcShort( 0x1 ),	/* 1 */
/* 844 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbols */

/* 846 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 848 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 850 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Return value */

/* 852 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 854 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure unsubscribeForLevel1 */

/* 858 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 860 */	NdrFcLong( 0x0 ),	/* 0 */
/* 864 */	NdrFcShort( 0x10 ),	/* 16 */
/* 866 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 868 */	NdrFcShort( 0x0 ),	/* 0 */
/* 870 */	NdrFcShort( 0x8 ),	/* 8 */
/* 872 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 874 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	NdrFcShort( 0x1 ),	/* 1 */
/* 880 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbols */

/* 882 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 884 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 886 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Return value */

/* 888 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 890 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure getPortfolioList */

/* 894 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 896 */	NdrFcLong( 0x0 ),	/* 0 */
/* 900 */	NdrFcShort( 0x11 ),	/* 17 */
/* 902 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0x8 ),	/* 8 */
/* 908 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 910 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 914 */	NdrFcShort( 0x0 ),	/* 0 */
/* 916 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 918 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure issueOrder */

/* 924 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 926 */	NdrFcLong( 0x0 ),	/* 0 */
/* 930 */	NdrFcShort( 0x12 ),	/* 18 */
/* 932 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 934 */	NdrFcShort( 0x50 ),	/* 80 */
/* 936 */	NdrFcShort( 0x8 ),	/* 8 */
/* 938 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xb,		/* 11 */
/* 940 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 942 */	NdrFcShort( 0x0 ),	/* 0 */
/* 944 */	NdrFcShort( 0x1 ),	/* 1 */
/* 946 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter portfolio */

/* 948 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 950 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 952 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter symbol */

/* 954 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 956 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 958 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter order_action */

/* 960 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 962 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter order_type */

/* 966 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 968 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter validity */

/* 972 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 974 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 976 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter price */

/* 978 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 980 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 982 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter amount */

/* 984 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 986 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 988 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter stop */

/* 990 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 992 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 994 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter cookie */

/* 996 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 998 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 1000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter eventClient */

/* 1002 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1004 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 1006 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 1008 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1010 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 1012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure checkConnectStatus */

/* 1014 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1016 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1020 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1022 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1024 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1028 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1030 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1032 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1036 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1038 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1040 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure cancelOrder */

/* 1044 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1046 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1050 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1052 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1054 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1056 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1058 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1060 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1062 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1064 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1066 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter portfolio */

/* 1068 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1070 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1072 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter symbol */

/* 1074 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1076 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1078 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter orderId */

/* 1080 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1082 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1084 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter cookie */

/* 1086 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1088 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter eventClient */

/* 1092 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1094 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1096 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 1098 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1100 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1102 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure cancelAllOrders */

/* 1104 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1106 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1110 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1112 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1114 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1118 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1120 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1122 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1126 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter cookie */

/* 1128 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1130 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter eventClient */

/* 1134 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1136 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1138 */	NdrFcShort( 0x38 ),	/* Type Offset=56 */

	/* Return value */

/* 1140 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1142 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure getBarsHistory */

/* 1146 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1152 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1154 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1156 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1158 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1160 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 1162 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1166 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1168 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbol */

/* 1170 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1172 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1174 */	NdrFcShort( 0x1c ),	/* Type Offset=28 */

	/* Parameter interval */

/* 1176 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1178 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter from_date */

/* 1182 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1184 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1186 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter from_count */

/* 1188 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1190 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1192 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter cookie */

/* 1194 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1196 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1200 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1202 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure subscribeForLevel2 */

/* 1206 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1212 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1214 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1216 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1218 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1220 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1222 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1226 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1228 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbols */

/* 1230 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1232 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1234 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Return value */

/* 1236 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1238 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure unsubscribeForLevel2 */

/* 1242 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1244 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1248 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1250 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1254 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1256 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x2,		/* 2 */
/* 1258 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1260 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1262 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1264 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter symbols */

/* 1266 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1268 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1270 */	NdrFcShort( 0x42c ),	/* Type Offset=1068 */

	/* Return value */

/* 1272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1274 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure getMyTradesOnce */

/* 1278 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1284 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1286 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1292 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1294 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1298 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1302 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1304 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure subscribeGetMyTrades */

/* 1308 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1310 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1314 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1316 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1320 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1322 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 1324 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1330 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pollPeriodSec */

/* 1332 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1334 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1338 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1340 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1342 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure unsubscribeGetMyTrades */

/* 1344 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1346 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1350 */	NdrFcShort( 0x1b ),	/* 27 */
/* 1352 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1354 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1356 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1358 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 1360 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1362 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1366 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 1368 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1370 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const smartcom_proxy2_MIDL_TYPE_FORMAT_STRING smartcom_proxy2__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x12, 0x0,	/* FC_UP */
/*  4 */	NdrFcShort( 0xe ),	/* Offset= 14 (18) */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/*  8 */	NdrFcShort( 0x2 ),	/* 2 */
/* 10 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 14 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 16 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 18 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 20 */	NdrFcShort( 0x8 ),	/* 8 */
/* 22 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (6) */
/* 24 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 26 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 28 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 30 */	NdrFcShort( 0x0 ),	/* 0 */
/* 32 */	NdrFcShort( 0x4 ),	/* 4 */
/* 34 */	NdrFcShort( 0x0 ),	/* 0 */
/* 36 */	NdrFcShort( 0xffde ),	/* Offset= -34 (2) */
/* 38 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 40 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 44 */	NdrFcShort( 0x0 ),	/* 0 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 50 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 52 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 54 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 56 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 58 */	NdrFcLong( 0xdedf458d ),	/* -555793011 */
/* 62 */	NdrFcShort( 0x7afb ),	/* 31483 */
/* 64 */	NdrFcShort( 0x495d ),	/* 18781 */
/* 66 */	0x9b,		/* 155 */
			0x96,		/* 150 */
/* 68 */	0xd9,		/* 217 */
			0xaa,		/* 170 */
/* 70 */	0xee,		/* 238 */
			0x29,		/* 41 */
/* 72 */	0x92,		/* 146 */
			0xc4,		/* 196 */
/* 74 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 76 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 78 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 80 */	NdrFcShort( 0x2 ),	/* Offset= 2 (82) */
/* 82 */	
			0x12, 0x0,	/* FC_UP */
/* 84 */	NdrFcShort( 0x3c6 ),	/* Offset= 966 (1050) */
/* 86 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 88 */	NdrFcShort( 0x18 ),	/* 24 */
/* 90 */	NdrFcShort( 0xa ),	/* 10 */
/* 92 */	NdrFcLong( 0x8 ),	/* 8 */
/* 96 */	NdrFcShort( 0x5a ),	/* Offset= 90 (186) */
/* 98 */	NdrFcLong( 0xd ),	/* 13 */
/* 102 */	NdrFcShort( 0x90 ),	/* Offset= 144 (246) */
/* 104 */	NdrFcLong( 0x9 ),	/* 9 */
/* 108 */	NdrFcShort( 0xb0 ),	/* Offset= 176 (284) */
/* 110 */	NdrFcLong( 0xc ),	/* 12 */
/* 114 */	NdrFcShort( 0x2aa ),	/* Offset= 682 (796) */
/* 116 */	NdrFcLong( 0x24 ),	/* 36 */
/* 120 */	NdrFcShort( 0x2d4 ),	/* Offset= 724 (844) */
/* 122 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 126 */	NdrFcShort( 0x2f0 ),	/* Offset= 752 (878) */
/* 128 */	NdrFcLong( 0x10 ),	/* 16 */
/* 132 */	NdrFcShort( 0x30a ),	/* Offset= 778 (910) */
/* 134 */	NdrFcLong( 0x2 ),	/* 2 */
/* 138 */	NdrFcShort( 0x324 ),	/* Offset= 804 (942) */
/* 140 */	NdrFcLong( 0x3 ),	/* 3 */
/* 144 */	NdrFcShort( 0x33e ),	/* Offset= 830 (974) */
/* 146 */	NdrFcLong( 0x14 ),	/* 20 */
/* 150 */	NdrFcShort( 0x358 ),	/* Offset= 856 (1006) */
/* 152 */	NdrFcShort( 0xffff ),	/* Offset= -1 (151) */
/* 154 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 156 */	NdrFcShort( 0x4 ),	/* 4 */
/* 158 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 164 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 166 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 168 */	NdrFcShort( 0x4 ),	/* 4 */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x1 ),	/* 1 */
/* 174 */	NdrFcShort( 0x0 ),	/* 0 */
/* 176 */	NdrFcShort( 0x0 ),	/* 0 */
/* 178 */	0x12, 0x0,	/* FC_UP */
/* 180 */	NdrFcShort( 0xff5e ),	/* Offset= -162 (18) */
/* 182 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 184 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 186 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 188 */	NdrFcShort( 0x8 ),	/* 8 */
/* 190 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 192 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 194 */	NdrFcShort( 0x4 ),	/* 4 */
/* 196 */	NdrFcShort( 0x4 ),	/* 4 */
/* 198 */	0x11, 0x0,	/* FC_RP */
/* 200 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (154) */
/* 202 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 204 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 206 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 216 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 218 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 220 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 222 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 224 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 228 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 232 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 234 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 238 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 240 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 242 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (206) */
/* 244 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 246 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 248 */	NdrFcShort( 0x8 ),	/* 8 */
/* 250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 252 */	NdrFcShort( 0x6 ),	/* Offset= 6 (258) */
/* 254 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 256 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 258 */	
			0x11, 0x0,	/* FC_RP */
/* 260 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (224) */
/* 262 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 266 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 272 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 276 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 278 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 280 */	NdrFcShort( 0xff0e ),	/* Offset= -242 (38) */
/* 282 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 284 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 286 */	NdrFcShort( 0x8 ),	/* 8 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x6 ),	/* Offset= 6 (296) */
/* 292 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 294 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 296 */	
			0x11, 0x0,	/* FC_RP */
/* 298 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (262) */
/* 300 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 302 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 304 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 306 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 308 */	NdrFcShort( 0x2 ),	/* Offset= 2 (310) */
/* 310 */	NdrFcShort( 0x10 ),	/* 16 */
/* 312 */	NdrFcShort( 0x2f ),	/* 47 */
/* 314 */	NdrFcLong( 0x14 ),	/* 20 */
/* 318 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 320 */	NdrFcLong( 0x3 ),	/* 3 */
/* 324 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 326 */	NdrFcLong( 0x11 ),	/* 17 */
/* 330 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 332 */	NdrFcLong( 0x2 ),	/* 2 */
/* 336 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 338 */	NdrFcLong( 0x4 ),	/* 4 */
/* 342 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 344 */	NdrFcLong( 0x5 ),	/* 5 */
/* 348 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 350 */	NdrFcLong( 0xb ),	/* 11 */
/* 354 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 356 */	NdrFcLong( 0xa ),	/* 10 */
/* 360 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 362 */	NdrFcLong( 0x6 ),	/* 6 */
/* 366 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (598) */
/* 368 */	NdrFcLong( 0x7 ),	/* 7 */
/* 372 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 374 */	NdrFcLong( 0x8 ),	/* 8 */
/* 378 */	NdrFcShort( 0xfe88 ),	/* Offset= -376 (2) */
/* 380 */	NdrFcLong( 0xd ),	/* 13 */
/* 384 */	NdrFcShort( 0xff4e ),	/* Offset= -178 (206) */
/* 386 */	NdrFcLong( 0x9 ),	/* 9 */
/* 390 */	NdrFcShort( 0xfea0 ),	/* Offset= -352 (38) */
/* 392 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 396 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (604) */
/* 398 */	NdrFcLong( 0x24 ),	/* 36 */
/* 402 */	NdrFcShort( 0xd2 ),	/* Offset= 210 (612) */
/* 404 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 408 */	NdrFcShort( 0xcc ),	/* Offset= 204 (612) */
/* 410 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 414 */	NdrFcShort( 0xfc ),	/* Offset= 252 (666) */
/* 416 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 420 */	NdrFcShort( 0xfa ),	/* Offset= 250 (670) */
/* 422 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 426 */	NdrFcShort( 0xf8 ),	/* Offset= 248 (674) */
/* 428 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 432 */	NdrFcShort( 0xf6 ),	/* Offset= 246 (678) */
/* 434 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 438 */	NdrFcShort( 0xf4 ),	/* Offset= 244 (682) */
/* 440 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 444 */	NdrFcShort( 0xf2 ),	/* Offset= 242 (686) */
/* 446 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 450 */	NdrFcShort( 0xdc ),	/* Offset= 220 (670) */
/* 452 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 456 */	NdrFcShort( 0xda ),	/* Offset= 218 (674) */
/* 458 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 462 */	NdrFcShort( 0xe4 ),	/* Offset= 228 (690) */
/* 464 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 468 */	NdrFcShort( 0xda ),	/* Offset= 218 (686) */
/* 470 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 474 */	NdrFcShort( 0xdc ),	/* Offset= 220 (694) */
/* 476 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 480 */	NdrFcShort( 0xda ),	/* Offset= 218 (698) */
/* 482 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 486 */	NdrFcShort( 0xd8 ),	/* Offset= 216 (702) */
/* 488 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 492 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (706) */
/* 494 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 498 */	NdrFcShort( 0xdc ),	/* Offset= 220 (718) */
/* 500 */	NdrFcLong( 0x10 ),	/* 16 */
/* 504 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 506 */	NdrFcLong( 0x12 ),	/* 18 */
/* 510 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 512 */	NdrFcLong( 0x13 ),	/* 19 */
/* 516 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 518 */	NdrFcLong( 0x15 ),	/* 21 */
/* 522 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 524 */	NdrFcLong( 0x16 ),	/* 22 */
/* 528 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 530 */	NdrFcLong( 0x17 ),	/* 23 */
/* 534 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 536 */	NdrFcLong( 0xe ),	/* 14 */
/* 540 */	NdrFcShort( 0xba ),	/* Offset= 186 (726) */
/* 542 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 546 */	NdrFcShort( 0xbe ),	/* Offset= 190 (736) */
/* 548 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 552 */	NdrFcShort( 0xbc ),	/* Offset= 188 (740) */
/* 554 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 558 */	NdrFcShort( 0x70 ),	/* Offset= 112 (670) */
/* 560 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 564 */	NdrFcShort( 0x6e ),	/* Offset= 110 (674) */
/* 566 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 570 */	NdrFcShort( 0x6c ),	/* Offset= 108 (678) */
/* 572 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 576 */	NdrFcShort( 0x62 ),	/* Offset= 98 (674) */
/* 578 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 582 */	NdrFcShort( 0x5c ),	/* Offset= 92 (674) */
/* 584 */	NdrFcLong( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x0 ),	/* Offset= 0 (588) */
/* 590 */	NdrFcLong( 0x1 ),	/* 1 */
/* 594 */	NdrFcShort( 0x0 ),	/* Offset= 0 (594) */
/* 596 */	NdrFcShort( 0xffff ),	/* Offset= -1 (595) */
/* 598 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 600 */	NdrFcShort( 0x8 ),	/* 8 */
/* 602 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 604 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 606 */	NdrFcShort( 0x2 ),	/* Offset= 2 (608) */
/* 608 */	
			0x12, 0x0,	/* FC_UP */
/* 610 */	NdrFcShort( 0x1b8 ),	/* Offset= 440 (1050) */
/* 612 */	
			0x12, 0x0,	/* FC_UP */
/* 614 */	NdrFcShort( 0x20 ),	/* Offset= 32 (646) */
/* 616 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 618 */	NdrFcLong( 0x2f ),	/* 47 */
/* 622 */	NdrFcShort( 0x0 ),	/* 0 */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 628 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 630 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 632 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 634 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 636 */	NdrFcShort( 0x1 ),	/* 1 */
/* 638 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 640 */	NdrFcShort( 0x4 ),	/* 4 */
/* 642 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 644 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 646 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 648 */	NdrFcShort( 0x10 ),	/* 16 */
/* 650 */	NdrFcShort( 0x0 ),	/* 0 */
/* 652 */	NdrFcShort( 0xa ),	/* Offset= 10 (662) */
/* 654 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 656 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 658 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (616) */
/* 660 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 662 */	
			0x12, 0x0,	/* FC_UP */
/* 664 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (634) */
/* 666 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 668 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 670 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 672 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 674 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 676 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 678 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 680 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 682 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 684 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 686 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 688 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 690 */	
			0x12, 0x0,	/* FC_UP */
/* 692 */	NdrFcShort( 0xffa2 ),	/* Offset= -94 (598) */
/* 694 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 696 */	NdrFcShort( 0xfd4a ),	/* Offset= -694 (2) */
/* 698 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 700 */	NdrFcShort( 0xfe12 ),	/* Offset= -494 (206) */
/* 702 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 704 */	NdrFcShort( 0xfd66 ),	/* Offset= -666 (38) */
/* 706 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 708 */	NdrFcShort( 0x2 ),	/* Offset= 2 (710) */
/* 710 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 712 */	NdrFcShort( 0x2 ),	/* Offset= 2 (714) */
/* 714 */	
			0x12, 0x0,	/* FC_UP */
/* 716 */	NdrFcShort( 0x14e ),	/* Offset= 334 (1050) */
/* 718 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 720 */	NdrFcShort( 0x2 ),	/* Offset= 2 (722) */
/* 722 */	
			0x12, 0x0,	/* FC_UP */
/* 724 */	NdrFcShort( 0x14 ),	/* Offset= 20 (744) */
/* 726 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 728 */	NdrFcShort( 0x10 ),	/* 16 */
/* 730 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 732 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 734 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 736 */	
			0x12, 0x0,	/* FC_UP */
/* 738 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (726) */
/* 740 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 742 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 744 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 746 */	NdrFcShort( 0x20 ),	/* 32 */
/* 748 */	NdrFcShort( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0x0 ),	/* Offset= 0 (750) */
/* 752 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 754 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 756 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 758 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 760 */	NdrFcShort( 0xfe34 ),	/* Offset= -460 (300) */
/* 762 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 764 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 766 */	NdrFcShort( 0x4 ),	/* 4 */
/* 768 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 772 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 774 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 776 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 778 */	NdrFcShort( 0x4 ),	/* 4 */
/* 780 */	NdrFcShort( 0x0 ),	/* 0 */
/* 782 */	NdrFcShort( 0x1 ),	/* 1 */
/* 784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	0x12, 0x0,	/* FC_UP */
/* 790 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (744) */
/* 792 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 794 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 796 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 798 */	NdrFcShort( 0x8 ),	/* 8 */
/* 800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 802 */	NdrFcShort( 0x6 ),	/* Offset= 6 (808) */
/* 804 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 806 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 808 */	
			0x11, 0x0,	/* FC_RP */
/* 810 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (764) */
/* 812 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 814 */	NdrFcShort( 0x4 ),	/* 4 */
/* 816 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 818 */	NdrFcShort( 0x0 ),	/* 0 */
/* 820 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 822 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 824 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 826 */	NdrFcShort( 0x4 ),	/* 4 */
/* 828 */	NdrFcShort( 0x0 ),	/* 0 */
/* 830 */	NdrFcShort( 0x1 ),	/* 1 */
/* 832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0x0 ),	/* 0 */
/* 836 */	0x12, 0x0,	/* FC_UP */
/* 838 */	NdrFcShort( 0xff40 ),	/* Offset= -192 (646) */
/* 840 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 842 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 844 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 846 */	NdrFcShort( 0x8 ),	/* 8 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 850 */	NdrFcShort( 0x6 ),	/* Offset= 6 (856) */
/* 852 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 854 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 856 */	
			0x11, 0x0,	/* FC_RP */
/* 858 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (812) */
/* 860 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 864 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 866 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 868 */	NdrFcShort( 0x10 ),	/* 16 */
/* 870 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 872 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 874 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (860) */
			0x5b,		/* FC_END */
/* 878 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 880 */	NdrFcShort( 0x18 ),	/* 24 */
/* 882 */	NdrFcShort( 0x0 ),	/* 0 */
/* 884 */	NdrFcShort( 0xa ),	/* Offset= 10 (894) */
/* 886 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 888 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 890 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (866) */
/* 892 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 894 */	
			0x11, 0x0,	/* FC_RP */
/* 896 */	NdrFcShort( 0xfd60 ),	/* Offset= -672 (224) */
/* 898 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 900 */	NdrFcShort( 0x1 ),	/* 1 */
/* 902 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 908 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 910 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 912 */	NdrFcShort( 0x8 ),	/* 8 */
/* 914 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 916 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 918 */	NdrFcShort( 0x4 ),	/* 4 */
/* 920 */	NdrFcShort( 0x4 ),	/* 4 */
/* 922 */	0x12, 0x0,	/* FC_UP */
/* 924 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (898) */
/* 926 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 928 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 930 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 932 */	NdrFcShort( 0x2 ),	/* 2 */
/* 934 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 936 */	NdrFcShort( 0x0 ),	/* 0 */
/* 938 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 940 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 942 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 944 */	NdrFcShort( 0x8 ),	/* 8 */
/* 946 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 948 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 950 */	NdrFcShort( 0x4 ),	/* 4 */
/* 952 */	NdrFcShort( 0x4 ),	/* 4 */
/* 954 */	0x12, 0x0,	/* FC_UP */
/* 956 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (930) */
/* 958 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 960 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 962 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 964 */	NdrFcShort( 0x4 ),	/* 4 */
/* 966 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 968 */	NdrFcShort( 0x0 ),	/* 0 */
/* 970 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 972 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 974 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 976 */	NdrFcShort( 0x8 ),	/* 8 */
/* 978 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 980 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 982 */	NdrFcShort( 0x4 ),	/* 4 */
/* 984 */	NdrFcShort( 0x4 ),	/* 4 */
/* 986 */	0x12, 0x0,	/* FC_UP */
/* 988 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (962) */
/* 990 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 992 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 994 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 996 */	NdrFcShort( 0x8 ),	/* 8 */
/* 998 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1000 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1002 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1004 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1006 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1008 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1010 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1012 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1014 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1016 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1018 */	0x12, 0x0,	/* FC_UP */
/* 1020 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (994) */
/* 1022 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1024 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1026 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1028 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1030 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1032 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1034 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1038 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1040 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1042 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 1044 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1046 */	NdrFcShort( 0xffec ),	/* Offset= -20 (1026) */
/* 1048 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1050 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1052 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1054 */	NdrFcShort( 0xffec ),	/* Offset= -20 (1034) */
/* 1056 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1056) */
/* 1058 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1060 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1062 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1064 */	NdrFcShort( 0xfc2e ),	/* Offset= -978 (86) */
/* 1066 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1068 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1070 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1072 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1074 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1076 */	NdrFcShort( 0xfc1a ),	/* Offset= -998 (78) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            LPSAFEARRAY_UserSize
            ,LPSAFEARRAY_UserMarshal
            ,LPSAFEARRAY_UserUnmarshal
            ,LPSAFEARRAY_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMainSmartComEngineCallback, ver. 0.0,
   GUID={0xDEDF458D,0x7AFB,0x495D,{0x9B,0x96,0xD9,0xAA,0xEE,0x29,0x92,0xC4}} */

#pragma code_seg(".orpc")
static const unsigned short IMainSmartComEngineCallback_FormatStringOffsetTable[] =
    {
    0,
    108,
    156,
    186,
    222,
    306,
    390,
    450
    };

static const MIDL_STUBLESS_PROXY_INFO IMainSmartComEngineCallback_ProxyInfo =
    {
    &Object_StubDesc,
    smartcom_proxy2__MIDL_ProcFormatString.Format,
    &IMainSmartComEngineCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IMainSmartComEngineCallback_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    smartcom_proxy2__MIDL_ProcFormatString.Format,
    &IMainSmartComEngineCallback_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _IMainSmartComEngineCallbackProxyVtbl = 
{
    &IMainSmartComEngineCallback_ProxyInfo,
    &IID_IMainSmartComEngineCallback,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onOrderSucceded */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onOrderFailed */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onConnect */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onDisconnect */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onAddTrade */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onSetMyTrade */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onFinishBarsHistory */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngineCallback::onTimer_1Sec */
};

const CInterfaceStubVtbl _IMainSmartComEngineCallbackStubVtbl =
{
    &IID_IMainSmartComEngineCallback,
    &IMainSmartComEngineCallback_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IMainSmartComEngine, ver. 0.0,
   GUID={0x44670410,0xF00B,0x42E6,{0xBF,0xD0,0x27,0x8D,0x54,0x26,0x2F,0xD9}} */

#pragma code_seg(".orpc")
static const unsigned short IMainSmartComEngine_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    480,
    522,
    558,
    642,
    678,
    714,
    744,
    774,
    822,
    858,
    894,
    924,
    1014,
    1044,
    1104,
    1146,
    1206,
    1242,
    1278,
    1308,
    1344
    };

static const MIDL_STUBLESS_PROXY_INFO IMainSmartComEngine_ProxyInfo =
    {
    &Object_StubDesc,
    smartcom_proxy2__MIDL_ProcFormatString.Format,
    &IMainSmartComEngine_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IMainSmartComEngine_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    smartcom_proxy2__MIDL_ProcFormatString.Format,
    &IMainSmartComEngine_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(28) _IMainSmartComEngineProxyVtbl = 
{
    &IMainSmartComEngine_ProxyInfo,
    &IID_IMainSmartComEngine,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* IDispatch::GetTypeInfoCount */ ,
    0 /* IDispatch::GetTypeInfo */ ,
    0 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::libInit */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::libDeinit */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::connect */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::subscribeForTicks */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::unsubscribeForTicks */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::querySymbols */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::disconnect */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::getPrintHistory */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::subscribeForLevel1 */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::unsubscribeForLevel1 */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::getPortfolioList */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::issueOrder */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::checkConnectStatus */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::cancelOrder */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::cancelAllOrders */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::getBarsHistory */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::subscribeForLevel2 */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::unsubscribeForLevel2 */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::getMyTradesOnce */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::subscribeGetMyTrades */ ,
    (void *) (INT_PTR) -1 /* IMainSmartComEngine::unsubscribeGetMyTrades */
};


static const PRPC_STUB_FUNCTION IMainSmartComEngine_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IMainSmartComEngineStubVtbl =
{
    &IID_IMainSmartComEngine,
    &IMainSmartComEngine_ServerInfo,
    28,
    &IMainSmartComEngine_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    smartcom_proxy2__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x8000253, /* MIDL Version 8.0.595 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * const _smartcom_proxy2_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IMainSmartComEngineProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IMainSmartComEngineCallbackProxyVtbl,
    0
};

const CInterfaceStubVtbl * const _smartcom_proxy2_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IMainSmartComEngineStubVtbl,
    ( CInterfaceStubVtbl *) &_IMainSmartComEngineCallbackStubVtbl,
    0
};

PCInterfaceName const _smartcom_proxy2_InterfaceNamesList[] = 
{
    "IMainSmartComEngine",
    "IMainSmartComEngineCallback",
    0
};

const IID *  const _smartcom_proxy2_BaseIIDList[] = 
{
    &IID_IDispatch,
    0,
    0
};


#define _smartcom_proxy2_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _smartcom_proxy2, pIID, n)

int __stdcall _smartcom_proxy2_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _smartcom_proxy2, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _smartcom_proxy2, 2, *pIndex )
    
}

const ExtendedProxyFileInfo smartcom_proxy2_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _smartcom_proxy2_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _smartcom_proxy2_StubVtblList,
    (const PCInterfaceName * ) & _smartcom_proxy2_InterfaceNamesList,
    (const IID ** ) & _smartcom_proxy2_BaseIIDList,
    & _smartcom_proxy2_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

