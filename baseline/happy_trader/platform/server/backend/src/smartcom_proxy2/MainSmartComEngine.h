// MainSmartComEngine.h : Declaration of the CMainSmartComEngine

#ifndef _SMARTCOM_PROXY2MAINSMARTCOMENGINE_INCLUDED
#define _SMARTCOM_PROXY2MAINSMARTCOMENGINE_INCLUDED

#include "resource.h"       // main symbols

#include "MainDlg.h"

#include "smartcom_proxy2defs.h"

#include "smartcom_proxy2_i.h"

#include <atlwin.h>
#include <atlstr.h>
#include "helperstruct.h"
#include "simulator.hpp"

namespace SmartComHelperLib {

		DEFINE_ENUM_ENTRY(N_OK,			0 )
		DEFINE_ENUM_ENTRY(N_ERROR,	1 )


#define NOTIFICATION(CODE, STR, ARG) \
	{	\
	ostrstream sstr_a; sstr_a << ARG << ends; \
	ostrstream sstr_s; sstr_s << STR << ends; \
	notification(CODE, sstr_s.str(), sstr_a.str() );	\
	sstr_s.freeze( false ); \
	sstr_a.freeze( false );		\
	}

#define NOTIFICATION1(CODE, STR) \
	{													 \
	ostrstream sstr_s; sstr_s << STR << ends; \
	notification(CODE, sstr_s.str() );	\
	sstr_s.freeze( false );		\
	}
	

struct Level2EntryExtended {
	Level2EntryExtended(bool const isBest, float const& volume, float const& price, bool const self, HlpStruct::Level2Entry::Type const entryType ):
		isBest_m(isBest),
		entry_m(volume, price, self, entryType)
	{
	}

	HlpStruct::Level2Entry entry_m;
	bool isBest_m;
};

struct Level2Data
{
	HlpStruct::RtLevel2Data level2_m;
	multimap<double, Level2EntryExtended> levelSorted_m;
};


struct DataPropagationContext
{
	DataPropagationContext():
		directPropagation_m( false ),
		cookie_m(-1)
	{
	}


	CppUtils::String symbol_m;

	map<double, Candle> delayedHistory_m;

	bool directPropagation_m;

	// task number to distinguish between same symbols
	int cookie_m;
};


#define IMYINTRFID 1

		// CMainSmartComEngine

	class ATL_NO_VTABLE CMainSmartComEngine :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CMainSmartComEngine, &CLSID_MainSmartComEngine>,
		public IDispatchImpl<IMainSmartComEngine, &IID_IMainSmartComEngine, &LIBID_smartcom_proxy2Lib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
		public IDispatchImpl<_IStClient, &DIID__IStClient, &LIBID_StClientLib, /* wMajor = */ 1>,
		//public IDispEventSimpleImpl<IMYINTRFID, CMainSmartComEngine, &DIID__IStClient>,
		public IDispEventImpl<IMYINTRFID, CMainSmartComEngine, &DIID__IStClient, &LIBID_StClientLib, 1, 0>,
		private CppUtils::Mutex,
		public CppUtils::RefCounted

