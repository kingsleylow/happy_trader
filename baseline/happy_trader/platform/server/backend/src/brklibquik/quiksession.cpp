#include "quikbroker.hpp"
#include "quiksession.hpp"


namespace BrkLib {

	CppUtils::Mutex QuikSession::quikApiMtx_m;

	QuikSession::QuikSession(CppUtils::String const& connectstring, QuikBroker &broker):
	BrokerSession(connectstring, broker),
	pathToQuik_m(broker.pathToQuik_m)
	{
		
		
	}

	QuikSession::~QuikSession()
	{
		
	}
		
	bool QuikSession::isConnected() const
	{
		// return true because we have the single connection to QUIK running instance which is global for 
		// the loaded broker, thus we will establish DLL connection only once upon load
		// and disconnect upon unload
		// and will check the connection status before issuing a transaction
		// in the wrost case our transaction will fail
		return true;
	}

	// -------------------------------
	// helpers
	bool QuikSession::h_isConnected() const
	{
		
		QuikBroker const& brk = (QuikBroker const&)getBroker();
		
		LOCK_FOR_SCOPE(quikApiMtx_m);
		return ( brk.isDllConnected() && brk.isQuikConnected() );	
	}
	
	void QuikSession::h_disconnectDll()
	{
		
		QuikBroker const& brk = (QuikBroker const&)getBroker();
		
		LOCK_FOR_SCOPE(quikApiMtx_m);
		if (h_isConnected())
			brk.qDisconnect();
	}

	void QuikSession::h_connectDll()
	{
		
		QuikBroker const& brk = (QuikBroker const&)getBroker();
		
		LOCK_FOR_SCOPE(quikApiMtx_m);
		if (!h_isConnected())
			brk.qConnect(pathToQuik_m);
	}

	void QuikSession::h_asynchTransact(CppUtils::String const& transactString)
	{
		
		QuikBroker const& brk = (QuikBroker const&)getBroker();
		
		LOCK_FOR_SCOPE(quikApiMtx_m);
		// connect DLL if necessary
    if ( !brk.isDllConnected() )
			brk.qConnect(pathToQuik_m);

		// we can't do anything if QUIK is NOT connected, so will try to send transaction
		brk.qAsynchTransact(transactString);
	}



	

}; // end of namespace