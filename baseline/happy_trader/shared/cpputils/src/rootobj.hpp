
/** \file

    Header file declaring RootObjectManager.

   
*/


#ifndef _CPPUTILS_ROOTOBJMGR_INCLUDED
#define _CPPUTILS_ROOTOBJMGR_INCLUDED


#include "cpputilsdefs.hpp"

#include "sharedres.hpp"
#include "refcounted.hpp"


namespace CppUtils {


/*
//=============================================================
// Class RootObjectManager
//

    Class managing instances implementing base class RefCounted,
    discriminating them by name supplied at registration-time.

    There is one global instance of this class, accessible
    via RootObjectManager::singleton(), which servers as
    central point of registration for system-wide objects, so
    called \b root \b objects.
*/

class CPPUTILS_EXP RootObjectManager : private Mutex {

public:

  //======================================
  // Creators

  /** Standard ctor. */
  RootObjectManager (
  );

  /** Dtor, destroying all registered objects. */
  ~RootObjectManager (
  );



  //======================================
  // Access to root object manager singleton

  /** Return a global static instance of the class. */
  static RootObjectManager & singleton (
  );



  //======================================
  // Root object management

  /** Discards all registered root objects. */
  void cleanup (
  );

  /** Register a root object under the given name and increment
      its ref count. If there has been an object under the same name,
      it is discarded and its ref count is decreased. */
  void registerRootObject (
    String const &name,
    RefCounted *obj
  );

  /** Unregister and dec ref count on a root object. If no such object
      is known, nothing is done. */
  void unregisterRootObject (
    String const &name
  );

  /** Retrieve the root object for the given name. Returns NULL
      if no object is known for the name. */
  RefCounted * resolveRootObject (
    String const &name
  ) const;



private:

  //======================================
  // Creators

  /** \attention Not implemented. */
  RootObjectManager (
    RootObjectManager const &
  );


  //======================================
  // Variables

  /** Table of known objects. */
  map< String, RefCountPtr<RefCounted> > rootObjMap_m;
};






//=============================================================
// Functions
//

/** Helper template function to retrieve the root object of a given type.
    The static function \c getRotObjectName() of the desired class is used to
    define the name of the root object. */
template<class T> T * getRootObject (
  )
{
  return dynamic_cast<T *>( RootObjectManager::singleton().resolveRootObject( T::getRootObjectName() ) );
}

/** Helper template function to retrieve the root object of a given type.
    The argument is used to define the name of the root object. */
template<class T> T * getRootObject (
    String const &name
  )
{
  return dynamic_cast<T *>( RootObjectManager::singleton().resolveRootObject( name ) );
}




}; // End of namespace



#endif // _CPPUTILS_ROOTOBJMGR_INCLUDED