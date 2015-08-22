#include "mtsession.hpp"
#include "mtbroker.hpp"


namespace BrkLib {
	MtSession::MtSession(CppUtils::String const& connectstring, MtBroker &broker):
	BrokerSession(connectstring, broker),
	isConnected_m(false)
	{
	}

	MtSession::~MtSession()
	{
	}
		
	bool MtSession::isConnected() const
	{
		// assume always connected
		return isConnected_m;
	}
}