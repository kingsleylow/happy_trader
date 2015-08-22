#include "brkconnect.hpp"
#include "broker.hpp"
#include "../inqueue/inqueue.hpp"


#define BRKCONNECT "BRKCONNECT"
namespace BrkLib {
	BrokerConnect::BrokerConnect(
			BrokerBase* brokerBase,
			const char* connectString,
			bool const autodestroy,
			bool const autodisconnect,
			char const* runName,
			char const* comment
	):
		autoDestroy_m(autodestroy),
		autoDisconnect_m(autodisconnect),
		brkSessionPtr_m(NULL)
	{
		brkBasePtr_m = brokerBase;
		if (brkBasePtr_m) {

		

			BrkLib::BrokerSession& session = brkBasePtr_m->createSessionObject(connectString, runName ? runName: "UNKNOWN RUN", comment ? comment:"" );
			brkSessionPtr_m  = &session;
			session.setRunName(runName);
			session.setRunComment( comment );

			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKCONNECT, "Created session object", brkBasePtr_m->getName() << session.getUid().toString() << session.getRunName() );
			
			
		}
	}

	BrokerConnect::BrokerConnect(
		BrkLib::BrokerSession &session, 
		bool const autodestroy,
		bool const autodisconnect
	):
		autoDestroy_m(autodestroy),
		autoDisconnect_m(autodisconnect),
		brkBasePtr_m(&session.getBroker()),
		brkSessionPtr_m(&session)
	{
		
	}

	BrokerConnect::BrokerConnect():
		autoDestroy_m(false),
		autoDisconnect_m(false),
		brkBasePtr_m(NULL),
		brkSessionPtr_m(NULL)
	{
	}

	BrokerConnect::~BrokerConnect()
	{ 
		
		if (!brkBasePtr_m)
			return;

		if (!brkSessionPtr_m)
			return;

		if (autoDisconnect_m) {
			brkBasePtr_m->disconnectFromBroker(*brkSessionPtr_m, brkSessionPtr_m->getRunName().c_str());
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  BRKCONNECT, "Disconnected from broker", brkBasePtr_m->getName() << brkSessionPtr_m->getUid().toString() << brkSessionPtr_m->getRunName());
		}

		if (autoDestroy_m) {
			LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKCONNECT, "Deleting session object", brkBasePtr_m->getName() << brkSessionPtr_m->getUid().toString() << brkSessionPtr_m->getRunName());
			brkBasePtr_m->deleteSessionObject(*brkSessionPtr_m, brkSessionPtr_m->getRunName());
		}

	}

	
	void BrokerConnect::connect()
	{
		if (!brkBasePtr_m)
			return;

		if (!brkSessionPtr_m)
			return;

		brkBasePtr_m->connectToBroker(*brkSessionPtr_m, brkSessionPtr_m->getRunName().c_str());
		
		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL, BRKCONNECT, "Attempt to connect to broker", brkBasePtr_m->getName() << brkSessionPtr_m->getUid().toString() << brkSessionPtr_m->isConnected() << brkSessionPtr_m->getRunName());

	}

	void BrokerConnect::disconnect()
	{
		if (!brkBasePtr_m)
			return;

		if (!brkSessionPtr_m)
			return;

		brkBasePtr_m->disconnectFromBroker(*brkSessionPtr_m, brkSessionPtr_m->getRunName().c_str());
		LOGEVENTA(HlpStruct::CommonLog::LL_DEBUGDETAIL,  BRKCONNECT, "Disconnected from broker", brkBasePtr_m->getName() << brkSessionPtr_m->getUid().toString() << brkSessionPtr_m->getRunName());
	}

	BrokerConnect& BrokerConnect::operator = (BrokerConnect const& src)
	{

		autoDestroy_m = src.autoDestroy_m,
		autoDisconnect_m = src.autoDisconnect_m;
		brkBasePtr_m = src.brkBasePtr_m,
		brkSessionPtr_m = src.brkSessionPtr_m;

		return *this;
	}
	

};