
#include "reference.hpp"

#include "misc_ht.hpp"



// We are in namespace CppUtils
namespace CppUtils {




// =======================================================
// Class Reference
// =======================================================
//======================================
// Methods
//======================================
String Reference::toString (
	) const
{
	return uid_m.toString() + "@" + homeId_m.toString() + ":" + type_m + "=" + name_m;
};
// -------------------------------------------------------
void Reference::fromString (
		String const &stringified
	)
{
	// Invalidate first
	//
	invalidate();


	// Valid string?
	//
	if( stringified == "" )
		return;


	// Scan string
	//
	long npos, pos = stringified.find_first_of( "@" );

	if( pos != 2 * Signature::length_scm )
		// No delim found or wrong position; invalid string
		//
		return;
	else {
		// Get the OID portion
		//
		String str;
		str.assign( stringified, 0, 2 * Signature::length_scm );
		uid_m.fromString( str );


		// Get the HID portion
		//
		npos = stringified.find_first_of( ":", pos + 1 );
		if( npos < 0 )
			// Ignore if not found
			return;

		str.assign( stringified, pos + 1, 2 * Signature::length_scm  );
		homeId_m.fromString( str );
		pos = npos;


		// Get the type
		//
		npos = stringified.find_first_of( "=", pos + 1 );
		if( npos < 0 )
			// Ignore if not found
			return;

		strncpy( type_m, stringified.c_str() + pos + 1, 31 );
		type_m[ min( npos - pos - 1, 31 ) ] = 0;
 


		// Get the name portion
		//
		strncpy( name_m, stringified.c_str() + npos + 1, 127 );
		name_m[ 127 ] = 0;
	}
};
// -------------------------------------------------------
Reference const &Reference::invalid (
	)
{
	static Reference ref_invalid;
	return ref_invalid;
}
// -------------------------------------------------------
Reference const &Reference::system (
	)
{
	static Reference ref_system( "513964E0EFDD11D5800000E02964B4BE@503964E0EFDD11D5800000E02964B4BE:user=System" );
	return ref_system;
}




// End of namespace
};
