#ifndef _BRKLIB_BROKERSESSION_INCLUDED
#define _BRKLIB_BROKERSESSION_INCLUDED

#include "brklibdefs.hpp"

namespace BrkLib {
/**
 helper class supporting sessionst to broker
*/
class BrokerBase;
class BRKLIB_EXP BrokerSession  {
public:

	enum SessionEventType {
		SET_Dummy = 0,
		SET_Create = 1,
		SET_Connect = 2,
		SET_Disconnect = 3,
		SET_Destroy= 4

	};

	// whether session is connected
	virtual bool isConnected() const = 0;

	// returns the connection string
	CppUtils::String const& getConnection() const;

	CppUtils::Uid const& getUid() const;

	BrokerBase& getBroker();

	BrokerBase const& getBroker() const;
	
	// state of machine is contained in a session
	inline int getState() const
	{
		return state_m;
	}

	inline void setState(int const newState)
	{
		state_m = newState;
	}

	inline HlpStruct::GlobalStorage& getSessionStorage()
	{
		return sessionStorage_m;
	}

	// ---------------------------

	inline CppUtils::String const& getRunName()
	{
		return runName_m;
	}

	void setRunName(CppUtils::String const& runName);

	void setRunName(char const* runName);
	
	// ------------------------

	inline CppUtils::String const& getRunComment()
	{
		return runComment_m;
	}

	void setRunComment(CppUtils::String const& runComment);

	void setRunComment(char const* runComment);

protected:


	virtual ~BrokerSession();
			
	// used in derived classes
	BrokerSession(CppUtils::String const& connectstring, BrokerBase& brokerbase);

	
private:


	// no-op
	BrokerSession( BrokerSession const& );
	
	BrokerSession& operator = ( BrokerSession const& );

	// connection string used by derived classes
	CppUtils::String connectString_m;

	// ref to broker base
	BrokerBase& brokerBase_m;

	// state
	int state_m;

	// run name
	CppUtils::String runName_m;

	// run copmment
	CppUtils::String runComment_m;

	
	// session id
	CppUtils::Uid sessionUid_m;

	// sesison global storage
	HlpStruct::GlobalStorage sessionStorage_m;

};





}; // end of namespace

#endif


