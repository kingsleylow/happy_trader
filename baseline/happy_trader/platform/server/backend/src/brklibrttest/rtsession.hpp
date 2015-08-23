#ifndef _BRKLIBRTTEST_SESSION_INCLUDED
#define _BRKLIBRTTEST_SESSION_INCLUDED

#include "brklibrttestdefs.hpp"

namespace BrkLib {

	class BRKLIBRTTEST_EXP RtSession: public BrokerSession {
		friend class RtBroker;
	private:

		RtSession(CppUtils::String const& connectstring, RtBroker &broker);

		virtual ~RtSession();
		
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