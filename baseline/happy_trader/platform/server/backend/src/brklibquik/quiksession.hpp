#ifndef _BRKLIBQUIK_QUIKSESSION_INCLUDED
#define _BRKLIBQUIK_QUIKSESSION_INCLUDED

#include "brklibquikdefs.hpp"


namespace BrkLib {


	/**
	It uses trans2quik_api.h and TRANS2QUIK.dll
	Thus as the connection string we will use the path to Quik executable and the name of DDE server
	*/
	class BRKLIBQUIK_EXP QuikSession: public BrokerSession {
		friend class QuikBroker;
	private:

		QuikSession(CppUtils::String const& connectstring, QuikBroker &broker);

		virtual ~QuikSession();
		
		virtual bool isConnected() const;

		
		
	private:

		// helper functions which directly calls QUIK API
		// they are mutexed and must be used in broker implementation
		bool h_isConnected() const;

		void h_disconnectDll();

		void h_connectDll();

		void h_asynchTransact(CppUtils::String const& transactString);
		

		// positions - the list of positions supported by QUIK instance via DDE exchange
		BrkLib::PositionList positionList_m;

		// static global mutex to secure QUIK API calls
		// as we have a single library need to mutex it (though for some calls may be not necessary)
		static CppUtils::Mutex quikApiMtx_m;

		CppUtils::String const& pathToQuik_m;
		
	};

};

#endif