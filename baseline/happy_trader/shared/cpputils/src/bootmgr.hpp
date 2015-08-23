
#ifndef _CPPUTILS_BOOTMGR_INCLUDED
#define _CPPUTILS_BOOTMGR_INCLUDED


#include "cpputilsdefs.hpp"


namespace CppUtils {


// =======================================================
// BootManager
//
/** Helper class to bootstrap the library. Exposes a static
		boot function, which registers the configuration objects
		as root objects and which optionally starts the
		\ref ServiceThread service thread.

		\author Olaf Lenzmann
*/


class CPPUTILS_EXP BootManager {

public:

	/** Static function that registers the configuration objects
			with the root object manager. Starts the
			\ref ServiceThread service thread if the argument
			\a startServiceThread is true. Returns a dummy value. */
	static int boot (
		bool startServiceThread = false
	);

	
private:

	/** Helper to let the manager boot automatically when built as DLL. 
			The service thread will be started. */
	static const int foo_scm;
};




// End of namespace
};



#endif // _CPPUTILS_BOOTMGR_INCLUDED
