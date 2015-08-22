#ifndef _MTQUEUE_HELPERSTRUCT_INCLUDED
#define _MTQUEUE_HELPERSTRUCT_INCLUDED

#include "mtqueuedefs.hpp"



#define MTQUEUE_VERSION "2.0.6"


// MT specific definitions
#pragma pack(push,1)
struct RateInfo
  {
   unsigned int      ctm;
   double            open;
   double            low;
   double            high;
   double            close;
   double            vol;
  };
#pragma pack(pop)




struct MqlStr
{
   int len;
   char* string;
};



namespace MtQueue {


	class MqlStrHelper
	{
	public:
		
		// retursns the number of copied
		static bool copy(MqlStr* dstMqlStr, char const* srcStr);
		
	private:
		static void initEmptyString(MqlStr* dstStr);

	};
	

	// ----------------------
	class Result
		{
			
		public:
			enum ResultCode
			{
				RC_OK = 0,
				RC_GENERAL_FAIL = -1
			};
			
			Result():
				code_m(RC_OK),
				readStringPtr_m(-1)
			{
			}

			Result(Result const& rhs):
				code_m(rhs.code_m),
				message_m(rhs.message_m),
				data_m(rhs.data_m),
				dataString_m(rhs.dataString_m)
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

			inline CppUtils::String& strData()
			{
				return dataString_m;
			}

			inline int& getReadStringPtr()
			{
				return readStringPtr_m;
			}

			// initializes string buffer and clears memory chunk
			void convertToStringResponse()
			{
				Result::initStringFromMemoryChunk(data_m, dataString_m);
				data_m.length(0);
			}

			static void initStringFromMemoryChunk(CppUtils::MemoryChunk const& srcMc, CppUtils::String& dstStr)
			{
				if (srcMc.length() == 0) {
					dstStr.clear();
					return;
				}

				// check the last symbol in srcMx

				CppUtils::Byte* lastSymbPtr = (CppUtils::Byte*)srcMc.data() + srcMc.length() - 1;
				if (*lastSymbPtr == '\0') {
					// just assign null terminated string
					dstStr = srcMc.data();
					
				}
				else {
					
					char* buffer = new char[srcMc.length()+1];

					// need to allocate and expliciteuy set
				
					strncpy(buffer, srcMc.data(), srcMc.length() );
					buffer[srcMc.length()] = '\0';
					dstStr = buffer;

					delete [] buffer;
				}

			
			}
			
			
		private:

			int code_m;
			CppUtils::String message_m;
			CppUtils::MemoryChunk data_m;

			// string response if we expect a string
			CppUtils::String dataString_m;

			int readStringPtr_m;
		};


	// ----------------------

	class HandlerBase
	{
	public:
		HandlerBase(): id_m(-1)
		{
		}

		virtual ~HandlerBase()
		{
			
		}

		virtual bool isValid() const = 0;

		// clear resources and invalidate
		virtual void clear() = 0;

		inline int getId() const
		{
			return id_m;
		}
		
		inline void setId(int id)
		{
			id_m = id;
		}

	protected:
		

	private:
		
		// private
		HandlerBase(HandlerBase const& rhs);

		

		int id_m;
	};


	


}; // end of ns

#endif