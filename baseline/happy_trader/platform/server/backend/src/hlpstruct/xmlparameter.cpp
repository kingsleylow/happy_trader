#include "xmlparameter.hpp"


namespace HlpStruct { 

	XmlParameter::XmlParameter()
	{
	}

	XmlParameter::~XmlParameter()
	{
	}
	

	void XmlParameter::clear() {
			name_m.clear();
			params_m.clear();
			xmlparamlist_m.clear();
			xmlparams_m.clear();
		}

		
	bool XmlParameter::operator == (
			XmlParameter const &rhs
		) const
		{
			return ( (name_m==rhs.name_m) && (rhs.params_m==params_m) );
		}
		
		CppUtils::String& XmlParameter::getName() 
		{
			return name_m;
		}

		CppUtils::String const& XmlParameter::getName() const
		{
			return static_cast<CppUtils::String const&>(name_m);
		}

		CppUtils::Value& XmlParameter::getParameter(CppUtils::String const& name) {
			return params_m[name];
		}

		CppUtils::Value const& XmlParameter::getParameter(CppUtils::String const& name) const {
			CppUtils::ValueMap::const_iterator it = params_m.find(name);
			if (it==params_m.end())
				THROW( CppUtils::OperationFailed, "exc_XmlParameterAccess", "ctx_InvalidAttributeName", name );

			return it->second;
		}

		CppUtils::String& XmlParameter::getStringParameter(CppUtils::String const& name)
		{
			return getParameter(name).getAsString(true);
		}

		CppUtils::String const& XmlParameter::getStringParameter(CppUtils::String const& name) const
		{
			return getParameter(name).getAsString();
		}

		bool XmlParameter::isKeyExists(CppUtils::String const& name) const
		{
			if (params_m.find(name) != params_m.end()) 
					return true;
			
			if (xmlparams_m.find(name) != xmlparams_m.end())
				return true;

			if (xmlparamlist_m.find(name) != xmlparamlist_m.end())
				return true;

			return false;

		}

		// recursive calls
		XmlParameter& XmlParameter::getXmlParameter(CppUtils::String const& name) {
			return xmlparams_m[name];
		}

		XmlParameter const& XmlParameter::getXmlParameter(CppUtils::String const& name) const {
			XmlParameterMap::const_iterator it = xmlparams_m.find(name);
			
			if (it==xmlparams_m.end())
				THROW( CppUtils::OperationFailed, "exc_XmlParameterAccess", "ctx_InvalidAttributeName", name );

			return it->second;

		}

		XmlParameter::XmlParameterList& XmlParameter::getXmlParameterList(CppUtils::String const& name) {
			return xmlparamlist_m[name];
		}

		XmlParameter::XmlParameterList const& XmlParameter::getXmlParameterList(CppUtils::String const& name) const {
			XmlParameterMapList::const_iterator it = xmlparamlist_m.find(name);
			
			if (it==xmlparamlist_m.end())
				THROW( CppUtils::OperationFailed, "exc_XmlParameterAccess", "ctx_InvalidAttributeName", name );

			return it->second;

		}

		// keys
		void XmlParameter::getValueKeys(CppUtils::StringList& keys) const {
			CppUtils::getKeys<CppUtils::String, CppUtils::Value>(keys, params_m);
		}

		void XmlParameter::getXmlParametersKeys(CppUtils::StringList& keys) const {
			CppUtils::getKeys<CppUtils::String, XmlParameter>(keys, xmlparams_m);
		}
		
		void XmlParameter::getXmlParametersListKeys(CppUtils::StringList& keys) const {
			CppUtils::getKeys<CppUtils::String, XmlParameterList>(keys, xmlparamlist_m);

		}

	// ------------------------------------------------------------------


	ParametersHandler::ParametersHandler():
	ent_Tag_m(NULL),
	val_Tag_m(NULL),
	par_Tag_m(NULL),
	xmlPar_Tag_m(NULL),
	map_key_Tag_m(NULL),
	map_val_Tag_m(NULL),
	parser_m(NULL),
	xmlcontent_m(NULL),
	srcOffset_m(-1),
	stringMapFlag_m(-1)
	{
		ent_Tag_m = XMLString::transcode("ent");
		par_Tag_m = XMLString::transcode("par");
		val_Tag_m = XMLString::transcode("val");
		map_key_Tag_m = XMLString::transcode("k");
		map_val_Tag_m = XMLString::transcode("v");
		xmlPar_Tag_m = XMLString::transcode("xmlpar");
		xmlParList_Tag_m = XMLString::transcode("xmlparlist");
	}

