#include "mtbroker.hpp"
#include "mtsession.hpp"

#include "../alglibmetatrader2/algorithmmt.hpp"

#define MTBROKER "MTBROKER"
namespace BrkLib {

	/**
	terminator and creator functions
	*/
	BrokerBase *creator (Inqueue::AlgorithmHolder* algholder)
	{
		return new MtBroker(*algholder);
	}

	void terminator (BrokerBase *pthis)
	{
		assert(pthis);
		delete pthis;
	}

	BrokerBase::CreatorBrokerFun createBroker = creator;

	BrokerBase::DestroyerBrokerFun destroyBroker = terminator;

	// -----------------------------------------

	MtBroker::MtBroker( Inqueue::AlgorithmHolder& algHolder):
	BrokerBase(algHolder)
	{
		
	}

	MtBroker::~MtBroker()
	{
	}

	void MtBroker::onLibraryInitialized(HlpStruct::AlgBrokerDescriptor const& descriptor)
	{
		BRK_LOG(getName().c_str(), BRK_MSG_INFO, MTBROKER, "Started" );
	}

	void MtBroker::onLibraryFreed()
	{
		BRK_LOG(getName().c_str(), BRK_MSG_INFO, MTBROKER, "Finished" );
	}

	CppUtils::String const& MtBroker::getName() const
	{
		static CppUtils::String sname("Metatrader Broker layer");
		return sname;
	}

	// -------------------------------------

	double MtBroker::getBrokerSystemTime()
	{
		return CppUtils::getTime();
	}

	// create and destroy session objects
	BrokerSession& MtBroker::createSessionObject(CppUtils::String const& connectstring, CppUtils::String const& runName, CppUtils::String const& comment)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		map<CppUtils::String, MtSession*>::iterator it = sessions_m.find(connectstring);

		if (it==sessions_m.end()) {
			MtSession* session = new MtSession(connectstring, *this);
			sessions_m[ connectstring ] = session;
			sessions2_m[session->getUid()] = session;

			// notify
			sendSessionCreateEvent(*session, runName, comment);
			return *session;
		}
		else
			return *it->second;
	}

	BrokerSession& MtBroker::resolveSessionObject(CppUtils::Uid const& uid)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		map<CppUtils::Uid, MtSession*>::iterator it = sessions2_m.find(uid);

		if (it==sessions2_m.end())
			THROW(CppUtils::OperationFailed, "exc_NotFound", "ctx_resolveSessionObject", uid.toString());

		return *it->second;
	}

	void MtBroker::getAvailableSessions(CppUtils::UidList& activeSesionsUids) const
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);
		activeSesionsUids.clear();

		for(map<CppUtils::Uid, MtSession*>::const_iterator it = sessions2_m.begin(); it != sessions2_m.end(); it++) {
			activeSesionsUids.push_back(it->first);
		}

	}

	void MtBroker::deleteSessionObject(BrokerSession& session, CppUtils::String const& runName)
	{
		LOCK_FOR_SCOPE(sessionsMtx_m);

		MtSession* mtSession = (MtSession*) &session;

		if (mtSession) {
			// remove from the list if exist


			int cnt = sessions_m.erase(mtSession->getConnection());
			if (cnt > 0) {

				sessions2_m.erase(mtSession->getUid());

				// notify
				sendSessionDestroyEvent(*mtSession, runName);

				delete mtSession;
			}
		}
	}


	void MtBroker::connectToBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		MtSession& mtSession = (MtSession&)session;
		mtSession.connect();

		// do not send connect event - to save time and resources
		sendSessionConnectEvent(mtSession, runName);
	}


	void MtBroker::disconnectFromBroker(BrokerSession& session, CppUtils::String const& runName)
	{
		MtSession& mtSession = (MtSession&)session;
		mtSession.disconnect();

		// do not send disconnect event - to save time and resources
		sendSessionDisconnectEvent(mtSession, runName);
	}


	bool MtBroker::unblockingIssueOrder(BrkLib::BrokerSession &session, BrkLib::Order const& order)
	{
		// N/A
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_unblockingIssueOrder", "");
	}

	void MtBroker::getBrokerResponseList(BrkLib::BrokerSession &session) const
	{
		// no-op
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}

	void MtBroker::getBrokerResponseList(BrkLib::BrokerSession &session, BrkLib::OperationResult const or) const
	{
		// no-op
		THROW(CppUtils::OperationFailed, "exc_NotSupported", "ctx_GetBrokerResponseList", "");
	}

	void MtBroker::getBrokerPositionList(int const posStateQuery, BrkLib::BrokerSession &session, PositionList &posList)
	{
		posList.clear();

		// TODO - need to implement
		// resolve necessary library

		
		AlgLib::AlgorithmMt* pt = static_cast<AlgLib::AlgorithmMt*>(this->getAlgHolder().getAlgorithm());


		// connect string would be acc numbr
		CppUtils::String const& connectString = session.getConnection();
		if (connectString.size() <=0)
			THROW(CppUtils::OperationFailed, "exc_MustBeValidAccountNumber", "ctx_getBrokerPositionList", "");
		
		pt->getPositionListForAccount(connectString, posList);

		

	}


	void MtBroker::onDataArrived(	HlpStruct::RtData const& rtdata )
	{
	}

	void MtBroker::initializeBacktest(
		BrkLib::SimulationProfileData const& simulProfileData, 
		BrokerBase::ProviderSymbolCookieMap const& providerSymbolCookieMap 
		)
	{
	
	}

	void MtBroker::initializeBacktest(BrkLib::SimulationProfileData const& simulProfileData )
	{

	}

	bool MtBroker::isBacktestLibrary() const
	{
		return false;
	}

	bool MtBroker::getTickInfo(
		BrkLib::BrokerSession &session, 
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom
		)
	{
		// No-op
		return false;
	}


	// subscribe for nes retreival
	bool MtBroker::subscribeForNews(
		BrkLib::BrokerSession &session
		)
	{
		// No-op
		return false;
	}

	// returns last error information
	bool MtBroker::getLastErrorInfo(
		BrkLib::BrokerSession &session
		)
	{
		// No-op
		return false;
	}

	// subscribe for quotes - they will arrive via onBrokerResponseArrived
	bool MtBroker::subscribeForQuotes(
		BrkLib::BrokerSession &session,
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom
		)
	{
		// No-op
		return false;
	}

	// returns the history from the server
	bool MtBroker::getHistoryInfo(
		BrkLib::BrokerSession &session,
		CppUtils::String const& symbolNum, 
		CppUtils::String const& symbolDenom,
		HlpStruct::AggregationPeriods aggrPeriod,
		int const multfactor
		)
	{
		// No-op
		return false;
	}

	

	/**
	* Helpers
	*/



		
	



}; // end of namespace