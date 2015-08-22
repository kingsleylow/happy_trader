#ifndef _MTQUEUE_BOOTMANAGER_INCLUDED
#define _MTQUEUE_BOOTMANAGER_INCLUDED



#include "mtqueuedefs.hpp"


namespace MtQueue {
	class BootManager
	{
	public:
		static int boot();
		
	private:
		static const int foo_scm;
	};
};

#endif