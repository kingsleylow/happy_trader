#include "btsession.hpp"
#include "btbroker.hpp"


namespace BrkLib {
	BtSession::BtSession(CppUtils::String const& connectstring, BtBroker &broker):
	BrokerSession(connectstring, broker),
	isConnected_m(false)
	{
	}

	BtSession::~BtSession()
	{
	}
		
	bool BtSession::isConnected() const
	{
		// assume always connected
		return isConnected_m;
	}
};