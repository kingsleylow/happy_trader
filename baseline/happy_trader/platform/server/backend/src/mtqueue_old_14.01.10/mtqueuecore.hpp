#ifndef _MTQUEUE_MTPROXYCORE_INCLUDED
#define _MTQUEUE_MTPROXYCORE_INCLUDED

#include "mtqueuedefs.hpp"
#include "mtqueuemanager.hpp"

// ------------------------
// constants

#define DLL_FUN_CALL_OK 0
#define DLL_FUN_CALL_FAILURE -1

#define DLL_FUN_CALL_TRUE  99999
#define DLL_FUN_CALL_FALSE  33333



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



#pragma pack(push,1)
struct MqlStr
{
   int len;
   char* string;
};
#pragma pack(pop)



namespace MtQueue {

	class MqlStrHelper
	{
	public:
	
		
		// retursns the number of copied
		static bool copy(MqlStr* dstMqlStr, char const* srcStr)
		{
			
			if (dstMqlStr && dstMqlStr->string){
				size_t dstBufSize =  strlen(dstMqlStr->string);
				if (dstBufSize > 0) {

					if (srcStr) {
					
						size_t scrStrLen = strlen(srcStr);
						if (scrStrLen ==0) {
							initEmptyString(dstMqlStr);
							// because empty string
							return true;
						}
						else {
							// copy at max dstBufSize
							strncpy(dstMqlStr->string, srcStr, dstBufSize );
							return (scrStrLen <= dstBufSize);
						}

						
					} else {
						initEmptyString(dstMqlStr);
						// because empty string
						return true;
					}
				
				}

			}

			return false;
		}

		

	private:
		static void initEmptyString(MqlStr* dstStr)
		{
			dstStr->len = 0;
			dstStr->string[0] = '\0';
		}

	};
	



}; // end of ns


extern "C" {


MTQUEUE_EXP int PROCCALL	sayHallo();

MTQUEUE_EXP int PROCCALL	initialize(int const logLevel, int const workThreadsCount);

MTQUEUE_EXP int PROCCALL	deinitialize();

MTQUEUE_EXP int PROCCALL	pushRemoveImmediately(char const* url, char const* data, char const* filename, int repeatCount);

MTQUEUE_EXP int PROCCALL	pushNeverRemove(char const* url, char const* data, char const* filename, int repeatCount);

MTQUEUE_EXP int PROCCALL	pushRemoveAfterFailsNumber(char const* url, char const* data, char const* filename, int failNumber, int repeatCount);

MTQUEUE_EXP int PROCCALL	rbt_getInfo(MqlStr* outData, char const* key, char const* secret);

}

#endif