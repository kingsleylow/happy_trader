
#ifndef _CPPUTILS_SIGNATURE_INCLUDED
#define _CPPUTILS_SIGNATURE_INCLUDED


#include "cpputilsdefs.hpp"



// For typedefs
#include <set>
#include <vector>


namespace CppUtils {


// Forward decl
class Signature;

/** Array of signature objects. */
typedef vector<Signature> SignatureList;

/** Set of signature objects. */
typedef block_allocated_set<Signature, 100> SignatureSet;



//=============================================================
// Class Signature
//=============================================================
/** Class Signature is used to represent bitstring of 128 bits
		that can be used as id, hashcode etc. The class provides
		functions to convert itself to/from a hexadecimal string
		representation and it offers comparison operators, == and <.

		
*/
class CPPUTILS_EXP Signature {

public:

	//======================================
	// Creators

	/** Standard ctor. Zeros out the bitfield. */
	Signature (
	);

	/** Construct signature from hexadecimal string representation
			by calling Signature::fromCString(). */
	explicit Signature (
		String const &
	);


	//======================================
	// Methods

	/** Function to convert the signature into a string of hexadecimal 
			digits. The length of the string will be 32 characters. */
	String toString (
	) const;

	/** Function to parse the signature from the hexadeicmal
			string representation passed in.

			\attention BadArgument is raised if the string
			is not a valid signature representation. 
	*/
	void fromString (
		String const &
	) throw (
		ExceptionInternal
	);

	/** Function to parse the signature from the hexadeicmal
			string representation passed in.
			
			\attention Raise no exception
	*/
	bool fromStringNoExceptHelper (
		String const &stringifiedRep
	); 

	/** Read the signature from the given memory address as 16 bytes. */
	void fromMemory (
			void const *arg
		)
	{
		memcpy( data_m, arg, length_scm );
	}

	/** Access the bitfield. Resets stringified flag. */
	Byte * data (
		)
	{
		return data_m;
	}

	/** Access the bitfield. */
	const Byte * data (
		) const
	{
		return data_m;
	}

	/** Invalidate by zeroing out the entire signature. */
	void invalidate (
		)
	{
		memset( data_m, 0, length_scm );
	}


	/** Check if the signature is valid, i.e. if it is not all zeros. */
	bool isValid (
		) const
	{
		return !!memcmp( data_m, invalidSignature_scm.data_m, length_scm );
	}


	//======================================
	// Operators

	/** Equality operator. */
	bool operator == (
			Signature const &rhs
		) const 
	{
		return !memcmp( data_m, rhs.data_m, length_scm );
	}

	/** Inequality operator. */
	bool operator != (
			Signature const &rhs
		) const
	{
		return !!memcmp( data_m, rhs.data_m, length_scm );
	}

	/** Less than operator. Only required for STL containers as sets. */
	bool operator < (
			Signature const &rhs
		) const
	{
		return (memcmp( data_m, rhs.data_m, length_scm ) < 0); 
	}

	/** Greater than operator. */
	bool operator > (
			Signature const &rhs
		) const
	{
		return (memcmp( data_m, rhs.data_m, length_scm ) > 0); 
	}

	/** Assignment operator not implemented: the compiler may do this better. */


	//======================================
	// Variables

	/** Static member holding the length of the bitfield in bytes (16). */
	static const unsigned int length_scm;


private:

	//======================================
	// Variables
	//
	/** 16 byte long bit field holding the signature. */
	Byte data_m[ 16 ];


	/** Invalid signature prototype. */
	static const Signature invalidSignature_scm;
};




//=============================================================
// Operators for class Signature
//=============================================================

/** Output operator for class Signature. */
CPPUTILS_EXP ostream & operator << (
		ostream &,
		Signature const &
	);

/** Input operator for class Signature.

		\attention Raises ExceptionInternal if the stream does
		not contain a valid stringified signature representation.
*/
CPPUTILS_EXP istream & operator >> (
		istream &,
		Signature &
	) throw (
		ExceptionInternal
	);



// End of namespace
}


#endif // _CPPUTILS_SIGNATURE_INCLUDED
