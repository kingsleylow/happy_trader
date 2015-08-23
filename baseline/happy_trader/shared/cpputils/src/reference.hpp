
/** \file

		Header file declaring class Reference.

	
*/



#ifndef _CPPUTILS_REFERENCE_INCLUDED
#define _CPPUTILS_REFERENCE_INCLUDED


#include "cpputilsdefs.hpp"
#include "uid.hpp"


namespace CppUtils {


// =======================================================
// Forward decl
class Reference;


// =======================================================
// Typedefs
//

/** Shorthand notation. */
typedef block_allocated_set<Reference, 16> ReferenceSet;

/** Shorthand notation. */
typedef vector<Reference> ReferenceList;

/** Shorthand notation. */
typedef vector<Reference *> ReferencePtrList;

/** Shorthand notation. */
typedef block_allocated_map<Reference, Reference, 16> ReferenceMap;

/** Shorthand notation. */
typedef vector<ReferenceList> ReferenceListList;

/** Shorthand notation. */
typedef block_allocated_map<Reference, ReferenceList, 16> ReferenceListMap;

/** Shorthand notation. */
typedef block_allocated_map<Reference, ReferenceMap, 16> ReferenceMapMap;




// =======================================================
// Class Reference
//
/** Class representing references to objects; each reference
		consists of a unique identifier (\ref Uid), and

		- a string type description
		- a uid identifying the home of the object (home identifier, HID).
		- a human-readable name of the referenced object.

	  Instances of this class can be stringified and de-stringified
	  for marshalling or storage; for better performance, the stringified
	  representation is cached by the object.

		\attention Any two references with the same uid are considered
		to refer to the same object.


		\author Olaf Lenzmann
*/
class CPPUTILS_EXP Reference {

public:

	//======================================
	// Creators

	/** Standard ctor, initializing with invalid uid and
			no optional data. */
	Reference (
	) :
		uid_m(),
		homeId_m()
	{
		name_m[ 0 ] = type_m[ 0 ] = 0;
	}

	/** Standard ctor, taking uid, type, home and name as arguments. */
	explicit Reference (
		Uid const &uid,
		Uid const &home = Uid(),
		String const &type = "",
		String const &name = ""
	) :
		uid_m( uid ),
		homeId_m( home )
	{
		setName( name );
		setType( type );
	}

	/** Standard ctor, taking stringified reference
			as produced by toCString() as argument. */
	explicit Reference (
		String const &stringified
	) :
		uid_m(),
		homeId_m()
	{
		fromString( stringified );
	}




	//======================================
	// Methods

	/** Obtain a stringified representation of the object,
			encapsulating all member data. */
	String toString (
	) const;

	/** Reconstruct the object from the given stringified
			representation, as obtained from toCString(). */
	void fromString (
		String const &stringified
	);

	/** Check if the reference is valid, that is if the
			uid is valid. */
	bool isValid (
	) const
	{
		return uid_m.isValid();
	}


	/** Invalidate the reference by invalidating the uid
			and setting the optional data as not present. */
	void invalidate (
	)
	{
		uid_m.invalidate();
		homeId_m.invalidate();
		name_m[ 0 ] = type_m[ 0 ] = 0;
	}





	//======================================
	// Accessors

	/** Access the uid. */
	Uid const & getUid (
	) const
	{
		return uid_m;
	}

	/** Access the uid for write access. */
	Uid & getUid (
	)
	{
		return uid_m;
	}

	/** Access the home. */
	Uid const & getHomeId (
	) const
	{
		return homeId_m;
	}

	/** Access the home for write access. */
	Uid & getHomeId (
	)
	{
		return homeId_m;
	}

	/** Get the name. */
	const Char * getName (
	) const
	{
		return name_m;
	}

	/** Set the name. */
	void setName (
		String const &name
	)
	{
		strncpy( name_m, name.c_str(), 127 );
		name_m[ 127 ] = 0;
	}

	/** Set the name. */
	void setName (
		const Char *name
	)
	{
		strncpy( name_m, name, 127 );
		name_m[ 127 ] = 0;
	}

	/** Get the type. */
	const Char * getType (
	) const
	{
		return type_m;
	}

	/** Set the type. */
	void setType (
		String const &type
	)
	{
		strncpy( type_m, type.c_str(), 31 );
		type_m[ 31 ] = 0;
	}

	/** Set the type. */
	void setType (
		const Char *type
	)
	{
		strncpy( type_m, type, 31 );
		type_m[ 31 ] = 0;
	}



	//======================================
	// Operators

	/** Comparison operator.

			\attention Compares only uids.
	*/
	bool operator == (
		Reference const &rhs
	) const
	{
		return (uid_m == rhs.uid_m);
	};


	/** Comparison operator.

			\attention Compares only uids.
	*/
	bool operator != (
		Reference const &rhs
	) const
	{
		return (uid_m != rhs.uid_m);
	};


	/** Comparison operator.

			\attention Compares only uids.
	*/
	bool operator < (
		Reference const &rhs
	) const
	{
		return (uid_m < rhs.uid_m);
	};


	/** Assignment operator not implemented; compiler can do this better. */


	/** Invalid reference. */
	static Reference const &invalid (
	);

	/** System reference. */
	static Reference const &system (
	);



protected:

	//======================================
	// Variables

	/** The unique identifier. */
	Uid uid_m;

	/** The home identifier. */
	Uid homeId_m;

	/** The name. */
	Char name_m[128];

	/** The type. */
	Char type_m[32];
};




// =======================================================
// Helper
//

// -------------------------------------------------------
/** Helper to convert a reference list into a uid list. */
inline void refl2uidl(
		ReferenceList const &refl,
		UidList &uidl
	)
{
	uidl.resize( refl.size() );
	for(int i = 0; i < (int) uidl.size(); ++i)
		uidl[ i ] = refl[ i ].getUid();
}

// -------------------------------------------------------
/** Helper to convert a uid list into a reference list. */
inline void uidl2refl(
		UidList const &uidl,
		ReferenceList &refl
	)
{
	refl.resize( uidl.size() );
	for(int i = 0; i < (int) uidl.size(); ++i) {
		refl[ i ].invalidate();
		refl[ i ].getUid() = uidl[ i ];
	}
}





// End of namespace
};



#endif // _CPPUTILS_REFERENCE_INCLUDED
