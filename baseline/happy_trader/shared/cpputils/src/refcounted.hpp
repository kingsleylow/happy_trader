/** 

		Header file declaring base class RefCounted.

		
*/


#ifndef _CPPUTILS_REFCOUNTED_INCLUDED
#define _CPPUTILS_REFCOUNTED_INCLUDED


#include "cpputilsdefs.hpp"


namespace CppUtils {


// =======================================================
// Class RefCounted
//
/** Mix-in class providing reference count tracking
		functionality.

	
*/
class CPPUTILS_EXP RefCounted {

public:

	//======================================
	// Creators

	/** Ctor; inits ref count to zero. */
	RefCounted (
	);

	/** Copy ctor; does not copy ref count. */
	RefCounted (
		RefCounted const &
	);

	/** Dtor, asserting ref count of 0. */
	virtual ~RefCounted (
	);


	//======================================
	// Methods

	/** Increments ref count by given increment. */
	void incRefCount (
		unsigned int inc = 1
	) const;

	/** Decrements ref count by given decrement. */
	void decRefCount (
		unsigned int dec = 1
	) const;

	/** Returns true if the ref count is not 0. */
	bool isReferenced (
	) const;

	

	//======================================
	// Operators

	/** Assignment operator; does not copy ref count. */
	RefCounted & operator = (
		RefCounted const &
	);





private:

	//======================================
	// Variables

	/** Ref count value. */
	mutable int refCount_m;
};





// =======================================================
// Helper class
//
/** Template class for auto pointers to ref counted objects.
		Template type must be derived from class RefCounted.

		
*/
template<class T> class RefCountPtr {

public:

	/** Standard ctor. */
	RefCountPtr (
	) :
		ptr_m( NULL )
	{
	}

	/** Copy ctor. */
	RefCountPtr (
		RefCountPtr const &rhs
	) :
		ptr_m( NULL )
	{
		acquire( rhs.ptr_m );
	}

	/** Ctor. */
	RefCountPtr (
		T *ptr
	) :
		ptr_m( NULL )
	{
		acquire( ptr );
	}

	/** Dtor. */
	~RefCountPtr (
	)
	{
		release();
	}

	/** Assignment operator. */
	RefCountPtr &operator = (
		RefCountPtr const &rhs
	)
	{
		acquire( rhs.ptr_m );
		return *this;
	}

	/** Assignment operator. */
	RefCountPtr &operator = (
		T const *ptr
	)
	{
		acquire( static_cast<RefCounted *>( const_cast<T *>( ptr ) ) );
		return *this;
	}

	/** Cast operator into template type. */
	operator T * (
		) const
	{
		return dynamic_cast<T *>( ptr_m );
	}

	/** Cast operator into template type. */
	T * operator -> (
		) const
	{
		return dynamic_cast<T *>( ptr_m );
	}

	/** Operator for STL containers. */
	bool operator < (
			RefCountPtr const &rhs
		) const
	{
		return ( ptr_m < rhs.ptr_m );
	}

	bool isValid() const {
		return (ptr_m != NULL);
	}
	


private:

	/** Give up reference on previous object and reference another object. */
	void acquire (
		RefCounted *ptr
	)
	{
		release();
		ptr_m = ptr;

		if( ptr_m )
			ptr_m->incRefCount();
	}

	/** Helper to give up reference to object; deletes the
			object pointed to if the ref count drops to zero. */
	void release (
	)
	{
		if( ptr_m ) {
			ptr_m->decRefCount();
			if( !ptr_m->isReferenced() )
				delete ptr_m;

			ptr_m = NULL;
		}
	}

	/** The actual pointer. */
	RefCounted *ptr_m;
};



// End of namespace
};



#endif // _CPPUTILS_REFCOUNTED_INCLUDED
