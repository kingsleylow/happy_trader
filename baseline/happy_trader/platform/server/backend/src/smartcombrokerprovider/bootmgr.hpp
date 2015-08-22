#ifndef _SMARTCOMBROKERPROVIDER_BOOTMGR_INCLUDED
#define _SMARTCOMBROKERPROVIDER_BOOTMGR_INCLUDED

#include "smartcombrokerproviderdefs.hpp"
namespace BrkLib {

class  BootManager_SmartComBrokerProvider {

private:

	/** Static function that registers the configuration objects
			with the root object manager. */
	static int boot (
	);

	/** Helper to bootstrap. */
	static const int foo_scm;
};

}; // end of namespace

#endif