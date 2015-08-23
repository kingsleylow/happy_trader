/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.helper;

import com.fin.httrader.utils.HtException;
import org.json.simple.JSONObject;

/**
 *
 * @author Victor_Zoubok
 */
public class HtMtJsonUtils {
		
		public static String getContext()
		{
				return HtMtJsonUtils.class.getSimpleName();
		}
		
		public static <T> T getJsonField(JSONObject json_object, String fieldName) throws Exception
		{
				T object_data = (T)json_object.get(fieldName);
				if (object_data == null)
						throw new HtException(getContext(), "getJsonField", "Invalid JSON entry: ("+fieldName+")");
				
				return object_data;
		}
		
		public static double getDoubleJsonField(JSONObject json_object, String fieldName) throws Exception
		{
				Object val = json_object.get(fieldName);
				if (val == null)
						throw new HtException(getContext(), "getDoubleJsonField", "Invalid JSON entry: ("+fieldName+")");
				
				if (val instanceof Number)
						return ((Number)val).doubleValue();
				
				if (val instanceof String)
					return Double.valueOf((String)val);
				
				throw new HtException(getContext(), "getDoubleJsonField", "Not Number entry: ("+fieldName+")");
		}
		
		public static long getLongJsonField(JSONObject json_object, String fieldName) throws Exception
		{
				Object val = json_object.get(fieldName);
				if (val == null)
						throw new HtException(getContext(), "getLongJsonField", "Invalid JSON entry: ("+fieldName+")");
				
				if (val instanceof Number)
						return ((Number)val).longValue();
				
				if (val instanceof String)
					return Long.valueOf((String)val);
				
				throw new HtException(getContext(), "getLongJsonField", "Not Number entry: ("+fieldName+")");
		}
		
		public static int getIntJsonField(JSONObject json_object, String fieldName) throws Exception
		{
				
				Object val = json_object.get(fieldName);
				if (val == null)
						throw new HtException(getContext(), "getIntJsonField", "Invalid JSON entry: ("+fieldName+")");
				
				if (val instanceof Number)
						return ((Number)val).intValue();
				
				if (val instanceof String)
					return Integer.valueOf((String)val);
				
				throw new HtException(getContext(), "getIntJsonField", "Not Number entry: ("+fieldName+")");
		}
		
		public static Boolean getBooleanJsonFieldNull(JSONObject json_object, String fieldName) throws Exception
		{
			Object val = json_object.get(fieldName);
			if (val == null)
				return null;
			
			return Boolean.valueOf(getBooleanJsonField(json_object, fieldName));
								
		}
		
		public static boolean getBooleanJsonField(JSONObject json_object, String fieldName) throws Exception
		{
				
				Object val = json_object.get(fieldName);
				if (val == null)
						throw new HtException(getContext(), "getBooleanJsonField", "Invalid JSON entry: ("+fieldName+")");
				
				if (val instanceof Boolean)
						return ((Boolean)val);
				
				if (val instanceof Number)
						return ((Number)val).longValue() != 0;
				
				if (val instanceof String)
						return ((String)val).equalsIgnoreCase("true");
				
				throw new HtException(getContext(), "getBooleanJsonField", "Not Number entry: ("+fieldName+")");
		}
}
