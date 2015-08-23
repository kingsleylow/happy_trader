#include "proxybase.hpp"

namespace HlpStruct {

	CallingContext::CallingContext()
	{
	}

	CallingContext::~CallingContext()
	{
		// clean up
		for(int i = 0; i < parameterPtr_m.size(); i++) {
			delete parameterPtr_m[ i ];
		}
	}

	void convertCallingContextFromXmlParameter(CallingContext& context, XmlParameter const& xmlparameter)
	{
		// function name
		context.callName_m = xmlparameter.getName();

		// convert parameters
		CppUtils::StringList paramList;
		xmlparameter.getXmlParametersKeys(
	}

	void convertCallingContextToXmlParameter(CallingContext const& context, XmlParameter& xmlparameter)
	{
	}

	

};