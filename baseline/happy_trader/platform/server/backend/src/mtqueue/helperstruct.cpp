#include "helperstruct.hpp"
#include "mtqueuemanager.hpp"

namespace MtQueue {

		bool MqlStrHelper::copy(MqlStr* dstMqlStr, char const* srcStr)
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
		};

		void MqlStrHelper::initEmptyString(MqlStr* dstStr)
		{
			dstStr->len = 0;
			dstStr->string[0] = '\0';
		}

		


}; // end of ns