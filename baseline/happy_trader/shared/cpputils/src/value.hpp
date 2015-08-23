
#ifndef _CPPUTILS_VALUE_INCLUDED
#define _CPPUTILS_VALUE_INCLUDED


#include "cpputilsdefs.hpp"

#include "chunk.hpp"
#include "uid.hpp"
#include "reference.hpp"




namespace CppUtils {


//=============================================================
// Class Value
//=============================================================
/** Helper class to provide a type-safe value that can encapsulate
		different types. This class can be understood as a union with an
		additional type indicator and memory management. Supported types are:

		- Int
		- Double
		- Date
		- String
		- Uid

		and arrays of these types.


		\author Olaf Lenzmann
*/
class CPPUTILS_EXP Value {

public:

	//======================================
	// Typedefs

	/** Enumeration listing the supported types of values. Lists of values
			are defined by or-in the scalar type and VT_List. */
	enum ValueType {
		VT_None      = 0x0000,
		VT_Int       = 0x0001,
		VT_Double    = 0x0002,
		VT_Date      = 0x0004,
		VT_String    = 0x0008,
		VT_Uid       = 0x0010,
		VT_Reference = 0x0020,
		VT_List          = 0x1000,
		VT_IntList       = 0x1001,
		VT_DoubleList    = 0x1002,
		VT_DateList      = 0x1004,
		VT_StringList    = 0x1008,
		VT_UidList       = 0x1010,
		VT_ReferenceList = 0x1020,
		VT_StringMap = 0x1040

		
	};



	//======================================
	// Con- & Destructor

	/** Standard ctor. */
	Value (
	);

	/** Copy ctor. */
	Value (
		Value const &
	);

	/** Destructor. Frees memory. */
	~Value(
	);



	//======================================
	// Management

	/** Retrieve the type of value stored. */
	ValueType getType (
	) const;

	/** List the type of value stored, discarding previous value
			unless the type does not change. */
	void setType (
		ValueType type
	);

	/** Frees any allocated memory and sets the
			type of the value to VT_None. */
	void clear (
	);

	/** Return the contents of the object as a blob. */
	void toBlob (
		MemoryChunk &blob
	) const;

	/** Read the contents of the object from a blob,
			assuming the given type. */
	void fromBlob (
		ValueType type,
		const Byte *blob
	);

	/** Add the content from another value; ignored if the argument
			is of different type; addition semantics depends on type. */
	void add (
		Value const &rhs
	);



	//======================================
	// Accessors

	/** Retrieve reference to the value if the type is VT_Int.

			\attention The behaviour is undefined if the
			type is not VT_Int.
	*/
	int const & getAsInt (
	) const;

	/** Retrieve reference to the value if the type is VT_Int.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Int.
	*/
	int & getAsInt (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Int | VT_ | VT_List.

			\attention The behaviour is undefined if the
			type is not VT_Int | VT_List.
	*/
	IntList const & getAsIntList (
	) const;

	/** Retrieve reference to the value if the type is VT_Int | VT_ | VT_List.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Int | VT_List.
	*/
	IntList & getAsIntList (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Double.

			\attention The behaviour is undefined if the
			type is not VT_Double.
	*/
	double const & getAsDouble (
	) const;

	/** Retrieve reference to the value if the type is VT_Double.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Double.
	*/
	double & getAsDouble (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Double | VT_List.

			\attention The behaviour is undefined if the
			type is not VT_Double | VT_List.
	*/
	DoubleList const & getAsDoubleList (
	) const;

	/** Retrieve reference to the value if the type is VT_Double | VT_List.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Double | VT_List.
	*/
	DoubleList & getAsDoubleList (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Date.

			\attention The behaviour is undefined if the
			type is not VT_Date.
	*/
	double const & getAsDate (
	) const;

	/** Retrieve reference to the value if the type is VT_Date.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Date.
	*/
	double & getAsDate (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Date | VT_List.

			\attention The behaviour is undefined if the
			type is not VT_Date | VT_List.
	*/
	DoubleList const & getAsDateList (
	) const;

	/** Retrieve reference to the value if the type is VT_Date | VT_List.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Date | VT_List.
	*/
	DoubleList & getAsDateList (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_String.

			\attention The behaviour is undefined if the
			type is not VT_String.
	*/
	String const & getAsString (
	) const;

	/** Retrieve reference to the value if the type is VT_String.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_String.
	*/
	String & getAsString (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_String | VT_List.

			\attention The behaviour is undefined if the
			type is not VT_String | VT_List.
	*/
	StringList const & getAsStringList (
	) const;

	/** Retrieve reference to the value if the type is VT_String | VT_List.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_String | VT_List.
	*/
	StringList & getAsStringList (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Uid.

			\attention The behaviour is undefined if the
			type is not VT_Uid.
	*/
	Uid const & getAsUid (
	) const;

	/** Retrieve reference to the value if the type is VT_Uid.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Uid.
	*/
	Uid & getAsUid (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Uid | VT_List.

			\attention The behaviour is undefined if the
			type is not VT_Uid | VT_List.
	*/
	UidList const & getAsUidList (
	) const;

	/** Retrieve reference to the value if the type is VT_Uid | VT_List.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Uid | VT_List.
	*/
	UidList & getAsUidList (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Reference.

			\attention The behaviour is undefined if the
			type is not VT_Reference.
	*/
	Reference const & getAsReference (
	) const;

	/** Retrieve reference to the value if the type is VT_Reference.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Reference.
	*/
	Reference & getAsReference (
		bool setType = false
	);

	/** Retrieve reference to the value if the type is VT_Reference | VT_List.

			\attention The behaviour is undefined if the
			type is not VT_Reference | VT_List.
	*/
	ReferenceList const & getAsReferenceList (
	) const;

		/** Retrieve reference to the value if the type is VT_Reference | VT_List.
			If \a setType is true, the type is set automatically if needed and the
			value is initialized; if the type was already set, the value is not
			changed.

			\attention The behaviour is undefined if the
			type is not VT_Reference | VT_List.
	*/
	ReferenceList & getAsReferenceList (
		bool setType = false
	);

	/** 
	*/

	StringMap const & getAsStringMap(
	) const;

	/**
	*/

	StringMap & getAsStringMap (
		bool setType = false
	);




	//======================================
	// Operators

	/** Assignment operator. */
	Value & operator = (
		Value const &
	);

	/** Equality operator. */
	bool operator == (
		Value const &
	) const;



private:


	//======================================
	// Variables

	/** Type that the value holds. */
	ValueType type_m;

	/** Union of possible values. */
	union {
		int intVal;
		IntList *intListPtrVal;
		double doubleVal;
		DoubleList *doubleListPtrVal;
		String *stringPtrVal;
		StringList *stringListPtrVal;
		Uid *uidPtrVal;
		UidList *uidListPtrVal;
		Reference *refPtrVal;
		ReferenceList *refListPtrVal;
		StringMap* stringMapPtrVal;

	} value_m;

};




//=============================================================
// Typedefs
//

/** List of values. */
typedef vector<Value> ValueList;

/** Dictionary of values, keys by strings. */
typedef map<String, Value> ValueMap;



// End of namespace
}


#endif // _CPPUTILS_VALUE_INCLUDED

