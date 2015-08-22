#ifndef _MTQUEUE_TRANSPORT_INCLUDED
#define _MTQUEUE_TRANSPORT_INCLUDED

#include "mtqueuedefs.hpp"
#include "helperstruct.hpp"
#include "parseurl.h"
#include "Wininet.h"

namespace MtQueue {
	class MtQueueManager;



	


	class TransportLayer
	{
	public:

		class Result
		{
			
		public:
			enum ResultCode
			{
				RC_OK = 0,
				RC_GENERAL_FAIL = -1
			};
			
			Result():
				code_m(RC_OK) 
			{
			}
			
			Result(int const code, CppUtils::String const& message):
				code_m(code), message_m(message)
			{
			}

			void setUp(int const code, CppUtils::String const& message)
			{
				message_m = message;
				code_m = code;
			}

			int inline getCode() const { 
				return code_m; 
			};

			CppUtils::String const& getMessage() const {
				return message_m; 
			};

			inline bool isOk() const 
			{
				return code_m == RC_OK;
			}

			inline CppUtils::MemoryChunk& data()
			{
				return data_m;
			}
		private:

			int code_m;
			CppUtils::String message_m;
			CppUtils::MemoryChunk data_m;
		};

		TransportLayer(MtQueueManager& base);
		
		~TransportLayer();

		Result sendData(DataToSend const& data);

		Result sendSimpleData(SimpleDataEntry const& data);
	private:

		CppUtils::String readHeaders(HINTERNET hRequest) const;

		CppUtils::MemoryChunk readData(HINTERNET hRequest) const;


		MtQueueManager& base_m;
	};
};


#endif