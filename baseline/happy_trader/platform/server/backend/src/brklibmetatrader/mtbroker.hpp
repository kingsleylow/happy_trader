#ifndef _BRKLIBRMETATRADER_BROKER_INCLUDED
#define _BRKLIBRMETATRADER_BROKER_INCLUDED

#include "brklibmetatraderdefs.hpp"

/**
This is the implementation of backets library that can work of any algorithm to perform history simulation

*/
namespace BrkLib {

	/**
	 Forwards
	*/

	class MtSession;
	


	/**
	 Helper structures
	*/


	

	

	
	// -----------------------------------------
	
	class BRKLIBMETATRADER_EXP MtBroker: 
		public BrkLib::BrokerBase
	{
		friend class MtSession;
		
		friend class BrokerEngine;
	public:
		
		
		MtBroker( Inqueue::AlgorithmHolder& algHolder);

		virtual ~MtBroker();
		
		virtual void onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor);

		virtual void onLibraryFreed();
		
		virtual CppUtils::String const& getName() const;

public:

		virtual double getBrokerSystemTime();

		// create and destroy session objects
		virtual BrokerSession& createSessionObject(CppUtils::String const& connectstring, CppUtils::String const& runName, CppUtils::String const& comment);

		virtual BrokerSession& resolveSessionObject(CppUtils::Uid const& uid);

		virtual void getAvailableSessions(CppUtils::UidList& activeSesionsUids) const;

		virtual void deleteSessionObject(BrokerSession& obj, CppUtils::String const& runName);
		

		// connect 
		virtual void connectToBroker(BrokerSession& session, CppUtils::String const& runName);

		// disconnect
		virtual void disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName);


		virtual bool unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order);

		virtual void getBrokerResponseList(BrkLib::BrokerSession &session) const;
		
		virtual void getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const;
		
		void getBrokerPositionList(int const posStateQuery, BrkLib::BrokerSession &session, PositionList &posList);

		
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
		)
		{
		}

		virtual void onThreadStarted( bool const firstLib, bool const lastLib
		)
		{
		};

		virtual void onThreadFinished( HlpStruct::TradingParameters const& tradeParams, bool const firstLib, bool const lastLib		)
		{
		};


	private:		

	
		inline CppUtils::String getSymbolContext(Order const& order) const
		{
			return order.provider_m + " [ " + order.symbol_m + " ] ";
		}

		

	private:

	
			
		// connection map 

		CppUtils::Mutex sessionsMtx_m;

		map<CppUtils::String, MtSession*> sessions_m;

		map<CppUtils::Uid, MtSession*> sessions2_m;
	


		
	};



}

#endif