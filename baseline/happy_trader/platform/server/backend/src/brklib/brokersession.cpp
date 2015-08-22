#include "brokersession.hpp"
#include "broker.hpp"


namespace BrkLib {

	BrokerSession::BrokerSession(CppUtils::String const& connectstring, BrokerBase& brokerbase):
		connectString_m(connectstring),
		brokerBase_m(brokerbase),
		state_m(-1)
	{
			
		
		sessionUid_m.generate();
		
	}

	BrokerSession::~BrokerSession()
	{
	}

	CppUtils::String const& BrokerSession::getConnection() const
	{
		return connectString_m;
	}

	CppUtils::Uid const& BrokerSession::getUid() const
	{
		return sessionUid_m;
	}

	BrokerBase const& BrokerSession::getBroker() const
	{
		return (BrokerBase const&)brokerBase_m;
	}

	BrokerBase& BrokerSession::getBroker()
	{
		return brokerBase_m;
	}

	void BrokerSession::setRunName(CppUtils::String const& runName)
	{
		runName_m = runName;
	}
	

	void BrokerSession::setRunName(char const* runName)
	{
		runName_m = (runName ? runName: "UNKNOWN RUN");
	}


	// -------------------------------

	void BrokerSession::setRunComment(CppUtils::String const& runComment)
	{
		runComment_m = runComment;
	}


	void BrokerSession::setRunComment(char const* runComment)
	{
		runComment_m = (runComment ? runComment : "");
	}

	

	

}; //end of namespace