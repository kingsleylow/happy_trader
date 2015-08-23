#include "quikbroker.hpp"
#include "quiksession.hpp"
#include "quikddewrap.hpp"

namespace BrkLib {

#define QUIKBROKER "QUIKBROKER"

// ----------------------------
// DllMain
// To register new window class 
extern "C" {  

	BOOL APIENTRY DllMain(HINSTANCE hinstDll, DWORD dwReasion, LPVOID lpReserved) {
		if(dwReasion == DLL_PROCESS_ATTACH) {
			DDEQuikServerWrap::registerWindowClass(hinstDll);
		}

		return TRUE;
	}

};

// --------------------------
	/**
	 terminator and creator functions
	*/
	BrokerBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new QuikBroker(*algholder);
	}

	void terminator (BrokerBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}
	
	BrokerBase::CreatorBrokerFun createBroker = creator;

	BrokerBase::DestroyerBrokerFun destroyBroker = terminator;

	// -----------------------------------------

	QuikBroker::QuikBroker( Inqueue::AlgorithmHolder& algHolder):
		BrokerBase(algHolder)
	{
	}

	QuikBroker::~QuikBroker()
	{
	}

	void QuikBroker::onLibraryInitialized(Inqueue::AlgBrokerDescriptor const& descriptor)
	{

		// get parameter
		CppUtils::String const &connectstring = descriptor.initialAlgBrkParams_m.brokerParams_m.getParameter("QUIK_CONNECT_STRING").getAsString();

		// parse connect string
		parseConnectionString(connectstring);

		// initialize DDE server
		// as this type of session is cached we are sure that this is done only once (the first time)
		quikPosHolder_m.initialize(ddeServiceName_m, this);
		LOGEVENT(HlpStruct::CommonLog::LL_INFO, QUIKBROKER, "Started DDE service: " << ddeServiceName_m);

		// force QUIK to connect, but we must check this status later also
		qConnect(pathToQuik_m);

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, QUIKBROKER, "Connected to QUIK instance: " << pathToQuik_m);
	}

	void QuikBroker::onLibraryFreed()
	{
		quikPosHolder_m.deinitialize();

		// forcibly disconnect
		if (isDllConnected()) 
			qDisconnect();

		LOGEVENT(HlpStruct::CommonLog::LL_INFO, QUIKBROKER, "Deinitialized");
		
	}

	CppUtils::String const& QuikBroker::getName() const
	{
		static CppUtils::String sname("quik broker implementation");
		return sname;
	}

	// ----------------------------------------

	double QuikBroker::getBrokerSystemTime()
	{
		return -1;
	}

	// create and destroy session objects
	BrokerSession& QuikBroker::createSessionObject(
		CppUtils::String const& connectstring, 
		CppUtils::String const& runName, 
		CppUtils::String const& comment
	)
	{
		LOCK_FOR_SCOPE(*this);

		map<CppUtils::String, QuikSession*>::iterator it = sessions_m.find(connectstring);

		if (it==sessions_m.end()) {
			QuikSession* quikSession = new QuikSession(connectstring, *this);
			
			sessions_m[ connectstring ] = quikSession;
			sessions2_m[quikSession->getUid()] = quikSession;
			// notify
			sendSessionCreateEvent(*quikSession, runName, comment);

			return *quikSession;
		}
		else
			return *it->second;
	}

	BrokerSession& QuikBroker::resolveSessionObject(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(*this);

		map<CppUtils::Uid, QuikSession*>::iterator it = sessions2_m.find(uid);

		if (it==sessions2_m.end())
			THROW(CppUtils::OperationFailed, "exc_NotFound", "ctx_resolveSessionObject", uid.toString());

		return *it->second;
	
	}

	void QuikBroker::getAvailableSessions(CppUtils::UidList& activeSesionsUids) const
	{
		LOCK_FOR_SCOPE(*this);
		activeSesionsUids.clear();

		for(map<CppUtils::Uid, QuikSession*>::const_iterator it = sessions2_m.begin(); it != sessions2_m.end(); it++) {
			activeSesionsUids.push_back(it->first);
		}
	}

	
	void QuikBroker::deleteSessionObject(BrokerSession& session, CppUtils::String const& runName)
	{
		LOCK_FOR_SCOPE(*this);
		QuikSession* quikSession = (QuikSession*) &session;

		if (quikSession) {
			
			// remove from the list if exist
			int cnt = sessions_m.erase( quikSession->getConnection() );

			if (cnt > 0) {
				sessions2_m.erase( quikSession->getUid() );

				// notify
				sendSessionDestroyEvent(session, runName);

				delete quikSession;
			}
		}
	} 
	

	// connect 
	void QuikBroker::connectToBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		// do nothing as connection is expected to be alive
	}

		// disconnect
	void QuikBroker::disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName)
	{
			// do nothing as connection is expected to be alive
		
	}


	bool QuikBroker::unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		QuikSession& qsession = (QuikSession&)session;
		qsession.h_asynchTransact(createQuikString(order));
		return true;
	}

	void QuikBroker::getBrokerResponseList(BrkLib::BrokerSession &session) const
	{
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}
		
	void QuikBroker::getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const
	{
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}
		
	BrkLib::PositionList const& QuikBroker::getBrokerPositionList(int const posState, BrkLib::BrokerSession &session)
	{
		static PositionList dummy;
		return dummy;
	}

	void QuikBroker::updatePositionTagFields(BrkLib::BrokerSession &session, CppUtils::Uid const posUid, int const tag1, int const tag2, int const posState)
	{
	}
		
	void QuikBroker::onDataArrived(	HlpStruct::RtData const& rtdata)
	{
		// this function is to be implemented only for backtessting (ask ZVV for details :))
		// thus here:
		assert( false );
	}
		
	void QuikBroker::initializeBacktest(
			Inqueue::SimulationProfileData const& simulProfileData, 
			BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		)
	{
		// this function is to be implemented only for backtessting as providerSymbolCookieMap used for fast acess to simulProfileData\
		// thus here:
		// Not implemented
		assert(false);
	}
		
	void QuikBroker::initializeBacktest(Inqueue::SimulationProfileData const& simulProfileData )
	{
		// this function is to be implemented only for backtessting in RT mode (ask ZVV for details :))
		// thus here:
		// Not implemented
		assert(false);
	}



	bool QuikBroker::isBacktestLibrary() const
	{
			// it's RT library, direct connect to quik
			return false;
	}

	bool QuikBroker::getTickInfo(
			BrkLib::BrokerSession &session, 
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
		{
			// No-op
			return false;
		}

		
		// subscribe for nes retreival
		bool QuikBroker::subscribeForNews(
			BrkLib::BrokerSession &session
		)
		{
			// No-op
			return false;
		}

		// returns last error information
		bool QuikBroker::getLastErrorInfo(
			BrkLib::BrokerSession &session
		)
		{
			// No-op
			return false;
		}

		// subscribe for quotes - they will arrive via onBrokerResponseArrived
		bool QuikBroker::subscribeForQuotes(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom
		)
		{
			// No-op
			return false;
		}

		// returns the history from the server
		bool QuikBroker::getHistoryInfo(
			BrkLib::BrokerSession &session,
			CppUtils::String const& symbolNum, 
			CppUtils::String const& symbolDenom,
			Inqueue::AggregationPeriods aggrPeriod,
			int const multfactor
		)
		{
			// No-op
			return false;
		}




		// -------------------------------------------
		// helpers

	void QuikBroker::parseConnectionString(CppUtils::String const& connectString)
	{
		// ; is delimiter
		CppUtils::StringList result;
		CppUtils::tokenize(connectString, result, ";", "", "", "");
  
		if (result.size() !=2) 
			THROW(CppUtils::OperationFailed, "exc_InvalidConnectionString", "ctx_parseConnectString", connectString);


		pathToQuik_m = result[0];
		ddeServiceName_m = result[1];
	}


	CppUtils::String QuikBroker::createQuikString(BrkLib::Order const& order)
	{
		return "";
	}

	// virtual callbacks
	void QuikBroker::transactionCallback(
			long nTransactionResult, 
			long nTransactionExtendedErrorCode, 
			long nTransactionReplyCode, 
			DWORD dwTransId, 
			double dOrderNum, 
			LPCSTR lpcstrTransactionReplyMessage
	)
	{
		// if there is a error delegate this to upper level
		// otherwise do nothing because all the info will come from QUIK DDE export
		if (nTransactionResult != TRANS2QUIK_SUCCESS ) {
			BrokerResponseOrder brokerResponse;
			brokerResponse.ID_m.generate();

			// TODO
			//brokerResponse.parentID_m = ???

			// so far this is the only position what we may change
			brokerResponse.brokerPositionID_m = CppUtils::long2String((long)dOrderNum);

			// delegate that to all sessions clients
			delegateResponseToSessions( brokerResponse );
		}

	}

	void QuikBroker::delegateResponseToSessions(BrokerResponseBase const& bresp)
	{
		{
				LOCK_FOR_SCOPE(*this);

				for (map<CppUtils::String, QuikSession*>::iterator it = sessions_m.begin(); it!=sessions_m.end();it++) {
						onBrokerResponseArrived(*it->second, bresp);
				}

			}
	}



}; // end of namespace