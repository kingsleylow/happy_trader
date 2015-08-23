#ifndef _ALGLIBMT2_EXCHANGE_PACKETS_BASE_INCLUDED
#define _ALGLIBMT2_EXCHANGE_PACKETS_BASE_INCLUDED

#include "shared/cpputils/src/cpputils.hpp"


/**
* This file encapsulate JSON exchange packets, so keep in hpp file to use in mt proxy project
*/
namespace AlgLib {

	#define MCTOR_ID(CLASS, ID, COMMAND)	\
	static const int CLASS_ID = ID;	\
	virtual int getClassId()	const \
	{	\
		return CLASS_ID;	\
	};	\
	virtual char const* getClassName() const	\
	{	\
		static const char* name = #CLASS;	\
		return name;	\
	};	\
	virtual CppUtils::String const& getClassName2() const	\
	{	\
		static const CppUtils::String name = #CLASS;	\
		return name;	\
	};	\
	static char const* getCommandNameStatic()	\
	{	\
		static const char* name = COMMAND;	\
		return name;	\
	};	\
	static CppUtils::String const& getCommandName2Static()	\
	{	\
		static const CppUtils::String name = COMMAND;	\
		return name;	\
	};	\
	char const* getCommandName() const	\
	{	\
		return CLASS::getCommandNameStatic();	\
	};	\
	CppUtils::String const& getCommandName2() const	\
	{	\
		return CLASS::getCommandName2Static();	\
	};	\
	typedef CLASS ThisClass;	\
	
	
	#define MCTOR(CLASS, COMMAND) MCTOR_ID(CLASS, __COUNTER__, COMMAND)

	// ------------------------------------------------
	// base class of all dialof elements
	// note that based n command name we use deseralization things

	
	class BaseDlgEntity: public CppUtils::RefCounted {
		public:

			static CppUtils::String jsonToString(json::Object const& json_init_root)
			{
				std::stringstream init_stream;
				json::Writer::Write(json_init_root, init_stream);

				CppUtils::String result = init_stream.str();
				return result;
			};

			static json::Object stringToJson(CppUtils::String const& data)
			{
				std::stringstream init_stream;
				init_stream << data;
				json::Object json_init_root;
				json::Reader::Read(json_init_root, init_stream);

				return json_init_root;
			};
			
			virtual ~BaseDlgEntity()
			{
			};

			virtual int getClassId() const = 0;

			virtual char const* getClassName() const = 0;

			virtual CppUtils::String const& getClassName2() const = 0;

			virtual char const* getCommandName() const = 0;

			virtual CppUtils::String const& getCommandName2() const = 0;

			// serialize to JSON
			virtual json::Object toJson() const	 = 0;

			// deserilaize this from JSON assuming this is correct object
			virtual void fromJson(json::Object const& json_init_root) =0;

		

			
	};


	
	// ------------------------------------------------
	

	class JsonSerializerHelper
	{
		public:

			CppUtils::String operator () (BaseDlgEntity const& base)
			{
				json::Object json_init_root= base.toJson();
				return BaseDlgEntity::jsonToString(json_init_root);
			}
	};

	template<class T>
	class JsonDeserializerHelper
	{
	public:

		// version to initailze from command
		CppUtils::RefCountPtr<BaseDlgEntity> operator () (CppUtils::String const& data, CppUtils::String command)
		{
			json::Object json_init_root  = BaseDlgEntity::stringToJson(data);
			
			CppUtils::RefCountPtr<BaseDlgEntity> ptr = T::creator(command);

			if (!ptr.isValid())
				THROW(CppUtils::OperationFailed, "exc_CannotCreateInstance", "ctx_JsonDeSerializerHelper", "");

			ptr->fromJson(json_init_root);

			return ptr;

		};
		
		CppUtils::RefCountPtr<BaseDlgEntity> operator () (CppUtils::String const& data)
		{
			json::Object json_init_root  = BaseDlgEntity::stringToJson(data);

			CppUtils::RefCountPtr<BaseDlgEntity> ptr = T::creator(json_init_root);

			if (!ptr.isValid())
				THROW(CppUtils::OperationFailed, "exc_CannotCreateInstance", "ctx_JsonDeSerializerHelper", "");

			ptr->fromJson(json_init_root);

			return ptr;

		};

		template<class RESULT>
		CppUtils::RefCountPtr<RESULT> deserialize (CppUtils::String const& data)
		{
			json::Object json_init_root  = BaseDlgEntity::stringToJson(data);

			CppUtils::RefCountPtr<RESULT> ptr = static_cast<RESULT*>(T::creator(json_init_root));

			if (!ptr.isValid())
				THROW(CppUtils::OperationFailed, "exc_CannotCreateInstance", "ctx_JsonDeSerializerHelper", "");

			ptr->fromJson(json_init_root);

			return ptr;

		};
	};

	

	// ------------------------------------------------

	template<class E, class T>
	class CallIfMatch
	{
	public:
		bool operator()(T* ptr) {
			return (typeid(E) == typeid(*ptr)) ;
		}
	};

	
	// ------------------------------------------------


	// better option not to use typeid
	template<class DERIVED_CLASS>
	class CallIfMatch2
	{
	public:
		bool operator()(BaseDlgEntity const* ptr) {
			return (DERIVED_CLASS::CLASS_ID == ptr->getClassId()) ;
		};

		bool operator()(BaseDlgEntity const& dlgRef) {
			return (DERIVED_CLASS::CLASS_ID == dlgRef.getClassId());
		};

	};

	// ------------------------------------------------


#define BEGIN_CREATOR_DLG_ENTRY(COMMAND)	\
	char const* __command_c = COMMAND;	\

#define CREATOR_DLG_ENTRY(DERIVED)	\
	if (_stricmp(DERIVED::getCommandNameStatic(), __command_c) == 0) {\
		return new DERIVED();\
	}\

#define END_CREATOR_DLG_ENTRY()	\

		
	


}; // end of ns

#endif