	ParametersHandler::~ParametersHandler()
	{
		if (ent_Tag_m)	XMLString::release(&ent_Tag_m);
		if (par_Tag_m)  XMLString::release(&par_Tag_m);
		if (val_Tag_m)	XMLString::release(&val_Tag_m);
		if (map_key_Tag_m)	XMLString::release(&map_key_Tag_m);
		if (map_val_Tag_m)	XMLString::release(&map_val_Tag_m);
		if (xmlPar_Tag_m)	XMLString::release(&xmlPar_Tag_m);
		if (xmlParList_Tag_m) XMLString::release(&xmlParList_Tag_m);

	}

	void ParametersHandler::startElement (
      const XMLCh* const name,
      AttributeList& attrs
    )
	{
		
		
		switch (state_m) {
			case PHS_Init: {
				if (!XMLString::compareIString(name, ent_Tag_m)) {
					state_m = PHS_InsideEntity;
				}
				
				TRANSCODE_XMLSTRING(attrs.getValue("name"), parameter_m->name_m );
				break;
			}
			case PHS_InsideEntity: {

				
				if (!XMLString::compareIString(name, xmlParList_Tag_m)) {
					state_m = PHS_InsideXmlParameterList;
				}
				else if (!XMLString::compareIString(name, xmlPar_Tag_m)) {
					state_m = PHS_InsideXmlParameter;
				}
        else if (!XMLString::compareIString(name, par_Tag_m)) {
					state_m = PHS_InsideParameter;
				}
				
				TRANSCODE_XMLSTRING(attrs.getValue("name"), name_val_m );
				CppUtils::String type_s;

				if (name_val_m.size()==0)
					THROW(CppUtils::OperationFailed, "exc_ParametersHandlerParsingError", "ctx_InvalidParameterName", "");

			
				if (state_m==PHS_InsideParameter) {
					TRANSCODE_XMLSTRING(attrs.getValue("type"), type_s );
				
					CppUtils::Value par_val;
					CppUtils::Value::ValueType vt = (CppUtils::Value::ValueType)atol(type_s.c_str());
				
					par_val.setType(vt);
					parameter_m->params_m[name_val_m] = par_val;
					
				}
				else if (state_m==PHS_InsideXmlParameter){
					XmlParameter new_parameter;
					parameter_m->xmlparams_m[name_val_m] = new_parameter;
				}
				else if (state_m==PHS_InsideXmlParameterList){
					XmlParameter::XmlParameterList parameterslist;
					parameter_m->xmlparamlist_m[name_val_m] = parameterslist;
				}
				
				entityDepth_m = 0;
				idxBegin_m = parser_m->getSrcOffset();
				break;
				
			}
			case PHS_InsideParameter: {
				if (!XMLString::compareIString(name, val_Tag_m)) {
					state_m = PHS_InsideValue;
				}
				break;
			}
			
			case PHS_InsideXmlParameter_Ent: {
				if (!XMLString::compareIString(name, ent_Tag_m)) {
					entityDepth_m++;
				}

				break;
																			 }

			case PHS_InsideXmlParameterList_Ent: {
				if (!XMLString::compareIString(name, ent_Tag_m)) {
					entityDepth_m++;
				}
				break;
																			 }
			case PHS_InsideXmlParameter: {
				if (!XMLString::compareIString(name, ent_Tag_m)) {
					state_m = PHS_InsideXmlParameter_Ent;
					entityDepth_m++;
				}
				break;
			}
			case PHS_InsideXmlParameterList: {
				if (!XMLString::compareIString(name, ent_Tag_m)) {
          state_m = PHS_InsideXmlParameterList_Ent;
					entityDepth_m++;
				}
				break;
			}
			case PHS_InsideValue:
				
				if (!XMLString::compareIString(name, map_key_Tag_m)) {
					stringMapFlag_m = 1;
				}
				else if (!XMLString::compareIString(name, map_val_Tag_m)) {
					stringMapFlag_m = 2;
				}

				break;
			default: ;

		};
		
	}
  

