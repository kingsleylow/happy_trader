#include "human_xmlparam_serializer.hpp"

namespace HlpStruct {

void HumanSerializer::serializeXmlParameter(XmlParameter const& xmlparameter, CppUtils::String& content)
{
	content.clear();
	CppUtils::StringList keys;
	xmlparameter.getValueKeys(keys);

	for(int i = 0; i < keys.size(); i++) {
		CppUtils::String const& key_i = keys[i];

		CppUtils::Value const& param_i = xmlparameter.getParameter(key_i);
		if (param_i.getType() == CppUtils::Value::VT_String) {
			content += key_i + ":" + param_i.getAsString() + "\r\n";
		}

	}
}

void HumanSerializer::deserializeXmlParameter(CppUtils::String const& content, XmlParameter& xmlparameter)
{
	xmlparameter.clear();
	
	CppUtils::StringList tokens;
	CppUtils::String content_s = content;
	CppUtils::replaceAll(content_s, "\r\n", "\n");
	
	CppUtils::tokenize(content_s, tokens, "\n:");

	int idx = 0;
	CppUtils::String var_name, var_value;
	while(idx < tokens.size()) {
		var_name = tokens[idx];

		int nidx = idx +1;
		if (nidx < tokens.size())
			var_value= tokens[nidx];
		else
			var_value = "";

		xmlparameter.getParameter(var_name).getAsString(true) = var_value;

		idx  = idx + 2;

	}

}


};