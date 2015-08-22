#include "callcontext.hpp"

namespace HlpStruct {

	
	void convertCallingContextFromXmlParameter(ProxyBaseCreator& creator, CallingContext& context, XmlParameter const& xmlparameter)
	{
		// function name

		if (xmlparameter.getName() !="function_call_wrap")
			THROW(CppUtils::OperationFailed, "exc_InvalidCallingContextXml", "ctx_ConvertCallingContextFromXmlParameter", "");

		context.callName_m = xmlparameter.getParameter("function").getAsString();
		context.callCustomName_m = xmlparameter.getParameter("function_custom").getAsString();

		context.resultCode_m = xmlparameter.getParameter("status_code").getAsInt();
		context.resultReason_m = CppUtils::unhexlify(xmlparameter.getParameter("status_reason").getAsString());
		
		XmlParameter const& xmlParams = xmlparameter.getXmlParameter("parameters");


		// convert parameters
		CppUtils::StringList paramList;
		xmlParams.getXmlParametersKeys( paramList );

		for(int i = 0; i < paramList.size(); i++) {
			

			XmlParameter const& param_i = xmlParams.getXmlParameter(paramList[i]);
			
			// instantiate basing on the type
			ParamBase* proxyInst = creator.instantiate(param_i.getParameter("type").getAsString().c_str());

			// value
			
			proxyInst->convertFromXmlparameter(param_i);

			// name
			proxyInst->getParamName() = paramList[i];

			context.parameterPtr_m[proxyInst->getParamName()] = proxyInst;
			
			
		}
	}

	void convertCallingContextToXmlParameter(CallingContext const& context, XmlParameter& xmlparameter)
	{
		xmlparameter.clear();
		xmlparameter.getName() = "function_call_wrap";
		xmlparameter.getParameter("function").getAsString(true) = context.callName_m;
		xmlparameter.getParameter("function_custom").getAsString(true) = context.callCustomName_m;
		
		
		XmlParameter xmlParams;
		

		for (map<CppUtils::String, ParamBase*>::const_iterator it = context.parameterPtr_m.begin(); it!=context.parameterPtr_m.end();it++) {
			ParamBase* proxyInst = it->second;

			XmlParameter param_i;
			proxyInst->convertToXmlParameter(param_i);

			// name & value
			xmlParams.getXmlParameter(it->first) = param_i;

		};

		// when converting back fill in the result code and result reason

		xmlparameter.getXmlParameter("parameters") = xmlParams;

		xmlparameter.getParameter("status_code").getAsInt(true) = context.resultCode_m;
		xmlparameter.getParameter("status_reason").getAsString(true) = CppUtils::hexlify(context.resultReason_m);

	}

	

};