	void ParametersHandler::endElement (
      const XMLCh* const name
    )
	{
	
		

		switch (state_m) {
			case PHS_Init:
				
				break;
			case PHS_InsideEntity:
				if (!XMLString::compareIString(name, ent_Tag_m)) {
					state_m = PHS_Init;
					
				}

				break;
			case PHS_InsideParameter:
				if (!XMLString::compareIString(name, par_Tag_m)) {
					state_m = PHS_InsideEntity;
					
				}
				break;
			case PHS_InsideValue:
				if (!XMLString::compareIString(name, val_Tag_m)) {
					state_m = PHS_InsideParameter;
					
				}
				break;
			case PHS_InsideXmlParameter_Ent: {
				if (!XMLString::compareIString(name, ent_Tag_m)) {
					idxEnd_m = parser_m->getSrcOffset();
					if (--entityDepth_m==0) {
						state_m = PHS_InsideXmlParameter;

						// catch entity 
						XmlParameter newXmlParameter;
						parsePiece(idxBegin_m, idxEnd_m, newXmlParameter);

						// assign to the parameter
						XmlParameter::XmlParameterMap::iterator it = parameter_m->xmlparams_m.find(name_val_m);

						if (it == parameter_m->xmlparams_m.end())
							THROW(CppUtils::OperationFailed, "exc_ParametersHandlerParsingError", "ctx_ParameterMustExist", name_val_m);
						it->second = newXmlParameter;
						
					}
				}

				break;
																			 }
			case PHS_InsideXmlParameterList_Ent: {
				if (!XMLString::compareIString(name, ent_Tag_m)) {
					idxEnd_m = parser_m->getSrcOffset();
					if (--entityDepth_m==0) {
						state_m = PHS_InsideXmlParameterList;

						// catch entity
						// extract piece of XML and parse 
						XmlParameter newXmlParameter;
						parsePiece(idxBegin_m, idxEnd_m, newXmlParameter);

						// assign to the parameter
						XmlParameter::XmlParameterMapList::iterator it = parameter_m->xmlparamlist_m.find(name_val_m);

						if (it == parameter_m->xmlparamlist_m.end())
							THROW(CppUtils::OperationFailed, "exc_ParametersHandlerParsingError", "ctx_ParameterMustExist", name_val_m);
						it->second.push_back(newXmlParameter);

						// adjust to another entity
						idxBegin_m = parser_m->getSrcOffset();
					}
				}

				break;
																			 }

			case PHS_InsideXmlParameterList: {
				if (!XMLString::compareIString(name, xmlParList_Tag_m)) {
						state_m = PHS_InsideEntity;
				}
				
				break;
																			 }
			case PHS_InsideXmlParameter: {
				if (!XMLString::compareIString(name, xmlPar_Tag_m)) {
					state_m = PHS_InsideEntity;
				}
				break;
																	 }
			default: ;

		};
	}
  
	 
	void ParametersHandler::characters (
      const XMLCh* const chars,
      const XMLSize_t length
    )
	{
	
		srcOffset_m = parser_m->getSrcOffset();

		switch (state_m) 
		{
			case PHS_Init: {
				
				break;
										 }
			case PHS_InsideEntity: {
				
				break;
														 }
			case PHS_InsideParameter: {
				
				break;
																}
			case PHS_InsideValue: {

				
				//CppUtils::String name_s;

				// here we may see very large content
				getContent(chars, length, strvalue_m);
				//LOG(MSG_INFO, "XMLPARAMETER", "Content received: ");
								 
				//
				if (name_val_m.size()==0)
					return;

				// reap he content
				CppUtils::ValueMap::iterator it = parameter_m->params_m.find(name_val_m);

				if (it == parameter_m->params_m.end())
					THROW(CppUtils::OperationFailed, "exc_ParametersHandlerParsingError", "ctx_ParameterMustExist", name_val_m);

				CppUtils::Value& val = it->second;

							
				switch(val.getType()) 
				{  
					
					case CppUtils::Value::VT_Date: 
						//val.getAsDate(false) = atof(strvalue_m.c_str());
						val.getAsDate(false) = CppUtils::stringToF64(strvalue_m.c_str(), NULL);
						break;
		  
					case CppUtils::Value::VT_Uid: 
						val.getAsUid(false).fromString(strvalue_m.c_str());
						break;


					case CppUtils::Value::VT_Double: 
						
						
						//val.getAsDouble(false) = atof(strvalue_m.c_str());
						val.getAsDouble(false) = CppUtils::stringToF64(strvalue_m.c_str(), NULL);
						break;

					case CppUtils::Value::VT_String: 
						val.getAsString(false) = strvalue_m;
						break;

					case CppUtils::Value::VT_Int:
						val.getAsInt(false) = atol(strvalue_m.c_str());
						break;

					case CppUtils::Value::VT_DoubleList: 
						//val.getAsDoubleList(false).push_back( atof(strvalue_m.c_str()) );
						val.getAsDoubleList(false).push_back( CppUtils::stringToF64(strvalue_m.c_str() , NULL) );
						
						break;

					case CppUtils::Value::VT_StringList: 
						val.getAsStringList(false).push_back(strvalue_m);
						break;
          
					case CppUtils::Value::VT_IntList: 
						val.getAsIntList(false).push_back( atol(strvalue_m.c_str()) );
						break;

					case CppUtils::Value::VT_DateList: 
						//val.getAsDateList(false).push_back(atof(strvalue_m.c_str()));
						val.getAsDateList(false).push_back(CppUtils::stringToF64(strvalue_m.c_str(), NULL));
						
						break;

					case CppUtils::Value::VT_UidList: 
					{
						CppUtils::Uid uid(strvalue_m);
						val.getAsUidList(false).push_back(uid);
						break;
					}

					case CppUtils::Value::VT_StringMap: 
					{
						if (stringMapFlag_m == 1) {
							// key
							stringMapKey_m = strvalue_m;
						}
						else if (stringMapFlag_m ==2) {
							// value
							val.getAsStringMap(false)[stringMapKey_m]  = strvalue_m;
						}

						break;
					}

					default: 
						THROW( CppUtils::OperationFailed, "exc_ParametersHandlerParsingError", "ctx_UnsupportedType", (long)val.getType() );
					
					
				};
				
				
				break;
			}
			
			default: ;

		};

	}
 

