#include "session.hpp"
#include "implement.hpp"


namespace BrkLib {

SmartComBrokerSession::SmartComBrokerSession(CppUtils::String const& connectstring, SmartComBroker &broker):
BrokerSession(connectstring, broker)
{
}

SmartComBrokerSession::~SmartComBrokerSession()
{
}

bool SmartComBrokerSession::isConnected() const
{
	return true;
}


};