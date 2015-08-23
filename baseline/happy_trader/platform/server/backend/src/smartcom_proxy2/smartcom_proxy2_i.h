

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#ifndef __smartcom_proxy2_i_h__
#define __smartcom_proxy2_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMainSmartComEngineCallback_FWD_DEFINED__
#define __IMainSmartComEngineCallback_FWD_DEFINED__
typedef interface IMainSmartComEngineCallback IMainSmartComEngineCallback;

#endif 	/* __IMainSmartComEngineCallback_FWD_DEFINED__ */


#ifndef __IMainSmartComEngine_FWD_DEFINED__
#define __IMainSmartComEngine_FWD_DEFINED__
typedef interface IMainSmartComEngine IMainSmartComEngine;

#endif 	/* __IMainSmartComEngine_FWD_DEFINED__ */


#ifndef __MainSmartComEngine_FWD_DEFINED__
#define __MainSmartComEngine_FWD_DEFINED__

#ifdef __cplusplus
typedef class MainSmartComEngine MainSmartComEngine;
#else
typedef struct MainSmartComEngine MainSmartComEngine;
#endif /* __cplusplus */

#endif 	/* __MainSmartComEngine_FWD_DEFINED__ */


#ifndef __MainSmartComEngineCallback_FWD_DEFINED__
#define __MainSmartComEngineCallback_FWD_DEFINED__

#ifdef __cplusplus
typedef class MainSmartComEngineCallback MainSmartComEngineCallback;
#else
typedef struct MainSmartComEngineCallback MainSmartComEngineCallback;
#endif /* __cplusplus */

#endif 	/* __MainSmartComEngineCallback_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IMainSmartComEngineCallback_INTERFACE_DEFINED__
#define __IMainSmartComEngineCallback_INTERFACE_DEFINED__

