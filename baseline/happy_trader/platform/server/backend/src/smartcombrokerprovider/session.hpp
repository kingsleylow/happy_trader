#ifndef _SMARTCOMBROKERPROVIDER_SESSION_INCLUDED
#define _SMARTCOMBROKERPROVIDER_SESSION_INCLUDED

#include "smartcombrokerproviderdefs.hpp"

namespace BrkLib {

	class SmartComBroker;
	class SMARTCOMBROKERPROVIDER_EXP SmartComBrokerSession: public BrokerSession {

	public:

		SmartComBrokerSession(CppUtils::String const& connectstring, SmartComBroker &broker);

		virtual ~SmartComBrokerSession();

		virtual bool isConnected() const;

				
	private:

		
		
		
	};

};

#endif