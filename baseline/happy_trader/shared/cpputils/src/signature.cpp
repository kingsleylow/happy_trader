


#include "signature.hpp"

// Needed to throw exception
#include "except.hpp"
//#include "stringtable.hpp"


namespace CppUtils {





//=============================================================
// Class Signature
//=============================================================
const Signature Signature::invalidSignature_scm = Signature();
//-------------------------------------------------------------
unsigned const int Signature::length_scm = 16;
//======================================
// Creators
//======================================
Signature::Signature (
	)
{
	// Zero out the signature
	memset( data_m, 0, length_scm );
}
//-------------------------------------------------------------
Signature::Signature (
		String const &str
	)
{
	fromString( str );
}
//======================================
// Methods
//======================================
String Signature::toString (
	) const
{
	Byte const *dat = data_m;
	Char buf[ 2 * length_scm + 1 ];

	Char *ptr = buf;

	// Write Signature in hexcode
	for(int i=0; i<length_scm; ++i, ++dat) {
		// High byte
		*ptr = (*dat>>4 & 0x0f);
		*ptr += (*ptr < 0x0a ? '0' : 'A'-10);
		ptr++;

		// Low byte
		*ptr = (*dat & 0x0f);
		*ptr += (*ptr < 0x0a ? '0' : 'A'-10);
		ptr++;
	}

	*ptr = 0;

	return buf;
}
//-------------------------------------------------------------
void Signature::fromString (
		String const &stringifiedRep
	) throw (
		ExceptionInternal
	)
{
	if (!fromStringNoExceptHelper(stringifiedRep))
		THROW( BadArgument, "util_exc_MalformedSignature", "", stringifiedRep );
}

// ------------------------------------------------------------

bool Signature::fromStringNoExceptHelper (
		String const &stringifiedRep
)
{
	// Correct size?
	if( stringifiedRep.length() != 2*length_scm )
		return false;

	// Go through string
	String::const_iterator ptr = stringifiedRep.begin();
	Byte *dat = data_m;
	Byte buf;

	for(int i=0; i<length_scm; ++i, ++dat) {
		// High byte
		buf = *ptr - (*ptr > '9' ? (*ptr >= 'a' ? 'a'-10 : 'A'-10) : '0');
		if( buf < 0 || buf > 0x0f )
			return false;

		*dat = (buf & 0x0f) << 4;
		++ptr;

		// Low byte
		buf = *ptr - (*ptr > '9' ? (*ptr >= 'a' ? 'a'-10 : 'A'-10) : '0');
		if( buf < 0 || buf > 0x0f )
			return false;

		*dat |= (buf & 0x0f);
		++ptr;
	}

	return true;
}



//=============================================================
// Operators for class Signature
//=============================================================
ostream & operator << (
		ostream &stream,
		Signature const &sig
	)
{
	return stream << sig.toString();
}
//-------------------------------------------------------------
istream & operator >> (
		istream &stream,
		Signature &sig
	) throw (
		ExceptionInternal
	)
{
	// Get the string
	String string;
	stream >> string;

	// Try to convert
	sig.fromString( string.c_str() );

	// Done
	return stream;
}



// End of namespace
}
