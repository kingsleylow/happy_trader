#include "bootmgr.hpp"

namespace Inqueue {

// =======================================================
// BootManager
//
const int BootManager::foo_scm = BootManager::boot();
//-------------------------------------------------------------
int BootManager::boot (
	)
{
	// Create objects which register themselves
	//new EventLoggerProxy();
	//new SessionResolverProxy();

	// Done
	return 42;
}




// End of namespace
};

