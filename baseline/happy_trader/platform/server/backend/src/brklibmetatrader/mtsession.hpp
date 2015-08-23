#ifndef _BRKLIBMETATRADER_SESSION_INCLUDED
#define _BRKLIBMETATRADER_SESSION_INCLUDED

#include "brklibmetatraderdefs.hpp"

namespace BrkLib {

	class BRKLIBMETATRADER_EXP MtSession: public BrokerSession {
		friend class MtBroker;
	private:

		MtSession(CppUtils::String const& connectstring, MtBroker &broker);

		virtual ~MtSession();
		
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