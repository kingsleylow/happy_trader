#ifndef _BRKLIBLASER_LASERBROKER_INCLUDED
#define _BRKLIBLASER_LASERBROKER_INCLUDED

#include "brkliblaserdefs.hpp"
#include "resource1.h"
#include "helperstruct.hpp"

#define RESPONSE_TAG_REMAIN_SHARES tag_m
#define OPTION_TIME_IN_FORCE "TFORCE" // time in force
#define OPTION_STOP_MKT "STOPMKT" // if stop order is market order


namespace BrkLib {

// forward
class LaserSession;
class WindowThreadProc;

class BRKLIBLASER_EXP LaserBroker: public BrkLib::BrokerBase, private CppUtils::Mutex
	{
	
	public:

		friend class LaserSession;
	
		LaserBroker( Inqueue::AlgorithmHolder& algHolder);

		virtual ~LaserBroker();
		
		virtual void onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor);

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
    		
		virtual BrkLib::PositionList const& getBrokerPositionList(int const posState, BrkLib::BrokerSession &session);

		virtual void updatePositionTagFields(BrkLib::BrokerSession &session, CppUtils::Uid const posUid, int const tag1, int const tag2, int const posState);
		
		virtual void onDataArrived(	HlpStruct::RtData const& rtdata);
		
		virtual void initializeBacktest(
			Inqueue::SimulationProfileData const& simulProfileData, 
			BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		);
		
		virtual void initializeBacktest(Inqueue::SimulationProfileData const& simulProfileData );



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
			Inqueue::AggregationPeriods aggrPeriod,
			int const multfactor
		);

		virtual void onCustomBrokerEventArrived(
			HlpStruct::CallingContext& callContext,
			CppUtils::String &customOutputData
		)
		{
		}

		virtual void onThreadStarted( bool const firstLib, bool const lastLib
		)
		{
		};

		virtual void onThreadFinished( bool const firstLib, bool const lastLib
		)
		{
		};



private:

		// parse host:port
		
			
		void parseFileSymbolList(CppUtils::StringList& symbols, CppUtils::String const& fileName);

		void createStockForSymbols();

		void removeStocksForSymbols();

		void propagateResponseAmongSessions(BrokerResponseBase& brokerResponse, bool const remove, long const sequenceID);

		// no need in parent ID
		void propagateResponseAmongSessions(BrokerResponseBase& brokerResponse);


		//PosType priceIndicatorToStandardPosType(int priceIndic, int chSide) const;

		void prepareOrderCall(
			BrkLib::Order const& order, 
			GTOrder32& order32,
			LPGTSTOCK& symbolHndl,
			double& price,
			MMID& method,
			MMID& place,
			long& timeInForce,
			bool& is_stop_market
		);

	
	public:

		 //int OnExecMsgLogin(LPGTSESSION hSession, BOOL bLogin);

		 //int OnExecConnected(LPGTSESSION hSession);

		 //int  OnExecDisconnected(LPGTSESSION hSession);

		 //int  OnExecMsgLoggedin(LPGTSESSION hSession);   

		 //int  OnExecMsgLogout(LPGTSESSION hSession);

		 int  OnExecMsgTrade(LPGTSESSION hSession, const GTTrade32 *trade);

		 int  OnExecMsgAccount(LPGTSESSION hSession, const GTAccount32 *account);

		 int  OnExecMsgOpenPosition(LPGTSESSION hSession, const GTOpenPosition32 *open);

		 int  OnExecMsgPending(LPGTSESSION hSession, const GTPending32 *pending);

		 int  OnExecMsgSending( LPGTSESSION hSession, const GTSending32 *sending);

		 int  OnExecMsgCanceling(LPGTSESSION hSession, const GTCancel32 * cancel);

		 int  OnExecMsgCancel(LPGTSESSION hSession, const GTCancel32 * cancel);

		 int  OnExecMsgReject(LPGTSESSION hSession, const GTReject32 *reject);

		 int  OnExecMsgRemove(LPGTSESSION hSession, const GTRemove32 *remove);

		 int  OnExecMsgRejectCancel(LPGTSESSION hSession, const GTRejectCancel32 *rejectcancel) ;

		 int  OnExecMsgStatus(LPGTSESSION hSession, const GTStatus32 *status);

		 int OnExecMsgState(LPGTSESSION hSession, const GTServerState32 *state);

		 //int OnGotQuoteLevel1(LPGTSESSION hSession,  const GTLevel132 *level1);

		 //int OnGotQuotePrint(LPGTSESSION hSession,  const GTPrint32 *printR);

		 int OnExecMsgErrMsg(LPGTSESSION hSession, const GTErrMsg32 *errorMsg);

		 // chart

		 int OnGotQuoteConnected(LPGTSESSION hSession);

		//int OnGotChartDisconnected(LPGTSESSION hSession);

		//int OnGotChartConnected(LPGTSESSION hSession);

		//int OnGotChartRecord(LPGTSESSION hSession, const GTChart32 *gtchart);

		//int OnGotChartRecordHistory(LPGTSESSION hSession, const GTChart32 *gtchart);

		//int OnGotChartRecordRefresh(LPGTSESSION hSession, const GTChart32 *gtchart);

		//int OnGotChartRecordDisplay(LPGTSESSION hSession, const GTChart32 *gtchart);


		 // stock
		 int OnExecMsgIOIStatus_Stock(LPGTSTOCK hStock, const GTIOIStatus32 *ioStatus);

		 int OnExecCmd_Stock(LPGTSTOCK hStock, const LPCSTR *execCmd);

		 
		 int OnExecMsgSending_Stock( LPGTSTOCK hStock, const GTSending32 *sending);

		 
		 int PrePlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order);

		 
		 int PostPlaceOrder_Stock( LPGTSTOCK hStock,const GTOrder32 *order);



				
private:

	
	WindowThreadProc *windowThreadProc_m;
	
	
	
	// sessions
	map<CppUtils::String, LaserSession*> sessions_m;

	map<CppUtils::Uid, LaserSession*> sessions2_m;

	
	
private:
	
	// helpers

  //

	
	



};
	

} // end of namesapce
#endif