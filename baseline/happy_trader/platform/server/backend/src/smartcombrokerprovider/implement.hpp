#ifndef _SMARTCOMBROKERPROVIDER_IMPL_INCLUDED
#define _SMARTCOMBROKERPROVIDER_IMPL_INCLUDED

// Many thanks to Zoubok Victor

#include "smartcombrokerproviderdefs.hpp"
#include "session.hpp"

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlsafe.h>

#define ORDER_EXPIR_TIME_DAY  -999999
#define ORDER_EXPIR_TIME_GTC  999999




namespace BrkLib {
	struct HistoryRetrivalTask
	{
		HistoryRetrivalTask():
			interval_m(StBarInterval_Tick),
			from_count_m(0),
			from_date_m(0),
			cookie_m(0)
		{
		};

		HistoryRetrivalTask(HistoryRetrivalTask const& src):
			interval_m(src.interval_m),
			from_count_m(src.from_count_m),
			from_date_m(src.from_date_m),
			cookie_m(src.cookie_m),
			symbol_m(src.symbol_m)
		{
		}

		HistoryRetrivalTask(StBarInterval const interval, BSTR const symbol, int const from_count, DATE const from_date, int const cookie):
			interval_m(interval),
			from_count_m(from_count),
			from_date_m(from_date),
			cookie_m(cookie),
			symbol_m(symbol)
		{
		}

		CComBSTR symbol_m;
		StBarInterval interval_m;
		int from_count_m;
		DATE from_date_m;
		int cookie_m;

		
	};

	struct HistoryRetrivalStructure {

		map<int, HistoryRetrivalTask> hstRetrMap_m;

		CppUtils::Mutex	hstRetrTaskMtx_m;

		static int cookie_m;
	};

	// --------------------



	typedef HRESULT (STDAPICALLTYPE* Pfn)(void);

	class SmartComCallbackModule : public CAtlDllModuleT< SmartComCallbackModule >
	{
	public :
		
	};

	extern class SmartComCallbackModule _AtlModule;

	// -----------
	struct OrderHolder
	{
		OrderHolder():
			session_m(NULL),
			orderType_m(BrkLib::OP_DUMMY)
		{
		}

		OrderHolder(SmartComBrokerSession* session, BrkLib::Order const& order):
			session_m(session),
			order_m(order)
		{
		}

		SmartComBrokerSession* session_m;
		
		BrkLib::Order order_m;

		BrkLib::OrderType orderType_m;

	};


	// -----------
	class SmartComCallback: 
		public CComObjectRootEx<CComSingleThreadModel>,
		public SmartComEngine::IMainSmartComEngineCallback ,
		public CppUtils::Mutex
	{
		BEGIN_COM_MAP(SmartComCallback)
			COM_INTERFACE_ENTRY(SmartComEngine::IMainSmartComEngineCallback)
			// COM_INTERFACE_ENTRY_IID(SmartComEngine::CLSID_MainSmartComEngineCallback, SmartComCallback )
		END_COM_MAP()

		// these are callback from smartcom_proxy2
		// must be derived
		STDMETHOD (onOrderSucceded)(
			long cookie, 
			BSTR symbol,
			long state,
			long action,
			long type,
			long validity,
			double price,
			double amount,
			double stop,
			double filled,
			double datetime,
			BSTR orderid,
			BSTR orderno
		)	{	return E_NOTIMPL;	}

		STDMETHOD (onAddTrade) (
			long cookie,
			BSTR portfolio,
			BSTR symbol,
			BSTR orderno,
			BSTR orderid,
			double price,
			double amount,
			double datetime,
			BSTR tradeno
		){	return E_NOTIMPL;	} 

		STDMETHOD (onOrderFailed)(long cookie, BSTR orderid, BSTR reason)	{	return E_NOTIMPL;	}
		STDMETHOD (onConnect)()	{	return E_NOTIMPL;	}
		STDMETHOD (onDisconnect)(BSTR reason)	{	return E_NOTIMPL;	}
		STDMETHOD (onSetMyTrade) (
			BSTR portfolio, 
			BSTR symbol, 
			double datetime, 
			double price, 
			double volume, 
			BSTR tradeno, 
			long buysell, 
			BSTR orderno,
			BSTR orderid
		)
		{
			return E_NOTIMPL;
		};

		STDMETHOD (onFinishBarsHistory)(
			int cookie,
			BSTR symbol,
			VARIANT_BOOL result,
			long count,
			IDispatch* eventClientPtr
		)
		{
			return E_NOTIMPL;
		};

		STDMETHOD (onTimer_1Sec)() { return E_NOTIMPL; };
		
	};

	class SMARTCOMBROKERPROVIDER_EXP SmartComBroker: 
			public BrokerBase,
			public CComObject<SmartComCallback>
	{

	public:

				

		SmartComBroker( Inqueue::AlgorithmHolder& algHolder);

		virtual ~SmartComBroker();

		virtual void onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor);

		virtual void onLibraryFreed();

		virtual CppUtils::String const& getName() const;
				
	public:

		virtual double getBrokerSystemTime();

		// create and destroy session objects
		virtual BrokerSession& createSessionObject(CppUtils::String const& connectstring, CppUtils::String const& runName, CppUtils::String const& comment);

		virtual BrokerSession& resolveSessionObject(CppUtils::Uid const& uid);

		virtual void getAvailableSessions(CppUtils::UidList& activeSesionsUids) const;

		virtual void deleteSessionObject(BrokerSession& session, CppUtils::String const& runName);


