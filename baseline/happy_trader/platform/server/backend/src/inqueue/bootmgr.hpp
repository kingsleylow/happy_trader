#ifndef _BACKEND_INQUEUE_BOOTMGR_INCLUDED
#define _BACKEND_INQUEUE_BOOTMGR_INCLUDED


#include "inqueuedefs.hpp"


namespace Inqueue {


// =======================================================
// BootManager
//
/** Helper class to bootstrap the library. Automatically
		registers root object instances of classes

		- DomainEventLogger
		- SessionResolver


*/
class INQUEUE_BCK_EXP BootManager {

private:

	/** Static function that registers the configuration objects
			with the root object manager. */
	static int boot (
	);

	/** Helper to bootstrap. */
	static const int foo_scm;
};




// End of namespace
};



#endif // _BACKEND_ADMIN_BOOTMGR_INCLUDED