	{
		friend class CMainDlg;				  	  
	public:

		static CppUtils::String getRootObjectName()
		{
			static CppUtils::String names("main_smartcom_engine");
			return names;
		}




		CMainSmartComEngine();
		virtual ~CMainSmartComEngine();


		DECLARE_CLASSFACTORY_SINGLETON(CMainSmartComEngine) 
		DECLARE_REGISTRY_RESOURCEID(IDR_MAINSMARTCOMENGINE)
		DECLARE_NOT_AGGREGATABLE(CMainSmartComEngine)


		BEGIN_COM_MAP(CMainSmartComEngine)
			//COM_INTERFACE_ENTRY2(IDispatch, _IStClient)
			COM_INTERFACE_ENTRY(IMainSmartComEngine)
			COM_INTERFACE_ENTRY_IID(CLSID_MainSmartComEngine, CMainSmartComEngine)
		END_COM_MAP()



		DECLARE_PROTECT_FINAL_CONSTRUCT()


		HRESULT FinalConstruct(){		return S_OK;	}
		void FinalRelease()	{	}

		
		
		BEGIN_SINK_MAP( CMainSmartComEngine )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x1,	Connected )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x2,  Disconnected )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x3,  UpdateQuote )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x4,  UpdateBidAsk )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x5,  AddTick )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0xf,  AddTickHistory )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x6,  AddBar )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x7,  SetPortfolio )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x8,  AddTrade )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x9,  UpdateOrder )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0xa,  UpdatePosition )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x14,  AddSymbol )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x1e,  OrderSucceeded )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x1f,  OrderFailed )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x20,  AddPortfolio )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x21,  SetSubscribtionCheckReult )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x22,  SetMyTrade )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x23,  SetMyOrder )  
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x25,  SetMyClosePos )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x26,  OrderCancelSucceeded )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x27,  OrderCancelFailed )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x28,  OrderMoveSucceeded )
			SINK_ENTRY_EX( IMYINTRFID, DIID__IStClient, 0x29,  OrderMoveFailed )
		END_SINK_MAP()
		

	public:


		// _IStClient Methods
	public:


		
		STDMETHOD (Connected)();
		STDMETHOD(Disconnected)(BSTR reason);
		STDMETHOD(UpdateQuote)( 
														BSTR symbol, 
                            DATE datetime, 
                            double open, 
                            double high, 
                            double low, 
                            double close, 
                            double last, 
                            double volume, 
                            double size, 
                            double bid, 
                            double ask, 
                            double bidsize, 
                            double asksize, 
                            double open_int, 
                            double go_buy, 
                            double go_sell, 
                            double go_base, 
                            double go_base_backed, 
                            double high_limit, 
                            double low_limit, 
                            long trading_status, 
                            double volat, 
                            double theor_price
										);
		STDMETHOD(UpdateBidAsk)(BSTR symbol, 
                            long row, 
                            long nrows, 
                            double bid, 
                            double bidsize, 
                            double ask, 
                            double asksize);

		STDMETHOD(AddTick)( BSTR symbol, 
                        DATE datetime, 
                        double price, 
                        double volume, 
                        BSTR tradeno, 
                        StOrder_Action action);

		STDMETHOD(AddTickHistory)( long row, 
                            long nrows, 
                            BSTR symbol, 
                            DATE datetime, 
                            double price, 
                            double volume, 
                            BSTR tradeno, 
                            StOrder_Action action
								);
		STDMETHOD(AddBar)( long row, 
                            long nrows, 
                            BSTR symbol, 
                            StBarInterval interval, 
                            DATE datetime, 
                            double open, 
                            double high, 
                            double low, 
                            double close, 
                            double volume, 
                            double open_int
												);
		STDMETHOD(SetPortfolio)(  BSTR portfolio, 
                             double cash, 
                             double leverage, 
                             double comission, 
                             double saldo);

		STDMETHOD(AddTrade)( BSTR portfolio, 
                             BSTR symbol, 
                             BSTR orderNo, 
                             double price, 
                             double amount, 
                             DATE datetime, 
                             BSTR tradeno
													);
		STDMETHOD(UpdateOrder)( BSTR portfolio, 
                             BSTR symbol, 
                             StOrder_State state, 
                             StOrder_Action action, 
                             StOrder_Type type, 
                             StOrder_Validity validity, 
                             double price, 
                             double amount, 
                             double stop, 
                             double filled, 
                             DATE datetime, 
                             BSTR orderid, 
                             BSTR orderno, 
                             long status_mask, 
                             long cookie
											);
		STDMETHOD(UpdatePosition)(  BSTR portfolio, 
                             BSTR symbol, 
                             double avprice, 
                             double amount, 
                             double planned
													);
		STDMETHOD(AddSymbol)( long row, 
                             long nrows, 
                             BSTR symbol, 
                             BSTR short_name, 
                             BSTR long_name, 
                             BSTR type, 
                             long decimals, 
                             long lot_size, 
                             double punkt, 
                             double step, 
                             BSTR sec_ext_id, 
                             BSTR sec_exch_name, 
                             DATE expiry_date, 
                             double days_before_expiry, 
                             double strike
									);
		STDMETHOD(OrderSucceeded)(long cookie, BSTR orderid);
		STDMETHOD(OrderFailed)(long cookie, BSTR orderid, BSTR reason);

		STDMETHOD(AddPortfolio)(  long row, 
                             long nrows, 
                             BSTR portfolioName, 
                             BSTR portfolioExch, 
                             StPortfolioStatus portfolioStatus
									);

		STDMETHOD(SetSubscribtionCheckReult)(long result);

		STDMETHOD(SetMyTrade)(  long row, 
                             long nrows, 
                             BSTR portfolio, 
                             BSTR symbol, 
                             DATE datetime, 
                             double price, 
                             double volume, 
                             BSTR tradeno, 
                             StOrder_Action buysell, 
                             BSTR orderno
								);

		STDMETHOD(SetMyOrder)( long row, 
                             long nrows, 
                             BSTR portfolio, 
                             BSTR symbol, 
                             StOrder_State state, 
                             StOrder_Action action, 
                             StOrder_Type type, 
                             StOrder_Validity validity, 
                             double price, 
                             double amount, 
                             double stop, 
                             double filled, 
                             DATE datetime, 
                             BSTR id, 
                             BSTR no, 
                             long cookie
										);
		STDMETHOD(SetMyClosePos)(  long row, 
                             long nrows, 
                             BSTR portfolio, 
                             BSTR symbol, 
                             double amount, 
                             double price_buy, 
                             double price_sell, 
                             DATE postime, 
                             BSTR order_open, 
                             BSTR order_close
							);
		STDMETHOD(OrderCancelSucceeded)(BSTR orderid);
		STDMETHOD(OrderCancelFailed)(BSTR orderid);
		STDMETHOD(OrderMoveSucceeded)(BSTR orderid);
		STDMETHOD(OrderMoveFailed)(BSTR orderid);
		



	public:

		void threadRun();

		//static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		int const& getRefCount() const
		{
			LOCK_FOR_SCOPE(*this);

			return refCount_m;
		}

		int& getRefCount()
		{
			LOCK_FOR_SCOPE(*this);

			return refCount_m;
		}



	private:

		CppUtils::Thread* thread_m;

		int threadId_m;

		// server class pointer
		IStServerPtr IStServerPtr_m;

		// to change pointers between apts
		CComPtr<IGlobalInterfaceTable> globalGIT_m;

		DWORD IStServerPtrCookie_m;

		int refCount_m;

		CppUtils::Event initialized_m;

		CppUtils::Event disconnectEvent_m;

		CMainDlg dlg_m;

	public:
		// this class interface pointers
		// must be secured by mtx as expected to be called from free thread apt
		STDMETHOD ( libInit) (IMainSmartComEngineCallback* eventClient, long* cookie);

		STDMETHOD (libDeinit)(long cookie); 

		STDMETHOD ( connect )( 
			BSTR rtProvider, 
			BSTR rtProviderBarPostfix,  
			VARIANT_BOOL accompanyTicks1minBar, 
			BSTR host, 
			SHORT port, 
			BSTR login, 
			BSTR password,
			SAFEARRAY * portfolioList,
			long loglevel
		);

		STDMETHOD (  disconnect )(); 

		STDMETHOD ( subscribeForTicks )(SAFEARRAY * symbols);

		STDMETHOD ( unsubscribeForTicks )(SAFEARRAY * symbols);

		STDMETHOD (  querySymbols )(); 

		STDMETHOD (  getPrintHistory )( 
			SAFEARRAY * symbols,
			DATE from_date,
			long from_count
			);

		STDMETHOD ( subscribeForLevel1)( SAFEARRAY * symbols);

		STDMETHOD ( unsubscribeForLevel1) ( SAFEARRAY * symbols);

		STDMETHOD ( subscribeForLevel2)( SAFEARRAY * symbols);

		STDMETHOD ( unsubscribeForLevel2) ( SAFEARRAY * symbols);

		STDMETHOD ( getPortfolioList ) ( );


		STDMETHOD (issueOrder)( 
			BSTR portfolio, 
			BSTR symbol,
			int order_action,
			int order_type,
			int validity,
			double price,
			double amount,
			double stop,
			long cookie,
			IMainSmartComEngineCallback* eventClient
		);

		 STDMETHOD (checkConnectStatus) ();

		 STDMETHOD (cancelOrder)( BSTR portfolio, BSTR symbol, BSTR orderId, long cookie, IMainSmartComEngineCallback* eventClient);
		 
		 STDMETHOD (cancelAllOrders)(long cookie, IMainSmartComEngineCallback* eventClient);
		  
		 // just return bars history - i.e AddBar will be called consequently
		 STDMETHOD (getBarsHistory)( 
            BSTR symbol,
						int interval,
            DATE from_date,
            long from_count,
						int cookie
			);

		STDMETHOD (getMyTradesOnce)();

		STDMETHOD	(subscribeGetMyTrades)( long pollPeriodSec);

		STDMETHOD	(unsubscribeGetMyTrades)();

	public:
		// these are functions not declared as com but used in GUI so thread safe
		void getSubscribedSymbols(CppUtils::StringSet& level1Symbols, CppUtils::StringSet& ticksSymbols, CppUtils::StringSet& level2Symbols);

		void freeHistoryBarRetrievalTasksForcibly();

		void onTimer_1_sec();

		void generate_Simulation_Bars_OnStart();
		
		void generate_Simulation_Bars_OnFinish();
				
		void generate_Simulation_Bars_1Min();

	private:

		// resolve marhalled pointer
		CComPtr<IStServer> getMarhshalledPointer_IStServer();


		static int smartComActionToNativeOpType(int const& opType);

		
		static CppUtils::String tradeAction2String(int const action);

		static CppUtils::String tradeType2String(int const otype);

		static CppUtils::String orderValidity2String(int const validity);

		static CppUtils::String orderState2String(int const state);
		
		static CppUtils::String bstrToString(BSTR value, int const encoding = -1);

		// these are for get bars
		static CppUtils::String createGetBarFunctorKey(BSTR symbol, long interval);

		static double prepareGetBarTime(long interval, DATE datetime);


		
		//
		static long subscrCounter_m;


		// makes notification to upper layer
		void notification(CppUtils::EnumerationHelper const& code, char const* str, char const* arg = NULL );

		void notification(CppUtils::EnumerationHelper const& code, CppUtils::String const& str, CppUtils::String const& arg );

		void initialize();

		void deinitialize();
