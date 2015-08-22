


#include "uid.hpp"
#include "log.hpp"
#include "except.hpp"

#include <assert.h>
#include "mutex.hpp"


namespace CppUtils {




//=============================================================
// Class Uid
//=============================================================
const Uid Uid::invalidUid_scm = Uid();
//-------------------------------------------------------------
Uid::Uid (
	)
{
}
//-------------------------------------------------------------
Uid::Uid (
		String const &arg
	) :
		Signature( arg )
{
}
//======================================
// Methods
//======================================
void Uid::generate (
	)
{
	// Create fresh UID
	generateGuid( data() );
}

// ------------------------------------------------------------

void Uid::fromStringNoExcept(
	String const& arg															
)
{	
	if (!fromStringNoExceptHelper( arg ))
		invalidate();
}

//-------------------------------------------------------------
Uid Uid::generateUid (
	)
{
	Uid uid;
	uid.generate();

	return uid;
}





//=============================================================
// Operators for class Uid
//=============================================================
ostream & operator << (
		ostream &stream,
		Uid const &obj
	)
{
	return stream << obj.toString();
}
//-------------------------------------------------------------
istream & operator >> (
		istream &stream,
		Uid &obj
	) throw (
		ExceptionInternal
	)
{
	// Get the string
	String string;
	stream >> string;

	// Try to convert
	obj.fromString( string.c_str() );

	// Done
	return stream;
}


// End of namespace
}
