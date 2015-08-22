
#ifndef _HLPSTRUCT_XMLWRAP_INCLUDED
#define _HLPSTRUCT_XMLWRAP_INCLUDED

#include "hlpstructdefs.hpp"
#include <list>


// XML parsing
//
#include "xercesc/util/platformutils.hpp" 
XERCES_CPP_NAMESPACE_USE
#include "xercesc/sax/errorhandler.hpp" 
XERCES_CPP_NAMESPACE_USE
#include "xercesc/sax/documenthandler.hpp" 
XERCES_CPP_NAMESPACE_USE
#include "xercesc/sax/saxparseexception.hpp" 
XERCES_CPP_NAMESPACE_USE
#include "xercesc/parsers/SAXparser.hpp" 
XERCES_CPP_NAMESPACE_USE
//#include "xercesc/framework/XMLFormatter.hpp" 
//XERCES_CPP_NAMESPACE_USE
#include <xercesc/framework/MemBufInputSource.hpp>
XERCES_CPP_NAMESPACE_USE
#include <xercesc/util/TransService.hpp>
XERCES_CPP_NAMESPACE_USE
//$(HT_DEVEL_ROOT)\extern\msvc\xerces-c-3.1.1\Build\Win32\VC10\Debug\xerces-c_3D.lib
//$(HT_DEVEL_ROOT)\extern\msvc\xerces-c-3.1.1\Build\Win32\VC10\Release\xerces-c_3.lib

namespace HlpStruct {


#define PARSER "XML PARSER"


// Helper for XML transcoding
#define TRANSCODE_XMLSTRING( A, B ) \
  { \
    char buf[2048]; \
		bool res = XMLString::transcode( A, buf, 2047 ); \
		HTASSERT( res ); \
		B = buf; \
  }

#define TRANSCODE_XMLSTRING2( A, B, BUFSIZE ) \
  { \
    char buf[BUFSIZE]; \
		bool res = XMLString::transcode( A, buf, 2047 ); \
		HTASSERT( res ); \
		B = buf; \
  }





// =======================================================
// SaxErrorHandler
// =======================================================
/** Basic error handler for SAX parsing.

    \author Olaf Lenzmann
*/

class SaxErrorHandler : public ErrorHandler {

public:

  /** Compose an error message from the SAX exception. */
	CppUtils::String extractMessage (
      const SAXParseException& exception
    );
  

  virtual void warning (
      const SAXParseException& exception
    );
  

  virtual void error (
      const SAXParseException& exception
    );
  

  virtual void fatalError (
      const SAXParseException& exception
    );
  

  virtual void resetErrors (
    );
  
};




// =======================================================
// SaxDocumentHandler
// =======================================================
/** Base document handler impl for parsing XML. Complete
    no-io implementation.

    \author Olaf Lenzmann
*/
class SaxDocumentHandler : public DocumentHandler {

public:

  SaxDocumentHandler (
    );
  

  virtual void startElement (
      const XMLCh* const name,
      AttributeList& attrs
    );
  

  virtual void endElement (
      const XMLCh* const name
    );
  

  virtual void ignorableWhitespace (
      const   XMLCh* const    chars, 
			const XMLSize_t     length
    );
  

  virtual void processingInstruction (
      const XMLCh* const target,
      const XMLCh* const data
    );
  

  virtual void characters (
      const XMLCh* const chars,
      const XMLSize_t length
    );
  

  virtual void resetDocument (
    );
  

  virtual void setDocumentLocator (
      const Locator* const locator
    );
  
  virtual void startDocument (
    );
  

  virtual void endDocument (
    );
  

	virtual CppUtils::String getDebugInfo() const;
	
};




// =======================================================
// Parsing
// =======================================================
/** Helper function to init the XML subsystem. */
inline void initXML (
  )
{
  // Init the XML subsystem
  //
  XMLPlatformUtils::Initialize();
}


/** Helper function to shut down the XML subsystem. */
inline void shutdownXML (
  )
{
  // Shut down the XML subsystem
  //
  XMLPlatformUtils::Terminate();
}

/** Helper function to perform parsing of an XML string,
    without schema validation.
*/

void parseXMLNoCatch(
	CppUtils::MemoryChunk const &xmlcontent,
	SAXParser *parser,
  DocumentHandler *docHandler
);

void parseXML (
	CppUtils::MemoryChunk const &xmlcontent,
	SAXParser *parser,
  SaxDocumentHandler *docHandler
);

/** Helper function to perform parsing of an XML file,
    using schema validation.
*/
void parseXML (
		CppUtils::String const &xmlFileName,
    CppUtils::String const &schemaName,
    CppUtils::String const &schemaFileName,
    SAXParser *parser,
    SaxDocumentHandler *docHandler
  );

/**
* XML parsing exception
*/

class HLPSTRUCT_EXP XmlParsingError : public CppUtils::OperationFailed
{
public:
	XmlParsingError(
		CppUtils::String const &str,
		CppUtils::String const &ctx,
		CppUtils::String const &arg
	);
	

};



// End of namespace
};


#endif



