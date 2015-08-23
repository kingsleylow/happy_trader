
#include "stringtable.hpp"
#include "xmlwrap.hpp"


namespace HlpStruct {


#define STRING_TABLE "STRING TABLE"



// =======================================================
// LocalizationSaxHandler
// =======================================================
/** Helper class to parse XML documents. */
class LocalizationSaxHandler : public SaxDocumentHandler {

private:

	/** Localization map being filled. */
	map<CppUtils::String, map<CppUtils::String, CppUtils::String> > &map_m;

	/** Localization currently processed. */
	map<CppUtils::String, CppUtils::String> *table_m;

	/** Current context string. */
	CppUtils::String prefix_m;

	/** Current key string. */
	CppUtils::String key_m;

	/** Prefix for next string. */
	char textPrefix_m[ 256 ];

	/** Current ref offset. */
	int refNoOffset_m;

	/** Current ref type. */
	CppUtils::String refType_m;


public:

	LocalizationSaxHandler (
			map<CppUtils::String, map<CppUtils::String, CppUtils::String> > &map
		) :
			map_m( map ),
			table_m( NULL ),
			prefix_m(),
			key_m()
	{
	}


	virtual void startElement (
			const XMLCh* const name,
			XERCES_CPP_NAMESPACE::AttributeList& attrs
	  )
	{
		CppUtils::String element;
		TRANSCODE_XMLSTRING( name, element );

		// Localization
		//
		if( element == "Localization" ) {
      // Get attributes
			//
			CppUtils::String language;
			TRANSCODE_XMLSTRING( attrs.getValue( "language" ), language );
			CppUtils::tolower( language );

			table_m = &map_m[ language ];

			LOG( MSG_DEBUG, STRING_TABLE, "Got localization in language \"" << language << "\"." );
		}

		// StringTable
		//
		else if( element == "StringTable" ) {
      // Get attributes
			//
			CppUtils::String refNoOffset;
			TRANSCODE_XMLSTRING( attrs.getValue( "prefix" ), prefix_m );
			TRANSCODE_XMLSTRING( attrs.getValue( "refType" ), refType_m );
			TRANSCODE_XMLSTRING( attrs.getValue( "refNoOffset" ), refNoOffset );

			if( sscanf( refNoOffset.c_str(), "%d", &refNoOffset_m ) != 1 )
				refNoOffset_m = -1;

			LOG( MSG_DEBUG, STRING_TABLE, "Got table with prefix \"" << prefix_m << "\"." );
		}

		// CppUtils::String
		//
		else if( element == "CppUtils::String" ) {
      // Get attributes
			//
			CppUtils::String str;
			TRANSCODE_XMLSTRING( attrs.getValue( "key" ), key_m );
			TRANSCODE_XMLSTRING( attrs.getValue( "refNo" ), str );

			int refNo;
			if( sscanf( str.c_str(), "%d", &refNo ) == 1 ) {
				// Generate prefix for string
				refNo += refNoOffset_m;
				sprintf( textPrefix_m, "[%s %d] ", refType_m.c_str(), refNo );
			}
			else 
				textPrefix_m[ 0 ] = 0;
		}
	}

	virtual void endElement (
			const XMLCh* const name
		)
	{
		CppUtils::String element;
		TRANSCODE_XMLSTRING( name, element );

		if( element == "StringTable" ) {
			// Finished with table 
			prefix_m = "";
		}
	}


