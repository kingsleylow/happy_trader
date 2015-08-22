#ifndef _BRKLIBBTEST_SESSION_INCLUDED
#define _BRKLIBBTEST_SESSION_INCLUDED

#include "brklibbtestdefs.hpp"

namespace BrkLib {

	class BRKLIBBTEST_EXP BtSession: public BrokerSession {
		friend class BtBroker;
	private:

		BtSession(CppUtils::String const& connectstring, BtBroker &broker);

		virtual ~BtSession();
		
		virtual bool isConnected() const;
		
		
	private:

		inline void connect()
		{
			isConnected_m = true;
		}

		inline void disconnect()
		{
			isConnected_m = false;
		}

		
		// whether is connected
		bool isConnected_m;

		// positions
		BrkLib::PositionList positionList_m;

		
		
	};

}; // end of namespace

#endif