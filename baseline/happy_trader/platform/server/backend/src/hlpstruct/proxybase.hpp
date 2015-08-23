#ifndef _HLPSTRUCT_PROXYBASE_INCLUDED
#define _HLPSTRUCT_PROXYBASE_INCLUDED

#include "hlpstructdefs.hpp"
#include "xmlparameter.hpp"

/**
This is the base class for all the proxies (e.i. objects that can be serailized and unserialized)
to revert its state. 
It is ues for inter-process communication
*/

// macros to have regeiser all the proxy types that we are using



namespace HlpStruct {

	enum ParamType {
		PT_In = 1,
		PT_Out = 2,
		PT_InOut = 3
	};

	class ParamBase {
	public:
		ParamBase():
			paramType_m(PT_InOut)
		{
		}

		virtual ~ParamBase()
		{
		}

			
		CppUtils::String& getParamName()  
		{
			return paramName_m;
		}
	

		ParamType& getType() 
		{
			return paramType_m;
		}

		virtual void convertToXmlParameter(XmlParameter &xmlparameter) = 0;

		virtual void convertFromXmlparameter(XmlParameter const &xmlparameter) = 0;



	private:

		ParamType paramType_m;

		CppUtils::String paramName_m;
	};

	template <typename T>
	class ProxyBase: public ParamBase {
	public:
		// ctor & dtor
		
		ProxyBase()
		{
		}

		virtual ~ProxyBase()
		{
		}
		
		T& get()
		{
			return static_cast<T&>(obj_m);
		}

		T const& get() const
		{
			return static_cast<T const&>(obj_m);
		}
		
	protected:

		T obj_m;


	};


};

#endif
