

#ifndef _HLPSTRUCT_BOOTMGR_INCLUDED
#define _HLPSTRUCT_BOOTMGR_INCLUDED


#include "hlpstructdefs.hpp"


namespace HlpStruct {


// =======================================================
// BootManager
//
/** Helper class to bootstrap the library. Exposes a static
		boot function, which registers the configuration objects
		as root objects and which optionally starts the
		\ref ServiceThread service thread.
*/


class HLPSTRUCT_EXP BootManager {

public:

	/** Static function that registers the configuration objects
			with the root object manager. Starts the
			\ref ServiceThread service thread if the argument
			\a startServiceThread is true. Returns a dummy value. */
	static int boot ();

	
private:

	/** Helper to let the manager boot automatically when built as DLL. 
			The service thread will be started. */
	static const int foo_scm;
};




// End of namespace
};



#endif // _HLPSTRUCT_BOOTMGR_INCLUDED