		// connect 
		virtual void connectToBroker(BrokerSession& session, CppUtils::String const& runName);

		// disconnect
		virtual void disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName);


		virtual bool unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order);

		virtual void getBrokerResponseList(BrkLib::BrokerSession &session) const;

		virtual void getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const;

		virtual void getBrokerPositionList(int const posStateQuery, BrokerSession &session, PositionList &posList);


		virtual void onDataArrived(	HlpStruct::RtData const& rtdata);

		virtual void initializeBacktest(
			BrkLib::SimulationProfileData const& simulProfileData, 
			BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
			);

		virtual void initializeBacktest(BrkLib::SimulationProfileData const& simulProfileData );



		virtual bool isBacktestLibrary() const;

		virtual bool getTickInfo(
			BrkLib::BrokerSession &session, 
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
			);


		// subscribe for nes retreival
		virtual bool subscribeForNews(
			BrkLib::BrokerSession &session
			) ;

		// returns last error information
		virtual bool getLastErrorInfo(
			BrkLib::BrokerSession &session
			) ;

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
		virtual bool subscribeForQuotes(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
			) ;

		// returns the history from the server
		virtual bool getHistoryInfo(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom,
			HlpStruct::AggregationPeriods aggrPeriod,
			int const multfactor
			);

		

		virtual void onCustomBrokerEventArrived(
			HlpStruct::CallingContext& callContext,
			CppUtils::String &customOutputData
		);

		virtual void onThreadStarted(bool const firstLib, bool const lastLib
		);
		
		virtual void onThreadFinished(HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib
		);

	public:
		
		// these are callbacks from smartcom_proxy layer
		STDMETHOD (onOrderSucceded)(
			long cookie, 
			BSTR symbol,
			long state,
			long action,
			long type,
			long validity,
			double price,
			double amount,
			double stop,
			double filled,
			double datetime,
			BSTR orderid,
			BSTR orderno
		);
		STDMETHOD (onOrderFailed)(long cookie, BSTR orderid, BSTR reason);
		STDMETHOD (onConnect)();
		STDMETHOD (onDisconnect)(BSTR reason);

		STDMETHOD (onAddTrade) (
			long cookie,
			BSTR portfolio,
			BSTR symbol,
			BSTR orderno,
			BSTR orderid,
			double price,
			double amount,
			double datetime,
			BSTR tradeno
		);

		STDMETHOD (onSetMyTrade)(
			BSTR portfolio, 
			BSTR symbol, 
			double datetime, 
			double price, 
			double volume, 
			BSTR tradeno, 
			long buysell, 
			BSTR orderno,
			BSTR orderid
		);

		STDMETHOD (onFinishBarsHistory)(
			int cookie,
			BSTR symbol,
			VARIANT_BOOL result,
			long count,
			IDispatch* eventClientPtr
		);

		STDMETHOD (onTimer_1Sec)();
		

	private:

		static CppUtils::String extractPortfolioFromOrder(BrkLib::Order const& order);
		
		static void initStringSafeArray(CComSafeArray<BSTR>& safearray, CppUtils::StringList const& sourceList);

		static BrkLib::OrderType smartComOperationFlagsToOrderType(bool const use_only_open, int const opType, int const action, int const parentOrderType );

		static long getOrderCookie();

		// initialize and deinitializa our smart com helper
		void operationsInit();

		void operationsDeinit();

		void registerComLibrary(CppUtils::String const& dllName);

		void issueBuyOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueSellOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
	
		void issueBuyStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueSellStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);

		void issueBuyLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueSellLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);

		void issueBuyStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueSellStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);

		void issueCloseShortOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueCloseLongOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);

		void issueCloseShortLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueCloseLongLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);

		void issueCloseShortStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueCloseLongStopOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);

		void issueCloseShortStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);
		void issueCloseLongStopLimitOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);

		void issueCancelOrderHelper(int const cookie, BrkLib::BrokerSession &session, BrkLib::Order const& order);


		// extract expiration time from order
		int getOrderExpirationTime(BrkLib::Order const& order) const;

			
		void propagateThroughAllSessions(BrokerResponseBase const& orderResp);

		void prepareBrokerResponseOrder_FromOrder(BrkLib::Order const& order, BrkLib::BrokerResponseOrder& orderResp) const;

		void sendErrorResponse(
			int const cookie, 
			CppUtils::String const& reason, 
			int const errcode,
			BrkLib::OrderType const postype
		);


		//CComPtr<SmartComEngine::IMainSmartComEngine> getMarhshalledPointer_smartComEngine();


	
		// -----------------

		CComPtr<SmartComEngine::IMainSmartComEngine> smartComEngine_m;

		// main engine pointer
		//CMainSmartComEngine* smartComEngine_m;

		long subscriptionCookie_m;

		// sessions
		map<CppUtils::String, SmartComBrokerSession*> sessions_m;

		map<CppUtils::Uid, SmartComBrokerSession*> sessions2_m;

		map<long, OrderHolder> sessionsForCookies_m;

		map<CppUtils::String, long> cancellations_m;

		HistoryRetrivalStructure hstRetrStruct_m;

		/*
		CComPtr<IGlobalInterfaceTable> globalGIT_m;

		DWORD smartComEnginePtrCookie_m;
		*/
			
	};


	

}; // end of namespace


#endif // _SMARTCOMBROKERPROVIDER_IMPL_INCLUDED
