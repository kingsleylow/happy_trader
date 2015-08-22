#ifndef _MTQUEUE_TRANSPORT_INCLUDED
#define _MTQUEUE_TRANSPORT_INCLUDED

#include "mtqueuedefs.hpp"
#include "helperstruct.hpp"
#include "parseurl.h"
#include "Wininet.h"

#define DEMO_VERSION 1

#ifdef DEMO_VERSION
#define MAX_REQUESTS 300
#endif

namespace MtQueue {
	class MtQueueManager;

	// -------------------

	struct SimpleDataEntry {
		SimpleDataEntry()
		{
		};

		CppUtils::String data_m;

		CppUtils::String url_m;

		CppUtils::StringMap props_m;
	};


	// -------------------------
	


	class TransportLayer
	{
	public:

		

		// ctor
		TransportLayer();
		
		// dtor
		~TransportLayer();

	
		void init(CppUtils::String const& key, CppUtils::String const& secret, CppUtils::String const btcGate);

		Result makeBtcRequest(CppUtils::String const& methodName, char const* nonce, CppUtils::String const& additionalParameters);
		
	private:

		Result sendSimpleData(SimpleDataEntry const& data);

		CppUtils::String signRequest(CppUtils::String const& requestString, CppUtils::String const& secret);

		CppUtils::String readHeaders(HINTERNET hRequest) const;

		void readData(HINTERNET hRequest, CppUtils::MemoryChunk &retVal) const;

		void readDataWithoutContentLength(HINTERNET hRequest, CppUtils::MemoryChunk &retVal) const;

		CppUtils::String key_m;

		CppUtils::String secret_m;

		CppUtils::String btcGate_m;

#ifdef DEMO_VERSION
		mutable int requestsCnt_m;
#endif

	};
};


#endif