  virtual void characters (
      const XMLCh* const chars,
      const unsigned int length
    )
  {
		// Insert into table
		//
		CppUtils::String txt;
		TRANSCODE_XMLSTRING( chars, txt );

		txt = CppUtils::String( textPrefix_m ) + txt;
		(*table_m)[ prefix_m + key_m ] = txt;
  }

};





//=============================================================
// Class StringTable
//=============================================================
const CppUtils::String StringTable::exc_UnknownStringIndex_scm = "Unknown string index : ";
//-------------------------------------------------------------
const CppUtils::String StringTable::exc_UnknownLocalization_scm = "Unknown localization : ";
//-------------------------------------------------------------
CppUtils::String const &StringTable::getRootObjectName (
	)
{
	static CppUtils::String name_s( "string_table" );
	return name_s;
}
//======================================
// Creators
//======================================
StringTable::StringTable (
		CppUtils::String const &defaultLang
	) :
		defaultLanguage_m( defaultLang ),
		stringMap_m()
{
	init();
}
//-------------------------------------------------------------
StringTable::~StringTable (
	)
{
}
//======================================
// Methods
//======================================
void StringTable::init (
	)
{
	// Log
  LOG( MSG_DEBUG, STRING_TABLE, "Reading localization string tables." );


	// Init
	//
	stringMap_m.clear();


	// Get the schema file
	//
	CppUtils::String schemaFile = CppUtils::expandPathName( "%HT_SERVER_DIR%\\schemas\\vpi_center_localization.xsd" );


	// Get the config file pattern
	//
	CppUtils::String configPath = CppUtils::expandPathName( "%HT_SERVER_DIR%\\config\\lang\\" );
	CppUtils::String configPattern = configPath + "*.xml";


	// Find all files in the path
	//
	CppUtils::StringList l_file, l_dir;
	CppUtils::getAllFileMatches( configPattern, l_file, l_dir );
	LOG( MSG_DEBUG, STRING_TABLE, "Found " << l_file.size() << " localization  files in \"" << configPattern << "\"." );


	// Init the XML system
	//
	//initXML();
	XERCES_CPP_NAMESPACE::SAXParser parser;
	LocalizationSaxHandler docHandler( stringMap_m );


	// Parse files
	//
	for(int i = 0; i < l_file.size(); ++i) {
		// Compose full name
		//
		CppUtils::String fileName = configPath + l_file[ i ];
		LOG( MSG_DEBUG, STRING_TABLE, "Parsing " << fileName << "..." );


		// Parse
		//
		parseXML(
			fileName,
			"vpi_center",
			CppUtils::getCanonicalFileName( schemaFile ),
			&parser,
			&docHandler
		);
	}


	// Shut down XML
	//
	//shutdownXML();


	// Log
  LOG( MSG_DEBUG, STRING_TABLE, "Finished reading localization files." );
}
//-------------------------------------------------------------
void StringTable::lookupStrings (
		CppUtils::String const &lang,
		CppUtils::StringList &l_str
	) const
{
	// Ignore empty key
	//
	if( l_str.empty()	)
		return;


	// Lock for access
	//
	LOCK_FOR_SCOPE( *this );


	// Retrieve the entry for the language
	//
	map<CppUtils::String, map<CppUtils::String, CppUtils::String> >::const_iterator i_table = stringMap_m.find( lang.empty() ? defaultLanguage_m : lang );
	if( i_table == stringMap_m.end() ) {
		// Unknown language; use default
		LOG( MSG_WARN, STRING_TABLE, exc_UnknownLocalization_scm << lang );
		i_table = stringMap_m.find( defaultLanguage_m );

		// If no default, give up
		LOG( MSG_ERROR, STRING_TABLE, "The default language \"" << defaultLanguage_m << "\" has no associated table." );
		return;
	}
	

	// Lookup the strings
	//
	for(int i = 0; i < l_str.size(); ++i) {
		// Find key
		map<CppUtils::String, CppUtils::String>::const_iterator i_entry = i_table->second.find( l_str[ i ] );

		if( i_entry != i_table->second.end() )
			// Found; replace
			l_str[ i ] = i_entry->second;
	}
}
//-------------------------------------------------------------
void StringTable::lookupStringLines (
		CppUtils::String const &lang,
		CppUtils::String &str
	) const
{
	// Ignore empty key
	//
	if( str.empty()	)
		return;


	// Split into lines
	//
	CppUtils::StringList l_str;
	char *s = strdup( str.c_str() );
	char *p1 = s;
	char *p2 = strchr( p1, '\n' );
	char *p3 = strchr( p1, '\r' );
	do {
		if( p3 && (!p2 || p3 < p2) )
			p2 = p3;

		if( p2 )
			*p2 = 0;

		l_str.push_back( p1 );

		if( p2 ) {
			p1 = p2+1;
			p2 = strchr( p1, '\n' );
			p3 = strchr( p1, '\r' );
		}
		else
			break;
	} while( true );



	// Delegate
	//
	lookupStrings( lang, l_str );


	// Re-compose
	//
	str = "";
	for(int i = 0; i < l_str.size(); ++i) {
		if( i ) str += "\n";
		str += l_str[ i ];
	}
}
//-------------------------------------------------------------
void StringTable::lookupString (
		CppUtils::String const &lang,
		CppUtils::String &str
	) const
{
	// Ignore empty key
	//
	if( str.empty()	)
		return;


	// Lock for access
	//
	LOCK_FOR_SCOPE( *this );


	// Retrieve the entry for the language
	//
	map<CppUtils::String, map<CppUtils::String, CppUtils::String> >::const_iterator i_table = stringMap_m.find( lang.empty() ? defaultLanguage_m : lang );
	if( i_table == stringMap_m.end() ) {
		// Unknown language; use default
		LOG( MSG_WARN, STRING_TABLE, exc_UnknownLocalization_scm << lang );
		i_table = stringMap_m.find( defaultLanguage_m );

		// If no default, give up
		LOG( MSG_ERROR, STRING_TABLE, "The default language \"" << defaultLanguage_m << "\" has no associated table." );
		return;
	}
	

	// Lookup the string
	//

	map<CppUtils::String, CppUtils::String>::const_iterator i_entry = i_table->second.find( str );

	if( i_entry != i_table->second.end() )
		// Found; replace
		str = i_entry->second;
}
//-------------------------------------------------------------
CppUtils::String StringTable::lookupStringInDefaultLang (
		CppUtils::String const &str
	) const
{
	// Lock for access
	//
	LOCK_FOR_SCOPE( *this );


	// Retrieve the entry for the default language
	//
	map<CppUtils::String, map<CppUtils::String, CppUtils::String> >::const_iterator i_table = stringMap_m.find( defaultLanguage_m );

	// If no default, give up
	if( i_table == stringMap_m.end() ) {
		LOG( MSG_ERROR, STRING_TABLE, "The default language \"" << defaultLanguage_m << "\" has no associated table." );
		return str;
	}


	// Lookup the string
	//
	map<CppUtils::String, CppUtils::String>::const_iterator i_entry = i_table->second.find( str );

	if( i_entry != i_table->second.end() )
		return i_entry->second;
	else
		return str;
}





// End of namespace
}

