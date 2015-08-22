/** \file

		Header file declaring class Uid which represents globally
		unique identifiers.
*/


#ifndef _CPPUTILS_UID_INCLUDED
#define _CPPUTILS_UID_INCLUDED


#include "cpputilsdefs.hpp"
#include "signature.hpp"


namespace CppUtils {


// Forward decl
//
class Uid;

/** Array of uids. */
typedef vector<Uid> UidList;

/** Set of uids. */
typedef block_allocated_set<Uid, 16> UidSet;

/** Map of uids. */
typedef block_allocated_map<Uid, Uid, 16> UidMap;

/** Dictionary of uids, keyed by a string. */
typedef block_allocated_map<String, Uid, 16> UidStringMap;

/** Dictionary of uid lists, keyed by uid. */
typedef block_allocated_map<Uid, UidList, 16> UidListMap;

/** List of uid maps. */
typedef vector<UidMap> UidMapList;

/** Well... */
typedef block_allocated_map<Uid, UidMap, 16> UidMapMap;




//=============================================================
// Class Uid
//=============================================================
/** \anchor UID

		Class Uid represents a unique identifier that unambigously
		qualifies an object or data entity. UIDs are created as
		128-bit numbers according to IETF draft found in
		<draft-leach-uuids-guids-01.txt>.

		Instances of class Uid own an instance of class Signature
		so they can convert themselves to/from a string representation,
		which is a 16-char hexadecimal format.

		An uid is considered invalid, if it contains all zeroes.

*/
class CPPUTILS_EXP Uid : public Signature {

public:

	//======================================
	// Creators

	/** Standard ctor. Initializes as invalid UID. */
	Uid (
	);

	/** Ctor initializing the object with the uid given by the
			argument string \a arg. Calls Uid::fromCString().

			\attention BadArgument is raised if the
			string is not a valid stringified uid representation.
	*/
	explicit Uid (
		String const &arg
	);



	//======================================
	// Methods

	/** Generate a fresh uid which will be globally unique. */
	void generate (
	);

	/** Generate a fresh uid which will be globally unique. */
	static Uid generateUid (
	);

	/** Convert UID from string but raise no exception */
	void fromStringNoExcept(
		String const& arg		
	);

	
	//======================================
	// Constants

	/** Invalid all zero uid. */
	static const Uid invalidUid_scm;
};




//=============================================================
// Operators for class Uid
//=============================================================
/** Output operator for class Uid. */
CPPUTILS_EXP ostream & operator << (
		ostream &,
		Uid const &
	);

/** Input operator for class Uid.

		\attention Raises ExceptionInternal if the stream does
		not contain a valid stringified uid representation.
*/
CPPUTILS_EXP istream & operator >> (
		istream &,
		Uid &
	) throw (
		ExceptionInternal
	);





// End of namespace
}



#endif // _CPPUTILS_UID_INCLUDED
