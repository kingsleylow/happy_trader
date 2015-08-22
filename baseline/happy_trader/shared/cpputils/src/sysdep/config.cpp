


#include "../config.hpp"


namespace CppUtils {


//=============================================================
// Constants
//
/** Constant giving top-level section for configuration
		data; e.g. on Windows this is the top-level registry
		section. */
String companyName (
	)
{
	return "HAPPY TRADER";
}



//=============================================================
// Class Configuration
//=============================================================
const Char Configuration::sectionDelimiter_scm = '/';
//-------------------------------------------------------------
String const &Configuration::getRootObjectName_System(
	)
{
	static String name_s( "configuration_system" );
	return name_s;
}
//-------------------------------------------------------------
String const &Configuration::getRootObjectName_User(
	)
{
	static String name_s( "configuration_user" );
	return name_s;
}




// End of namespace
}





//=============================================================
// Specialization is OS-specific
//=============================================================
#if defined (_WIN32)

#include "win32/config.cpp"

#endif // _WIN32

