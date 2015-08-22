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
		
		public static long getLongJsonField(JSONObject json_object, String fieldName) throws Exception
		{
				Object val = json_object.get(fieldName);
				if (val == null)
						throw new HtException(getContext(), "getLongJsonField", "Invalid JSON entry: ("+fieldName+")");
				
				if (val instanceof Number)
						return ((Number)val).longValue();
				
				throw new HtException(getContext(), "getLongJsonField", "Not Number entry: ("+fieldName+")");
		}
		
		public static int getIntJsonField(JSONObject json_object, String fieldName) throws Exception
		{
				
				return (int)getLongJsonField(json_object, fieldName);
		}
}
