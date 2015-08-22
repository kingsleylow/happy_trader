#ifndef _HLPSTRUCT_XMLPARAMETER_INCLUDED
#define _HLPSTRUCT_XMLPARAMETER_INCLUDED

#include "hlpstructdefs.hpp"
#include "xmlwrap.hpp"



namespace HlpStruct {

	  
	// this is general structure to hold XML - like parametrs to pass over external connections
//#pragma pack(1)

	class HLPSTRUCT_EXP XmlParameter {															
	public:

		XmlParameter();

		~XmlParameter();
	
		friend class ParametersHandler;

		typedef vector< XmlParameter > XmlParameterList;

		typedef map<CppUtils::String, XmlParameter> XmlParameterMap;

		typedef map<CppUtils::String, XmlParameter::XmlParameterList > XmlParameterMapList;

		void clear();

		
		bool operator == (
			XmlParameter const &rhs
		) const;
		
		CppUtils::String& getName();

		CppUtils::String const& getName() const;

		CppUtils::Value& getParameter(CppUtils::String const& name);

		CppUtils::Value const& getParameter(CppUtils::String const& name) const;

		// simple access to string parameter
		CppUtils::String& getStringParameter(CppUtils::String const& name);

		CppUtils::String const& getStringParameter(CppUtils::String const& name) const;

		// whether this parameter exists no matter what type
		bool isKeyExists(CppUtils::String const& name) const;

		// recursive calls
		XmlParameter& getXmlParameter(CppUtils::String const& name);

		XmlParameter const& getXmlParameter(CppUtils::String const& name) const;

		XmlParameterList& getXmlParameterList(CppUtils::String const& name);

		XmlParameterList const& getXmlParameterList(CppUtils::String const& name) const;

		// keys
		void getValueKeys(CppUtils::StringList& keys) const;

		void getXmlParametersKeys(CppUtils::StringList& keys) const;
		
		void getXmlParametersListKeys(CppUtils::StringList& keys) const;



		// name of parameter
		CppUtils::String name_m;

		// parameters map
		CppUtils::ValueMap params_m;

		// recursive parameters map
		XmlParameterMap xmlparams_m;

		// recursive parameter map list
		XmlParameterMapList xmlparamlist_m;

				
	};

//#pragma pack()

	class HLPSTRUCT_EXP ParametersHandler:  public SaxDocumentHandler {
			
	public:

		ParametersHandler();

		~ParametersHandler();

		// functions to work with XML content

		void serializeXmlParameter(XmlParameter const& parameter, CppUtils::MemoryChunk& xmlcontent);

		void deserializeXmlParameter(XmlParameter& parameter, CppUtils::MemoryChunk const& xmlcontent);

		void deserializeXmlParameter(XmlParameter& parameter, CppUtils::String const& xmlcontent);

		void serializeXmlParameter(XmlParameter const &parameter, CppUtils::String& xmlcontent);

	private:

	public:

		void serializeXmlParameterHelper(XmlParameter const& parameter, CppUtils::CleanedStreamBuffer<ostrstream>& out);

		enum State {
			PHS_Init = 0,
			PHS_InsideEntity = 1,
			PHS_InsideParameter = 2,
			PHS_InsideValue = 3,
			
			PHS_InsideXmlParameter = 4,

			// inside xmlparam and entity
			PHS_InsideXmlParameter_Ent = 5,

			PHS_InsideXmlParameterList = 6,

			// when we are inside xmlparamlist and entity
			PHS_InsideXmlParameterList_Ent = 7,
		};
		
		virtual void startElement (
      const XMLCh* const name,
      AttributeList& attrs
    );
  

		virtual void endElement (
      const XMLCh* const name
    );
  

 
		virtual void characters (
      const XMLCh* const chars,
      const XMLSize_t length
    );

		
		virtual CppUtils::String getDebugInfo() const;

		// parse piece of source content
		void parsePiece(int const idxBeg, int const idxEnd, XmlParameter& newXml);
 
		// parameter ptr
		XmlParameter* parameter_m;

		// finite machine state
		State state_m;

		// current parameter type and name
		CppUtils::String name_val_m;

		CppUtils::String strvalue_m;

		int srcOffset_m;
		
		
		// tags to be processed
		XMLCh* ent_Tag_m;

		XMLCh* par_Tag_m;

		XMLCh* xmlPar_Tag_m;

		XMLCh* xmlParList_Tag_m;

		XMLCh* val_Tag_m;

		// for VT_StringMap
		XMLCh* map_key_Tag_m;

		XMLCh* map_val_Tag_m;

		int stringMapFlag_m;

		CppUtils::String stringMapKey_m;

		// read & transcode content
		void getContent(
			const XMLCh* const chars,
			const unsigned int length,
			CppUtils::String &content
		);

		
		// for recusrive parsing
		int entityDepth_m;

		int idxBegin_m;

		int idxEnd_m;


		// reference to the parser
		SAXParser* parser_m;

	
		CppUtils::MemoryChunk const* xmlcontent_m;

		CppUtils::Mutex deserializeMtx_m;

		
	};

	// non member functions
	void HLPSTRUCT_EXP deserializeXmlParameter(XmlParameter& parameter, CppUtils::String const& xmlcontent);

	void HLPSTRUCT_EXP serializeXmlParameter(XmlParameter const& parameter, CppUtils::String& xmlcontent);

	void HLPSTRUCT_EXP serializeXmlParameter(XmlParameter const& parameter, CppUtils::MemoryChunk& xmlcontent);


}; // end namespace


#endif