	void ParametersHandler::serializeXmlParameter(XmlParameter const& parameter, CppUtils::MemoryChunk& xmlcontent)
	{
		CppUtils::CleanedStreamBuffer<ostrstream> out(false);
		xmlcontent.length(0);

		//out() << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
		out() << "<?xml version=\"1.0\" encoding=\"windows-1251\"?>";
		serializeXmlParameterHelper(parameter, out);
		
		// no multi byte characters
		xmlcontent.length(out().pcount());
		memcpy(xmlcontent.data(), out().str(), out().pcount());

	}

	void ParametersHandler::serializeXmlParameterHelper(XmlParameter const& parameter, CppUtils::CleanedStreamBuffer<ostrstream>& out)
	{
		
		//CppUtils::CleanedStreamBuffer<ostrstream> out(false);
		
		// transform XmlParameter to valid XML content
		//xmlcontent.length(0);

		//if (!recursive)
		//	out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
		
		out() << "<ent name=\"" << parameter.name_m << "\">";
		
		// recursive xml parameters
		if (parameter.xmlparams_m.size() > 0) {
			//CppUtils::MemoryChunk xmlcontent;
			for(XmlParameter::XmlParameterMap::const_iterator it = parameter.xmlparams_m.begin(); it != parameter.xmlparams_m.end(); it++) {
				out() << "<xmlpar name=\"" << it->first << "\">";
				
				// serialize it->second
				//XmlParameter const& xmlparameter  = it->second;
				//serializeXmlParameterHelper(xmlparameter, xmlcontent);

				//out().write(xmlcontent.data(), xmlcontent.length());		

				CppUtils::CleanedStreamBuffer<ostrstream> out2(false);
				serializeXmlParameterHelper(it->second, out2);
				
				out().write(out2().str(), out2().pcount());



				out() << "</xmlpar>";
			}
		}

		// recursive xml parameter list
		if (parameter.xmlparamlist_m.size() > 0) {
			//CppUtils::MemoryChunk xmlcontent;
			for(XmlParameter::XmlParameterMapList::const_iterator it = parameter.xmlparamlist_m.begin(); it != parameter.xmlparamlist_m.end(); it++) {
				out() << "<xmlparlist name=\"" << it->first << "\">";
				
				// serialize it->second
				XmlParameter::XmlParameterList const& xmlparameterlist  = it->second;
				for(int i = 0; i < xmlparameterlist.size(); i++) {


					//serializeXmlParameterHelper(xmlparameterlist[i], xmlcontent);
					//out().write(xmlcontent.data(), xmlcontent.length());			

					CppUtils::CleanedStreamBuffer<ostrstream> out3(false);
					serializeXmlParameterHelper(xmlparameterlist[i], out3);
					out().write(out3().str(), out3().pcount());
				}

				out() << "</xmlparlist>";
			}
		}

			// scalar parameters
			for(CppUtils::ValueMap::const_iterator it = parameter.params_m.begin(); it != parameter.params_m.end(); it++) {
				
							
				out() << "<par name=\"" << it->first << "\" type=\"";
				switch(it->second.getType()) {
					case CppUtils::Value::VT_Date: 
					{
						out() << it->second.getType() << "\">" << "<val>" << CppUtils::double2String(it->second.getAsDate(),16) << "</val>";
						break;
					}

					case CppUtils::Value::VT_Uid: 
					{
						out() << it->second.getType() << "\">" << "<val>" << it->second.getAsUid().toString() << "</val>";
						break;
					}
					
					case CppUtils::Value::VT_Double: 
					{
						out() << it->second.getType() << "\">" << "<val>" << CppUtils::double2String(it->second.getAsDouble())<< "</val>";
						break;
					}
					
					case CppUtils::Value::VT_String: 
					{
						out() << it->second.getType() << "\">" << "<val>" << it->second.getAsString()<< "</val>";
						break;
					}

					case CppUtils::Value::VT_Int:
					{
						out() << it->second.getType() << "\">" << "<val>" << it->second.getAsInt()<< "</val>";
						break;
					}

					case CppUtils::Value::VT_IntList: 
					{

						out() << it->second.getType() << "\">";
						CppUtils::IntList const& intlist = it->second.getAsIntList();
						for (int i =0; i < intlist.size();i++) {
							out() << "<val>" << intlist[ i ]<< "</val>";
						}
						break;
					}

					case CppUtils::Value::VT_DoubleList: 
					{

						out() << it->second.getType() << "\">";
						CppUtils::DoubleList const& doublelist = it->second.getAsDoubleList();
						for (int i =0; i < doublelist.size();i++) {
							out() << "<val>" << CppUtils::double2String(doublelist[ i ])<< "</val>";
						}
						break;
					}

					case CppUtils::Value::VT_StringList: 
					{

						out() << it->second.getType() << "\">";
						CppUtils::StringList const& strlist = it->second.getAsStringList();
						for (int i =0; i < strlist.size();i++) {
							out() << "<val>" << strlist[ i ]<< "</val>";
						}
						break;
					}

					case CppUtils::Value::VT_DateList: 
					{

						out() << it->second.getType() << "\">";
						CppUtils::DoubleList const& datelist = it->second.getAsDateList();
						for (int i =0; i < datelist.size();i++) {
							out() << "<val>" << CppUtils::double2String(datelist[ i ])<< "</val>";
						}
						break;
					}

					case CppUtils::Value::VT_UidList: 
					{

						out() << it->second.getType() << "\">";
						CppUtils::UidList const& uidlist = it->second.getAsUidList();
						for (int i =0; i < uidlist.size();i++) {
							out() << "<val>" << uidlist[ i ].toString()<< "</val>";
						}
						break;
					}

					case CppUtils::Value::VT_StringMap:
					{

						out() << it->second.getType() << "\">";
						CppUtils::StringMap const& valmap = it->second.getAsStringMap();

						for (CppUtils::StringMap::const_iterator q2 = valmap.begin(); q2 != valmap.end(); q2++  ) {
							out() << "<val><k>" <<	q2->first << "</k><v>" << q2->second << "</v></val>";
						}
						break;
					}
					

					default: 
					{
						THROW( CppUtils::OperationFailed, "exc_ParameterHandlerError", "ctx_UnsupportedType", (long)it->second.getType() );
					}
				};
				
				out() << "</par>";
			}

			
			

		out() <<  "</ent>";
	

		// no multi byte characters
		//xmlcontent.length(out().pcount());
		//memcpy(xmlcontent.data(), out().str(), out().pcount());

		// autofreeze
		//out.freeze(false);
		
		
	}

