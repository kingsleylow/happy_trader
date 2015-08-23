#include "xmlwrap.hpp"


namespace HlpStruct {
	

/** Compose an error message from the SAX exception. */
CppUtils::String SaxErrorHandler::extractMessage (
      const SAXParseException& exception
    )
  {
    CppUtils::String msg, sid;
    TRANSCODE_XMLSTRING( exception.getMessage(), msg );
    TRANSCODE_XMLSTRING( exception.getSystemId(), sid );

		
    char buf[64];
    sprintf( buf, " @ line %I64d, col %I64d", exception.getLineNumber(), exception.getColumnNumber() );

    return msg + buf + " in " + sid;
  }

void SaxErrorHandler::warning (
      const SAXParseException& exception
    )
  {
    LOG( MSG_WARN, PARSER, "Warning : " << extractMessage( exception ) );
  }

void SaxErrorHandler::error (
      const SAXParseException& exception
    )
  {
    CppUtils::String msg = extractMessage( exception );
    LOG( MSG_ERROR, PARSER, "Error : " << msg );
    THROW( CppUtils::OperationFailed, "exc_XMLParserError", "ctx_ParsingXML", msg );
  }

void SaxErrorHandler::fatalError (
      const SAXParseException& exception
    )
  {
    CppUtils::String msg = extractMessage( exception );
		LOG( MSG_ERROR, PARSER, "Fatal error parsing XML : " << extractMessage( exception )  );
		THROW( CppUtils::OperationFailed, "exc_XMLParserError", "ctx_ParsingXML", msg );
  }

void SaxErrorHandler::resetErrors (
 )
 {
 }

// ----------------------------------

SaxDocumentHandler::SaxDocumentHandler (
    )
  {
  }

void SaxDocumentHandler::startElement (
      const XMLCh* const name,
      AttributeList& attrs
    )
  {
  }

void SaxDocumentHandler::endElement (
      const XMLCh* const name
    )
  {
  }

void SaxDocumentHandler::ignorableWhitespace (
      const   XMLCh* const    chars, 
			const XMLSize_t     length
    )
  {
  }

void SaxDocumentHandler::processingInstruction (
      const XMLCh* const target,
      const XMLCh* const data
    )
  {
  }

void SaxDocumentHandler::characters (
      const XMLCh* const chars,
      const XMLSize_t length
    )
  {
  }

void SaxDocumentHandler::resetDocument (
    )
  {
  }

void SaxDocumentHandler::setDocumentLocator (
      const Locator* const locator
    )
  {
  }

void SaxDocumentHandler::startDocument (
    )
  {
  }

void SaxDocumentHandler::endDocument (
    )
  {
  }

CppUtils::String SaxDocumentHandler::getDebugInfo() const
{
		return "(no debug info)";
}

XmlParsingError::XmlParsingError(CppUtils::String const &str,
		CppUtils::String const &ctx,
		CppUtils::String const &arg
	):CppUtils::OperationFailed(str, ctx, arg)
{
}

// ------------------

void parseXML (
	CppUtils::MemoryChunk const &xmlcontent,
	SAXParser *parser,
  SaxDocumentHandler *docHandler
)
{
	//LOG( MSG_DEBUG, PARSER, "Parsing pure XML content" );

	// Parse
  //
  try {
    // Set doc & error handler
    //
    parser->setDocumentHandler( docHandler );

    SaxErrorHandler errHandler;
    parser->setErrorHandler( &errHandler );


    // Set parser properties
    //
    parser->setDoNamespaces( false );
    parser->setDoSchema( false );
    parser->setValidationScheme( SAXParser::Val_Never );
    

    // Parse content (expecting always single-byte characters)
		MemBufInputSource memBufIS( (const XMLByte*)xmlcontent.data(),xmlcontent.length(), "memory" , false );

    //
    parser->parse(memBufIS );
  }
  catch (const XMLException& e) {
		CppUtils::String problem;
    TRANSCODE_XMLSTRING( e.getMessage(), problem );
		LOG( MSG_ERROR, PARSER, "Debug info on exception: " << docHandler->getDebugInfo() );
    THROW( CppUtils::OperationFailed, "exc_XMLParserError", "ctx_ParsingXML", problem );
  }
  catch (const SAXException& e) {
		CppUtils::String problem;
    TRANSCODE_XMLSTRING( e.getMessage(), problem );
		LOG( MSG_ERROR, PARSER, "Debug info on exception: " << docHandler->getDebugInfo() );
    THROW( CppUtils::OperationFailed, "exc_SAXParserError", "ctx_ParsingXML", problem );
  }
	catch (CppUtils::Exception& e) {
		LOG( MSG_ERROR, PARSER, "Debug info on exception: " << docHandler->getDebugInfo() );
		THROW( CppUtils::OperationFailed, "exc_ParserError", "ctx_ParsingXML", e.message()  );
  }
	
	//LOG( MSG_DEBUG, PARSER, "Parsed pure XML content" );
}

// ----------------------------

void parseXML (
		CppUtils::String const &xmlFileName,
    CppUtils::String const &schemaName,
    CppUtils::String const &schemaFileName,
    SAXParser *parser,
    SaxDocumentHandler *docHandler
  )
{
  LOG( MSG_DEBUG, PARSER, "Parsing XML file \"" << xmlFileName << "\" with schema \"" << schemaFileName << "\"..." );


  // Compose the schema argument
  //
  CppUtils::String schema = schemaName + " " + CppUtils::expandPathName( schemaFileName );
	if( !CppUtils::fileExists( schemaFileName ) )
		THROW( CppUtils::ResourceFailure, "exc_CannotOpenXMLSchema", "", schemaFileName );


  // Parse
  //
  try {
    // Set doc & error handler
    //
    parser->setDocumentHandler( docHandler );

    SaxErrorHandler errHandler;
    parser->setErrorHandler( &errHandler );


    // Set parser properties
    //
    parser->setDoNamespaces( true );
    parser->setDoSchema( true );
    parser->setValidationScheme( SAXParser::Val_Always );
    parser->setExternalSchemaLocation( schema.c_str() );


    // Parse file
    //
    parser->parse( xmlFileName.c_str() );
  }
  catch (const XMLException& e) {
		
		CppUtils::String problem;
    TRANSCODE_XMLSTRING( e.getMessage(), problem );
		LOG( MSG_ERROR, PARSER, "Debug info on exception: " << docHandler->getDebugInfo() );
    THROW( CppUtils::OperationFailed, "exc_XMLParserError", "ctx_ParsingXML", problem );
  }
  catch (const SAXException& e) {
		CppUtils::String problem;
    TRANSCODE_XMLSTRING( e.getMessage(), problem );
		LOG( MSG_ERROR, PARSER, "Debug info on exception: " << docHandler->getDebugInfo() );
    THROW( CppUtils::OperationFailed, "exc_XMLParserError", "ctx_ParsingXML", problem );
  }
	catch (CppUtils::Exception& e) {
		LOG( MSG_ERROR, PARSER, "Debug info on exception: " << docHandler->getDebugInfo() );
		THROW( CppUtils::OperationFailed, "exc_ParserError", "ctx_ParsingXML", e.message()  );
  }


  // Log
  LOG( MSG_DEBUG, PARSER, "Parsed XML file \"" << xmlFileName );
}


// ------------------------------

void parseXMLNoCatch(
	CppUtils::MemoryChunk const &xmlcontent,
	SAXParser *parser,
  DocumentHandler *docHandler
)
{
		parser->setDocumentHandler( docHandler );

    SaxErrorHandler errHandler;
    parser->setErrorHandler( &errHandler );


    // Set parser properties
    //
    parser->setDoNamespaces( false );
    parser->setDoSchema( false );
    parser->setValidationScheme( SAXParser::Val_Never );
    

    // Parse content (expecting always single-byte characters)
		MemBufInputSource memBufIS( (const XMLByte*)xmlcontent.data(),xmlcontent.length(), "memory" , false );

    //
    parser->parse(memBufIS );
}

// --------------------------------


}; // end of namespace