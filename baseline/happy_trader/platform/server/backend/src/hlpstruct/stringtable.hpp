#ifndef _HLPSTRUCT_STRINGTABLEMGR_INCLUDED
#define _HLPSTRUCT_STRINGTABLEMGR_INCLUDED


#include "hlpstructdefs.hpp"



namespace HlpStruct {


//=============================================================
// Class StringTable
//=============================================================
/** Class StringTable is a support class for localization,
		managing string tables. One instance is installed as
		root object.

		\todo This is a placeholder; actual string resolution is not
		implemented.

*/
	class HLPSTRUCT_EXP StringTable : public CppUtils::RefCounted, private CppUtils::Mutex {

public:


	//======================================
	// Con- & Destructor

	/** Ctor. */
	StringTable (
		CppUtils::String const &defaultLang = "en"
	);

	/** Dtor. */
	~StringTable (
	);



	//======================================
	// Methods

	/** Replace the strings in the argument list with their
			localized strings; if the given language is unknown,
			revert back to the default language. Unknown string
			keys are ignored. */
	void lookupStrings (
		CppUtils::String const &lang,
		CppUtils::StringList &l_str
	) const;

	/** Replace the string in the argument with its localized string;
			if the given language is unknown, revert back to the default language.
			Unknown string keys are ignored. */
	void lookupString (
		CppUtils::String const &lang,
		CppUtils::String &str
	) const;

	/** Replace the each of the lines in the string in the argument with 
			its localized string; if the given language is unknown, revert back 
			to the default language. Unknown string keys are ignored. */
	void lookupStringLines (
		CppUtils::String const &lang,
		CppUtils::String &str
	) const;

	/** Lookup and return a single string in the default language. */
	CppUtils::String lookupStringInDefaultLang (
		CppUtils::String const &str
	) const;



	//======================================
	// Helper

	/** Name under which an instance is registered as root object. */
	static CppUtils::String const &getRootObjectName (
	);



private:


	//======================================
	// Creators

	/** Private, unimplemented ctor. */
	StringTable (
		StringTable const &
	);

	
	//======================================
	// Helper

	void init (
	);



	//======================================
	// Variables

	/** The default language string. */
	CppUtils::String defaultLanguage_m;

	/** Map holding strings keyed by index, keyed by language. */
	map<CppUtils::String, map<CppUtils::String, CppUtils::String> > stringMap_m;


	/** Exception text. */
	static const CppUtils::String exc_UnknownLocalization_scm;

	/** Exception text. */
	static const CppUtils::String exc_UnknownStringIndex_scm;
};



/** Helper macro to resolve a single string in the default language. */
#define GET_LANG_STRING( STR ) CppUtils::getRootObject<HlpStruct::StringTable>()->lookupStringInDefaultLang( STR )



// End of namespace
}


#endif // _CPPUTILS_STRINGTABLEMGR_INCLUDED