	void ParametersHandler::deserializeXmlParameter(XmlParameter& parameter, CppUtils::MemoryChunk const& xmlcontent)
	{
		// locking the process
		LOCK_FOR_SCOPE(deserializeMtx_m);

		//LOG(MSG_DEBUG_DETAIL, "XMLPARAMETER", "Deserialize XML: \"" << xmlcontent.toString() );

		name_val_m.clear();
		

		idxBegin_m = -1;
		idxEnd_m = -1;

		parser_m = new SAXParser();
		parser_m->setCalculateSrcOfs(true); 

		parameter.clear(); 
		parameter_m = &parameter;
		xmlcontent_m = &xmlcontent;

		state_m = PHS_Init;

	try {
				parseXMLNoCatch(xmlcontent, parser_m, this);
			delete parser_m;
	}
	catch (const XMLException& e) {
		CppUtils::String problem;
    TRANSCODE_XMLSTRING( e.getMessage(), problem );
		LOG( MSG_ERROR, "XMLPARAMETER", "Debug info on XML exception: " << getDebugInfo() );

		delete parser_m;
		THROW( XmlParsingError, "exc_XMLParserError", "ctx_deserializeXmlParameter", problem);
  }
  catch (const SAXException& e) {
		CppUtils::String problem;
    TRANSCODE_XMLSTRING( e.getMessage(), problem );
		LOG( MSG_ERROR, "XMLPARAMETER", "Debug info on SAX exception: " << getDebugInfo() );

		delete parser_m;
		THROW( XmlParsingError, "exc_SAXParserError", "ctx_deserializeXmlParameter", problem );
  }
	catch (CppUtils::Exception& e) {
		LOG( MSG_ERROR, "XMLPARAMETER", "Debug info on exception: " << getDebugInfo() );

		delete parser_m;
		THROW( XmlParsingError, "exc_Exception", "ctx_deserializeXmlParameter", EXC2STR(e));
  }
#ifdef HANDLE_NATIVE_EXCEPTIONS
	catch(Testframe::Exception& e) {
		LOG( MSG_ERROR, "XMLPARAMETER", "Debug info on MSVC exception: " << getDebugInfo() );

		delete parser_m;
		THROW( XmlParsingError, "exc_MSVCException", "ctx_deserializeXmlParameter", e.message());
	}
#endif
		
		parser_m = NULL;
		xmlcontent_m = NULL;
		
	}

