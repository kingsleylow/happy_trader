/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.webserver;

import com.fin.httrader.utils.hlpstruct.HtPair;
import com.fin.httrader.utils.hlpstruct.XmlHandler;
import com.fin.httrader.utils.hlpstruct.XmlParameter;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author Victor_Zoubok
 */
public class ExportImportXGridUtils {
 
	public String getContext() {
		return ExportImportXGridUtils.class.getSimpleName();
	}
	
	public static List<HtPair<String, String>> getAsPropertiesList(String data) throws Exception
	{
	  List<HtPair<String, String>> result = new ArrayList<HtPair<String, String>>();
	  
	 
	  XmlParameter xmlparameter = XmlHandler.deserializeParameterStatic(data);
	  XmlParameter value_data_2 = xmlparameter.getXmlParameter("data");
	  List<XmlParameter> rows_data = value_data_2.getXmlParameterList("rows");
	  
	  for (int i = 0; i < rows_data.size(); i++) {
		XmlParameter row_i = rows_data.get(i);
		
		String name = row_i.getXmlParameter("Name").getString("value");
		String value = row_i.getXmlParameter("Value").getString("value");
			
		
		result.add(new HtPair(name, value));
		
	  }

	  return result;
	}
}
