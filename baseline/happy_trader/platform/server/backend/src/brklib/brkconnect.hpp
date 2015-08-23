#ifndef _BRKLIB_BRKCONNECT_INCLUDED
#define _BRKLIB_BRKCONNECT_INCLUDED

#include "brklibdefs.hpp"
#include "brokerstruct.hpp"
#include "brokersession.hpp"



namespace BrkLib {
/**
 This is the interface class that provides the ONLY interface to work with broker.
 It incapsulates connection agains broker
*/
	class BrokerBase;
	class BRKLIB_EXP BrokerConnect {
		friend class BrokerOperation;
	public:
		BrokerConnect(
			BrokerBase* brokerBase,
			const char* connectString,
			bool const autodestroy,
			bool const autodisconnect,
			char const* runName,
			char const* comment
		);

		// construct from session
		// just wrapper
		BrokerConnect(
			BrkLib::BrokerSession &session,
			bool const autodestroy,
			bool const autodisconnect
		);

		// default ctor for serialization
		BrokerConnect();

		// dtor
		~BrokerConnect();

		// connects to broker layer
		void connect();

		// disconnect from broker layer
		void disconnect();

		// copy operator - to initialize from existing sources
		BrokerConnect& operator = (BrokerConnect const& src);

		// returns session object
		inline BrokerSession& getSession()
		{
			return *brkSessionPtr_m;
		}

	

	private:
	
		// ptr to broker base object
		// can be NULL
		BrokerBase* brkBasePtr_m;

		// broker session object
		BrokerSession* brkSessionPtr_m;

		// if destroy BrokerSession object when out of scopy
		bool autoDestroy_m;
		
		// if disconnect from BrokerSession when out of scopy
		bool autoDisconnect_m;


	};
};


#endif