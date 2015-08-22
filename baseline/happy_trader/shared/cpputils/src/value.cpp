
#include "value.hpp"
#include "assertdef.hpp"
#include "log.hpp"

namespace CppUtils {


//=============================================================
// Class Value
//=============================================================
Value::Value ( 
	) :
		type_m ( VT_None )

{
	value_m.stringPtrVal = NULL;
}
//-------------------------------------------------------------
Value::Value (
		Value const &rhs
	) :
		type_m ( VT_None )
{
	// Use assignment operator
	*this = rhs;
}
//-------------------------------------------------------------
Value::~Value(
	)
{
	clear();
}
//======================================
// Management
//======================================
Value::ValueType Value::getType (
	) const
{
	return type_m;
}
//-------------------------------------------------------------
void Value::setType (
		Value::ValueType type
	)
{
	// New type?
	//
	if( type == type_m )
		// No; bail out
		return;


	// Get rid of previous data
	//
	clear();


	// List new type
	//
	type_m = type;


	// Allocate data if needed
	//
	switch( type_m ) {
		case VT_IntList:
			value_m.intListPtrVal = new IntList;
			break;

		case VT_DoubleList:
		case VT_DateList:
			value_m.doubleListPtrVal = new DoubleList;
			break;

		case VT_String:
			value_m.stringPtrVal = new String;
			break;

		case VT_StringList:
			value_m.stringListPtrVal = new StringList;
			break;

		case VT_Uid:
			value_m.uidPtrVal = new Uid;
			break;

		case VT_UidList:
			value_m.uidListPtrVal = new UidList;
			break;

		case VT_Reference:
			value_m.refPtrVal = new Reference;
			break;

		case VT_ReferenceList:
			value_m.refListPtrVal = new ReferenceList;
			break;

		case VT_StringMap:
			value_m.stringMapPtrVal = new StringMap;
			break;

	}
}
//-------------------------------------------------------------
void Value::clear (
	)
{
	// Destroy pointer if needed
	switch( type_m ) {
		case VT_IntList:
			HTASSERT( value_m.intListPtrVal );
			delete value_m.intListPtrVal;
			break;

		case VT_DoubleList:
		case VT_DateList:
			HTASSERT( value_m.doubleListPtrVal );
			delete value_m.doubleListPtrVal;
			break;

		case VT_String:
			HTASSERT( value_m.stringPtrVal );
			delete value_m.stringPtrVal;
			break;

		case VT_StringList:
			HTASSERT( value_m.stringListPtrVal );
			delete value_m.stringListPtrVal;
			break;

		case VT_Uid:
			HTASSERT( value_m.uidPtrVal );
			delete value_m.uidPtrVal;
			break;

		case VT_UidList:
			HTASSERT( value_m.uidListPtrVal );
			delete value_m.uidListPtrVal;
			break;

		case VT_Reference:
			HTASSERT( value_m.refPtrVal );
			delete value_m.refPtrVal;
			break;

		case VT_ReferenceList:
			HTASSERT( value_m.refListPtrVal );
			delete value_m.refListPtrVal;
			break;
		
		case VT_StringMap:
			HTASSERT(value_m.stringMapPtrVal);
			delete value_m.stringMapPtrVal;
			break;

	}

	// List any of the pointer to NULL
	value_m.stringPtrVal = NULL;

	// List type to none
	type_m = VT_None;
}
//-------------------------------------------------------------
void Value::toBlob (
		MemoryChunk &blob
	) const
{
	int i;

	switch( type_m ) {
		case VT_Int:
			blob.length( sizeof(int) );
			*(int *) blob.data() = value_m.intVal;
			break;

		case VT_IntList:
		{
			blob.length( ( value_m.intListPtrVal->size() + 1 ) * sizeof(int) );
			int *data = ((int *) blob.data()) + 1;

			*(int *) blob.data() = value_m.intListPtrVal->size();
			for(i = 0; i < value_m.intListPtrVal->size(); ++i)
				*data++ = (*value_m.intListPtrVal)[ i ];

			break;
		}

		case VT_Double:
		case VT_Date:
			blob.length( sizeof(double) );
			*(double *) blob.data() = value_m.doubleVal;
			break;

		case VT_DoubleList:
		case VT_DateList:
		{
			blob.length( value_m.doubleListPtrVal->size() * sizeof(double) + sizeof(int) );
			double *data = (double *) (((int *) blob.data()) + 1);

			*(int *) blob.data() = value_m.doubleListPtrVal->size();
			for(i = 0; i < value_m.doubleListPtrVal->size(); ++i)
				*data++ = (*value_m.doubleListPtrVal)[ i ];

			break;
		}

		case VT_String:
			blob.length( value_m.stringPtrVal->length() + 1 );
			strcpy( blob.data(), value_m.stringPtrVal->c_str() );
			break;

		case VT_StringList:
		{
			int len = 0;

			for(i = 0; i < value_m.stringListPtrVal->size(); ++i)
				len += (*value_m.stringListPtrVal)[ i ].length() + 1;

			blob.length( len + sizeof(int) );
			Byte *data = (Byte *) (((int *) blob.data()) + 1);

			*(int *) blob.data() = value_m.stringListPtrVal->size();
			for(i = 0; i < value_m.stringListPtrVal->size(); ++i) {
				strcpy( data, (*value_m.stringListPtrVal)[ i ].c_str() );
				data += (*value_m.stringListPtrVal)[ i ].length() + 1;
			}

			break;
		}

		case VT_Uid:
			blob.length( Signature::length_scm );
			memcpy( blob.data(), value_m.uidPtrVal->data(), Signature::length_scm );
			break;

		case VT_UidList:
		{
			blob.length( value_m.uidListPtrVal->size() * Signature::length_scm + sizeof(int) );
			Byte *data = (Byte *) (((int *) blob.data()) + 1);

			*(int *) blob.data() = value_m.uidListPtrVal->size();

			for(UidList::const_iterator i_sig = value_m.uidListPtrVal->begin();
				i_sig != value_m.uidListPtrVal->end(); ++i_sig) {
				memcpy( data, i_sig->data(), Signature::length_scm );
				data += Signature::length_scm;
			}

			break;
		}

		case VT_Reference:
		{
			String val = value_m.refPtrVal->toString();
			blob.length( val.length() + 1 );
			strcpy( blob.data(), val.c_str() );
			break;
		}

		case VT_ReferenceList:
		{
			StringList val( value_m.refListPtrVal->size() );
			for(i = 0; i < value_m.refListPtrVal->size(); ++i)
				val[ i ] = (*value_m.refListPtrVal)[ i ].toString();

			int len = 0;

			for(i = 0; i < val.size(); ++i)
				len += val[ i ].length() + 1;

			blob.length( len + sizeof(int) );
			Byte *data = (Byte *) (((int *) blob.data()) + 1);

			*(int *) blob.data() = val.size();
			for(i = 0; i < val.size(); ++i) {
				strcpy( data, val[ i ].c_str() );
				data += val[ i ].length() + 1;
			}

			break;
		}

		case VT_StringMap:
		{
			int len = 0;

			for (StringMap::const_iterator it = value_m.stringMapPtrVal->begin(); it != value_m.stringMapPtrVal->end(); it++) {
				len += it->first.length() + it->second.length() + 2;
			}

			blob.length( len + sizeof(int) );
			Byte *data = (Byte *) (((int *) blob.data()) + 1);
			
			*(int *) blob.data() = value_m.stringMapPtrVal->size();

			for (StringMap::const_iterator it = value_m.stringMapPtrVal->begin();it != value_m.stringMapPtrVal->end(); it++) {
				strcpy( data, it->first.c_str() );
				data += (it->first.length() + 1);

				strcpy( data, it->second.c_str() );
				data += (it->second.length() + 1);
			}
			break;
		}

	}
}
//-------------------------------------------------------------
void Value::fromBlob (
		ValueType type,
		const Byte *blob
	)
{
	int i;

	// Set type
	setType( type );

	if (blob==NULL)
		return;

	switch( type_m ) {
		case VT_Int:
			getAsInt() = *(int *) blob;
			break;

		case VT_IntList:
		{
			int len = *(int *) blob;
			int *data = ((int *) blob) + 1;

			IntList &ilist = getAsIntList();
			ilist.resize( len );

			for(i = 0; i < len; ++i)
				ilist[ i ] = *data++;

			break;
		}

		case VT_Double:
			getAsDouble() = *(double *) blob;
			break;

		case VT_Date:
			getAsDate() = *(double *) blob;
			break;

		case VT_DoubleList:
		{
			int len = *(int *) blob;
			double *data = (double *) (((int *) blob) + 1);

			DoubleList &dlist = getAsDoubleList();
			dlist.resize( len );

			for(i = 0; i < len; ++i)
				dlist[ i ] = *data++;

			break;
		}


		case VT_DateList:
		{
			int len = *(int *) blob;
			double *data = (double *) (((int *) blob) + 1);

			DoubleList &dlist = getAsDateList();
			dlist.resize( len );

			for(i = 0; i < len; ++i)
				dlist[ i ] = *data++;

			break;
		}

		case VT_String:
			getAsString() = (Byte *) blob;
			break;

		case VT_StringList:
		{
			int len = *(int *) blob;
			Byte *data = (Byte *) (((int *) blob) + 1);

			StringList &slist = getAsStringList();
			slist.resize( len );

			for(i = 0; i < len; ++i) {
				slist[ i ] = data;
				data += strlen( data ) + 1;
			}

			break;
		}

		case VT_Uid:
			getAsUid().fromMemory( (Byte *) blob );
			break;

		case VT_UidList:
		{
			int len = *(int *) blob;
			Byte *data = (Byte *) (((int *) blob) + 1);

			UidList &slist = getAsUidList();
			slist.resize( len );

			for(i = 0; i < len; ++i) {
				slist[ i ].fromMemory( data );
				data += Signature::length_scm;
			}

			break;
		}

		case VT_Reference:
			getAsReference().fromString( (Byte *) blob );
			break;

		case VT_ReferenceList:
		{
			int len = *(int *) blob;
			Byte *data = (Byte *) (((int *) blob) + 1);

			ReferenceList &reflist = getAsReferenceList();
			reflist.resize( len );

			for(i = 0; i < len; ++i) {
				reflist[ i ].fromString( data );
				data += strlen( data ) + 1;
			}

			break;
		}

		case VT_StringMap:
		{
			int len = *(int *) blob;
			Byte *data = (Byte *) (((int *) blob) + 1);
			Byte *kdata;

			StringMap &smap = getAsStringMap();
			smap.clear();

			for(i = 0; i < len; ++i) {
				
				kdata = data;
				data += strlen( data ) + 1;
				smap[kdata] =  data;
				data += strlen( data ) + 1;
			}

			break;
		}

	}
}
//-------------------------------------------------------------
void Value::add (
		Value const &rhs
	)
{
	// Equal types
	if( rhs.getType() == type_m ) {
		switch( type_m ) {

		case VT_Int:
			value_m.intVal += rhs.getAsInt();
			break;
				
		case VT_Date:
		case VT_Double:
			value_m.doubleVal += rhs.value_m.doubleVal;
			break;
				
		case VT_String:
			*value_m.stringPtrVal += rhs.getAsString();
			break;
				
		case VT_IntList:
			value_m.intListPtrVal->insert( 
				value_m.intListPtrVal->end(),
				rhs.getAsIntList().begin(),
				rhs.getAsIntList().end()
			);
			break;
				
		case VT_DateList:
		case VT_DoubleList:
			value_m.doubleListPtrVal->insert( 
				value_m.doubleListPtrVal->end(),
				rhs.value_m.doubleListPtrVal->begin(),
				rhs.value_m.doubleListPtrVal->end()
			);
			break;
				
		case VT_StringList:
			value_m.stringListPtrVal->insert( 
				value_m.stringListPtrVal->end(),
				rhs.getAsStringList().begin(),
				rhs.getAsStringList().end()
			);
			break;
				
		case VT_UidList:
			value_m.uidListPtrVal->insert( 
				value_m.uidListPtrVal->end(),
				rhs.getAsUidList().begin(),
				rhs.getAsUidList().end()
			);
			break;
			
		case VT_ReferenceList:
			value_m.refListPtrVal->insert( 
				value_m.refListPtrVal->end(),
				rhs.getAsReferenceList().begin(),
				rhs.getAsReferenceList().end()
			);
			break;
		
		
		case VT_StringMap:
			HTASSERT( false );
			break;
		
		}

	}

	// Scalar added to list type
	else if( rhs.getType() == (type_m & ~VT_List) && (VT_List & type_m) ) {
		switch( type_m ) {
		case VT_IntList:
			value_m.intListPtrVal->push_back( rhs.getAsInt() ); 
			break;
				
		case VT_DateList:
		case VT_DoubleList:
			value_m.doubleListPtrVal->push_back( rhs.value_m.doubleVal ); 
			break;
				
		case VT_StringList:
			value_m.stringListPtrVal->push_back( rhs.getAsString() ); 
			break;
				
		case VT_UidList:
			value_m.uidListPtrVal->push_back( rhs.getAsUid() ); 
			break;
			
		case VT_ReferenceList:
			value_m.refListPtrVal->push_back( rhs.getAsReference() ); 
			break;
		

		case VT_StringMap:
			HTASSERT( false);
			break;
		}

	}
}
//======================================
// Accessors
//======================================
int const & Value::getAsInt (
	) const
{
	HTASSERT( type_m == VT_Int );

	return value_m.intVal;
}
//-------------------------------------------------------------
int & Value::getAsInt (
		bool setType
	)
{
	if( setType ) this->setType( VT_Int );
	HTASSERT( type_m == VT_Int );

	return value_m.intVal;
}
//-------------------------------------------------------------
IntList const & Value::getAsIntList (
	) const
{
	HTASSERT( type_m == VT_IntList );
	HTASSERT( value_m.intListPtrVal != NULL );

	return *value_m.intListPtrVal;
}
//-------------------------------------------------------------
IntList & Value::getAsIntList (
		bool setType
	)
{
	if( setType ) this->setType( VT_IntList );
	HTASSERT( type_m == VT_IntList );
	HTASSERT( value_m.intListPtrVal != NULL );

	return *value_m.intListPtrVal;
}
//-------------------------------------------------------------
double const & Value::getAsDouble (
	) const
{
	HTASSERT( type_m == VT_Double );

	return value_m.doubleVal;
}
//-------------------------------------------------------------
double & Value::getAsDouble (
		bool setType
	)
{
	if( setType ) this->setType( VT_Double );
	HTASSERT( type_m == VT_Double );

	return value_m.doubleVal;
}
//-------------------------------------------------------------
DoubleList const & Value::getAsDoubleList (
	) const
{
	HTASSERT( type_m == VT_DoubleList );
	HTASSERT( value_m.doubleListPtrVal != NULL );

	return *value_m.doubleListPtrVal;
}
//-------------------------------------------------------------
DoubleList & Value::getAsDoubleList (
		bool setType
	)
{
	if( setType ) this->setType( VT_DoubleList );
	HTASSERT( type_m == VT_DoubleList );
	HTASSERT( value_m.doubleListPtrVal != NULL );

	return *value_m.doubleListPtrVal;
}
//-------------------------------------------------------------
double const & Value::getAsDate (
	) const
{
	HTASSERT( type_m == VT_Date );

	return value_m.doubleVal;
}
//-------------------------------------------------------------
double & Value::getAsDate (
		bool setType
	)
{
	if( setType ) this->setType( VT_Date );
	HTASSERT( type_m == VT_Date );

	return value_m.doubleVal;
}
//-------------------------------------------------------------
DoubleList const & Value::getAsDateList (
	) const
{
	HTASSERT( type_m == VT_DateList );
	HTASSERT( value_m.doubleListPtrVal != NULL );

	return *value_m.doubleListPtrVal;
}
//-------------------------------------------------------------
DoubleList & Value::getAsDateList (
		bool setType
	)
{
	if( setType ) this->setType( VT_DateList );
	HTASSERT( type_m == VT_DateList );
	HTASSERT( value_m.doubleListPtrVal != NULL );

	return *value_m.doubleListPtrVal;
}
//-------------------------------------------------------------
String const & Value::getAsString (
	) const
{
	HTASSERT( type_m == VT_String );
	HTASSERT( value_m.stringPtrVal != NULL );

	return *value_m.stringPtrVal;
}
//-------------------------------------------------------------
String & Value::getAsString (
		bool setType
	)
{
	if( setType ) this->setType( VT_String );
	HTASSERT( type_m == VT_String );
	HTASSERT( value_m.stringPtrVal != NULL );

	return *value_m.stringPtrVal;
}
//-------------------------------------------------------------
StringList const & Value::getAsStringList (
	) const
{
	HTASSERT( type_m == VT_StringList );
	HTASSERT( value_m.stringListPtrVal != NULL );

	return *value_m.stringListPtrVal;
}

//-------------------------------------------------------------
StringList & Value::getAsStringList (
		bool setType
	)
{
	if( setType ) this->setType( VT_StringList );
	HTASSERT( type_m == VT_StringList );
	HTASSERT( value_m.stringListPtrVal != NULL );

	return *value_m.stringListPtrVal;
}
//-------------------------------------------------------------
Uid const & Value::getAsUid (
	) const
{
	HTASSERT( type_m == VT_Uid );
	HTASSERT( value_m.uidPtrVal != NULL );

	return *value_m.uidPtrVal;
}
//-------------------------------------------------------------
Uid & Value::getAsUid (
		bool setType
	)
{
	if( setType ) this->setType( VT_Uid );
	HTASSERT( type_m == VT_Uid );
	HTASSERT( value_m.uidPtrVal != NULL );

	return *value_m.uidPtrVal;
}
//-------------------------------------------------------------
UidList const & Value::getAsUidList (
	) const
{
	HTASSERT( type_m == VT_UidList );
	HTASSERT( value_m.uidListPtrVal != NULL );

	return *value_m.uidListPtrVal;
}
//-------------------------------------------------------------
UidList & Value::getAsUidList (
		bool setType
	)
{
	if( setType ) this->setType( VT_UidList );
	HTASSERT( type_m == VT_UidList );
	HTASSERT( value_m.uidListPtrVal != NULL );

	return *value_m.uidListPtrVal;
}
//-------------------------------------------------------------
Reference const & Value::getAsReference (
	) const
{
	HTASSERT( type_m == VT_Reference );
	HTASSERT( value_m.refPtrVal != NULL );

	return *value_m.refPtrVal;
}
//-------------------------------------------------------------
Reference & Value::getAsReference (
		bool setType
	)
{
	if( setType ) this->setType( VT_Reference );
	HTASSERT( type_m == VT_Reference );
	HTASSERT( value_m.refPtrVal != NULL );

	return *value_m.refPtrVal;
}
//-------------------------------------------------------------
ReferenceList const & Value::getAsReferenceList (
	) const
{
	HTASSERT( type_m == VT_ReferenceList );
	HTASSERT( value_m.refListPtrVal != NULL );

	return *value_m.refListPtrVal;
}
//-------------------------------------------------------------

ReferenceList & Value::getAsReferenceList (
		bool setType
	)
{
	if( setType ) this->setType( VT_ReferenceList );
	HTASSERT( type_m == VT_ReferenceList );
	HTASSERT( value_m.refListPtrVal != NULL );

	return *value_m.refListPtrVal;
}

//-------------------------------------------------------------


StringMap const & Value::getAsStringMap(
) const
{
	HTASSERT( type_m == VT_StringMap );
	HTASSERT( value_m.stringMapPtrVal != NULL );

	return *value_m.stringMapPtrVal;
}

//-------------------------------------------------------------


StringMap & Value::getAsStringMap (
	bool setType
)
{
	if( setType ) this->setType( VT_StringMap );
	HTASSERT( type_m == VT_StringMap );
	HTASSERT( value_m.stringMapPtrVal != NULL );

	return *value_m.stringMapPtrVal;
}


//======================================
// Operators
//======================================
Value & Value::operator = (
		Value const &rhs
	)
{
	// Get ref of current data
	clear();

	// Copy data
	type_m = rhs.type_m;

	switch( type_m ) {
		case VT_Int:
			value_m.intVal = rhs.value_m.intVal;
			break;

		case VT_IntList:
			value_m.intListPtrVal = new IntList( *rhs.value_m.intListPtrVal );
			break;

		case VT_Double:
			value_m.doubleVal = rhs.value_m.doubleVal;
			break;

		case VT_DoubleList:
			value_m.doubleListPtrVal = new DoubleList( *rhs.value_m.doubleListPtrVal );
			break;

		case VT_Date:
			value_m.doubleVal = rhs.value_m.doubleVal;
			break;

		case VT_DateList:
			value_m.doubleListPtrVal = new DoubleList( *rhs.value_m.doubleListPtrVal );
			break;

		case VT_String:
			value_m.stringPtrVal = new String( *rhs.value_m.stringPtrVal );
			break;

		case VT_StringList:
			value_m.stringListPtrVal = new StringList( *rhs.value_m.stringListPtrVal );
			break;

		case VT_Uid:
			value_m.uidPtrVal = new Uid( *rhs.value_m.uidPtrVal );
			break;

		case VT_UidList:
			value_m.uidListPtrVal = new UidList( *rhs.value_m.uidListPtrVal );
			break;

		case VT_Reference:
			value_m.refPtrVal = new Reference( *rhs.value_m.refPtrVal );
			break;

		case VT_ReferenceList:
			value_m.refListPtrVal = new ReferenceList( *rhs.value_m.refListPtrVal );
			break;

		case VT_StringMap:
			value_m.stringMapPtrVal = new StringMap( *rhs.value_m.stringMapPtrVal );
			break;

	}

	// Done
	return *this;
}
//-------------------------------------------------------------
bool Value::operator == (
		Value const &rhs
	) const
{
	// Matching type?
	if( type_m != rhs.type_m )
		return false;

	// Compare content
	switch( type_m ) {
		case VT_Int:
			return value_m.intVal == rhs.value_m.intVal;
			break;

		case VT_IntList:
			return *value_m.intListPtrVal == *rhs.value_m.intListPtrVal;
			break;

		case VT_Double:
			return value_m.doubleVal == rhs.value_m.doubleVal;
			break;

		case VT_DoubleList:
			return *value_m.doubleListPtrVal == *rhs.value_m.doubleListPtrVal;
			break;

		case VT_Date:
			return value_m.doubleVal == rhs.value_m.doubleVal;
			break;

		case VT_DateList:
			return *value_m.doubleListPtrVal == *rhs.value_m.doubleListPtrVal;
			break;

		case VT_String:
			return *value_m.stringPtrVal == *rhs.value_m.stringPtrVal;
			break;

		case VT_StringList:
			return *value_m.stringListPtrVal == *rhs.value_m.stringListPtrVal;
			break;

		case VT_Uid:
			return *value_m.uidPtrVal == *rhs.value_m.uidPtrVal;
			break;

		case VT_UidList:
			return *value_m.uidListPtrVal == *rhs.value_m.uidListPtrVal;
			break;

		case VT_Reference:
			return *value_m.refPtrVal == *rhs.value_m.refPtrVal;
			break;

		case VT_ReferenceList:
			return *value_m.refListPtrVal == *rhs.value_m.refListPtrVal;
			break;

		case VT_StringMap:
			return *value_m.stringMapPtrVal == *rhs.value_m.stringMapPtrVal;
			break;

	}

	// Done
	return false;
}



// End of namespace
}

