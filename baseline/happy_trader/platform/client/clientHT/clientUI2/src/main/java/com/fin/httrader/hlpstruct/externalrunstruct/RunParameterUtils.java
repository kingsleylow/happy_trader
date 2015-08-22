/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.hlpstruct.externalrunstruct;

import com.fin.httrader.utils.HtException;
import java.util.List;
import java.util.Map;

/**
 *
 * @author Victor_Zoubok
 */
public class RunParameterUtils {
	private static String getContext()
	{
		return RunParameterUtils.class.getSimpleName();
	}
	
	public static String getSimleStringParameter(Map<String, RunParameter> parameters, String paramName, boolean required) throws Exception
	{
		RunParameter rp =parameters.get(paramName);
		
		if (rp==null) {
			if (required)
				throw new HtException(getContext(), "getSimleStringParameter", "Parameter is not found: '" + paramName + "'");
			else
				return null;
		}
		
		
			// rp != null
		if (rp.getType() == RunParameter.STRING_TYPE) 
				return rp.<RunParameterString>getData().getString();
		else
				throw new HtException(getContext(), "getSimleStringParameter", "Parameter : '" + paramName + "' is not of STRING type");
	}
	

	public static List<String> getMapListParameter(
					Map<String, RunParameter> parameters, 
					String mainParamName,
					boolean required
	) throws Exception
	{
		RunParameter rp =parameters.get(mainParamName);
		
		if (rp==null) {
			if (required)
				throw new HtException(getContext(), "getMapListParameter", "Parameter is not found: '" + mainParamName + "'");
			else
				return null;
		}
		
		if (rp.getType() == RunParameter.STRING_LIST_TYPE) 	
			return rp.<RunParameterStringList>getData().getStringList();
		else 
			throw new HtException(getContext(), "getMapListParameter", "Parameter : '" + mainParamName + "' is not of STRING LIST type");
	}
	
	public static Map<String,String> getMapStringParameter(
					Map<String, RunParameter> parameters, 
					String mainParamName,
					boolean required
	) throws Exception
	{
		RunParameter rp =parameters.get(mainParamName);
		
		if (rp==null) {
			if (required)
				throw new HtException(getContext(), "getMapStringParameter", "Parameter is not found: '" + mainParamName + "'");
			else
				return null;
		}
		
		if (rp.getType() == RunParameter.STRING_MAP_TYPE) 	
			return rp.<RunParameterStringMap>getData().getStringMap();
		else 
			throw new HtException(getContext(), "getMapStringParameter", "Parameter : '" + mainParamName + "' is not of STRING MAP type");
	}
					
	public static String getMapStringParameterResolveKey(
					Map<String, RunParameter> parameters, 
					String mainParamName, 
					String mapKey,
					boolean required
	) throws Exception
	{
		RunParameter rp =parameters.get(mainParamName);
		if (rp==null) {
			if (required)
				throw new HtException(getContext(), "getMapStringParameter", "Parameter is not found: '" + mainParamName + "'");
			else
				return null;
		}
		
		if (rp.getType() == RunParameter.STRING_MAP_TYPE) 	{
			Map<String,String> d = rp.<RunParameterStringMap>getData().getStringMap();
			
			String result = d.get(mapKey);
			if (result == null) {
				if (required)
					throw new HtException(getContext(), "getMapStringParameter", "Parameter is found: '" + mainParamName + "' but map does not contain the key: '" + mapKey+"'" );
				else
					return null;
			}
			
			return result;
			
		}
		else 
			throw new HtException(getContext(), "getMapStringParameter", "Parameter : '" + mainParamName + "' is not of STRING MAP type");
		
	}

}