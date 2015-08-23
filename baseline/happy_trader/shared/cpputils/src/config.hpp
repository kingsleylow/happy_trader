

#ifndef _CPPUTILS_CONFIG_INCLUDED
#define _CPPUTILS_CONFIG_INCLUDED


#include "cpputilsdefs.hpp"

#include "sharedres.hpp"
#include "refcounted.hpp"


namespace CppUtils {


//=============================================================
// Constants
//=============================================================
/** Constant giving top-level section for configuration
    data; e.g. on Windows this is the top-level registry
    section. */
extern String companyName();



//=============================================================
// Class Configuration
//=============================================================
/** Derived implementations of the abstract base class Configuration
    allow to store system-wide as well as per user configuration
    data using key/value pairs. Only string values are supported.
    The entries are organized in sections, which can be nested.
    Each key is given relative to a section, where the section is
    specified through a slash delimited string to allow for nested
    structures: "a/b/c" qualifies section "c" within section "b"
    within section "a". Neither keys nor sections may use the slash
    as character.

    The implementation of derived classes is OS-specific, e.g.
    on Windows the registry is used.

    Two instances of this class are registered with the root object
    manager: one for the system and one for the user.


    
*/
class CPPUTILS_EXP Configuration : public RefCounted, protected SharedResource {

public:

  //======================================
  // Typedefs

  /** Enumeration classifying the type of configuration data.
      Data of type CT_User are stored on per user basis. */
  enum ConfigType {
    /** System-wide configuration. */
    CT_System,
    /** Per-user (OS-user) configuration. */
    CT_User
  };


  //======================================
  // Methods

  /** Set a string value with given key in given section. */
  virtual void setValue (
    String const &section,
    String const &key,
    String const &value
  ) throw (
    ExceptionInternal
  ) = 0;

  /** Retrieve a string value with given key from given section. If the key or
      section can not be found, the default value is retuned. */
  virtual String getValue (
    String const &section,
    String const &key,
    String const &def = ""
  ) const = 0;

  /** Retrieve the set of all keys present within a section. The keys
      are returned as a list of strings. If the section is unknown, an empty
      list is returned. */
  virtual StringList getKeys (
    String const &section
  ) const = 0;

  /** Retrieve all entries of a section. If the section is unknown, an empty
      map is returned. */
  virtual void getAllEntries (
    String const &section,
    StringMap &entries
  ) const = 0;

  /** Retrieve the set of all sections present within a section. The sections
      are returned as a list of strings. If the section is unknown, an empty
      list is returned. */
  virtual StringList getSections (
    String const &section
  ) const = 0;

  /** Erase a key/value pair from the given section. Ignore
      if section or key is unknown. */
  virtual void deleteEntry (
    String const &section,
    String const &key
  ) = 0;

  /** Erase an entire section. Ignore if section is unknown. */
  virtual void deleteSection (
    String const &section
  ) = 0;



  //======================================
  // Variables

  /** Delimiter for sections: \c / */
  static const Char sectionDelimiter_scm;

  /** Name under which the system configuration is
      registered with the root object manager. \c configuration_system */
  static String const &getRootObjectName_System(
	);

  /** Name under which the user configuration is
      registered with the root object manager. \c configuration_user */
  static String const &getRootObjectName_User(
	);
};





//=============================================================
// Factory for class Configuration
//=============================================================
/** Returns an OS-specfic implementation of abstract base class
    Configuration. Raises ExceptionInternal if the objects
    could not be created.
*/
Configuration * getConfiguration (
    Configuration::ConfigType ct
  ) throw (
    ExceptionInternal
  );



// End of namespace
}


#endif // _CPPUTILS_CONFIG_INCLUDED


