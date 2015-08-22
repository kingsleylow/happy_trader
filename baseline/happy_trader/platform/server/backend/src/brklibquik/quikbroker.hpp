#ifndef _BRKLIBQUIK_QUIKBROKER_INCLUDED
#define _BRKLIBQUIK_QUIKBROKER_INCLUDED

#include "brklibquikdefs.hpp"
#include "quikposcont.hpp"
#include "wraptrans2quik.hpp"


namespace BrkLib {

	// forward
	class QuikSession;

	class BRKLIBQUIK_EXP QuikBroker: 	public BrkLib::BrokerBase, private CppUtils::Mutex, public QuikApiWrap
	{
	
		friend class QuikSession;
		friend class QuikPosHolder;
	public:
		
		
		QuikBroker( Inqueue::AlgorithmHolder& algHolder);

		virtual ~QuikBroker();
		
		virtual void onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor);

		virtual void onLibraryFreed();
		
		virtual CppUtils::String const& getName() const;

public:

		// create and destroy session objects

		virtual double getBrokerSystemTime();

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

		// -----------------------------
		// helpers
		void parseConnectionString(CppUtils::String const& connectString);

		// this delegates broker response to all sessions
		// as no way to check the exact session
		void delegateResponseToSessions(BrokerResponseBase const& bresp);

		// virtual callbacks
		virtual void transactionCallback(
			long nTransactionResult, 
			long nTransactionExtendedErrorCode, 
			long nTransactionReplyCode, 
			DWORD dwTransId, 
			double dOrderNum, 
			LPCSTR lpcstrTransactionReplyMessage
		);

		CppUtils::String createQuikString(BrkLib::Order const& order);

		// sessions map
		map<CppUtils::String, QuikSession*> sessions_m;

		map<CppUtils::Uid, QuikSession*> sessions2_m;

	

		// quik related data
		// path to QUIK
		CppUtils::String pathToQuik_m;

		// DDE service name
		CppUtils::String ddeServiceName_m;

		// helper class containing raw data arrived from quik
		QuikPosHolder quikPosHolder_m;

	};

}; // end of namespace

#endif