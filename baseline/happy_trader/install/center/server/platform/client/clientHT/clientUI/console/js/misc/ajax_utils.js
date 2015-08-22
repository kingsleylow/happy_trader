var AjaxHelperObject = {


	
	wrapToCDataTags: function(srcstring)
	{

		result ="<![CDATA[";
		result += (new String(srcstring)).replace(/]]>/g,"]]]]><![CDATA[>");
		result += "]]>";

		return result;
	},

	assosiativeArrayToXmlParameter: function(parameters, entity_name)
	{

		var result='<?xml version="1.0" encoding="ISO-8859-1"?>';
		result += '<ent name="'+entity_name+'">';

		for (k_i in parameters) {
			result += '<par name="'+k_i+'" type="8">';
			result += '<val>'+AjaxHelperObject.wrapToCDataTags(parameters[k_i])+'</val>';
			result += '</par>';
		}

		result += '</ent>';
		return result;
	},

	assosiativeArrayToXmlParameterNoCDATA: function(parameters, entity_name)
	{

		var result='<?xml version="1.0" encoding="ISO-8859-1"?>';
		result += '<ent name="'+entity_name+'">';

		for (k_i in parameters) {
			result += '<par name="'+k_i+'" type="8">';
			result += '<val>'+parameters[k_i]+'</val>';
			result += '</par>';
		}

		result += '</ent>';
		return result;
	}


}