/* interface IMainSmartComEngineCallback */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IMainSmartComEngineCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DEDF458D-7AFB-495D-9B96-D9AAEE2992C4")
    IMainSmartComEngineCallback : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onOrderSucceded( 
            /* [in] */ long cookie,
            /* [in] */ BSTR symbol,
            /* [in] */ long state,
            /* [in] */ long action,
            /* [in] */ long type,
            /* [in] */ long validity,
            /* [in] */ double price,
            /* [in] */ double amount,
            /* [in] */ double stop,
            /* [in] */ double filled,
            /* [in] */ double datetime,
            /* [in] */ BSTR orderid,
            /* [in] */ BSTR orderno) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onOrderFailed( 
            /* [in] */ long cookie,
            /* [in] */ BSTR orderid,
            /* [in] */ BSTR reason) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onConnect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onDisconnect( 
            /* [in] */ BSTR reason) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onAddTrade( 
            /* [in] */ long cookie,
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ BSTR orderno,
            /* [in] */ BSTR orderid,
            /* [in] */ double price,
            /* [in] */ double amount,
            /* [in] */ double datetime,
            /* [in] */ BSTR tradeno) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onSetMyTrade( 
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ double datetime,
            /* [in] */ double price,
            /* [in] */ double volume,
            /* [in] */ BSTR tradeno,
            /* [in] */ long buysell,
            /* [in] */ BSTR orderno,
            /* [in] */ BSTR orderid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onFinishBarsHistory( 
            /* [in] */ int cookie,
            /* [in] */ BSTR symbol,
            /* [in] */ VARIANT_BOOL result,
            /* [in] */ long count,
            /* [in] */ IDispatch *eventClientPtr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE onTimer_1Sec( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMainSmartComEngineCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMainSmartComEngineCallback * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMainSmartComEngineCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMainSmartComEngineCallback * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onOrderSucceded )( 
            IMainSmartComEngineCallback * This,
            /* [in] */ long cookie,
            /* [in] */ BSTR symbol,
            /* [in] */ long state,
            /* [in] */ long action,
            /* [in] */ long type,
            /* [in] */ long validity,
            /* [in] */ double price,
            /* [in] */ double amount,
            /* [in] */ double stop,
            /* [in] */ double filled,
            /* [in] */ double datetime,
            /* [in] */ BSTR orderid,
            /* [in] */ BSTR orderno);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onOrderFailed )( 
            IMainSmartComEngineCallback * This,
            /* [in] */ long cookie,
            /* [in] */ BSTR orderid,
            /* [in] */ BSTR reason);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onConnect )( 
            IMainSmartComEngineCallback * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onDisconnect )( 
            IMainSmartComEngineCallback * This,
            /* [in] */ BSTR reason);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onAddTrade )( 
            IMainSmartComEngineCallback * This,
            /* [in] */ long cookie,
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ BSTR orderno,
            /* [in] */ BSTR orderid,
            /* [in] */ double price,
            /* [in] */ double amount,
            /* [in] */ double datetime,
            /* [in] */ BSTR tradeno);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onSetMyTrade )( 
            IMainSmartComEngineCallback * This,
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ double datetime,
            /* [in] */ double price,
            /* [in] */ double volume,
            /* [in] */ BSTR tradeno,
            /* [in] */ long buysell,
            /* [in] */ BSTR orderno,
            /* [in] */ BSTR orderid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onFinishBarsHistory )( 
            IMainSmartComEngineCallback * This,
            /* [in] */ int cookie,
            /* [in] */ BSTR symbol,
            /* [in] */ VARIANT_BOOL result,
            /* [in] */ long count,
            /* [in] */ IDispatch *eventClientPtr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *onTimer_1Sec )( 
            IMainSmartComEngineCallback * This);
        
        END_INTERFACE
    } IMainSmartComEngineCallbackVtbl;

    interface IMainSmartComEngineCallback
    {
        CONST_VTBL struct IMainSmartComEngineCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMainSmartComEngineCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMainSmartComEngineCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMainSmartComEngineCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMainSmartComEngineCallback_onOrderSucceded(This,cookie,symbol,state,action,type,validity,price,amount,stop,filled,datetime,orderid,orderno)	\
    ( (This)->lpVtbl -> onOrderSucceded(This,cookie,symbol,state,action,type,validity,price,amount,stop,filled,datetime,orderid,orderno) ) 

#define IMainSmartComEngineCallback_onOrderFailed(This,cookie,orderid,reason)	\
    ( (This)->lpVtbl -> onOrderFailed(This,cookie,orderid,reason) ) 

#define IMainSmartComEngineCallback_onConnect(This)	\
    ( (This)->lpVtbl -> onConnect(This) ) 

#define IMainSmartComEngineCallback_onDisconnect(This,reason)	\
    ( (This)->lpVtbl -> onDisconnect(This,reason) ) 

#define IMainSmartComEngineCallback_onAddTrade(This,cookie,portfolio,symbol,orderno,orderid,price,amount,datetime,tradeno)	\
    ( (This)->lpVtbl -> onAddTrade(This,cookie,portfolio,symbol,orderno,orderid,price,amount,datetime,tradeno) ) 

#define IMainSmartComEngineCallback_onSetMyTrade(This,portfolio,symbol,datetime,price,volume,tradeno,buysell,orderno,orderid)	\
    ( (This)->lpVtbl -> onSetMyTrade(This,portfolio,symbol,datetime,price,volume,tradeno,buysell,orderno,orderid) ) 

#define IMainSmartComEngineCallback_onFinishBarsHistory(This,cookie,symbol,result,count,eventClientPtr)	\
    ( (This)->lpVtbl -> onFinishBarsHistory(This,cookie,symbol,result,count,eventClientPtr) ) 

#define IMainSmartComEngineCallback_onTimer_1Sec(This)	\
    ( (This)->lpVtbl -> onTimer_1Sec(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMainSmartComEngineCallback_INTERFACE_DEFINED__ */


#ifndef __IMainSmartComEngine_INTERFACE_DEFINED__
#define __IMainSmartComEngine_INTERFACE_DEFINED__

/* interface IMainSmartComEngine */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IMainSmartComEngine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("44670410-F00B-42E6-BFD0-278D54262FD9")
    IMainSmartComEngine : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE libInit( 
            /* [in] */ IMainSmartComEngineCallback *eventClient,
            /* [out] */ long *cookie) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE libDeinit( 
            /* [in] */ long cookie) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE connect( 
            /* [in] */ BSTR rtProvider,
            /* [in] */ BSTR rtProviderBarPostfix,
            /* [in] */ VARIANT_BOOL accompanyTicks1minBar,
            /* [in] */ BSTR host,
            /* [in] */ SHORT port,
            /* [in] */ BSTR login,
            /* [in] */ BSTR password,
            /* [in] */ SAFEARRAY * portfolioList,
            /* [in] */ long loglevel) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE subscribeForTicks( 
            /* [in] */ SAFEARRAY * symbols) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE unsubscribeForTicks( 
            /* [in] */ SAFEARRAY * symbols) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE querySymbols( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE disconnect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getPrintHistory( 
            /* [in] */ SAFEARRAY * symbols,
            /* [in] */ DATE from_date,
            /* [in] */ long from_count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE subscribeForLevel1( 
            /* [in] */ SAFEARRAY * symbols) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE unsubscribeForLevel1( 
            /* [in] */ SAFEARRAY * symbols) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getPortfolioList( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE issueOrder( 
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ int order_action,
            /* [in] */ int order_type,
            /* [in] */ int validity,
            /* [in] */ double price,
            /* [in] */ double amount,
            /* [in] */ double stop,
            /* [in] */ long cookie,
            /* [in] */ IMainSmartComEngineCallback *eventClient) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE checkConnectStatus( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE cancelOrder( 
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ BSTR orderId,
            /* [in] */ long cookie,
            /* [in] */ IMainSmartComEngineCallback *eventClient) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE cancelAllOrders( 
            /* [in] */ long cookie,
            /* [in] */ IMainSmartComEngineCallback *eventClient) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getBarsHistory( 
            /* [in] */ BSTR symbol,
            /* [in] */ int interval,
            /* [in] */ DATE from_date,
            /* [in] */ long from_count,
            /* [in] */ int cookie) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE subscribeForLevel2( 
            /* [in] */ SAFEARRAY * symbols) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE unsubscribeForLevel2( 
            /* [in] */ SAFEARRAY * symbols) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE getMyTradesOnce( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE subscribeGetMyTrades( 
            /* [in] */ long pollPeriodSec) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE unsubscribeGetMyTrades( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMainSmartComEngineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMainSmartComEngine * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMainSmartComEngine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMainSmartComEngine * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMainSmartComEngine * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMainSmartComEngine * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMainSmartComEngine * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMainSmartComEngine * This,
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
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *libInit )( 
            IMainSmartComEngine * This,
            /* [in] */ IMainSmartComEngineCallback *eventClient,
            /* [out] */ long *cookie);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *libDeinit )( 
            IMainSmartComEngine * This,
            /* [in] */ long cookie);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *connect )( 
            IMainSmartComEngine * This,
            /* [in] */ BSTR rtProvider,
            /* [in] */ BSTR rtProviderBarPostfix,
            /* [in] */ VARIANT_BOOL accompanyTicks1minBar,
            /* [in] */ BSTR host,
            /* [in] */ SHORT port,
            /* [in] */ BSTR login,
            /* [in] */ BSTR password,
            /* [in] */ SAFEARRAY * portfolioList,
            /* [in] */ long loglevel);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *subscribeForTicks )( 
            IMainSmartComEngine * This,
            /* [in] */ SAFEARRAY * symbols);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *unsubscribeForTicks )( 
            IMainSmartComEngine * This,
            /* [in] */ SAFEARRAY * symbols);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *querySymbols )( 
            IMainSmartComEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *disconnect )( 
            IMainSmartComEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getPrintHistory )( 
            IMainSmartComEngine * This,
            /* [in] */ SAFEARRAY * symbols,
            /* [in] */ DATE from_date,
            /* [in] */ long from_count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *subscribeForLevel1 )( 
            IMainSmartComEngine * This,
            /* [in] */ SAFEARRAY * symbols);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *unsubscribeForLevel1 )( 
            IMainSmartComEngine * This,
            /* [in] */ SAFEARRAY * symbols);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getPortfolioList )( 
            IMainSmartComEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *issueOrder )( 
            IMainSmartComEngine * This,
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ int order_action,
            /* [in] */ int order_type,
            /* [in] */ int validity,
            /* [in] */ double price,
            /* [in] */ double amount,
            /* [in] */ double stop,
            /* [in] */ long cookie,
            /* [in] */ IMainSmartComEngineCallback *eventClient);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *checkConnectStatus )( 
            IMainSmartComEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cancelOrder )( 
            IMainSmartComEngine * This,
            /* [in] */ BSTR portfolio,
            /* [in] */ BSTR symbol,
            /* [in] */ BSTR orderId,
            /* [in] */ long cookie,
            /* [in] */ IMainSmartComEngineCallback *eventClient);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *cancelAllOrders )( 
            IMainSmartComEngine * This,
            /* [in] */ long cookie,
            /* [in] */ IMainSmartComEngineCallback *eventClient);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getBarsHistory )( 
            IMainSmartComEngine * This,
            /* [in] */ BSTR symbol,
            /* [in] */ int interval,
            /* [in] */ DATE from_date,
            /* [in] */ long from_count,
            /* [in] */ int cookie);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *subscribeForLevel2 )( 
            IMainSmartComEngine * This,
            /* [in] */ SAFEARRAY * symbols);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *unsubscribeForLevel2 )( 
            IMainSmartComEngine * This,
            /* [in] */ SAFEARRAY * symbols);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *getMyTradesOnce )( 
            IMainSmartComEngine * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *subscribeGetMyTrades )( 
            IMainSmartComEngine * This,
            /* [in] */ long pollPeriodSec);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *unsubscribeGetMyTrades )( 
            IMainSmartComEngine * This);
        
        END_INTERFACE
    } IMainSmartComEngineVtbl;

    interface IMainSmartComEngine
    {
        CONST_VTBL struct IMainSmartComEngineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMainSmartComEngine_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMainSmartComEngine_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMainSmartComEngine_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMainSmartComEngine_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMainSmartComEngine_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMainSmartComEngine_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMainSmartComEngine_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMainSmartComEngine_libInit(This,eventClient,cookie)	\
    ( (This)->lpVtbl -> libInit(This,eventClient,cookie) ) 

#define IMainSmartComEngine_libDeinit(This,cookie)	\
    ( (This)->lpVtbl -> libDeinit(This,cookie) ) 

#define IMainSmartComEngine_connect(This,rtProvider,rtProviderBarPostfix,accompanyTicks1minBar,host,port,login,password,portfolioList,loglevel)	\
    ( (This)->lpVtbl -> connect(This,rtProvider,rtProviderBarPostfix,accompanyTicks1minBar,host,port,login,password,portfolioList,loglevel) ) 

#define IMainSmartComEngine_subscribeForTicks(This,symbols)	\
    ( (This)->lpVtbl -> subscribeForTicks(This,symbols) ) 

#define IMainSmartComEngine_unsubscribeForTicks(This,symbols)	\
    ( (This)->lpVtbl -> unsubscribeForTicks(This,symbols) ) 

#define IMainSmartComEngine_querySymbols(This)	\
    ( (This)->lpVtbl -> querySymbols(This) ) 

#define IMainSmartComEngine_disconnect(This)	\
    ( (This)->lpVtbl -> disconnect(This) ) 

#define IMainSmartComEngine_getPrintHistory(This,symbols,from_date,from_count)	\
    ( (This)->lpVtbl -> getPrintHistory(This,symbols,from_date,from_count) ) 

#define IMainSmartComEngine_subscribeForLevel1(This,symbols)	\
    ( (This)->lpVtbl -> subscribeForLevel1(This,symbols) ) 

#define IMainSmartComEngine_unsubscribeForLevel1(This,symbols)	\
    ( (This)->lpVtbl -> unsubscribeForLevel1(This,symbols) ) 

#define IMainSmartComEngine_getPortfolioList(This)	\
    ( (This)->lpVtbl -> getPortfolioList(This) ) 

#define IMainSmartComEngine_issueOrder(This,portfolio,symbol,order_action,order_type,validity,price,amount,stop,cookie,eventClient)	\
    ( (This)->lpVtbl -> issueOrder(This,portfolio,symbol,order_action,order_type,validity,price,amount,stop,cookie,eventClient) ) 

#define IMainSmartComEngine_checkConnectStatus(This)	\
    ( (This)->lpVtbl -> checkConnectStatus(This) ) 

#define IMainSmartComEngine_cancelOrder(This,portfolio,symbol,orderId,cookie,eventClient)	\
    ( (This)->lpVtbl -> cancelOrder(This,portfolio,symbol,orderId,cookie,eventClient) ) 

#define IMainSmartComEngine_cancelAllOrders(This,cookie,eventClient)	\
    ( (This)->lpVtbl -> cancelAllOrders(This,cookie,eventClient) ) 

#define IMainSmartComEngine_getBarsHistory(This,symbol,interval,from_date,from_count,cookie)	\
    ( (This)->lpVtbl -> getBarsHistory(This,symbol,interval,from_date,from_count,cookie) ) 

#define IMainSmartComEngine_subscribeForLevel2(This,symbols)	\
    ( (This)->lpVtbl -> subscribeForLevel2(This,symbols) ) 

#define IMainSmartComEngine_unsubscribeForLevel2(This,symbols)	\
    ( (This)->lpVtbl -> unsubscribeForLevel2(This,symbols) ) 

#define IMainSmartComEngine_getMyTradesOnce(This)	\
    ( (This)->lpVtbl -> getMyTradesOnce(This) ) 

#define IMainSmartComEngine_subscribeGetMyTrades(This,pollPeriodSec)	\
    ( (This)->lpVtbl -> subscribeGetMyTrades(This,pollPeriodSec) ) 

#define IMainSmartComEngine_unsubscribeGetMyTrades(This)	\
    ( (This)->lpVtbl -> unsubscribeGetMyTrades(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMainSmartComEngine_INTERFACE_DEFINED__ */



#ifndef __smartcom_proxy2Lib_LIBRARY_DEFINED__
#define __smartcom_proxy2Lib_LIBRARY_DEFINED__

/* library smartcom_proxy2Lib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_smartcom_proxy2Lib;

EXTERN_C const CLSID CLSID_MainSmartComEngine;

#ifdef __cplusplus

class DECLSPEC_UUID("149C2130-0832-47D4-A0D8-EE141D31C6B9")
MainSmartComEngine;
#endif

EXTERN_C const CLSID CLSID_MainSmartComEngineCallback;

#ifdef __cplusplus

class DECLSPEC_UUID("ED10803B-9E22-4C0C-B859-52C6E862BE41")
MainSmartComEngineCallback;
#endif
#endif /* __smartcom_proxy2Lib_LIBRARY_DEFINED__ */

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


