#include "rtsession.hpp"
#include "rtbroker.hpp"


namespace BrkLib {
	RtSession::RtSession(CppUtils::String const& connectstring, RtBroker &broker):
	BrokerSession(connectstring, broker),
	isConnected_m(false)
	{
	}

	RtSession::~RtSession()
	{
	}
		
	bool RtSession::isConnected() const
	{
		// assume always connected
		return isConnected_m;
	}
}