#ifndef _HLPSTRUCT_CALLCONTEXT_INCLUDED
#define _HLPSTRUCT_CALLCONTEXT_INCLUDED

#include "hlpstructdefs.hpp"
#include "xmlparameter.hpp"
#include "proxybase.hpp"

namespace HlpStruct {

	// macro to create creator
#define BEGIN_PROXY_CREATOR(	)	\
	virtual HlpStruct::ParamBase* instantiate(char const* instName) {

#define END_PROXY_CREATOR()	\
		return NULL;	\
	};

#define PROXY_CREATOR(NAME, INST)	\
	if (strcmp(NAME, instName)==0) {	\
		return new INST;	\
	};


	// basic class to create the firlds participated in the exchange
	class HLPSTRUCT_EXP ProxyBaseCreator {
	public:
		ProxyBaseCreator()
		{
		}

		virtual ~ProxyBaseCreator()
		{
		}

		// instantiate defined ProxyBase
		virtual ParamBase* instantiate(char const* instName) = 0;
	};

	// structure responsible for the context of data passed to remote function
	class HLPSTRUCT_EXP CallingContext {
		
		friend HLPSTRUCT_EXP void convertCallingContextFromXmlParameter(ProxyBaseCreator& creator, CallingContext& context, XmlParameter const& xmlparameter);
		
		friend HLPSTRUCT_EXP void convertCallingContextToXmlParameter(CallingContext const& context, XmlParameter& xmlparameter);
	
	public:
		// ctor & dtor
		CallingContext():
			resultCode_m(0)
		{
		}

		~CallingContext()
		{
		

			// clean up
			for (map<CppUtils::String, ParamBase*>::iterator it = parameterPtr_m.begin(); it!=parameterPtr_m.end();it++) 
				delete it->second;
			
		}
		
		inline CppUtils::String& getCallName()
		{
			return callName_m;
		}

		inline CppUtils::String const& getCallName() const
		{
			return (CppUtils::String const&)callName_m;
		}

		inline CppUtils::String& getCallCustomName()
		{
			return callCustomName_m;
		}

		inline CppUtils::String const& getCallCustomName() const
		{
			return (CppUtils::String const&)callCustomName_m;
		}


		inline int& getResultCode()
		{
			return resultCode_m;
		}

		inline CppUtils::String& getResultReason()
		{
			return resultReason_m;
		}

		void removeParameter(CppUtils::String const& pname)
		{
			map<CppUtils::String, ParamBase*>::iterator it = parameterPtr_m.find(pname);
			if (it == parameterPtr_m.end())
				THROW(CppUtils::OperationFailed, "exc_InvalidParameterName", "ctx_CallingContext", pname );
			
			delete it->second;	
			parameterPtr_m.erase(pname);
		}

		ParamBase& getParameter(CppUtils::String const& pname)
		{
			map<CppUtils::String, ParamBase*>::const_iterator it = parameterPtr_m.find(pname);

			if (it == parameterPtr_m.end())
				THROW(CppUtils::OperationFailed, "exc_InvalidParameterName", "ctx_CallingContext", pname );

			return *it->second;
		}

		void initializeParameter(ProxyBaseCreator& creator, CppUtils::String const& pname, char const* paramType)
		{
			map<CppUtils::String, ParamBase*>::const_iterator it = parameterPtr_m.find(pname);

			if (it != parameterPtr_m.end())
				delete it->second;	

			ParamBase* proxyInst = creator.instantiate(paramType);
			parameterPtr_m[pname] = proxyInst;
		}

		
		
		
		// templatized function making type cast
		template <class T>
		T& getParameter(CppUtils::String const& pname)
		{
			return dynamic_cast< T& >(getParameter(pname));
		}

		template <class T>
		T const& getParameter(CppUtils::String const& pname) const
		{
			return dynamic_cast< T const& >(getParameter(pname));
		}

				
	private:

	

		// name identifying the functions
		CppUtils::String callName_m;

		// another string identifying call custom name
		CppUtils::String callCustomName_m;

		// passed parameters 
		map<CppUtils::String, ParamBase*> parameterPtr_m;

		int resultCode_m;

		CppUtils::String resultReason_m;
	};

	

	HLPSTRUCT_EXP void convertCallingContextFromXmlParameter(ProxyBaseCreator& creator, CallingContext& context, XmlParameter const& xmlparameter);

	HLPSTRUCT_EXP void convertCallingContextToXmlParameter(CallingContext const& context, XmlParameter& xmlparameter);

	

};


#endif