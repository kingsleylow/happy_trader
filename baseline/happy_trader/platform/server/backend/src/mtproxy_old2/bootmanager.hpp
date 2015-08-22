#ifndef _MTPROXY_BOOTMANAGER_INCLUDED
#define _MTPROXY_BOOTMANAGER_INCLUDED



#include "mtproxydefs.hpp"


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