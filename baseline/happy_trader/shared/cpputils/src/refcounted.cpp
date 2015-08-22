
/** \file

		File implementing RefCounted.

		
*/


#include "refcounted.hpp"
#include "assertdef.hpp"
#include "log.hpp"


namespace CppUtils {


// =======================================================
// Class RefCounted
//
RefCounted::RefCounted (
	) :
		refCount_m ( 0 )
{
}
//-------------------------------------------------------------
RefCounted::RefCounted(
		RefCounted const &
	) :
		refCount_m ( 0 )
{
}
//-------------------------------------------------------------
RefCounted::~RefCounted(
	)
{
	HTASSERT( refCount_m == 0 );
}
//======================================
// Methods
//======================================
void RefCounted::incRefCount (
		unsigned int inc
	) const
{
	refCount_m += inc;
}
//-------------------------------------------------------------
void RefCounted::decRefCount (
		unsigned int dec
	) const
{
	refCount_m -= dec;
	HTASSERT( refCount_m >= 0 );
}
//-------------------------------------------------------------
bool RefCounted::isReferenced (
	) const
{
	return ( refCount_m > 0 );
}
//======================================
// Operators
//======================================
RefCounted & RefCounted::operator = (
		RefCounted const &
	)
{
	// No-op
	return *this;
}



// End of namespace
};
