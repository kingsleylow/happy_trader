/** 
    File implementing RefCounted and RootObjectManager.
*/

#include "rootobj.hpp"
#include "log.hpp"

namespace CppUtils {

#define ROOT_OBJ_MGR "ROOT OBJECT MANAGER"

//=============================================================
// Class RootObjectManager
//
RootObjectManager::RootObjectManager (
  ) :
    Mutex(),
    rootObjMap_m()
{
}
//-------------------------------------------------------------
RootObjectManager::~RootObjectManager (
  )
{
  // TODO: Ought to call cleanup() here,
  // but the DLLs may be gone already
}
//======================================
// Access to root object manager singleton
//======================================
RootObjectManager & RootObjectManager::singleton (
  )
{
  static Mutex mutex_s;
  static volatile RootObjectManager *manager_s = NULL;

	if( !manager_s ) {
		LOCK_FOR_SCOPE( mutex_s );

		if( !manager_s )
			// Create root object manager
			manager_s = new RootObjectManager;
	}

  return const_cast<RootObjectManager&>(*manager_s);
}
//======================================
// Root object management
//======================================
void RootObjectManager::cleanup (
  )
{
  // Clear all objects
  rootObjMap_m.clear();
}
//-------------------------------------------------------------
void RootObjectManager::registerRootObject (
    String const &name,
    RefCounted *obj
  )
{
  assert( obj );

  // Lock the object
  LOCK_FOR_SCOPE( *this );

  LOG( MSG_DEBUG, ROOT_OBJ_MGR, "Registering root object \"" << name << "\" @ " << obj << \
    " of type \"" << typeid( *obj ).name() << "\"" );

  // Name already known?
  map< String, RefCountPtr<RefCounted> >::iterator it = rootObjMap_m.find( name );
  if( it != rootObjMap_m.end() ) {
    // Yes; we will discard the old object
    LOG( MSG_DEBUG, ROOT_OBJ_MGR, "Implicitly unregistering root object \"" << name << \
      "\" @ " << it->second << " of type \"" << typeid( *it->second ).name() << "\"" );
  }

  // Insert the new object, possibly discarding the previous one
  rootObjMap_m[ name ] = obj;
}
//-------------------------------------------------------------
void RootObjectManager::unregisterRootObject (
    String const &name
  )
{
  // Lock the object
  LOCK_FOR_SCOPE( *this );

  // Find the object being unregistered
  map< String, RefCountPtr<RefCounted> >::iterator it = rootObjMap_m.find( name );
  if( it != rootObjMap_m.end() ) {
    // Got the object; get rid of it
    LOG( MSG_DEBUG, ROOT_OBJ_MGR, "Unregistering root object \"" << name << \
      "\" @ " << it->second << " of type \"" << typeid( *it->second ).name() << "\"" );

    rootObjMap_m.erase( name );
  }
}
//-------------------------------------------------------------
RefCounted * RootObjectManager::resolveRootObject (
    String const &name
  ) const
{
  // Lock the object
  LOCK_FOR_SCOPE( *this );

  // Find the root object
  map< String, RefCountPtr<RefCounted> >::const_iterator it = rootObjMap_m.find( name );

  if( it == rootObjMap_m.end() ) {
    LOG( MSG_WARN, ROOT_OBJ_MGR, "Resolving root object \"" << name << "\" failed." );
    return NULL;
  }
  else
    return it->second;
}





}; // End of namespace