	void ParametersHandler::deserializeXmlParameter(XmlParameter& parameter, CppUtils::String const& xmlcontent)
	{
		CppUtils::MemoryChunk xmlcontent_buf(xmlcontent);
		deserializeXmlParameter(parameter, xmlcontent_buf);

	}

	void ParametersHandler::serializeXmlParameter(XmlParameter const& parameter, CppUtils::String& xmlcontent)
	{
		CppUtils::MemoryChunk xmlcontent_buf;
		serializeXmlParameter(parameter, xmlcontent_buf);
		
		xmlcontent = xmlcontent_buf.toString();

	}

	void ParametersHandler::getContent(
		const XMLCh* const chars,
    const unsigned int length,
		CppUtils::String &content
	)
	{
				content.clear();
				char* chars_s = NULL;
				try {
					if (length > 0) {
						chars_s =  XMLString::transcode(chars);
						content = chars_s ? chars_s: "";
						if (chars_s)
							XMLString::release(&chars_s);
					}
				}
				catch(...) {

					LOG(MSG_WARN, "XMLPARAMETER", "Exception in getContent(...)");

					if (chars_s)
						XMLString::release(&chars_s);
				}
	}

	CppUtils::String ParametersHandler::getDebugInfo() const
	{
		return "ATTRIBUTE NAME: \"" + name_val_m + "\" VALUE: \"" + strvalue_m + "\" ENTITY DEPTH: " + CppUtils::long2String(entityDepth_m) + 
			" SRC OFFSET: " + CppUtils::long2String(srcOffset_m);

		//return ParametersHandler::getDebugInfo();
	}

	void ParametersHandler::parsePiece(int const idxBeg, int const idxEnd, XmlParameter& newXml)
	{
		CppUtils::MemoryChunk newXmlChunk;
		newXmlChunk.length(idxEnd_m-idxBegin_m);
		memcpy(newXmlChunk.data(), xmlcontent_m->data()+idxBegin_m, newXmlChunk.length());

		ParametersHandler newXmlHandler;
		newXmlHandler.deserializeXmlParameter( newXml, newXmlChunk);

	}

	void deserializeXmlParameter(XmlParameter& parameter, CppUtils::String const& xmlcontent)
	{
		HlpStruct::ParametersHandler hndl;
		hndl.deserializeXmlParameter(parameter,xmlcontent );
	}

	void serializeXmlParameter(XmlParameter const &parameter, CppUtils::String& xmlcontent)
	{
		HlpStruct::ParametersHandler hndl;
		hndl.serializeXmlParameter(parameter, xmlcontent);
	}

	void serializeXmlParameter(XmlParameter const& parameter, CppUtils::MemoryChunk& xmlcontent)
	{
		HlpStruct::ParametersHandler hndl;
		hndl.serializeXmlParameter(parameter, xmlcontent);
	}

}; // end of namespace