public:
		void silentlyDisconnectBeforeStartEnd();
private:

		void onConnected();

		void onDisconnected();

		bool checkLayerInitialized();

		bool checkLayerConnected();

		bool checkPassedPortfolio(BSTR portfolio);

		void registerInternalResponse(CppUtils::String const& orderId_s, InternalOrder const& orderUpdateStruct);

		void registerInternalTrade(CppUtils::String const& orderNo_s, InternalTrade const& tradeStruct);

		void registerResponseCookie(CppUtils::String const& orderId_s, long const cookie);

		void registerOrderNoOrderIdMap(CppUtils::String const& orderNo_s, CppUtils::String const& orderId_s);

		// this is called whenever OrderSucceded || OrderFailed || UpdateOrder executed
		void generateOrderSucceded(CppUtils::String const& orderId_s);

		void generateAddTrade(CppUtils::String const& orderNo_s);

		// ---
		void registerHistoryBarRetrievalTask(BSTR symbol, long const interval, int const cookie);
		
		// returns true if we need to pass this bar
		int freeHistoryBarRetrievalTask(long const cbars, int const pending_task_count, BSTR symbol, long interval);

		void freeHistoryBarRetrievalTasksForcibly(BSTR symbol, long interval);

		int getHistoryBarRetrievalTaskNumber(BSTR symbol, long interval);

		// -------------------------------------


		void pushOHCLToCacheOrPropagate(
			CppUtils::String const& symbol, 
			double const& datetime, 
			double const& open, 
			double const& high, 
			double const& low, 
			double const& close, 
			double const& volume,
			double const& open_int
		);

		void releaseCachedRetrivalAndSend(CppUtils::String const& symbol);

		void releaseCachedRetrivalAndSendAllCachedSymbols();
		
		void acquireCachedRetrival(CppUtils::String const& symbol, int const cookie);

		void saveDelayedHistoryToFile(map<double, Candle> const& delayedHistory, CppUtils::String const& symbol,int const cookie);

		void saveNothing(CppUtils::String const& symbol,int const cookie);
		
		// -------------------------------------
		


		// tick RT provider name
		CppUtils::String rtProvider_m;

		// bars RT provider name
		CppUtils::String rtProviderBars_m;

		bool accompanyTicks1minBar_m;

		// this is the map of sequences to perform operations after tick time changes it's value
		map<CppUtils::String, CppUtils::UnsignedLongLong> symbolSequenceNum_m;


		// map holding all calbacks
		map<long, IMainSmartComEngineCallback*> callbacks_m;

		CppUtils::Mutex mtxCallbacks_m;

		// this holds order order sources via cookies
		map<long,  IMainSmartComEngineCallback*> orderSources_m;

		CppUtils::Mutex mtxOrderSources_m;

		// flags
		bool subscribedForPortfolio_m;

		//CppUtils::String activePortfolio_m;

		CppUtils::StringList activePortfolios_m;

		int subscribedForGetMyTradesPollSec_m;

		CppUtils::Mutex subscribedForGetMyTradesMtx_m; 

		int subscribedForGetMyTradesCurSec_m;

		bool connected_m;

		// these are subscribed symbols as ticks and level1
		CppUtils::StringSet subscribedSymbolsAsTicks_m;

		CppUtils::StringSet subscribedSymbolsAsLevel1_m;

		CppUtils::StringSet subscribedSymbolsAsLevel2_m;

		

		CppUtils::Mutex sybscribedSymbolsMtx_m;

		// task for get history bars
		map<CppUtils::String, int> getBarHistoryPendingTask_m;
			
		CppUtils::Mutex getBarHistoryPendingTaskMtx_m;


		// these are arrived reposponses on sent orders
		map<CppUtils::String, InternalOrderList> arrivedOrders_m;

		// map of arrive trades via order id
		map<CppUtils::String, InternalTradesList> arrivedTrades_OrderNos_m;

		// this is filled when order succeeded or failed = when we know cookie
		map<CppUtils::String, long> orderCookie_m;

		CppUtils::StringMap orderIdOrderNoMap_m;


		// LEVEL2
		map<CppUtils::String, Level2Data> level2data_m;


		// for sending together rt history and bar history
		map<CppUtils::String, DataPropagationContext > historyContext_m;

		CppUtils::Mutex historyContextMtx_m;

		//map<CppUtils::String, map<double, Candle> > lastCandle_m;

		Simulator simulator_m;
		
		

	};

	OBJECT_ENTRY_AUTO(__uuidof(MainSmartComEngine), CMainSmartComEngine)

}; 

#endif